// SPDX-License-Identifier: ISC
#include <roli_blocks_basics/roli_blocks_basics.h>
#include "event_loop.h"

#include <cerrno>
#include <chrono>
#include <cmath>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <unistd.h>

namespace {
namespace fs = std::filesystem;
using Clock = std::chrono::steady_clock;
constexpr int defaultHeapSize = 512;
volatile std::sig_atomic_t interrupted = 0;

enum ExitCode { success = 0, failure = 1, usage = 2, unavailable = 3, interruptedExit = 130 };

struct Error : std::runtime_error {
    Error(int code, const std::string& text) : std::runtime_error(text), exitCode(code) {}
    int exitCode;
};

struct Options {
    std::string command, source, output, device, format = "binary";
    double timeout = 10.0;
};

void printHelp()
{
    std::cout << "blocksctl " BLOCKSCTL_VERSION "\n"
        "Compile Littlefoot programs and operate ROLI BLOCKS.\n\n"
        "Usage:\n"
        "  blocksctl compile SOURCE -o OUTPUT [--format binary|header]\n"
        "  blocksctl devices [--timeout SECONDS]\n"
        "  blocksctl upload SOURCE --device SERIAL [--timeout SECONDS]\n"
        "  blocksctl save-default SOURCE --device SERIAL [--timeout SECONDS]\n"
        "  blocksctl --help | --version\n\n"
        "compile is offline. Source includes are resolved beside SOURCE.\n"
        "The base heap is 512 bytes; a source #heapsize: directive overrides it.\n"
        "devices actively enters the SDK protocol and can interrupt a running app.\n"
        "upload runs a temporary program until Ctrl-C; it does not save a default.\n"
        "save-default uploads SOURCE and requests a save; flash is not acknowledged.\n"
        "--timeout bounds discovery and transfer, not a loaded upload session.\n";
}

Options parseOptions(int argc, char** argv)
{
    if (argc < 2)
        throw Error(usage, "A command is required. Use --help.");
    Options result;
    result.command = argv[1];
    if (result.command != "compile" && result.command != "devices"
        && result.command != "upload" && result.command != "save-default")
        throw Error(usage, "Unknown command: " + result.command);

    bool seenTimeout = false, seenFormat = false;
    for (int i = 2; i < argc; ++i) {
        const std::string arg = argv[i];
        auto value = [&]() -> std::string {
            if (++i >= argc || std::string(argv[i]).empty() || argv[i][0] == '-')
                throw Error(usage, "Missing value for " + arg);
            return argv[i];
        };
        if (arg == "-o" || arg == "--output") {
            if (result.command != "compile" || !result.output.empty())
                throw Error(usage, "--output is allowed once, for compile only.");
            result.output = value();
        } else if (arg == "--device") {
            if ((result.command != "upload" && result.command != "save-default") || !result.device.empty())
                throw Error(usage, "--device is allowed once, for upload or save-default only.");
            result.device = value();
        } else if (arg == "--format") {
            if (result.command != "compile" || seenFormat)
                throw Error(usage, "--format is allowed once, for compile only.");
            result.format = value();
            seenFormat = true;
            if (result.format != "binary" && result.format != "header")
                throw Error(usage, "Format must be binary or header.");
        } else if (arg == "--timeout") {
            if (result.command == "compile" || seenTimeout)
                throw Error(usage, "--timeout is allowed once, for device commands only.");
            const auto text = value();
            char* end = nullptr;
            result.timeout = std::strtod(text.c_str(), &end);
            if (end == text.c_str() || *end != '\0' || !std::isfinite(result.timeout)
                || result.timeout <= 0 || result.timeout > 3600)
                throw Error(usage, "Timeout must be greater than 0 and at most 3600 seconds.");
            seenTimeout = true;
        } else if (!arg.empty() && arg[0] != '-' && result.source.empty() && result.command != "devices") {
            result.source = arg;
        } else {
            throw Error(usage, "Unexpected argument: " + arg);
        }
    }
    if (result.command != "devices" && result.source.empty())
        throw Error(usage, "A Littlefoot source file is required.");
    if (result.command == "compile" && result.output.empty())
        throw Error(usage, "compile requires -o OUTPUT.");
    if ((result.command == "upload" || result.command == "save-default") && result.device.empty())
        throw Error(usage, "Select a device with --device SERIAL. Use devices to find its serial.");
    return result;
}

struct Source {
    juce::String text;
    juce::Array<juce::File> searchPaths;
};

Source readSource(const std::string& path)
{
    if (!fs::is_regular_file(path))
        throw Error(failure, "Source is not a regular file: " + path);
    std::ifstream input(path, std::ios::binary);
    if (!input)
        throw Error(failure, "Cannot open source: " + path);
    const std::string text{std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
    if (input.bad())
        throw Error(failure, "Cannot read source: " + path);
    if (text.find('\0') != std::string::npos)
        throw Error(failure, "Source contains a NUL byte: " + path);
    Source source;
    source.text = juce::String::fromUTF8(text.data(), static_cast<int>(text.size()));
    source.searchPaths.add(juce::File(juce::String(fs::absolute(path).parent_path().string())));
    return source;
}

struct Compiled {
    std::vector<unsigned char> bytes;
    unsigned int heapBytes = 0;
};

Compiled compile(const Source& source)
{
    roli::littlefoot::Compiler compiler;
    compiler.addNativeFunctions(roli::PhysicalTopologySource::getStandardLittleFootFunctions());
    const auto result = compiler.compile(source.text, defaultHeapSize, source.searchPaths);
    if (result.failed())
        throw Error(failure, result.getErrorMessage().toStdString());
    if (compiler.compiledObjectCode.size() < 10)
        throw Error(failure, "Compiler returned an incomplete program.");
    const auto program = compiler.getCompiledProgram();
    if (program.getProgramSize() != static_cast<unsigned int>(compiler.compiledObjectCode.size())
        || !program.checksumMatches())
        throw Error(failure, "Compiler returned an invalid program checksum or size.");
    return {std::vector<unsigned char>(compiler.compiledObjectCode.begin(), compiler.compiledObjectCode.end()),
            program.getHeapSizeBytes()};
}

void checkOutputPath(const std::string& source, const std::string& output)
{
    if (fs::weakly_canonical(source) == fs::weakly_canonical(output)
        || (fs::exists(output) && fs::equivalent(source, output)))
        throw Error(usage, "Output must not replace the source file.");
    if (!fs::is_directory(fs::absolute(output).parent_path()))
        throw Error(failure, "Output directory does not exist.");
}

// A sibling temporary file plus POSIX rename preserves the last good output.
void writeAtomically(const std::string& output, const std::vector<unsigned char>& bytes)
{
    auto pattern = fs::absolute(output).string() + ".XXXXXX";
    std::vector<char> temporary(pattern.begin(), pattern.end());
    temporary.push_back('\0');
    int fd = ::mkstemp(temporary.data());
    if (fd < 0)
        throw Error(failure, "Cannot create output: " + std::string(std::strerror(errno)));
    try {
        size_t offset = 0;
        while (offset < bytes.size()) {
            const auto written = ::write(fd, bytes.data() + offset, bytes.size() - offset);
            if (written < 0 && errno == EINTR) continue;
            if (written <= 0) throw Error(failure, "Cannot write output.");
            offset += static_cast<size_t>(written);
        }
        const int closeResult = ::close(fd);
        fd = -1;
        if (closeResult != 0 || ::rename(temporary.data(), output.c_str()) != 0)
            throw Error(failure, "Cannot replace output: " + std::string(std::strerror(errno)));
    } catch (...) {
        if (fd >= 0) ::close(fd);
        ::unlink(temporary.data());
        throw;
    }
}

std::vector<unsigned char> asHeader(const std::vector<unsigned char>& bytes)
{
    std::ostringstream output;
    output << "// Generated by blocksctl " BLOCKSCTL_VERSION ".\n#pragma once\n\n"
           << "static constexpr unsigned char littlefoot_program[] = {\n";
    for (size_t i = 0; i < bytes.size(); ++i) {
        if (i % 12 == 0) output << "    ";
        output << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(bytes[i]) << ',';
        output << (i % 12 == 11 || i + 1 == bytes.size() ? '\n' : ' ');
    }
    output << "};\n";
    const auto text = output.str();
    return {text.begin(), text.end()};
}

void onSignal(int) { interrupted = 1; }

bool before(Clock::time_point deadline) { return Clock::now() < deadline; }

roli::Block::Ptr findDevice(roli::PhysicalTopologySource& topology, const std::string& serial)
{
    roli::Block::Ptr selected;
    for (const auto& block : topology.getCurrentTopology().blocks) {
        if (block->isConnected() && block->serialNumber.toStdString() == serial) {
            if (selected != nullptr)
                throw Error(unavailable, "More than one connected device has serial " + serial);
            selected = block;
        }
    }
    return selected;
}

void checkInterrupted()
{
    if (interrupted)
        throw Error(interruptedExit, "Interrupted.");
}

struct SourceProgram : roli::Block::Program {
    SourceProgram(roli::Block& block, const Source& sourceToUse)
        : roli::Block::Program(block), source(sourceToUse) {}
    juce::String getLittleFootProgram() override { return source.text; }
    juce::Array<juce::File> getSearchPaths() override { return source.searchPaths; }
    Source source;
};

struct LoadedListener : roli::Block::ProgramLoadedListener {
    explicit LoadedListener(roli::Block::Ptr target) : block(std::move(target))
    { block->addProgramLoadedListener(this); }
    ~LoadedListener() override { block->removeProgramLoadedListener(this); }
    void handleProgramLoaded(roli::Block&) override { loaded = true; }
    roli::Block::Ptr block;
    bool loaded = false;
};

int runDeviceCommand(const Options& options, const Source* source)
{
    std::signal(SIGINT, onSignal);
    std::signal(SIGTERM, onSignal);
    juce::ScopedJuceInitialiser_GUI platform;
    roli::PhysicalTopologySource topology;
    const auto deadline = Clock::now() + std::chrono::duration_cast<Clock::duration>(std::chrono::duration<double>(options.timeout));
    std::cerr << "Active BLOCKS discovery. Other devices on the same connection can enter API mode.\n";

    if (options.command == "devices") {
        while (before(deadline)) {
            checkInterrupted();
            blocksctl::dispatchEvents();
        }
        checkInterrupted();
        std::map<std::string, roli::Block::Ptr> devices;
        for (const auto& block : topology.getCurrentTopology().blocks)
            if (block->isConnected()) devices[block->serialNumber.toStdString()] = block;
        if (devices.empty()) throw Error(unavailable, topology.isLockedFromOutside()
            ? "Another SDK host holds a device lock. Close it and retry."
            : "No BLOCKS devices found before timeout.");
        std::cout << "serial\tuid\tmodel\tfirmware\ttransport\n";
        for (const auto& entry : devices) {
            const auto& block = entry.second;
            std::cout << block->serialNumber << '\t' << juce::String::toHexString(static_cast<juce::int64>(block->uid))
                      << '\t' << block->getDeviceDescription() << '\t' << block->versionNumber
                      << '\t' << (block->isConnectedViaBluetooth() ? "bluetooth" : "usb/dna") << '\n';
        }
        return success;
    }

    roli::Block::Ptr block;
    while (before(deadline) && block == nullptr) {
        checkInterrupted();
        blocksctl::dispatchEvents();
        block = findDevice(topology, options.device);
    }
    checkInterrupted();
    if (block == nullptr) throw Error(unavailable, topology.isLockedFromOutside()
        ? "Device not found; another SDK host holds a device lock. Close it and retry."
        : "Device not found before timeout: " + options.device);
    if (!before(deadline)) throw Error(unavailable, "Discovery timeout expired before upload could start.");

    LoadedListener listener(block);
    const bool saveDefault = options.command == "save-default";
    auto newProgram = std::make_unique<SourceProgram>(*block, *source);
    const auto* expectedProgram = newProgram.get();
    const auto result = block->setProgram(std::move(newProgram),
        saveDefault ? roli::Block::ProgramPersistency::setAsDefault : roli::Block::ProgramPersistency::setAsTemp);
    if (result.failed()) throw Error(failure, result.getErrorMessage().toStdString());
    while (!listener.loaded && before(deadline)) {
        checkInterrupted();
        blocksctl::dispatchEvents();
        if (findDevice(topology, options.device) != block || block->getProgram() != expectedProgram)
            throw Error(unavailable, "Device disconnected during upload.");
    }
    checkInterrupted();
    if (!listener.loaded) throw Error(unavailable, "Upload timed out before the device reported a loaded program.");

    if (saveDefault) {
        std::cout << "Program loaded. Save-default requested for " << options.device
                  << ". Flash storage is not acknowledged; verify with a power cycle.\n" << std::flush;
        const auto settleUntil = Clock::now() + std::chrono::seconds(1);
        while (before(settleUntil)) {
            checkInterrupted();
            blocksctl::dispatchEvents();
            if (findDevice(topology, options.device) != block || block->getProgram() != expectedProgram)
                throw Error(unavailable, "Device disconnected after save request; persistence is unknown.");
        }
    } else {
        std::cout << "Temporary program loaded on " << options.device
                  << ". Keep this process running. Ctrl-C ends the session and can resume the saved default.\n" << std::flush;
        while (!interrupted) {
            checkInterrupted();
            blocksctl::dispatchEvents();
            if (findDevice(topology, options.device) != block || block->getProgram() != expectedProgram)
                throw Error(unavailable, "Device disconnected; the temporary session has ended.");
        }
        std::cerr << "Temporary session ended. No save-default request was sent.\n";
    }
    return success;
}
} // namespace

int main(int argc, char** argv)
{
    try {
        for (int i = 1; i < argc; ++i)
            if (std::string(argv[i]) == "--help" || std::string(argv[i]) == "-h") {
                printHelp();
                return success;
            }
        if (argc == 2 && std::string(argv[1]) == "--version") {
            std::cout << "blocksctl " BLOCKSCTL_VERSION "\n";
            return success;
        }
        const auto options = parseOptions(argc, argv);
        if (options.command == "devices") return runDeviceCommand(options, nullptr);

        const auto source = readSource(options.source);
        // Compile before opening MIDI so syntax and include errors never contact hardware.
        const auto program = compile(source);
        if (options.command == "compile") {
            checkOutputPath(options.source, options.output);
            writeAtomically(options.output, options.format == "header" ? asHeader(program.bytes) : program.bytes);
            std::cout << "Compiled " << program.bytes.size() << " bytes; heap " << program.heapBytes
                      << " bytes. Wrote " << options.output << '\n';
            return success;
        }
        return runDeviceCommand(options, &source);
    } catch (const Error& error) {
        std::cerr << "blocksctl: " << error.what() << '\n';
        return error.exitCode;
    } catch (const std::exception& error) {
        std::cerr << "blocksctl: " << error.what() << '\n';
        return failure;
    }
}
