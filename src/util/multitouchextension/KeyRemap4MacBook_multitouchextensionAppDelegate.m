//
//  KeyRemap4MacBook_multitouchextensionAppDelegate.m
//  KeyRemap4MacBook_multitouchextension
//
//  Created by Takayama Fumihiko on 09/11/24.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#include <IOKit/IOKitLib.h>
#import "KeyRemap4MacBook_multitouchextensionAppDelegate.h"

enum { MAX_FINGERS = 3 };
static int current_status_[MAX_FINGERS];

@implementation KeyRemap4MacBook_multitouchextensionAppDelegate

- (id) init
{
  self = [super init];

  if (self) {
    for (int i = 0; i < MAX_FINGERS; ++i) {
      current_status_[i] = 0;
    }
  }

  return self;
}

- (void) dealloc
{
  [mtdevices_ release];

  [super dealloc];
}

// ------------------------------------------------------------
struct Finger;
typedef void* MTDeviceRef;
typedef int (* MTContactCallbackFunction)(int, struct Finger*, int, double, int);

CFMutableArrayRef MTDeviceCreateList(void);
void MTRegisterContactFrameCallback(MTDeviceRef, MTContactCallbackFunction);
void MTUnregisterContactFrameCallback(MTDeviceRef, MTContactCallbackFunction);
void MTDeviceStart(MTDeviceRef, int);
void MTDeviceStop(MTDeviceRef, int);

org_pqrs_KeyRemap4MacBook_Client* global_client_ = nil;

static void setPreference(int fingers, int newvalue) {
  NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
  {
    NSString* name = [PreferencesController getSettingName:fingers];
    if ([name length] > 0) {
      [[global_client_ proxy] setValueForName:newvalue forName:name];
    }
  }
  [pool drain];
}

- (void) resetPreferences
{
  for (int i = 0; i < MAX_FINGERS; ++i) {
    setPreference(i + 1, 0);
  }
}

// ------------------------------------------------------------
// Multitouch callback
static int callback(int device, struct Finger* data, int fingers, double timestamp, int frame) {
  NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
  {
    // deactivate settings first.
    for (int i = 0; i < MAX_FINGERS; ++i) {
      if (current_status_[i] && fingers != i + 1) {
        current_status_[i] = 0;
        setPreference(i + 1, 0);
      }
    }

    if (fingers > 0 && current_status_[fingers - 1] == 0) {
      current_status_[fingers - 1] = 1;

      if ([PreferencesController isSettingEnabled:fingers]) {
        setPreference(fingers, 1);
      }
    }
  }
  [pool drain];

  return 0;
}

- (void) setcallback:(BOOL)isset {
  @synchronized(self) {
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    {
      // ------------------------------------------------------------
      // unset callback (even if isset is YES.)
      if (mtdevices_) {
        for (NSUInteger i = 0; i < [mtdevices_ count]; ++i) {
          MTDeviceRef device = [mtdevices_ objectAtIndex:i];
          if (! device) continue;

          MTDeviceStop(device, 0);
          MTUnregisterContactFrameCallback(device, callback);
        }
        [mtdevices_ release];
        mtdevices_ = nil;
      }

      // ------------------------------------------------------------
      // set callback if needed
      if (isset) {
        mtdevices_ = (NSArray*)(MTDeviceCreateList());
        if (mtdevices_) {
          for (NSUInteger i = 0; i < [mtdevices_ count]; ++i) {
            MTDeviceRef device = [mtdevices_ objectAtIndex:i];
            if (! device) continue;

            MTRegisterContactFrameCallback(device, callback);
            MTDeviceStart(device, 0);
          }
        }
      }

      [self resetPreferences];
    }
    [pool drain];
  }
}

// ------------------------------------------------------------
// IONotification
- (void) release_iterator:(io_iterator_t)iterator {
  for (;;) {
    io_object_t obj = IOIteratorNext(iterator);
    if (! obj) break;

    IOObjectRelease(obj);
  }
}

static void observer_IONotification(void* refcon, io_iterator_t iterator) {
  NSLog(@"observer_IONotification");

  KeyRemap4MacBook_multitouchextensionAppDelegate* self = refcon;

  [self release_iterator:iterator];

  [self setcallback:YES];
}

- (void) unregisterIONotification {
  NSLog(@"unregisterIONotification");

  @synchronized(self) {
    if (notifyport_) {
      if (loopsource_) {
        CFRunLoopSourceInvalidate(loopsource_);
        loopsource_ = nil;
      }
      IONotificationPortDestroy(notifyport_);
      notifyport_ = nil;
    }
  }
}

- (void) registerIONotification {
  NSLog(@"registerIONotification");

  @synchronized(self) {
    if (notifyport_) {
      [self unregisterIONotification];
    }

    notifyport_ = IONotificationPortCreate(kIOMasterPortDefault);
    if (! notifyport_) {
      NSLog(@"[ERROR] IONotificationPortCreate");
      return;
    }

    // ------------------------------------------------------------
    NSMutableDictionary* match = [(NSMutableDictionary*)(IOServiceMatching("AppleMultitouchDevice")) autorelease];

    // ----------------------------------------------------------------------
    io_iterator_t it;
    kern_return_t kr;

    [match retain]; // for kIOTerminatedNotification
    kr = IOServiceAddMatchingNotification(notifyport_,
                                          kIOTerminatedNotification,
                                          (CFMutableDictionaryRef)(match),
                                          &observer_IONotification,
                                          self,
                                          &it);
    if (kr != kIOReturnSuccess) {
      NSLog(@"[ERROR] IOServiceAddMatchingNotification");
      return;
    }
    [self release_iterator:it];

    [match retain]; // for kIOMatchedNotification
    kr = IOServiceAddMatchingNotification(notifyport_,
                                          kIOMatchedNotification,
                                          (CFMutableDictionaryRef)(match),
                                          &observer_IONotification,
                                          self,
                                          &it);
    if (kr != kIOReturnSuccess) {
      NSLog(@"[ERROR] IOServiceAddMatchingNotification");
      return;
    }
    [self release_iterator:it];

    // ----------------------------------------------------------------------
    loopsource_ = IONotificationPortGetRunLoopSource(notifyport_);
    if (! loopsource_) {
      NSLog(@"[ERROR] IONotificationPortGetRunLoopSource");
      return;
    }
    CFRunLoopAddSource(CFRunLoopGetCurrent(), loopsource_, kCFRunLoopDefaultMode);
  }
}

// ------------------------------------------------------------
- (void) observer_NSWorkspaceDidWakeNotification:(NSNotification*)notification
{
  NSLog(@"observer_NSWorkspaceDidWakeNotification");
  [self setcallback:YES];
}

- (void) registerWakeNotification
{
  [[[NSWorkspace sharedWorkspace] notificationCenter] addObserver:self
                                                         selector:@selector(observer_NSWorkspaceDidWakeNotification:)
                                                             name:NSWorkspaceDidWakeNotification
                                                           object:nil];
}

- (void) unregisterWakeNotification
{
  [[[NSWorkspace sharedWorkspace] notificationCenter] removeObserver:self
                                                                name:NSWorkspaceDidWakeNotification
                                                              object:nil];
}

// ----------------------------------------
- (void) observer_NSWorkspaceSessionDidBecomeActiveNotification:(NSNotification*)notification
{
  NSLog(@"observer_NSWorkspaceSessionDidBecomeActiveNotification");
  [self registerIONotification];
  [self registerWakeNotification];
  [self setcallback:YES];
}

- (void) observer_NSWorkspaceSessionDidResignActiveNotification:(NSNotification*)notification
{
  NSLog(@"observer_NSWorkspaceSessionDidResignActiveNotification");
  [self unregisterIONotification];
  [self unregisterWakeNotification];
  [self setcallback:NO];
}

// ------------------------------------------------------------
- (void) applicationDidFinishLaunching:(NSNotification*)aNotification {
  [preferences_ load];

  global_client_ = client_;

  [self registerIONotification];
  [self registerWakeNotification];

  [[[NSWorkspace sharedWorkspace] notificationCenter] addObserver:self
                                                         selector:@selector(observer_NSWorkspaceSessionDidBecomeActiveNotification:)
                                                             name:NSWorkspaceSessionDidBecomeActiveNotification
                                                           object:nil];

  [[[NSWorkspace sharedWorkspace] notificationCenter] addObserver:self
                                                         selector:@selector(observer_NSWorkspaceSessionDidResignActiveNotification:)
                                                             name:NSWorkspaceSessionDidResignActiveNotification
                                                           object:nil];

  [self setcallback:YES];
}

- (void) applicationWillTerminate:(NSNotification*)aNotification {
  [self setcallback:NO];
}

- (BOOL) applicationShouldHandleReopen:(NSApplication*)theApplication hasVisibleWindows:(BOOL)flag
{
  [preferences_ show];
  return YES;
}

@end
