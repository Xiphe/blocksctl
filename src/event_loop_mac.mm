// SPDX-License-Identifier: ISC
#include "event_loop.h"
#import <Foundation/Foundation.h>

void blocksctl::dispatchEvents()
{
    @autoreleasepool {
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode
                               beforeDate:[NSDate dateWithTimeIntervalSinceNow:0.02]];
    }
}
