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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with SimpleX3D. If not, see <http://www.gnu.org/licenses/>.
*/
 
#include "internal/SF.h"
 
namespace X3D {
 
#define COLOR2CHAR(x) ((unsigned char) ((x) * 255))
#define CHAR2COLOR(x) (((float) x) / 255)
 
SFImage& SFImage::operator=(const SFImage& i) {
    if (i.width != width || i.height != height || i.components != components)
        throw X3DError("mismatched image properties in =");
    setBytes(i.bytes);
}
 
bool SFImage::operator==(const SFImage& i) const {
    if (i.width != width || i.height != height || i.components != components)
        return false;
    return 0 == memcmp(bytes, i.bytes, size);
}
 
bool SFImage::operator!=(const SFImage& i) const {
    if (i.width != width || i.height != height || i.components != components)
        return true;
    return 0 != memcmp(bytes, i.bytes, size);
}

SFImage& SFImage::operator()(const X3DField& f) {
    return *this = unwrap(f);
}
 
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
 
void SFImage::realloc(int width, int height, int components) {
    if (bytes != NULL)
        delete[] bytes;
    alloc(width, height, components);
}
 
SFImage::SFImage(int width, int height, int components, unsigned char* bytes) {
    if (bytes == NULL)
        throw X3DError("tried to assign NULL bytes");
    alloc(width, height, components);
    setBytes(bytes);
}
 
SFImage::~SFImage() {
    if (bytes != NULL)
        delete[] bytes;
}
 
SFImage& SFImage::setBytes(const unsigned char* array) {
    if (size) {
        if (array == NULL)
            throw X3DError("tried to assign NULL bytes");
        memcpy(this->bytes, array, size);
    }
    return *this;
}
 
unsigned int SFImage::getPixel(int x, int y) const {
    const unsigned char* ptr = locate(x, y);
    unsigned int pixel = 0;
    for (int i = 0; i < components; i++)
        pixel = (pixel << 8) | *ptr++;
    return pixel;
}
 
void SFImage::setPixel(int x, int y, unsigned int pixel) {
    unsigned char* ptr = locate(x, y) + components;
        for (int i = 0; i < components; i++) {
            *--ptr = pixel & 255;
        pixel >>= 8;
    }
}
 
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
 
bool SFImage::parse(istream& is) {
    int width, height, components;
    is >> width >> height >> components;
    if (is.fail())
        return false;
    unsigned int max;
    if (components < 4)
        max = (2 << (components * 8)) - 1;
    else
        max = 0xffffffff;
    SFImage image(width, height, components);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            unsigned int pixel;
            is >> pixel >> std::ws;
            if (is.fail())
                return false;
            if (!pixel && is.peek() == 'x') {
                is.get();
                is >> std::hex >> pixel >> std::dec;
                if (is.fail())
                    return false;
            }
            if (pixel > max)
                return false;
            image.setPixel(x, y, pixel);
        }
    }
    realloc(width, height, components);
    *this = image;
    return true;
}

void SFImage::print(ostream& os) const {
    os << width << ' ' << height << ' ' << components;
    os << std::hex;
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            os << ' ' << getPixel(x,y);
    os << std::dec;
}
 
}
