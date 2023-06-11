#pragma once

#include <mutex>
#include "LandscapeComponent.h"

class HeightMap {

// https://refactoring.guru/design-patterns/singleton/cpp/example#example-1
private:
	static HeightMap *pinstance_;
	static std::mutex mutex_;

	static std::vector<unsigned char> _image;
	static int _width;
	static int _height;
	static int _mapWidth;

	static UWorld* _world;

public:
	HeightMap();
	~HeightMap();

	// Singletons should not be cloneable or assignable.
	HeightMap(HeightMap &other) = delete;
	void operator=(const HeightMap &) = delete;

	static HeightMap *GetInstance();

	void SetWorld(UWorld*);
	float GetZFromWorld(FVector Point, float MaxZ = 9000, float MinZ = -1000);
	std::tuple<int, int, int> GetImageSize();
	// std::vector<unsigned char> GetImage();
	uint16 GetImageValue(int, int);
	bool SetImageValue(int pixelX, int pixelY, uint16 imageValue);
	float GetHeightFromPixelValue(int imageValue, int bits = 16, int maxPixelValue = -1, float minMeters = -1000, float maxMeters = 9000);

	float GetTerrainHeightAtPoint(FVector point, float minMeters = -1000, float maxMeters = 9000, int bits = 16);
	uint16 GetImageValueAtPoint(FVector point);
	FVector2D GetPixelFromPoint(FVector point);
	static std::tuple<int, int> GetPixelFromXY(int pointX, int pointY, int imageWidth, int imageHeight, float metersPerPixel);
	static std::tuple<std::vector<unsigned char>, int, int> GetImage(FString filePath = "Source/Conditional/Images/image-heightmap.png");
	bool SaveImage(FString filePath = "Source/Conditional/Images/image-heightmap-adjusted.png");
	bool CarveLine(FVector start, FVector end, float widthMeters);
};
