#include "RenderSettings.h"
#include <fstream>
#include <sstream>
#include "TextLoader.h"

bool RenderSettings::LoadFromFile(const char* path)
{
	auto s = TextLoader::TxtFileToString(path);

	//width
	int v = TextLoader::FindIntValueInString(s, "screenwidth");
	screenWidth = v < 0 ? screenWidth : v;
	texWidth = screenWidth;

	//height
	v = TextLoader::FindIntValueInString(s, "screenheight");
	screenHeight = v < 0 ? screenHeight : v;
	texHeight = screenHeight;

	//blockCount
	v = TextLoader::FindIntValueInString(s, "blockcount");
	blockCount = v < 0 ? blockCount : v;
	if (blockCount > texHeight)
		blockCount = texHeight;
	if (blockCount > texWidth)
		blockCount = texWidth;

	//shadow samples
	v = TextLoader::FindIntValueInString(s, "shadowsamplecount");
	shadowSampleCount = v < 0 ? shadowSampleCount : v;

	//AA samples
	v = TextLoader::FindIntValueInString(s, "antialiassamplecount");
	antiAliasSampleCount = v < 0 ? antiAliasSampleCount : v;

	//Roughness samples
	v = TextLoader::FindIntValueInString(s, "roughnesssamplecount");
	roughnessSampleCount = v < 0 ? roughnessSampleCount : v;

	//DoF samples
	v = TextLoader::FindIntValueInString(s, "dofsamplecount");
	dofSampleCount = v < 0 ? dofSampleCount : v;

	//Camera FoV
	v = TextLoader::FindIntValueInString(s, "camerafov");
	cameraFOV = v < 0 ? cameraFOV : v;

	//Max render Depth
	v = TextLoader::FindIntValueInString(s, "maxrenderdepth");
	maxRenderDepth = v < 0 ? maxRenderDepth : v;

	//Global illumination render depth
	v = TextLoader::FindIntValueInString(s, "gimaxdepth");
	GIMaxDepth = v < 0 ? GIMaxDepth : v;

	//Global illumination sample count
	v = TextLoader::FindIntValueInString(s, "gisamplecount");
	GISampleCount = v < 0 ? GISampleCount : v;

	//Global illumination distance
	float f = TextLoader::FindFloatValueInString(s, "gidistance");
	GIDistance = f < 0 ? GIDistance : f;

	//Ambient occlusion sample count
	v = TextLoader::FindIntValueInString(s, "aosamplecount");
	AOSampleCount = v < 0 ? AOSampleCount : v;

	//Ambient occlusion distance
	f = TextLoader::FindFloatValueInString(s, "aodistance");
	AODistance = f < 0 ? AODistance : f;

	//sRGB
	v = TextLoader::FindIntValueInString(s, "enablesrgb");
	enableSrgb = v < 0 ? enableSrgb : v != 0;

	//Auto rerender
	v = TextLoader::FindIntValueInString(s, "autorerender");
	autoRerender = v < 0 ? autoRerender : v != 0;

	//Render Downscaling
	f = TextLoader::FindFloatValueInString(s, "downscaling");
	if (f > 0)
		DownScaleRender(f);

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
