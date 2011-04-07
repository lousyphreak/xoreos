/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/images/dumptga.cpp
 *  A simple TGA image dumper.
 */

#include <cstdio>
#include <cstring>

#include "common/error.h"
#include "common/ustring.h"
#include "common/file.h"

#include "graphics/images/decoder.h"

namespace Graphics {

static void writePixel(Common::DumpFile &file, const byte *&data, PixelFormat format) {
	if (format == kPixelFormatRGB) {
		file.writeByte(data[2]);
		file.writeByte(data[1]);
		file.writeByte(data[0]);
		file.writeByte(0xFF);
		data += 3;
	} else if (format == kPixelFormatBGR) {
		file.writeByte(data[0]);
		file.writeByte(data[1]);
		file.writeByte(data[2]);
		file.writeByte(0xFF);
		data += 3;
	} else if (format == kPixelFormatRGBA) {
		file.writeByte(data[2]);
		file.writeByte(data[1]);
		file.writeByte(data[0]);
		file.writeByte(data[3]);
		data += 4;
	} else if (format == kPixelFormatBGRA) {
		file.writeByte(data[0]);
		file.writeByte(data[1]);
		file.writeByte(data[2]);
		file.writeByte(data[3]);
		data += 4;
	} else
		throw Common::Exception("Unsupported pixel format: %d", (int) format);

}

void dumpTGA(const Common::UString &fileName, const byte *data, int width, int height, PixelFormat format) {
	if ((width <= 0) || (height <= 0) || !data)
		throw Common::Exception("Invalid image data (%dx%d %d)", width, height, data != 0);

	Common::DumpFile file;

	if (!file.open(fileName))
		throw Common::Exception(Common::kOpenError);

	file.writeByte(0); // ID Length
	file.writeByte(0); // Palette size
	file.writeByte(2); // Unmapped RGB
	file.writeUint32LE(0); // Color map
	file.writeByte(0);     // Color map
	file.writeUint16LE(0); // X
	file.writeUint16LE(0); // Y

	file.writeUint16LE(width);
	file.writeUint16LE(height);

	file.writeByte(32); // Pixel depths

	file.writeByte(0);

	uint32 count = width * height;
	for (uint32 i = 0; i < count; i++)
		writePixel(file, data, format);

	if (file.err())
		throw Common::Exception("Write error");

	file.close();
}

void dumpTGA(const Common::UString &fileName, const ImageDecoder *image) {
	if (!image || (image->getMipMapCount() < 1))
		throw Common::Exception("No image");

	const ImageDecoder::MipMap &mipMap = image->getMipMap(0);

	dumpTGA(fileName, mipMap.data, mipMap.width, mipMap.height, image->getFormat());
}

} // End of namespace Graphics