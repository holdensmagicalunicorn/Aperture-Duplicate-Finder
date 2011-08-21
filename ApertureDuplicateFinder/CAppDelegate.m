//
//  CAppDelegate.m
//  ApertureDuplicateFinder
//
//  Created by Jonathan Wight on 8/17/11.
//  Copyright (c) 2011 toxicsoftware.com. All rights reserved.
//

#import "CAppDelegate.h"

#import "ApertureLibraryProcessor.h"

@interface CAppDelegate ()
@property (readwrite, nonatomic, retain) ApertureLibraryProcessor *processor;
@end

@implementation CAppDelegate

@synthesize window = _window;
@synthesize processor;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
    {
    // Insert code here to initialize your application
    NSURL *theLibraryURL = [[NSUserDefaults standardUserDefaults] URLForKey:@"LibraryURL"];
    if (theLibraryURL)
        {
        self.processor = [[ApertureLibraryProcessor alloc] initWithLibraryURL:theLibraryURL];
        }
    }

- (IBAction)choose:(id)inSender
    {
    NSOpenPanel *theOpenPanel = [NSOpenPanel openPanel];
    
    theOpenPanel.allowedFileTypes = [NSArray arrayWithObject:@"aplibrary"];

    [theOpenPanel beginSheetModalForWindow:self.window completionHandler:^(NSInteger result) {
        if (result != NSOKButton)
            {
            return;
            }

        self.processor = [[ApertureLibraryProcessor alloc] initWithLibraryURL:theOpenPanel.URL];

        
        [[NSUserDefaults standardUserDefaults] setURL:theOpenPanel.URL forKey:@"LibraryURL"];
        [[NSUserDefaults standardUserDefaults] synchronize];
        }];
    
    
    }


- (IBAction)start:(id)inSender
    {
    NSError *theError = NULL;
    BOOL theResult = [self.processor main:&theError];
    if (theResult == NO)
        {
        [self.window presentError:theError];
        }
    }


@end
