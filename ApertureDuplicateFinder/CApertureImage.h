//
//  CApertureImage.h
//  ApertureDuplicateFinder
//
//  Created by Jonathan Wight on 8/20/11.
//  Copyright (c) 2011 toxicsoftware.com. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface CApertureImage : NSObject

@property (readwrite, nonatomic, copy) NSString *modelID;
@property (readwrite, nonatomic, copy) NSString *UUID;
@property (readwrite, nonatomic, copy) NSString *path;
@property (readwrite, nonatomic, copy) NSData *digest;
@property (readwrite, nonatomic, copy) NSString *type;
@property (readwrite, nonatomic, copy) NSArray *versionUUIDs;


@end
