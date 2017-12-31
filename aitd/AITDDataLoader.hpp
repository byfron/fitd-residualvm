#pragma once

#include "ResourceManager.hpp"
#include <pak.h>
#include <Eigen/Dense>

class AITDDataLoader {
public:
	static void loadColorPalette() {
		byte *aux = new byte [65068];
		Fitd::loadPakToPtr("ITD_RESS", 3, (char*)aux);
		ColorPalette color_palette(aux, OBJECT_PALETTE_ID);
		ResourceManager::pushResource<ColorPalette>(color_palette);
	}
	
};
