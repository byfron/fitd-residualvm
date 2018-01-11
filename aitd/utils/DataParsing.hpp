#pragma once

#include <assert.h>
#include <vector>
#include <stdint.h>

extern short int cosTable[10000];

class DataParsing {

public:	

	static void init();
	
	static float computeCos(int index) {

		return 2*float(cosTable[index])/0xFFFF;
		
		assert(index <= 0x3FF);
		return cos_table[index & 0x3FF];
	}

	static float computeSin(int index) {


		//assert(index + 0x100 < 0x3FF);
		
		return 2*float(cosTable[(index + 0x100)&0x3FF])/0xFFFF;
		
		
		assert(index <= 0x3FF);
		return cos_table[(index + 0x200) & 0x3FF];
	}

protected:

	static std::vector<float> cos_table;
	
};
