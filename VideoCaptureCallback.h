/*
 * Copyright (c) 2016, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#ifndef VIDEO_CAPTURE_CALLBACK_H
#define VIDEO_CAPTURE_CALLBACK_H

#include <functional>

class VideoDevice;

/**
 * @brief An interface for a function that process video frames
 *
 * @param data
 * @param len
 * @param dev
 */
typedef std::function<void(unsigned char* data, int len, VideoDevice* dev)> VideoCaptureCallback;

#endif // VIDEO_CAPTURE_CALLBACK_H
