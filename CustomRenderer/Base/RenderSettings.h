#pragma once

struct RenderSettings
{
	int screenWidth = 800;
	int screenHeight = 600;
	int texWidth = 800;
	int texHeight = 600;
	int blockCount = 75;
	int maxRenderDepth = 10;
	int shadowSampleCount = 1;
	int antiAliasSampleCount = 1;
	bool enableSrgb = false;
	Color clearColor = Color(12, 12, 12);
	float cameraFOV = 60;
	bool autoRerender = true;
};