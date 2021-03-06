//
//  KeyRemap4MacBook_serverAppDelegate.m
//  KeyRemap4MacBook_server
//
//  Created by Takayama Fumihiko on 09/11/01.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Carbon/Carbon.h>
#import "KeyRemap4MacBook_serverAppDelegate.h"
#import "KeyRemap4MacBookKeys.h"
#import "StatusWindow.h"
#import "UserClient_userspace.h"
#include <stdlib.h>

@implementation KeyRemap4MacBook_serverAppDelegate

@synthesize window;

// ----------------------------------------
- (void) statusBarItemSelected:(id)sender {
  [statusbar_ statusBarItemSelected:sender];
}

// ------------------------------------------------------------
- (void) send_workspacedata_to_kext {
  struct BridgeUserClientStruct bridgestruct;
  bridgestruct.type   = BRIDGE_USERCLIENT_TYPE_SET_WORKSPACEDATA;
  bridgestruct.option = 0;
  bridgestruct.data   = (uintptr_t)(&bridgeworkspacedata_);
  bridgestruct.size   = sizeof(bridgeworkspacedata_);

  [UserClient_userspace synchronized_communication:&bridgestruct];
}

- (void) observer_NSWorkspaceDidActivateApplicationNotification:(NSNotification*)notification
{
  NSString* name = [WorkSpaceData getActiveApplicationName];
  if (name) {
    // We ignore our investigation application.
    if (! [name isEqualToString:@"org.pqrs.KeyRemap4MacBook.KeyDump"]) {
      bridgeworkspacedata_.applicationtype = [WorkSpaceData getApplicationType:name];
      [self send_workspacedata_to_kext];

      NSDictionary* userInfo = [NSDictionary dictionaryWithObject:name forKey:@"name"];
      [[NSDistributedNotificationCenter defaultCenter] postNotificationName:kKeyRemap4MacBookApplicationChangedNotification object:kKeyRemap4MacBookNotificationKey userInfo:userInfo];
    }
  }
}

- (void) observer_kTISNotifySelectedKeyboardInputSourceChanged:(NSNotification*)notification
{
  NSString* name = [WorkSpaceData getTISPropertyInputModeID];
  if (name) {
    [WorkSpaceData getInputMode:name
               output_inputmode:(&(bridgeworkspacedata_.inputmode))
         output_inputmodedetail:(&(bridgeworkspacedata_.inputmodedetail))];
    [self send_workspacedata_to_kext];

    NSDictionary* userInfo = [NSDictionary dictionaryWithObject:name forKey:@"name"];
    [[NSDistributedNotificationCenter defaultCenter] postNotificationName:kKeyRemap4MacBookInputSourceChangedNotification object:kKeyRemap4MacBookNotificationKey userInfo:userInfo];
  }
}

// ------------------------------------------------------------
- (void) send_remapclasses_initialize_vector_to_kext {
  NSArray* a = [[ConfigXMLParser getInstance] remapclasses_initialize_vector];
  if (! a) {
    NSLog(@"[WARNING] remapclasses_initialize_vector == nil.");
    return;
  }

  size_t size = [a count] * sizeof(uint32_t);
  uint32_t* data = (uint32_t*)(malloc(size));
  if (! data) {
    NSLog(@"[WARNING] malloc failed.");
    return;

  } else {
    // --------------------
    uint32_t* p = data;
    for (NSNumber* number in a) {
      *p++ = [number unsignedIntValue];
    }

    // --------------------
    struct BridgeUserClientStruct bridgestruct;
    bridgestruct.type   = BRIDGE_USERCLIENT_TYPE_SET_REMAPCLASSES_INITIALIZE_VECTOR;
    bridgestruct.option = 0;
    bridgestruct.data   = (uintptr_t)(data);
    bridgestruct.size   = size;

    [UserClient_userspace synchronized_communication:&bridgestruct];

    free(data);
  }
}

- (void) send_config_to_kext {
  PreferencesManager* preferencesmanager = [PreferencesManager getInstance];
  ConfigXMLParser*    configxmlparser    = [ConfigXMLParser    getInstance];

  NSArray* essential_config = [preferencesmanager essential_config];
  if (! essential_config) {
    NSLog(@"[WARNING] essential_config == nil.");
    return;
  }

  // ------------------------------------------------------------
  NSUInteger essential_config_count = [essential_config count];
  NSUInteger remapclasses_count     = [configxmlparser count];
  size_t size = (essential_config_count + remapclasses_count) * sizeof(int32_t);
  int32_t* data = (int32_t*)(malloc(size));
  if (! data) {
    NSLog(@"[WARNING] malloc failed.");
    return;

  } else {
    int32_t* p = data;

    // --------------------
    // essential_config
    for (NSNumber* number in essential_config) {
      *p++ = [number intValue];
    }

    // --------------------
    // remapclasses config
    for (NSUInteger i = 0; i < remapclasses_count; ++i) {
      NSString* name = [configxmlparser configname:(int)(i)];
      if (! name) {
        NSLog(@"[WARNING] %s name == nil. private.xml has error?", __FUNCTION__);
        *p++ = 0;
      } else {
        *p++ = [preferencesmanager value:name];
      }
    }

    // --------------------
    struct BridgeUserClientStruct bridgestruct;
    bridgestruct.type   = BRIDGE_USERCLIENT_TYPE_SET_CONFIG;
    bridgestruct.option = 0;
    bridgestruct.data   = (uintptr_t)(data);
    bridgestruct.size   = size;

    [UserClient_userspace synchronized_communication:&bridgestruct];

    free(data);
  }
}

// ------------------------------------------------------------
static void observer_IONotification(void* refcon, io_iterator_t iterator) {
  NSLog(@"observer_IONotification");

  KeyRemap4MacBook_serverAppDelegate* self = refcon;
  if (! self) {
    NSLog(@"[ERROR] observer_IONotification refcon == nil\n");
    return;
  }

  for (;;) {
    io_object_t obj = IOIteratorNext(iterator);
    if (! obj) break;

    IOObjectRelease(obj);
  }
  // Do not release iterator.

  // = Documentation of IOKit =
  // - Introduction to Accessing Hardware From Applications
  //   - Finding and Accessing Devices
  //
  // In the case of IOServiceAddMatchingNotification, make sure you release the iterator only if you’re also ready to stop receiving notifications:
  // When you release the iterator you receive from IOServiceAddMatchingNotification, you also disable the notification.

  // ------------------------------------------------------------
  // [UserClient_userspace refresh_connection] may fail by kIOReturnExclusiveAccess
  // when NSWorkspaceSessionDidBecomeActiveNotification.
  // So, we retry the connection some times.
  for (int retrycount = 0; retrycount < 10; ++retrycount) {
    [UserClient_userspace refresh_connection];
    if ([UserClient_userspace connected]) break;

    [NSThread sleepForTimeInterval:0.5];
  }

  [self send_remapclasses_initialize_vector_to_kext];
  [self send_config_to_kext];
  [self send_workspacedata_to_kext];
}

- (void) unregisterIONotification {
  if (notifyport_) {
    if (loopsource_) {
      CFRunLoopSourceInvalidate(loopsource_);
      loopsource_ = nil;
    }
    IONotificationPortDestroy(notifyport_);
    notifyport_ = nil;
  }
}

- (void) registerIONotification {
  [self unregisterIONotification];

  notifyport_ = IONotificationPortCreate(kIOMasterPortDefault);
  if (! notifyport_) {
    NSLog(@"[ERROR] IONotificationPortCreate failed\n");
    return;
  }

  // ----------------------------------------------------------------------
  io_iterator_t it;
  kern_return_t kernResult;

  kernResult = IOServiceAddMatchingNotification(notifyport_,
                                                kIOMatchedNotification,
                                                IOServiceNameMatching("org_pqrs_driver_KeyRemap4MacBook"),
                                                &observer_IONotification,
                                                self,
                                                &it);
  if (kernResult != kIOReturnSuccess) {
    NSLog(@"[ERROR] IOServiceAddMatchingNotification failed");
    return;
  }
  observer_IONotification(self, it);

  // ----------------------------------------------------------------------
  loopsource_ = IONotificationPortGetRunLoopSource(notifyport_);
  if (! loopsource_) {
    NSLog(@"[ERROR] IONotificationPortGetRunLoopSource failed");
    return;
  }
  CFRunLoopAddSource(CFRunLoopGetCurrent(), loopsource_, kCFRunLoopDefaultMode);
}

// ------------------------------------------------------------
- (void) observer_ConfigXMLReloaded:(NSNotification*)notification {
  [self send_remapclasses_initialize_vector_to_kext];
  [self send_config_to_kext];
}

- (void) observer_ConfigListChanged:(NSNotification*)notification {
  [statusbar_ refresh];
}

- (void) observer_PreferencesChanged:(NSNotification*)notification {
  [self send_config_to_kext];
}

// ------------------------------------------------------------
- (void) observer_NSWorkspaceSessionDidBecomeActiveNotification:(NSNotification*)notification
{
  NSLog(@"observer_NSWorkspaceSessionDidBecomeActiveNotification");

  [[StatusWindow getInstance] resetStatusMessage];

  [self registerIONotification];
}

- (void) observer_NSWorkspaceSessionDidResignActiveNotification:(NSNotification*)notification
{
  NSLog(@"observer_NSWorkspaceSessionDidResignActiveNotification");

  [[StatusWindow getInstance] resetStatusMessage];

  [self unregisterIONotification];
  [UserClient_userspace disconnect_from_kext];
}

// ------------------------------------------------------------
- (void) applicationDidFinishLaunching:(NSNotification*)aNotification {
  [[StatusWindow getInstance] resetStatusMessage];

  [statusbar_ refresh];

  [self registerIONotification];

  // ------------------------------------------------------------
  [[[NSWorkspace sharedWorkspace] notificationCenter] addObserver:self
                                                         selector:@selector(observer_NSWorkspaceDidActivateApplicationNotification:)
                                                             name:NSWorkspaceDidActivateApplicationNotification
                                                           object:nil];

  [[NSDistributedNotificationCenter defaultCenter] addObserver:self
                                                      selector:@selector(observer_kTISNotifySelectedKeyboardInputSourceChanged:)
                                                          name:(NSString*)(kTISNotifySelectedKeyboardInputSourceChanged)
                                                        object:nil];

  // ------------------------------
  [[[NSWorkspace sharedWorkspace] notificationCenter] addObserver:self
                                                         selector:@selector(observer_NSWorkspaceSessionDidBecomeActiveNotification:)
                                                             name:NSWorkspaceSessionDidBecomeActiveNotification
                                                           object:nil];

  [[[NSWorkspace sharedWorkspace] notificationCenter] addObserver:self
                                                         selector:@selector(observer_NSWorkspaceSessionDidResignActiveNotification:)
                                                             name:NSWorkspaceSessionDidResignActiveNotification
                                                           object:nil];

  // ------------------------------
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(observer_ConfigListChanged:) name:@"ConfigListChanged" object:nil];

  [[NSDistributedNotificationCenter defaultCenter] addObserver:self
                                                      selector:@selector(observer_ConfigXMLReloaded:)
                                                          name:kKeyRemap4MacBookConfigXMLReloadedNotification
                                                        object:kKeyRemap4MacBookNotificationKey];

  [[NSDistributedNotificationCenter defaultCenter] addObserver:self
                                                      selector:@selector(observer_PreferencesChanged:)
                                                          name:kKeyRemap4MacBookPreferencesChangedNotification
                                                        object:kKeyRemap4MacBookNotificationKey];

  // ------------------------------------------------------------
  [self observer_NSWorkspaceDidActivateApplicationNotification:nil];
  [self observer_kTISNotifySelectedKeyboardInputSourceChanged:nil];

  // ------------------------------------------------------------
  // Kick updater
  [[NSWorkspace sharedWorkspace] launchApplication:@"/Library/org.pqrs/KeyRemap4MacBook/app/KeyRemap4MacBook.app"];
}

@end
