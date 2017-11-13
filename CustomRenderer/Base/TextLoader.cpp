#include "TextLoader.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <regex>

TextLoader::TextLoader()
{
}


TextLoader::~TextLoader()
{
}

std::string TextLoader::TxtFileToString(const char* filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open())
		return false;
	std::stringstream r;
	r << file.rdbuf();
	auto s = r.str();
	std::transform(s.begin(), s.end(), s.begin(), tolower);
	return s;
}

int TextLoader::FindIntValueInString(const std::string& text, const std::string& value)
{
	std::sregex_iterator end;
	//Look for line with int
	std::regex rgxLine(value + "[\\s]*=[\\s]*[\\d]+");
	std::sregex_iterator nextLine(text.begin(), text.end(), rgxLine);
	std::string line;
	if (nextLine != end)
	{
		std::smatch m = *nextLine;
		line = m.str();
	}

	//Extract number from line
	std::regex rgx("[\\d]+");
	std::sregex_iterator next(line.begin(), line.end(), rgx);
	int v = -1;
	if (next != end)
	{
		std::smatch match = *next;
		v = std::stoi(match.str());
	}

	return v;
}

float TextLoader::FindFloatValueInString(const std::string& text, const std::string& value)
{
	std::sregex_iterator end;
	//Look for line with float
	std::regex rgxLine(value + "[\\s]*=[\\s]*[-+]?[0-9]+\.?[0-9]*(?:f)?");
	std::sregex_iterator nextLine(text.begin(), text.end(), rgxLine);
	std::string line;
	if (nextLine != end)
	{
		std::smatch m = *nextLine;
		line = m.str();
	}

	//Extract number from line
	std::regex rgx("[-+]?[0-9]+\.?[0-9]*(?:f)?");
	std::sregex_iterator next(line.begin(), line.end(), rgx);
	float v = -1.f;
	if (next != end)
	{
		std::smatch match = *next;
		auto s = match.str();
		v = std::stof(match.str());
	}

	return v;
}

Vec3 TextLoader::FindVectorValueInString(const std::string& text, const std::string& value)
{
	std::sregex_iterator end;
	//Look for line with color
	std::regex rgxLine(value + "[\\s]*=[\\s]*[-+]?[0-9]+\.?[0-9]*(?:f)?[\\s]*[,][\\s]*[-+]?[0-9]+\.?[0-9]*(?:f)?[\\s]*[,][\\s]*[-+]?[0-9]+\.?[0-9]*(?:f)?");
	std::sregex_iterator nextLine(text.begin(), text.end(), rgxLine);
	std::string line;
	if (nextLine != end)
	{
		std::smatch m = *nextLine;
		line = m.str();
	}

	//Extract numbers from line
	float values[3];
	values[0] = -1;
	std::regex rgx("[-+]?[0-9]+\.?[0-9]*(?:f)?");
	std::sregex_iterator next(line.begin(), line.end(), rgx);
	std::smatch res;
	int ind = 0;
	while (std::regex_search(line, res, rgx))
	{
		auto s = res[0].str();
		values[ind] = stof(res[0].str());
		++ind;
		line = res.suffix();
	}

	return Vec3(values[0], values[1], values[2]);
}

FloatColor TextLoader::FindColorValueInString(const std::string& text, const std::string& value)
{
	std::sregex_iterator end;
	//Look for line with color
	std::regex rgxLine(value + "[\\s]*=[\\s]*[\\d]+[\\s]*[,][\\s]*[\\d]+[\\s]*[,][\\s]*[\\d]+");
	std::sregex_iterator nextLine(text.begin(), text.end(), rgxLine);
	std::string line;
	if (nextLine != end)
	{
		std::smatch m = *nextLine;
		line = m.str();
	}

	//Extract numbers from line
	int values[3];
	values[0] = -1;
	std::regex rgx("[\\d]+");
	std::sregex_iterator next(line.begin(), line.end(), rgx);
	std::smatch res;
	int ind = 0;
	while (std::regex_search(line, res, rgx))
	{
		values[ind] = stoi(res[0].str());
		++ind;
		line = res.suffix();
	}

	return FloatColor(values[0], values[1], values[2]);
}

std::string TextLoader::FindStringValueInString(const std::string& text, const std::string& value)
{
	std::sregex_iterator end;
	//Look for line with text identifier
	std::regex rgxLine(value + "[\\s]*=[\\s]*[a-z]+[.]?[a-z]+");
	std::sregex_iterator nextLine(text.begin(), text.end(), rgxLine);
	std::string line;
	if (nextLine != end)
	{
		std::smatch m = *nextLine;
		line = m.str();
	}

	if (line.size() == 0)
		return line;

	line.erase(remove_if(line.begin(), line.end(), isspace), line.end());
	return line.substr(value.size() + 1);
}
