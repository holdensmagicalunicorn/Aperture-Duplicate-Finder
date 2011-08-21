//
//  CApertureImage.m
//  ApertureDuplicateFinder
//
//  Created by Jonathan Wight on 8/20/11.
//  Copyright (c) 2011 toxicsoftware.com. All rights reserved.
//

#import "CApertureImage.h"

@implementation CApertureImage

@synthesize modelID;
@synthesize UUID;
@synthesize path;
@synthesize digest;
@synthesize type;
@synthesize versionUUIDs;

- (NSString *)description
    {
    return([NSString stringWithFormat:@"%@ (UUID: %@, Digest: %@)", [super description], self.UUID, self.digest]);
    }


@end
