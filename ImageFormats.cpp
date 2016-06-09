/*
 * Copyright (c) 2016, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#include <QByteArray>

#include "ImageFormats.h"

QImage simpleQimage(const unsigned char* data, unsigned len,
                    unsigned width, unsigned height, QImage::Format format) {
	return QImage(data, width, height, format);
}

QImage makeARGB32Image(const unsigned char* data, unsigned len,
                       unsigned width, unsigned height) {
	return simpleQimage(data, len, width, height, QImage::Format_ARGB32).rgbSwapped();
}

QImage makeRGB32Image(const unsigned char* data, unsigned len,
                      unsigned width, unsigned height) {
	return simpleQimage(data, len, width, height, QImage::Format_RGB32).rgbSwapped();
}

QImage makeRGB24Image(const unsigned char* data, unsigned len,
                      unsigned width, unsigned height) {
	return simpleQimage(data, len, width, height, QImage::Format_RGB888).rgbSwapped();
}

QImage makeRGB555Image(const unsigned char* data, unsigned len,
                       unsigned width, unsigned height) {
	return simpleQimage(data, len, width, height, QImage::Format_RGB555).rgbSwapped();
}

QImage makeMjpgImage(const unsigned char* data, unsigned len,
                     unsigned width, unsigned height) {
	QByteArray bytes(reinterpret_cast<const char*>(data), len);
	QImage image;
	image.loadFromData(bytes, "JPEG");
	return image;
}