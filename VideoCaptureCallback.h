/*
 * Copyright (c) 2016, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#ifndef VIDEO_CAPTURE_CALLBACK_H
#define VIDEO_CAPTURE_CALLBACK_H

typedef void (*VideoCaptureCallback)(unsigned char* data, int len, VideoDevice* dev);

#endif // VIDEO_CAPTURE_CALLBACK_H

