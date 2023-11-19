#include "Util.h"

namespace Util {

std::vector<std::string> splitString(const std::string& str) {
	std::stringstream strStream(str);
	std::string word;
	std::vector<std::string> words;

	while (strStream >> word)
		words.push_back(word);

	return words;
}


int generateRandomValFromRange(int low, int high) {
	int newValue = (rand() % (high - low + 1)) + low;
	return newValue;
}

}