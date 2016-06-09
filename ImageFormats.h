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

/**
 * @brief Interface for a function that makes QImage from raw data
 *
 * @param data
 * @param len
 * @param width
 * @param height
 * @return QImage
 */
typedef std::function<QImage(const unsigned char* data, unsigned len, unsigned width, unsigned height)> QImageMaker;

/**
 * @brief Description for image format table's row
 *
 * @param name
 * @param directshowFormat
 * @param makeQImage
 */
struct ImageFormatRow {
    /**
     * @brief Frendly format name
     */
    std::string name;

    /**
     * @brief DirectShow format GUID
     */
    GUID directshowFormat;

    /**
     * @brief Function that makes QImage from raw data
     */
    QImageMaker makeQImage;
};

/**
 * @brief Make QImage from ARGB32 frame
 *
 * @param data
 * @param len
 * @param width
 * @param height
 * @return QImage
 */
QImage makeARGB32Image(const unsigned char* data, unsigned len, unsigned width, unsigned height);

/**
 * @brief Make QImage from RGB32 frame
 *
 * @param data
 * @param len
 * @param width
 * @param height
 * @return QImage
 */
QImage makeRGB32Image(const unsigned char* data, unsigned len, unsigned width, unsigned height);

/**
 * @brief Make QImage from RGB24 frame
 *
 * @param data
 * @param len
 * @param width
 * @param height
 * @return QImage
 */
QImage makeRGB24Image(const unsigned char* data, unsigned len, unsigned width, unsigned height);

/**
 * @brief Make QImage from RGB555 frame
 *
 * @param data
 * @param len
 * @param width
 * @param height
 * @return QImage
 */
QImage makeRGB555Image(const unsigned char* data, unsigned len, unsigned width, unsigned height);

/**
 * @brief Make QImage from RGB565 frame
 *
 * @param data
 * @param len
 * @param width
 * @param height
 * @return QImage
 */
QImage makeRGB565Image(const unsigned char* data, unsigned len, unsigned width, unsigned height);

/**
 * @brief Make QImage from JPEG frame
 *
 * @param data
 * @param len
 * @param width
 * @param height
 * @return QImage
 */
QImage makeJPGImage(const unsigned char* data, unsigned len, unsigned width, unsigned height);

/**
 * @brief A dummy function which returns empty QImage
 *
 * @param data
 * @param len
 * @param width
 * @param height
 * @return QImage
 */
QImage dummy(const unsigned char* data, unsigned len, unsigned width, unsigned height);

/**
 * @brief Image format table
 */
static ImageFormatRow ImageFormatTable[] = {
    {"ARGB32", MEDIASUBTYPE_ARGB32, makeARGB32Image},
    {"RGB32", MEDIASUBTYPE_RGB32, makeRGB32Image},
    {"RGB24", MEDIASUBTYPE_RGB24, makeRGB24Image},
    {"RGB555", MEDIASUBTYPE_RGB555, makeRGB555Image},
    {"RGB565", MEDIASUBTYPE_RGB565, makeRGB565Image},
    {"MJPG", MEDIASUBTYPE_MJPG, makeJPGImage},
    {"JPG", MEDIASUBTYPE_IJPG, makeJPGImage},
    {"AYUV", MEDIASUBTYPE_AYUV, dummy},
    {"IYUV", MEDIASUBTYPE_IYUV, dummy},
    {"YV12", MEDIASUBTYPE_YV12, dummy},
    {"UYVY", MEDIASUBTYPE_UYVY, dummy},
    {"YUYV", MEDIASUBTYPE_YUYV, dummy},
    {"YUY2", MEDIASUBTYPE_YUY2, dummy},
    {"NV12", MEDIASUBTYPE_NV12, dummy},
    {"IMC1", MEDIASUBTYPE_IMC1, dummy},
    {"IMC2", MEDIASUBTYPE_IMC2, dummy},
    {"IMC3", MEDIASUBTYPE_IMC3, dummy},
    {"IMC4", MEDIASUBTYPE_IMC4, dummy}
};

#endif // IMAGE_FORMATS_H

