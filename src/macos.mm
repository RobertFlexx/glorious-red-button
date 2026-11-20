#include "reasons.hpp"

#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

#include <string>

@interface JokeDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@property(nonatomic, strong) NSWindow* window;
@property(nonatomic, strong) NSTextField* bait;
@property(nonatomic, strong) NSButton* button;
@property(nonatomic, strong) NSTextField* result;
@end

@implementation JokeDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)notification {
    (void)notification;
    NSRect bounds = NSMakeRect(0, 0, prank::window_width, prank::window_height);
    self.window = [[NSWindow alloc] initWithContentRect:bounds
        styleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable)
        backing:NSBackingStoreBuffered defer:NO];
    [self.window setTitle:@"Totally Legit"];
    [self.window center];
    [self.window setDelegate:self];
    [self.window setReleasedWhenClosed:NO];
    [self.window setBackgroundColor:[NSColor whiteColor]];

    const std::string reason = prank::random_reason();
    NSString* text = [NSString stringWithUTF8String:reason.c_str()];
    self.bait = [NSTextField labelWithString:text];
    self.bait.frame = NSMakeRect(20, 166, 410, 44);
    self.bait.alignment = NSTextAlignmentCenter;
    self.bait.font = [NSFont systemFontOfSize:18];
    self.bait.textColor = [NSColor colorWithCalibratedRed:0.15 green:0.17 blue:0.21 alpha:1];
    [[self.window contentView] addSubview:self.bait];

    self.button = [[NSButton alloc] initWithFrame:NSMakeRect(105, 63, 240, 72)];
    self.button.title = @"PRESS ME";
    self.button.bordered = NO;
    self.button.font = [NSFont boldSystemFontOfSize:20];
    self.button.wantsLayer = YES;
    self.button.layer.backgroundColor = [NSColor colorWithCalibratedRed:0.902
        green:0.224 blue:0.275 alpha:1].CGColor;
    self.button.layer.cornerRadius = 12;
    self.button.attributedTitle = [[NSAttributedString alloc]
        initWithString:@"PRESS ME" attributes:@{
            NSForegroundColorAttributeName: NSColor.whiteColor,
            NSFontAttributeName: [NSFont boldSystemFontOfSize:20]
        }];
    self.button.target = self;
    self.button.action = @selector(pressed:);
    self.button.keyEquivalent = @"\r";
    [[self.window contentView] addSubview:self.button];

    NSString* punchline = [NSString stringWithUTF8String:prank::punchline];
    self.result = [NSTextField labelWithString:punchline];
    self.result.frame = NSMakeRect(20, 96, 410, 72);
    self.result.alignment = NSTextAlignmentCenter;
    self.result.font = [NSFont boldSystemFontOfSize:27];
    self.result.textColor = [NSColor colorWithCalibratedRed:0.902 green:0.224
        blue:0.275 alpha:1];
    self.result.hidden = YES;
    [[self.window contentView] addSubview:self.result];

    [self.window makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
}

- (void)pressed:(id)sender {
    (void)sender;
    if (!self.result.hidden) return;
    self.bait.hidden = YES;
    self.button.hidden = YES;
    self.result.hidden = NO;
    [self performSelector:@selector(finishJoke) withObject:nil
              afterDelay:prank::punchline_ms / 1000.0];
}

- (void)finishJoke {
    [NSApp terminate:nil];
}

- (void)windowWillClose:(NSNotification*)notification {
    (void)notification;
    [NSApp terminate:nil];
}
@end

int main() {
    @autoreleasepool {
        NSApplication* app = [NSApplication sharedApplication];
        [app setActivationPolicy:NSApplicationActivationPolicyRegular];
        JokeDelegate* delegate = [[JokeDelegate alloc] init];
        app.delegate = delegate;
        [app run];
    }
    return 0;
}
