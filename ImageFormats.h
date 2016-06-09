/*
 * Copyright (c) 2016, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#ifndef IMAGE_FORMATS_H
#define IMAGE_FORMATS_H

#include <QImage>
#include <string>
#include <functional>
#include <dshow.h>

typedef std::function<QImage(const unsigned char* data, unsigned len, unsigned width, unsigned height)> QImageMaker;

struct ImageFormatRow {
	std::string name;
	GUID directshowFormat;
	QImageMaker makeQImage;
};

QImage makeARGB32Image(const unsigned char* data, unsigned len, unsigned width, unsigned height);
QImage makeRGB32Image(const unsigned char* data, unsigned len, unsigned width, unsigned height);
QImage makeRGB24Image(const unsigned char* data, unsigned len, unsigned width, unsigned height);
QImage makeRGB555Image(const unsigned char* data, unsigned len, unsigned width, unsigned height);
QImage makeMjpgImage(const unsigned char* data, unsigned len, unsigned width, unsigned height);

static ImageFormatRow ImageFormatTable[] = {
		{"ARGB32", MEDIASUBTYPE_ARGB32, makeARGB32Image},
		{"RGB32", MEDIASUBTYPE_RGB32, makeRGB32Image},
		{"RGB24", MEDIASUBTYPE_RGB24, makeRGB24Image},
		{"RGB555", MEDIASUBTYPE_RGB555, makeRGB555Image},
		{"MJPG", MEDIASUBTYPE_MJPG, makeMjpgImage}
};

#endif // IMAGE_FORMATS_H

