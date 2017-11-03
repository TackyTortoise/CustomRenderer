#pragma once
#include <string>
#include "../Math/Color.h"

class TextLoader
{
public:
	TextLoader();
	~TextLoader();

	static std::string TxtFileToString(const char* filePath);
	static int FindIntValueInString(const std::string& text, const std::string& value);
	static float FindFloatValueInString(const std::string& text, const std::string& value);
	static FloatColor FindColorValueInString(const std::string& text, const std::string& value);
	static std::string FindStringValueInString(const std::string& text, const std::string& value);
};

