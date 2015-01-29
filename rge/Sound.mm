//
//  Sound.cpp
//  rge_sample
//
//  Created by Yuhei Akamine on 2015/01/28.
//  Copyright (c) 2015年 Yuhei Akamine. All rights reserved.
//

#include "Sound.h"

#import <Cocoa/Cocoa.h>

static NSSound *appear=0;
static NSSound *disappear=0;


void audio_init()
{
    appear = [[NSSound alloc] initWithContentsOfFile:@"/Users/125766J/Downloads/rge1.04/rge/magic-stick1.mp3" byReference:YES];
    disappear = [[NSSound alloc] initWithContentsOfFile:@"/Users/125766J/Downloads/rge1.04/rge/heal02.mp3" byReference:YES];
}

void play_appear()
{
    [appear play];
}

void play_disappear()
{
    [disappear play];
}