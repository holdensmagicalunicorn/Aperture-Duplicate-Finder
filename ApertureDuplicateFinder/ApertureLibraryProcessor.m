//
//  ApertureLibraryProcessor.m
//  ApertureDuplicateFinder
//
//  Created by Jonathan Wight on 8/17/11.
//  Copyright (c) 2011 toxicsoftware.com. All rights reserved.
//

#import "ApertureLibraryProcessor.h"

#import <CommonCrypto/CommonDigest.h>
#import <CoreServices/CoreServices.h>

#import "TouchSQL.h"
#import "CApertureImage.h"
#import "Aperture.h"

@interface ApertureLibraryProcessor ()
@property (readwrite, nonatomic, assign) BOOL processing;
@property (readwrite, nonatomic, assign) NSUInteger numberOfImages;
@property (readwrite, nonatomic, assign) NSUInteger numberOfProcessedImages;
@property (readwrite, nonatomic, assign) CFAbsoluteTime start;
@property (readwrite, nonatomic, assign) CFAbsoluteTime finish;
@property (readwrite, nonatomic, assign) CFAbsoluteTime estimatedTimeRemaining;

@property (readwrite, nonatomic, strong) CSqliteDatabase *database;
@property (readwrite, nonatomic, assign) CGColorSpaceRef colorSpace;
@property (readwrite, nonatomic, strong) NSMutableDictionary *imagesByUUID;

- (BOOL)updateDigests:(NSError **)outError;
- (BOOL)searchForDuplicates:(NSError **)outError;

- (NSData *)digestForImage:(CGImageRef)inImage;
@end

#pragma mark -

@implementation ApertureLibraryProcessor

@synthesize libraryURL;
@synthesize processing;
@synthesize numberOfImages;
@synthesize numberOfProcessedImages;
@synthesize start;
@synthesize finish;
@synthesize estimatedTimeRemaining;

@synthesize database;
@synthesize colorSpace;
@synthesize imagesByUUID;

- (id)initWithLibraryURL:(NSURL *)inLibraryURL
    {
    if ((self = [super init]) != NULL)
        {
        libraryURL = inLibraryURL;
        imagesByUUID = [[NSMutableDictionary alloc] init];
        
        
        NSURL *theDatabaseURL = [self.libraryURL URLByAppendingPathComponent:@"Database/Library.apdb"];
        
        // Open the aperture library
        NSError *theError = NULL;
        database = [[CSqliteDatabase alloc] initWithURL:theDatabaseURL flags:SQLITE_OPEN_READONLY error:&theError];
        if (database == NULL)
            {
            NSLog(@"COULD NOT OPEN DB: %@", theError);
            self = NULL;
            return(self);
            }

        colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);

        }
    return self;
    }

- (void)dealloc
    {
    }

- (void)setNumberOfProcessedImages:(NSUInteger)inNumberOfProcessedImages
    {
    numberOfProcessedImages = inNumberOfProcessedImages;

    CFAbsoluteTime theCurrent = CFAbsoluteTimeGetCurrent();
    CFAbsoluteTime theTimePerImage = (theCurrent - self.start) / (CFAbsoluteTime)numberOfProcessedImages;

    self.estimatedTimeRemaining = (self.numberOfImages - numberOfProcessedImages) * theTimePerImage;
    }

- (BOOL)main:(NSError **)outError
    {
    ApertureApplication *theApplication = [SBApplication applicationWithBundleIdentifier:@"com.apple.Aperture"];
    if (theApplication.isRunning == YES)
        {
        [theApplication quitSaving:ApertureSaveOptionsNo];
        }



    self.start = CFAbsoluteTimeGetCurrent();
    
    self.processing = YES;
    
    NSURL *theMastersURL = [self.libraryURL URLByAppendingPathComponent:@"Masters"];
    
    dispatch_group_t theGroup = dispatch_group_create();
    dispatch_queue_t theSerialQueue = dispatch_queue_create("com.toxicsoftware.some_queue", DISPATCH_QUEUE_SERIAL);
    dispatch_queue_t theGlobalQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    
    dispatch_group_async(theGroup, theGlobalQueue, ^{
        
        // Get all images from the Aperture Library
        NSString *theStatementString = @"SELECT * FROM RKMaster";
//        NSString *theStatementString = @"SELECT * FROM RKMaster LIMIT 100";
        CSqliteStatement *theStatement = [self.database statementWithString:theStatementString];
        [theStatement enumerateObjectsWithOptions:NSEnumerationConcurrent usingBlock:^(CSqliteRow *row, NSUInteger idx, BOOL *stop) {
            
            NSString *thePath = [row objectForKey:@"imagePath"];
            
            NSURL *theImageURL = [theMastersURL URLByAppendingPathComponent:thePath];
            NSError *theError = NULL;
            if ([theImageURL checkResourceIsReachableAndReturnError:&theError] == NO)
                {
                return;
                }

            // Get the UT type and make sure it conforms to public.image...
            NSString *theType = (__bridge_transfer NSString *)UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension, (__bridge CFStringRef)[theImageURL pathExtension], NULL);
            if (UTTypeConformsTo((__bridge CFStringRef)theType, kUTTypeImage) == NO)
                {
                return;
                }
            
            NSString *theUUID = [row objectForKey:@"uuid"];
            
            dispatch_group_async(theGroup, theSerialQueue, ^{
                CApertureImage *theApertureImage = [self.imagesByUUID objectForKey:theUUID];
                if (theApertureImage == NULL)
                    {
                    theApertureImage = [[CApertureImage alloc] init];
                    theApertureImage.modelID = [row objectForKey:@"modelId"];
                    theApertureImage.UUID = theUUID;
                    theApertureImage.path = thePath;
                    theApertureImage.type = theType;
//                    theApertureImage.originalVersionUUID = [row objectForKey:@"originalVersionUuid"];
                    [self.imagesByUUID setObject:theApertureImage forKey:theUUID];
                    }
                });
            
            }];
        });
        
    dispatch_group_notify(theGroup, dispatch_get_main_queue(), ^{
    
        dispatch_release(theGroup);
        dispatch_release(theSerialQueue);
    
        NSLog(@"DONE");
        
        [self updateDigests:NULL];
    
        });
        
     
    return(YES);
    }
    
- (BOOL)updateDigests:(NSError **)outError
    {
    NSURL *theMastersURL = [self.libraryURL URLByAppendingPathComponent:@"Masters"];

    // Find images that have no digest yet.    
    NSArray *theImages = [self.imagesByUUID allValues];
    theImages = [theImages sortedArrayUsingDescriptors:[NSArray arrayWithObject:[[NSSortDescriptor alloc] initWithKey:@"UUID" ascending:YES]]];
    theImages = [theImages filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"digest == NULL"]];
    
    // Set our properties (so bound UI can update)...
    self.numberOfImages = [theImages count];
    self.numberOfProcessedImages = 0;

    // Prepare for warp speed...
    dispatch_group_t theGroup = dispatch_group_create();

    dispatch_queue_t theGlobalQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    
    // All IO (hopefully) occurs on a serial queue...
    dispatch_queue_t theIOQueue = dispatch_queue_create("com.toxicsoftware.image_io", DISPATCH_QUEUE_SERIAL);

    // We only want to limit the number of background operations to some multiple of # of CPUs.
    // See http://www.mikeash.com/pyblog/friday-qa-2009-09-25-gcd-practicum.html for more information.
    const NSUInteger theCPUCount = [NSProcessInfo processInfo].processorCount;
    const NSUInteger kCPUMultiplier = 2;
    dispatch_semaphore_t theSemaphore = dispatch_semaphore_create(theCPUCount * kCPUMultiplier);

    dispatch_group_async(theGroup, dispatch_get_main_queue(), ^{

        // Enumerate through all aperture images...
        [theImages enumerateObjectsWithOptions:0 usingBlock:^(id obj, NSUInteger idx, BOOL *stop) {

            CApertureImage *theApertureImage = obj;
            
            // Let's do some work on the (serial) IO ueue...
            dispatch_group_async(theGroup, theIOQueue, ^{
            
                NSURL *theImageURL = [theMastersURL URLByAppendingPathComponent:theApertureImage.path];
                
                // We wont use these images again - so don't cache, and we have the UT type already so let's hint CoreImage...
                NSMutableDictionary *theCreationOptions = [NSMutableDictionary dictionaryWithObjectsAndKeys:
                    [NSNumber numberWithBool:NO], kCGImageSourceShouldCache,
                    theApertureImage.type, kCGImageSourceTypeIdentifierHint,
                    NULL];
                
                // Load 'em up!...
                CGImageSourceRef theSourceRef = CGImageSourceCreateWithURL((__bridge CFURLRef)theImageURL, (__bridge CFDictionaryRef)theCreationOptions);
                CGImageRef theImage = CGImageSourceCreateImageAtIndex(theSourceRef, 0, (__bridge CFDictionaryRef)theCreationOptions);
                CFRelease(theSourceRef);

                // If we don't load an image we bail early (but still increment # of processed images so UI & timing are correct)...
                if (theImage == NULL)
                    {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        self.numberOfProcessedImages++;
                        });
                    return;
                    }

                // Wait for the next available resource (in this case resources are (portions of) CPUs)...
                dispatch_semaphore_wait(theSemaphore, DISPATCH_TIME_FOREVER);

                // Produce the digest in the background...
                dispatch_group_async(theGroup, theGlobalQueue, ^{

                    theApertureImage.digest = [self digestForImage:theImage];

                    // We don't need the image any more...
                    CFRelease(theImage);
                    
                    // Update number of processed images (in main thread so bound UI wont bork)
                    dispatch_async(dispatch_get_main_queue(), ^{
                        self.numberOfProcessedImages++;
                        });

                    // We are done. Signal that we're done with this resource...
                    dispatch_semaphore_signal(theSemaphore);
                    });

                });
            }];

        });

    // We need to disp
    dispatch_group_notify(theGroup, dispatch_get_main_queue(), ^{

        dispatch_release(theGroup);
        dispatch_release(theIOQueue);
        dispatch_release(theSemaphore);

        self.processing = NO;
        self.finish = CFAbsoluteTimeGetCurrent();
        NSLog(@"TOTAL: %f", self.finish - self.start);
        
        [self searchForDuplicates:NULL];
        
        });

    return(YES);
    }

#pragma mark -

- (BOOL)searchForDuplicates:(NSError **)outError
    {
    NSMutableDictionary *theImagesByDigest = [NSMutableDictionary dictionary];

    NSPredicate *thePredicate = [NSPredicate predicateWithFormat:@"digest != NULL"];

    NSArray *theImagesWithDigest = [[self.imagesByUUID allValues] filteredArrayUsingPredicate:thePredicate];

    for (CApertureImage *theImage in theImagesWithDigest)
        {
        NSData *theDigest = theImage.digest;
        
        NSMutableArray *theImages = [theImagesByDigest objectForKey:theDigest];
        if (theImages == NULL)
            {
            theImages = [NSMutableSet set];
            [theImagesByDigest setObject:theImages forKey:theDigest];
            }
        [theImages addObject:theImage];
        }

        
                    
    NSMutableSet *theDuplicateDigests = [NSMutableSet set];
    
    [theImagesByDigest enumerateKeysAndObjectsUsingBlock:^(id key, id obj, BOOL *stop) {
        if ([obj count] > 1)
            {
            [theDuplicateDigests addObject:key];
            for (CApertureImage *theImage in obj)
                {
                CSqliteStatement *theStatement = [self.database statementWithFormat:@"SELECT * FROM RKVersion WHERE masterId == '%@' ORDER BY 'versionNumber'", theImage.modelID];
                NSError *theError = NULL;
                NSArray *theRows = [theStatement rows:&theError];

                theImage.versionUUIDs = [theRows valueForKey:@"uuid"];
                }
            
            }
        }];
    NSLog(@"%lu", [theDuplicateDigests count]);

    self.database = NULL;

    ApertureApplication *theApplication = [SBApplication applicationWithBundleIdentifier:@"com.apple.Aperture"];
    if (theApplication.isRunning == NO)
        {
        [theApplication activate];
        }

    [theImagesByDigest enumerateKeysAndObjectsUsingBlock:^(id key, id obj, BOOL *stop) {
        if ([obj count] > 1)
            {
            for (CApertureImage *theImage in obj)
                {
                for (NSString *theVersionUUID in theImage.versionUUIDs)
                    {
                    ApertureImageVersion *theVersion = [theApplication.imageVersions objectWithID:theVersionUUID];
                    NSLog(@"SETTING");
                    theVersion = [theVersion get];
                    NSDictionary *theProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                        @"x-duplicate", @"name",
                        NULL];
                    ApertureKeyword *theKeyword = [[[theApplication classForScriptingClass:@"keyword"] alloc] initWithProperties:theProperties];
                    [theVersion.keywords addObject:theKeyword];
                    }
                }
            }
        }];




//

    

    return(YES);
    }


#pragma mark -

- (NSData *)digestForImage:(CGImageRef)inImage
    {
    const CGFloat theWidth = CGImageGetWidth(inImage);
    const CGFloat theHeight = CGImageGetHeight(inImage);
    const size_t bitmapBytesPerRow = theWidth * 4;
    const size_t theBitmapLength = bitmapBytesPerRow * theHeight;

    NSMutableData *theImageBuffer = [NSMutableData dataWithLength:theBitmapLength];
    
    CGColorSpaceRef theColorSpace = self.colorSpace;

    CGContextRef theContext = CGBitmapContextCreate([theImageBuffer mutableBytes], theWidth, theHeight, 8, bitmapBytesPerRow, theColorSpace, kCGImageAlphaPremultipliedLast);
    
    CGContextDrawImage(theContext, (CGRect){ .size = { theWidth, theHeight }}, inImage);

    CFRelease(theContext);
    
    NSMutableData *theDigestData = [NSMutableData dataWithLength:CC_MD5_DIGEST_LENGTH];
    CC_MD5([theImageBuffer bytes], (CC_LONG)theBitmapLength, [theDigestData mutableBytes]);
    
    
    return(theDigestData);
    }

@end
