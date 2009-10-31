/*
 * Copyright 2009 Nathan Matthews <lowentropy@gmail.com>
 *
 * This file is part of SimpleX3D.
 *
 * SimpleX3D is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SimpleX3D is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SimpleX3D.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "internal/types.h"

using namespace X3D;

#define COLOR2CHAR(x) ((unsigned char) ((x) * 255))
#define CHAR2COLOR(x) (((float) x) / 255)

/**
 * Equals operator.
 *
 * @param i the image to copy
 */
const SFImage& SFImage::operator=(const SFImage& i) {
	if (i.width != width || i.height != height || i.components != components)
		throw X3DError("mismatched image properties in =");
	setBytes(i.bytes);
}

/**
 * Comparison operator.
 * 
 * @param i image to compare to
 * @returns if images are equal
 */
bool SFImage::operator==(const SFImage& i) const {
    if (i.width != width || i.height != height || i.components != components)
        return false;
    return 0 == memcmp(bytes, i.bytes, size);
}

/**
 * Comparison operator.
 * 
 * @param i image to compare to
 * @returns if images are not equal
 */
bool SFImage::operator!=(const SFImage& i) const {
    if (i.width != width || i.height != height || i.components != components)
        return true;
    return 0 != memcmp(bytes, i.bytes, size);
}

/**
 * Empty image constructor.
 * 
 * The total size of the image will be
 * #width * #height * #components. If all
 * three parameters are zero, the bytes
 * array will be NULL.

 * @param width image width
 * @param height image height
 * @param components image depth (0-4)
 */
SFImage::SFImage(int width, int height, int components) {
	alloc(width, height, components);
}

void SFImage::alloc(int width, int height, int components) {
	if (width < 0 || height < 0)
		throw X3DError("size cannot be negative");
	if (components < 0 || components > 4)
		throw X3DError("components must be in range [0,4]");
	if (components == 0 && (width !=0 || height != 0))
		throw X3DError("for non-empty image, components must be in range [1,4]");
	this->width = width;
	this->height = height;
	this->components = components;
	this->size = width * height * components;
	bytes = this->size ? new unsigned char[size] : NULL;
}

/**
 * Raw copy constructor.
 * 
 * This constructor creates a blank image as in SFImage(),
 * then copies image bytes directly between the #bytes pointers.
 * 
 * @param width image width
 * @param height image height
 * @param components image depth (0-4)
 * @param bytes array to copy data from
 */
SFImage::SFImage(int width, int height, int components, unsigned char* bytes) {
	alloc(width, height, components);
	setBytes(bytes);
}

/**
 * Image destructor. If bytes is not NULL,
 * it is freed.
 */
SFImage::~SFImage() {
	if (bytes != NULL)
		delete[] bytes;
}

/**
 * Direct memory assignment.
 * 
 * Replaces the image bytes with a COPY of the input array.
 * 
 * @param array array of bytes to copy
 * @returns reference to this
 */
SFImage& SFImage::setBytes(const unsigned char* array) {
	if (size) {
		if (array == NULL)
			throw X3DError("tried to assign NULL");
		memcpy(this->bytes, array, size);
	}
	return *this;
}

/**
 * Extract a raw pixel value.
 * 
 * The value returned is an unsigned int, but will only
 * have a number of lower bytes set equal to #components.
 * The high bytes are always set to zero.
 * 
 * @param x X coordinate
 * @param y Y coordinate
 * @returns raw (packed) pixel
 * @see setPixel
 */
unsigned int SFImage::getPixel(int x, int y) const {
	const unsigned char* ptr = locate(x, y);
	unsigned int pixel = 0;
	for (int i = 0; i < components; i++)
		pixel = (pixel << 8) | *ptr++;
	return pixel;
}

/**
 * Inject a raw pixel value.
 * 
 * The unsigned int argument should only have a number of lower
 * bytes set equal to #components. The upper bytes will be ignored.
 * 
 * @param x X coordinate
 * @param y Y coordinate
 * @param pixel packed pixel value
 * @see getPixel
 */
void SFImage::setPixel(int x, int y, unsigned int pixel) {
	unsigned char* ptr = locate(x, y) + components;
	for (int i = 0; i < components; i++) {
		*--ptr = pixel & 255;
		pixel >>= 8;
	}
}

/**
 * Retrieve a pixel's RGB color.
 * 
 * The alpha channel, if present, is not returned. If the image
 * is grayscale, all three color channels are set equal to the
 * grayscale value.
 * 
 * @param x X coordinate
 * @param y Y coordinate
 * @returns RGB color for pixel
 * @see setColor
 */
SFColor SFImage::getColor(int x, int y) const {
	SFColor c;
	const unsigned char* ptr = locate(x, y);
	if (components < 3) {
		c.r = c.g = c.b = CHAR2COLOR(*ptr);
	} else {
		c.r = CHAR2COLOR(*ptr++);
		c.g = CHAR2COLOR(*ptr++);
		c.b = CHAR2COLOR(*ptr);
	}
	return c;
}

/**
 * Updates a pixel's RGB color.
 * 
 * If the image has an alpha channel, it is not changed. If the image
 * is grayscale, then the updated value will be the average intensity
 * of the given color.
 * 
 * @param x X coordinate
 * @param y Y coordinate
 * @param c RGB color to set
 * @see getColor
 */
void SFImage::setColor(int x, int y, const SFColor c) {
	unsigned char* ptr = locate(x, y);
	if (components < 3) {
		*ptr = COLOR2CHAR((c.r + c.g + c.b) / 3);
	} else {
		*ptr++ = COLOR2CHAR(c.r);
		*ptr++ = COLOR2CHAR(c.g);
		*ptr++ = COLOR2CHAR(c.b);
	}
}

/**
 * Retrieve a pixel's RGBA color.
 * 
 * If the image does not have an alpha channel, the returned alpha will
 * be 255 (opaque). If the image is grayscale, all three color channels
 * will be set equal to the intensity value.
 * 
 * @param x X coordinate
 * @param y Y coordinate
 * @returns RGBA color for pixel
 * @see setColorRGBA
 */
SFColorRGBA SFImage::getColorRGBA(int x, int y) const {
	SFColorRGBA c;
	const unsigned char* ptr = locate(x, y);
	if (components < 3) {
		c.r = c.g = c.b = CHAR2COLOR(*ptr++);
	} else {
		c.r = CHAR2COLOR(*ptr++);
		c.g = CHAR2COLOR(*ptr++);
		c.b = CHAR2COLOR(*ptr++);
	}
	c.a = (components % 2 == 0) ? CHAR2COLOR(*ptr) : 1;
	return c;
}

/**
 * Update a pixel's RGBA color.
 * 
 * If the image does not have an alpha channel, the alpha color value
 * is ignored. If the image is grayscale, the updated value will be
 * the average intensity of the color.
 * 
 * @param x X coordinate
 * @param y Y coordinate
 * @param c RGBA color to set
 * @see getColorRGBA
 */
void SFImage::setColorRGBA(int x, int y, const SFColorRGBA c) {
	unsigned char* ptr = locate(x, y);
	if (components < 3) {
		*ptr++ = COLOR2CHAR((c.r + c.g + c.b) / 3);
	} else {
		*ptr++ = COLOR2CHAR(c.r);
		*ptr++ = COLOR2CHAR(c.g);
		*ptr++ = COLOR2CHAR(c.b);
	}
	if (components % 2 == 0)
		*ptr = COLOR2CHAR(c.a);
}

// PRIVATE STUFF HERE

unsigned char* SFImage::locate(int x, int y) {
	int index = (y * width + x) * components;
	if (index < 0 || index >= size)
		throw X3DError("coordinates out of bounds");
	return &bytes[index];
}

const unsigned char* SFImage::locate(int x, int y) const {
	int index = (y * width + x) * components;
	if (index < 0 || index >= size)
		throw X3DError("coordinates out of bounds");
	return &bytes[index];
}
