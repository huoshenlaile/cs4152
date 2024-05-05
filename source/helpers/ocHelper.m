//
//  Example.m
//  dusty_paints-ios
//
//  Created by Xilai Dai on 5/3/24.
//

#import "ocHelper.h"
#include <sys/sysctl.h>

@implementation ocHelper

- (NSString *)getDeviceModelIdentifier {
    size_t size;
    sysctlbyname("hw.machine", NULL, &size, NULL, 0);
    char *model = malloc(size);
    sysctlbyname("hw.machine", model, &size, NULL, 0);
    NSString *modelIdentifier = [NSString stringWithUTF8String:model];
    free(model);
    return modelIdentifier;
}

@end
