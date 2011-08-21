//
//  ApertureLibraryProcessor.h
//  ApertureDuplicateFinder
//
//  Created by Jonathan Wight on 8/17/11.
//  Copyright (c) 2011 toxicsoftware.com. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface ApertureLibraryProcessor : NSObject

@property (readwrite, nonatomic, strong) NSURL *libraryURL;

@property (readonly, nonatomic, assign) BOOL processing;
@property (readonly, nonatomic, assign) NSUInteger numberOfImages;
@property (readonly, nonatomic, assign) NSUInteger numberOfProcessedImages;
@property (readonly, nonatomic, assign) CFAbsoluteTime start;
@property (readonly, nonatomic, assign) CFAbsoluteTime finish;
@property (readonly, nonatomic, assign) CFAbsoluteTime estimatedTimeRemaining;

- (id)initWithLibraryURL:(NSURL *)inLibraryURL;

- (BOOL)main:(NSError **)outError;

@end
