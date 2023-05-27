#include "HeightMap.h"

// #include <iostream>
// #include <stdint.h>
// #include <stdlib.h>
#include "math.h"
// #include "Misc/Paths.h"
// #include "../../ThirdParty/lodepng/lodepng.h"
#include "lodepng.h"
// #define STBimage__IMPLEMENTATION
// #include "stbimage_.h"
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
std::vector<unsigned char> HeightMap::image_;
int HeightMap::width_;
int HeightMap::height_;
UWorld* HeightMap::_world;

HeightMap::HeightMap() {
}

HeightMap::~HeightMap() {
}

HeightMap *HeightMap::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new HeightMap();

		auto [image1, width1, height1] = GetImage();
		image_ = image1;
		width_ = width1;
		height_ = height1;
	}
	return pinstance_;
	// if(heightMap_ == nullptr) {
 //        heightMap_ = new HeightMap();
 //    }
 //    return heightMap_;
}

void HeightMap::SetWorld(UWorld* world) {
	_world = world;
}

// https://stackoverflow.com/questions/2076475/reading-an-image-file-in-c-c
// https://forums.unrealengine.com/t/how-can-i-best-resolve-these-compile-errors/281345/6
float HeightMap::GetTerrainHeightAtPoint(FVector point, float minMeters, float maxMeters, int bits) {
	int mapWidth = 5000;

	float heightMeters = 0;
	// if (_world) {
	// TODO - not working?
	if (false) {
		heightMeters = GetZFromWorld(point);
	} else {
		float metersPerPixel = (float)mapWidth / (float)width_;
		// UE_LOG(LogTemp, Display, TEXT("metersPerPixel %f %d %d"), metersPerPixel, width_, height_);
		auto [pixelX, pixelY] = GetPixelFromXY(point.X, point.Y, width_, height_, metersPerPixel);
		int size = image_.size();

		uint16 imageValue = 0, BigEiV, LitEiV;
		int imageIndex,imageIndexB;
		uint8* bptr, byte1, byte2;
		size_t ii;
		imageIndex = pixelY * width_ + pixelX;
		imageIndexB = imageIndex * 2;
		bptr = (uint8*)&image_[imageIndexB];
		byte1 = *bptr++;
		byte2 = *bptr++;
		// Big endian
		BigEiV = (byte1 << 8) | byte2;
		// Little endian
		LitEiV = (byte2 << 8) | byte1;
		imageValue = BigEiV;

		// int size = sizeof(image);
		// TODO - not quite working; the output for writing has x 1088 y 745 [6553] but we are getting 4627 (-293.96 meters).
		// The image terrain in Unreal looks correct so I believe the image is properly written, it is just a reading problem here.
		// TESTING
		// pixelX = 1088;
		// pixelY = 745;
		// pixelX = 745;
		// pixelY = 1088;
		// UE_LOG(LogTemp, Display, TEXT("pixelX %d pixelY %d image length %d"), pixelX, pixelY, size);
		// // 8 bit to 16 bit means we need to get and combine 2 bytes.
		// // https://github.com/lvandeve/lodepng/issues/74
		// // int imageIndex = pixelY * width + pixelX;
		// // int imageValue = image[imageIndex];
		// char byte1 = image_[(2 * width_ * pixelY + 2 * pixelX + 0)];
		// char byte2 = image_[(2 * width_ * pixelY + 2 * pixelX + 1)];
		// // Big endian
		// uint16 imageValue = (byte1 << 8) | byte2;
		// // Little endian
		// // uint16 imageValue = (byte2 << 8) | byte1;
		// UE_LOG(LogTemp, Display, TEXT("imageValue %d %d %d"), imageValue, byte1, byte2);
		// UE_LOG(LogTemp, Display, TEXT("imageValue %d %d %d point x %f y %f"), imageValue, pixelX, pixelY, point.X, point.Y);

		int maxPixelValue = pow(2, bits) - 1;
		heightMeters = Lodash::RangeValue(imageValue, 0, maxPixelValue, minMeters, maxMeters);
	}
	// auto [zScale, zOffset] = GetZScale();
	// UE_LOG(LogTemp, Display, TEXT("heightMeters %f"), heightMeters);
	return heightMeters;
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
	// UE_LOG(LogTemp, Display, TEXT("filename %s path %s"), *str, *projectPath);
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
// 	//	 for (int32 Y = 0; Y < texture->SizeY; Y++)
// 	//	 {
// 	//		 FColor PixelColor = ImageData[Y * texture->SizeX + X];
// 	//	 }
// 	// }

// 	texture->PlatformData->Mips[0].BulkData.Unlock();

// 	// Texture->CompressionSettings = OldCompressionSettings;
// 	// Texture->MipGenSettings = OldMipGenSettings;
// 	// Texture->SRGB = OldSRGB;
// 	// Texture->UpdateResource();
// 	return {ImageData, width, height};
// }

// https://forums.unrealengine.com/t/creating-a-lanscape-using-c-or-the-python-api/504997/4
// https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=&ved=2ahUKEwiv7qy015P_AhX0In0KHd_kBpkQFnoECAwQAQ&url=https%253A%252F%252Fwww.diva-portal.org%252Fsmash%252Fget%252Fdiva2%253A1440337%252FFULLTEXT01.pdf
// https://hippowombat.tumblr.com/post/180615213251/blueprint-powered-landscape-edits-in-ue4-420
// https://www.reddit.com/r/unrealengine/comments/hore2q/landscape_runtime_editing/
void HeightMap::SetHeightMap(ALandscape* Landscape) {
	FString FullPath = "Source/GCPlan/Assets/Images/image-heightmap-flat.png";
	FString projectPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	FullPath = projectPath + FullPath;
	UTexture2D* texture = FImageUtils::ImportFileAsTexture2D(FullPath);

	int components = Landscape->LandscapeComponents.Num();
	UE_LOG(LogTemp, Display, TEXT("components %d"), components);
	// Landscape->LandscapeComponents[0]->GetHeightmap();
	// TODO
	// Landscape->LandscapeComponents[0]->SetHeightmap(texture);

	int MaxX = 2017;
	int MaxY = 2017;
	// TArray<uint16> HeightData = {};
	// for (int yy = 0; yy < MaxY; yy++) {
	// 	for (int xx = 0; xx < MaxX; xx++) {
	// 		uint16 val = 6553;
	// 		val = yy * 10;
	// 		HeightData.Add(val);
	// 	}
	// }


// ComponentSizeQuads = InNumSubsections * InSubsectionSizeQuads;
// const int32 ComponentSizeVerts = NumSubsections * (SubsectionSizeQuads + 1);
// const int32 WeightmapSize = (SubsectionSizeQuads + 1) * NumSubsections;
	// (63 * 2 + 1) * 2

	// int32 ComponentCountX = 16;
	// int32 ComponentCountY = 16;
	// int32 SectionSize = 63;
	// int32 SectionsPerComponent = 2;
	// int32 QuadsPerComponent = SectionSize * SectionsPerComponent;
	// int32 SizeX = ComponentCountX * QuadsPerComponent + 1;
	// int32 SizeY = ComponentCountY * QuadsPerComponent + 1;

	// TArray<uint16> HeightData;
	// HeightData.SetNum(SizeX * SizeY);
	// for (int32 i = 0; i < HeightData.Num(); i++) {
	// 	HeightData[i] = 6553;
	// }

	// TMap<FGuid, TArray<uint16>> HeightDataPerLayers;
	// TMap<FGuid, TArray<FLandscapeImportLayerInfo>> MaterialLayerDataPerLayers;
	// TArray<FLandscapeImportLayerInfo> MaterialImportLayers;
	// MaterialImportLayers.Reserve(0);

	// HeightDataPerLayers.Add(FGuid(), MoveTemp(HeightData));
	// // ComputeHeightData will also modify/expand material layers data, which is why we create MaterialLayerDataPerLayers after calling ComputeHeightData
	// MaterialLayerDataPerLayers.Add(FGuid(), MoveTemp(MaterialImportLayers));

	// Landscape->Import(FGuid::NewGuid(), 0, 0, SizeX - 1, SizeY - 1, SectionsPerComponent, QuadsPerComponent,
	// 	HeightDataPerLayers, nullptr, MaterialLayerDataPerLayers, ELandscapeImportAlphamapType::Additive);

	// Not working? No heightmap change in terrain..
	ULandscapeInfo* LandscapeInfo = Landscape->GetLandscapeInfo();
	// FHeightmapAccessor<false> HeightmapAccessor(LandscapeInfo);
	// HeightmapAccessor.SetData(0, 0, MaxX, MaxY, HeightData.GetData());

	// LandscapeEditorUtils::SetHeightmapData(Landscape, HeightData);
	LandscapeInfo->UpdateLayerInfoMap(Landscape);
	Landscape->RegisterAllComponents();
	// Need to explicitly call PostEditChange on the LandscapeMaterial property or the landscape proxy won't update its material
	FPropertyChangedEvent MaterialPropertyChangedEvent(FindFieldChecked< FProperty >(Landscape->GetClass(), FName("LandscapeMaterial")));
	Landscape->PostEditChangeProperty(MaterialPropertyChangedEvent);
	Landscape->PostEditChange();
	// UE_LOG(LogTemp, Display, TEXT("HeightmapAccessor should have changed data"));

	// for (int ii = 0; ii < Landscape->LandscapeLayers.Num(); ii++) {
	// 	UE_LOG(LogTemp, Display, TEXT("LandscapeLayer %d %s"), ii, Landscape->LandscapeLayers[ii].Guid);
	// }
}

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
