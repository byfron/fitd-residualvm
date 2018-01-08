#pragma once

#include "ResourceManager.hpp"
#include <pak.h>

namespace {
typedef unsigned char byte;
}

struct Color {
	byte a;
	byte r;
	byte g;
	byte b;

	static uint32_t toABGR32(const Color& col) 	{
		uint32_t c32 = 
			 col.r        |
			(col.g << 8)  |
			(col.b << 16) |
			(col.a << 24);
		return c32;
	}
};

class ColorPalette : public Resource {
public:

	typedef std::shared_ptr<ColorPalette> Ptr;
	
	static uint32_t type() { return COLOR_PALETTE_RESOURCE_TYPE; }
	
	void init() {}
	
	ColorPalette(byte *data, uint32_t id) : Resource(id) {		
		byte *ptr = data;		
		for (int i = 0; i < num_colors; i++) {
			Color color;
			color.a = 255;
			color.r = (*(ptr + 0));
			color.g = (*(ptr + 1));
			color.b = (*(ptr + 2));
			palette.push_back(color);
			ptr += 3;
		}
	}

	Color getColor(int index) {
		assert(index < palette.size());
		return palette[index];
	}

	static void loadPalette() {
		byte *aux = new byte [65068];
		Fitd::loadPakToPtr("ITD_RESS", 3, (char*)aux);
		ColorPalette color_palette(aux, OBJECT_PALETTE_ID);
		ResourceManager::pushResource<ColorPalette>(color_palette);
	}
	
protected:

	int num_colors = 256;
	std::vector<Color> palette;
	
};
