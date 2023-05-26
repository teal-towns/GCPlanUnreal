#pragma once

class HeightMap {
	bool seeded = false;
	// std::vector<unsigned char> image;
	// unsigned width, height;

public:
	HeightMap();
	~HeightMap();

	static float GetTerrainHeightAtPoint(FVector point, float minMeters = -1000, float maxMeters = 9000, int bits = 16);
	static std::tuple<int, int> GetPixelFromXY(int pointX, int pointY, int imageWidth, int imageHeight, float metersPerPixel);
	static std::tuple<std::vector<unsigned char>, int, int> GetImage();
	// static std::tuple<FColor*, int, int> GetImageTexture();
	// static std::tuple<unsigned char*, int, int> GetImage();	// static std::tuple<float, float> GetZScale(float minMeters = -1000, float maxMeters = 9000);
};
