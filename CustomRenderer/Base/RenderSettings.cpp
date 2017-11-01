#include "RenderSettings.h"
#include <fstream>
#include <sstream>

bool RenderSettings::LoadFromFile(const char* path)
{
	std::ifstream file(path);
	if (!file.is_open())
		return false;
	std::stringstream r;
	r << file.rdbuf();
	auto s = r.str();
	std::transform(s.begin(), s.end(), s.begin(), tolower);

	//width
	int v = FindValueInFile(s, "screenwidth");
	screenWidth = v < 0 ? screenWidth : v;
	texWidth = screenWidth;

	//height
	v = FindValueInFile(s, "screenheight");
	screenHeight = v < 0 ? screenHeight : v;
	texHeight = screenHeight;

	//blockCount
	v = FindValueInFile(s, "blockcount");
	blockCount = v < 0 ? blockCount : v;
	if (blockCount > texHeight)
		blockCount = texHeight;
	if (blockCount > texWidth)
		blockCount = texWidth;

	//shadow samples
	v = FindValueInFile(s, "shadowsamplecount");
	shadowSampleCount = v < 0 ? shadowSampleCount : v;

	//AA samples
	v = FindValueInFile(s, "antialiassamplecount");
	antiAliasSampleCount = v < 0 ? antiAliasSampleCount : v;

	//Roughness samples
	v = FindValueInFile(s, "roughnesssamplecount");
	roughnessSampleCount = v < 0 ? roughnessSampleCount : v;

	//DoF samples
	v = FindValueInFile(s, "dofsamplecount");
	dofSampleCount = v < 0 ? dofSampleCount : v;

	//Camera FoV
	v = FindValueInFile(s, "camerafov");
	cameraFOV = v < 0 ? cameraFOV : v;

	//Max render Depth
	v = FindValueInFile(s, "maxrenderdepth");
	maxRenderDepth = v < 0 ? maxRenderDepth : v;

	//Global illumination render depth
	v = FindValueInFile(s, "gimaxdepth");
	GIMaxDepth = v < 0 ? GIMaxDepth : v;

	//Global illumination sample count
	v = FindValueInFile(s, "gisamplecount");
	GISampleCount = v < 0 ? GISampleCount : v;

	//sRGB
	v = FindValueInFile(s, "enablesrgb");
	enableSrgb = v < 0 ? enableSrgb : v != 0;

	//Auto rerender
	v = FindValueInFile(s, "autorerender");
	autoRerender = v < 0 ? autoRerender : v != 0;

	return true;
}

void RenderSettings::DownScaleRender(const float f)
{
	texWidth = screenWidth / f;
	texHeight = screenHeight / f;
	if (blockCount > texHeight)
		blockCount = texHeight;
	if (blockCount > texWidth)
		blockCount = texWidth;
}

int RenderSettings::FindValueInFile(const std::string& text, const std::string& value) const
{
	std::sregex_iterator end;
	//Look for line with setting
	std::regex rgxLine(value + "[\\s]*=[\\s]*[\\d]+");
	std::sregex_iterator nextLine(text.begin(), text.end(), rgxLine);
	std::string line;
	if (nextLine != end)
	{
		std::smatch m = *nextLine;
		line = m.str();
	}

	//extract number from line
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
