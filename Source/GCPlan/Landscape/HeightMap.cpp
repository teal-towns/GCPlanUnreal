#include "HeightMap.h"

// #include <iostream>
// #include <stdint.h>
// #include <stdlib.h>
#include "math.h"
// #include "Misc/Paths.h"
// #include "../../ThirdParty/lodepng/lodepng.h"
#include "lodepng.h"
// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"
#include "../Common/Lodash.h"

HeightMap::HeightMap() {
	if (!seeded) {
		seeded = true;

		// TODO - need to make this a singleton or somehow access this data from a static function?
		// auto [image, width, height] = GetImage();
	}
}

HeightMap::~HeightMap() {
}

// https://stackoverflow.com/questions/2076475/reading-an-image-file-in-c-c
// https://forums.unrealengine.com/t/how-can-i-best-resolve-these-compile-errors/281345/6
float HeightMap::GetTerrainHeightAtPoint(FVector point, float minMeters, float maxMeters, int bits) {
	int mapWidth = 5000;
	// TODO - do this ONCE and store the result.
	auto [image, width, height] = GetImage();

	float metersPerPixel = (float)mapWidth / (float)width;
	UE_LOG(LogTemp, Display, TEXT("metersPerPixel %f %d %d"), metersPerPixel, width, height);
	auto [pixelX, pixelY] = GetPixelFromXY(point.X, point.Y, width, height, metersPerPixel);
	// int size = image.size();
	// int size = sizeof(image);
	// TODO - not quite working; the output for writing has x 1088 y 745 [6553] but we are getting 4627 (-293.96 meters).
	// The image terrain in Unreal looks correct so I believe the image is properly written, it is just a reading problem here.
	// TESTING
	pixelX = 1088;
	pixelY = 745;
	// pixelX = 745;
	// pixelY = 1088;
	UE_LOG(LogTemp, Display, TEXT("pixelX %d pixelY %d image length %d"), pixelX, pixelY);
	// 8 bit to 16 bit means we need to get and combine 2 bytes.
	// https://github.com/lvandeve/lodepng/issues/74
	// int imageIndex = pixelY * width + pixelX;
	// int imageValue = image[imageIndex];
	char byte1 = 2 * width * pixelY + 2 * pixelX + 0;
	char byte2 = 2 * width * pixelY + 2 * pixelX + 1;
	// Big endian
	uint16 imageValue = (byte1 << 8) | byte2;
	// Little endian
	// uint16 imageValue = (byte2 << 8) | byte1;
	UE_LOG(LogTemp, Display, TEXT("imageValue %d %d %d"), imageValue, byte1, byte2);

	int maxPixelValue = pow(2, bits) - 1;
	float heightMeters = Lodash::RangeValue(imageValue, 0, maxPixelValue, minMeters, maxMeters);
	// auto [zScale, zOffset] = GetZScale();
	UE_LOG(LogTemp, Display, TEXT("heightMeters %f"), heightMeters);
	return heightMeters;
}

// Get pixel from point. Assumes 0,0 is in center.
// Image 0,0 is top left.
std::tuple<int, int> HeightMap::GetPixelFromXY(int pointX, int pointY, int imageWidth, int imageHeight, float metersPerPixel) {
	int pixelX = pointX / metersPerPixel + imageWidth / 2;
	int pixelY = pointY / metersPerPixel + imageHeight / 2;
	return {pixelX, pixelY};
}

std::tuple<std::vector<unsigned char>, int, int> HeightMap::GetImage() {
// std::tuple<unsigned char*, int, int> HeightMap::GetImage() {
	FString filePath = "Source/GCPlan/Assets/Images/image-heightmap.png";
	// /Script/CoreUObject.Class'/Script/GCPlan.GCPlanGameInstance'
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
	UE_LOG(LogTemp, Display, TEXT("filename %s path %s"), *str, *projectPath);
	unsigned error = lodepng::decode(image, width, height, filename, LCT_GREY, 16);
	// unsigned error = lodepng::decode(image, width, height, filename);
	if (error) {
		FString err(lodepng_error_text(error));
		UE_LOG(LogTemp, Display, TEXT("error %s"), *err);
	}
	return {image, width, height};
}

// https://forums.unrealengine.com/t/load-png-image-from-disk/680334
// https://isaratech.com/ue4-reading-the-pixels-from-a-utexture2d/
// std::tuple<FColor*, int, int> HeightMap::GetImageTexture() {
// 	FString FullPath = "Source/GCPlan/Assets/Images/image-heightmap.png";
// 	FString projectPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
// 	FullPath = projectPath + FullPath;
// 	// FString contentPath = FPaths::ProjectContentDir();
// 	// FString fullContentPath = FPaths::ConvertRelativePathToFull(contentPath);
// 	// FString FullPath = fullContentPath + File;
// 	UTexture2D* texture = nullptr;
// 	if (FullPath.Len() > 0) {
// 		FullPath.ReplaceInline(TEXT("\\"), TEXT("/"), ESearchCase::CaseSensitive);
// 		FullPath.ReplaceInline(TEXT("//"), TEXT("/"), ESearchCase::CaseSensitive);
// 		FullPath.RemoveFromStart(TEXT("/"));
// 		FullPath.RemoveFromEnd(TEXT("/"));
// 		FPlatformMisc::NormalizePath(FullPath);
// 		texture = FImageUtils::ImportFileAsTexture2D(FullPath);
// 	}
// 	// return texture;

// 	// TextureCompressionSettings OldCompressionSettings = texture->CompressionSettings;
// 	// TextureMipGenSettings OldMipGenSettings = texture->MipGenSettings;
// 	// bool OldSRGB = texture->SRGB;

// 	// texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
// 	// texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
// 	// texture->SRGB = false;
// 	// texture->UpdateResource();

// 	const FColor* ImageData = static_cast<const FColor*>( texture->PlatformData->Mips[0].BulkData.LockReadOnly());
// 	int width = texture->SizeX;
// 	int height = texture->SizeY;
// 	UE_LOG(LogTemp, Display, TEXT("image texture width %d height %d pixel %d"), width, height, ImageData[(745 * width + 1088)]);
// 	// for(int32 X = 0; X < texture->SizeX; X++)
// 	// {
// 	//     for (int32 Y = 0; Y < texture->SizeY; Y++)
// 	//     {
// 	//         FColor PixelColor = ImageData[Y * texture->SizeX + X];
// 	//     }
// 	// }

// 	texture->PlatformData->Mips[0].BulkData.Unlock();

// 	// Texture->CompressionSettings = OldCompressionSettings;
// 	// Texture->MipGenSettings = OldMipGenSettings;
// 	// Texture->SRGB = OldSRGB;
// 	// Texture->UpdateResource();
// 	return {ImageData, width, height};
// }

// std::tuple<float, float> HeightMap::GetZScale(float minMeters, float maxMeters) {
// 	// To allow editing in both directions a landscape’s origin is in the middle of the height range.
// 	// So if your imported heightmap’s black point is -300 and the white point is 3580,
// 	// you should place your landscapes at a height of (3580 + 300) / 2 + -300 = 1600.
// 	// https://docs.unrealengine.com/5.2/en-US/landscape-technical-guide-in-unreal-engine/
// 	float zPixelRange = (maxMeters - minMeters);
// 	float zScale = zPixelRange * 1 / 512 * 100;
// 	float zOffset = (zPixelRange / 2 + minMeters) * 100;
// 	return {zScale, zOffset};
// }
