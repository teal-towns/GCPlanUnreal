#include "HeightMap.h"

// #include <iostream>
// #include <stdint.h>
// #include <stdlib.h>
#include "math.h"
// #include "Misc/Paths.h"
// #include "../../ThirdParty/lodepng/lodepng.h"
#include "lodepng.h"
// #define STB_image_IMPLEMENTATION
// #include "stb_image.h"
// #include "Landscape.h"
#include "ImageUtils.h"
#include "Landscape.h"
#include "LandscapeEdit.h"
#include "LandscapeEditorUtils.h"
#include "LandscapeInfo.h"
#include "LandscapeProxy.h"
#include "LandscapeComponent.h"

#include "../Common/Lodash.h"

// Static methods should be defined outside the class.
// NOTE this can NOT be in a header file (as that will be included multiple times and then get a duplicate symbol error).
HeightMap* HeightMap::pinstance_{nullptr};
std::mutex HeightMap::mutex_;
std::vector<unsigned char> HeightMap::_image;
int HeightMap::_width;
int HeightMap::_height;
UWorld* HeightMap::_world;
int HeightMap::_mapWidth;

HeightMap::HeightMap() {
}

HeightMap::~HeightMap() {
}

HeightMap *HeightMap::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new HeightMap();

		_mapWidth = 2500;
		auto [image1, width1, height1] = GetImage();
		_image = image1;
		_width = width1;
		_height = height1;
	}
	return pinstance_;
}

void HeightMap::SetWorld(UWorld* world) {
	_world = world;
}

std::tuple<int, int, int> HeightMap::GetImageSize() {
	return {_width, _height, _mapWidth};
}

uint16 HeightMap::GetImageValue(int pixelX, int pixelY) {
	if (pixelX >= _width) {
		UE_LOG(LogTemp, Warning, TEXT("HeightMap.GetTerrainHeightAtPoint pixelX > _width %d %d"), pixelX, _width);
		pixelX = _width - 1;
	}
	if (pixelY >= _height) {
		UE_LOG(LogTemp, Warning, TEXT("HeightMap.GetTerrainHeightAtPoint pixelY > _height %d %d"), pixelY, _height);
		pixelY = _height - 1;
	}
	if (pixelX < 0) {
		UE_LOG(LogTemp, Warning, TEXT("HeightMap.GetTerrainHeightAtPoint pixelX < 0 %d %d"), pixelX, 0);
		pixelX = 0;
	}
	if (pixelY < 0) {
		UE_LOG(LogTemp, Warning, TEXT("HeightMap.GetTerrainHeightAtPoint pixelY < 0 %d %d"), pixelY, 0);
		pixelY = 0;
	}

	int size = _image.size();

	uint16 imageValue = 0, BigEiV;
	// uint16 LitEiV;
	int imageIndex,imageIndexB;
	uint8* bptr, byte1, byte2;
	imageIndex = pixelY * _width + pixelX;
	imageIndexB = imageIndex * 2;
	bptr = (uint8*)&_image[imageIndexB];
	byte1 = *bptr++;
	byte2 = *bptr++;
	// Big endian
	BigEiV = (byte1 << 8) | byte2;
	// Little endian
	// LitEiV = (byte2 << 8) | byte1;
	imageValue = BigEiV;
	return imageValue;
}

// https://github.com/lvandeve/lodepng/issues/74
bool HeightMap::SetImageValue(int pixelX, int pixelY, uint16 imageValue) {
	// uint8 bytes[sizeof(imageValue)];
	// memcpy(bytes, &imageValue, sizeof(imageValue));
	uint8 byte1 = (imageValue >> 8) & 0xFF;
	uint8 byte2 = imageValue & 0xFF;
	// Big Endian (most significant first, least significnant 2nd byte).
	_image[2 * _width * pixelY + 2 * pixelX + 0] = byte1;
	_image[2 * _width * pixelY + 2 * pixelX + 1] = byte2;
	return true;
}

float HeightMap::GetHeightFromPixelValue(int imageValue, int bits, int maxPixelValue, float minMeters, float maxMeters) {
	if (maxPixelValue < 0) {
		maxPixelValue = pow(2, bits) - 1;
	}
	float heightMeters = Lodash::RangeValue(imageValue, 0, maxPixelValue, minMeters, maxMeters);
	return heightMeters;
}

// https://stackoverflow.com/questions/2076475/reading-an-image-file-in-c-c
// https://forums.unrealengine.com/t/how-can-i-best-resolve-these-compile-errors/281345/6
float HeightMap::GetTerrainHeightAtPoint(FVector point, float minMeters, float maxMeters, int bits) {
	float heightMeters = 0;
	// if (_world) {
	// TODO - not working?
	if (false) {
		heightMeters = GetZFromWorld(point);
	} else {
		uint16 imageValue = GetImageValueAtPoint(point);
		heightMeters = GetHeightFromPixelValue(imageValue, bits, -1, minMeters, maxMeters);
	}
	// auto [zScale, zOffset] = GetZScale();
	// UE_LOG(LogTemp, Display, TEXT("heightMeters %f"), heightMeters);
	return heightMeters;
}

uint16 HeightMap::GetImageValueAtPoint(FVector point) {
	float metersPerPixel = (float)_mapWidth / (float)_width;
	// UE_LOG(LogTemp, Display, TEXT("metersPerPixel %f %d %d"), metersPerPixel, _width, _height);
	auto [pixelX, pixelY] = GetPixelFromXY(point.X, point.Y, _width, _height, metersPerPixel);

	uint16 imageValue = GetImageValue(pixelX, pixelY);
	return imageValue;
}

FVector2D HeightMap::GetPixelFromPoint(FVector point) {
	float metersPerPixel = (float)_mapWidth / (float)_width;
	auto [pixelX, pixelY] = GetPixelFromXY(point.X, point.Y, _width, _height, metersPerPixel);
	return FVector2D(pixelX, pixelY);
}

// Get pixel from point. Assumes 0,0 is in center.
// Image 0,0 is top left.
std::tuple<int, int> HeightMap::GetPixelFromXY(int pointX, int pointY, int imageWidth, int imageHeight, float metersPerPixel) {
	int pixelX = pointX / metersPerPixel + imageWidth / 2;
	int pixelY = pointY / metersPerPixel + imageHeight / 2;
	return {pixelX, pixelY};
}

float HeightMap::GetZFromWorld(FVector Point, float MaxZ, float MinZ) {
	if (_world) {
		FVector StartLocation{ Point.X, Point.Y, MaxZ * 100 };
		FVector EndLocation{ Point.X, Point.Y, MinZ * 100 };
		FHitResult HitResult;
		_world->LineTraceSingleByObjectType(
			OUT HitResult,
			StartLocation,
			EndLocation,
			FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
			FCollisionQueryParams()
		);
		UE_LOG(LogTemp, Display, TEXT("GetZFromWorld %s"), *Point.ToString());
		if (HitResult.GetActor()) {
			return HitResult.ImpactPoint.Z;
		}
	}
	return -9999;
}

std::tuple<std::vector<unsigned char>, int, int> HeightMap::GetImage(FString filePath) {
// std::tuple<unsigned char*, int, int> HeightMap::GetImage() {
	// filePath = "Source/GCPlan/Assets/Images/homes.png";
	FString projectPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	filePath = projectPath + filePath;

	std::string VectorString(TCHAR_TO_UTF8(*filePath));
	const char* filename = VectorString.c_str();

	// int width, height, bpp;
	// unsigned char* image = stbi_load(filename, &width, &height, &bpp, 1);

	std::vector<unsigned char> image;
	unsigned width, height;
	FString str(filename);
	// UE_LOG(LogTemp, Display, TEXT("filename %s path %s"), *str, *projectPath);
	unsigned error = lodepng::decode(image, width, height, filename, LCT_GREY, 16);
	// unsigned error = lodepng::decode(image, width, height, filename);
	if (error) {
		FString err(lodepng_error_text(error));
		UE_LOG(LogTemp, Display, TEXT("error %s"), *err);
	}
	return {image, width, height};
}

bool HeightMap::SaveImage(FString filePath) {
	FString projectPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	filePath = projectPath + filePath;
	std::string VectorString(TCHAR_TO_UTF8(*filePath));
	const char* filename = VectorString.c_str();
	unsigned error = lodepng::encode(filename, _image, _width, _height, LCT_GREY, 16);
	return true;
}

bool HeightMap::CarveLine(FVector start, FVector end, float widthMeters) {
	float metersPerPixel = (float)_mapWidth / (float)_width;
	float stepDistance = metersPerPixel;
	FVector pathLine = end - start;
	float totalDistance = pathLine.Size();
	int stepCount = ceil(totalDistance / stepDistance);

	int pixelRange = ceil(widthMeters / metersPerPixel);
	int pixelRangeHalf = ceil(pixelRange / 2);

	FVector currentPoint = start;
	FVector2D pixel;
	uint16 imageValue;
	int xMin, xMax, yMin, yMax;
	for (int ii = 0; ii < stepCount; ii++) {
		pixel = GetPixelFromPoint(currentPoint);
		imageValue = GetImageValue(pixel.X, pixel.Y);
		xMin = pixel.X - pixelRangeHalf;
		xMax = pixel.X + pixelRangeHalf;
		yMin = pixel.Y - pixelRangeHalf;
		yMax = pixel.Y + pixelRangeHalf;
		if (xMin < 0) {
			xMin = 0;
		}
		if (xMax >= _width) {
			xMax = _width - 1;
		}
		if (yMin < 0) {
			yMin = 0;
		}
		if (yMax >= _height) {
			yMax = _height - 1;
		}
		for (int yy = yMin; yy <= yMax; yy++) {
			for (int xx = xMin; xx <= xMax; xx++) {
				SetImageValue(xx, yy, imageValue);
			}
		}

		currentPoint += pathLine.GetClampedToMaxSize(stepDistance);
	}
	return true;
}
