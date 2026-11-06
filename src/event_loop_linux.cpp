// SPDX-License-Identifier: ISC
#include "event_loop.h"
#include <juce_events/juce_events.h>

void blocksctl::dispatchEvents()
{
    juce::MessageManager::getInstance()->runDispatchLoopUntil(20);
}
