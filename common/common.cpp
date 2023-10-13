#include "common.h"


std::string ConvertToHexStr(Gauge num) {
	std::stringstream ss;
	ss << std::hex << num;
	return ss.str();
}