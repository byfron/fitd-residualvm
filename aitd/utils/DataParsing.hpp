#pragma once

#include <assert.h>
#include <vector>

class DataParsing {

public:	

	static void init();
	
	static float computeCos(int index) {
		assert(index <= 0x3FF);
		return cos_table[index & 0x3FF];
	}

	static float computeSin(int index) {
		assert(index <= 0x3FF);
		return cos_table[(index + 0x100) & 0x3FF];
	}

protected:

	static std::vector<float> cos_table;
	
};
