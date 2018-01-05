#include "DataParsing.hpp"
#include <math.h>
#include <iostream>

std::vector<float> DataParsing::cos_table;

void DataParsing::init() {

	//AITD angles are encoded as indices in a cosinus table as follows:	
    //-PI/2   0    PI/2   PI     3PI/2
	//0...    1 ... 0 ... -1 ... 0 (1024)
	float inc = (2.0*M_PI)/0x3FF;
	float start = -M_PI/2.0;
	cos_table.clear();
	
	for (int i = 0; i < 0x3FF; i++) {
		cos_table.push_back(cosf(start + i*inc));
	}
}
