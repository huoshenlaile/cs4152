//
//  ObjectiveCBridge.m
//  dusty_paints-ios
//
//  Created by Xilai Dai on 5/3/24.
//

#import <Foundation/Foundation.h>
#import "ocHelper.h"

#ifdef __cplusplus
extern "C" {
#endif

const char* getDeviceModelIdentifier(void) {
    ocHelper *helper = [[ocHelper alloc] init];
    NSString *modelIdentifier = [helper getDeviceModelIdentifier];
    const char *cStringModelIdentifier = [modelIdentifier UTF8String];
    return cStringModelIdentifier;
}

#ifdef __cplusplus
}
#endif
