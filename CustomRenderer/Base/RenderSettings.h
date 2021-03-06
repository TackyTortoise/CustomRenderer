#pragma once

#include <regex>
#include "../Math/Color.h"

struct RenderSettings
{
	int screenWidth = 800;
	int screenHeight = 600;
	int texWidth = 800;
	int texHeight = 600;
	int blockCount = 75;
	int maxRenderDepth = 10;
	int shadowSampleCount = 1;
	int antiAliasSampleCount = 0;
	int roughnessSampleCount = 1;
	int dofSampleCount = 0;
	int GISampleCount = 0;
	int GIMaxDepth = 0;
	float GIDistance = 10.f;
	int AOMaxDepth = 0;
	int AOSampleCount = 10;
	float AODistance = 2.f;
	bool enableSrgb = false;
	Color clearColor = Color(12, 12, 12);
	float cameraFOV = 60;
	bool autoRerender = true;

	bool LoadFromFile(const char* path);
	void DownScaleRender(const float f);
};
