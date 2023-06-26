#include "MeshTerrain.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "ProceduralMeshComponent.h"
// #include "UObject/Object.h"

#include "HeightMap.h"
#include "ImageUtils.h"
#include "../Common/DataConvert.h"
#include "../Common/Lodash.h"

MeshTerrain* MeshTerrain::pinstance_{nullptr};
std::mutex MeshTerrain::mutex_;

MeshTerrain::MeshTerrain() {
}

MeshTerrain::~MeshTerrain() {
}

MeshTerrain *MeshTerrain::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new MeshTerrain();
	}
	return pinstance_;
}

void MeshTerrain::SetProceduralMesh(UProceduralMeshComponent* ProceduralMesh1, UMaterialInterface* Material1) {
// void MeshTerrain::SetProceduralMesh(UProceduralMeshComponent* ProceduralMesh1, UMaterialInstance* Material1) {
	ProceduralMesh = ProceduralMesh1;
	Material = Material1;
	MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(Material, NULL);
	// MaterialInstance = Material1;
}

void MeshTerrain::ClearMesh() {
	ProceduralMesh->ClearAllMeshSections();
}

void MeshTerrain::CreateTerrainFromHeightMap(float Scale, int UVScale) {
	auto [XSize, YSize] = CreateVerticesFromHeightMap(Scale, UVScale);
	CreateTriangles(XSize, YSize);
	ProceduralMesh->ClearAllMeshSections();
	ProceduralMesh->bUseComplexAsSimpleCollision = true;
	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), UV0,
		TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	// ProceduralMesh->SetMaterial(0, Material);
	ProceduralMesh->SetMaterial(0, MaterialInstanceDynamic);
}

std::tuple<int, int> MeshTerrain::CreateVerticesFromHeightMap(float Scale, int UVScale, float ZScale) {
	HeightMap* heightMap = HeightMap::GetInstance();
	auto [XSize, YSize, MapXSize] = heightMap->GetImageSize();
	// std::vector<unsigned char> Image = heightMap->GetImage();
	int bits = 16;
	int maxPixelValue = pow(2, bits) - 1;
	float imageValue, zz;
	int xHeightMap, yHeightMap;
	for (int xx = 0; xx <= XSize; xx++) {
		for (int yy = 0; yy <= YSize; yy++) {
			xHeightMap = (xx < XSize) ? xx : XSize - 1;
			yHeightMap = (yy < YSize) ? yy : YSize - 1;
			// float zz = heightMap->GetTerrainHeightAtPoint(FVector(xx * Scale, yy * Scale, 0));
			imageValue = heightMap->GetImageValue(xHeightMap, yHeightMap);
			zz = heightMap->GetHeightFromPixelValue(imageValue, bits, maxPixelValue);
			Vertices.Add(FVector(xx * Scale, yy * Scale, zz * ZScale));
			UV0.Add(FVector2D(xx * UVScale, yy * UVScale));
		}
	}
	return {XSize, YSize};
}

void MeshTerrain::CreateTriangles(int XSize, int YSize) {
	int Vertex = 0;
	for (int xx = 0; xx < XSize; xx++) {
		for (int yy = 0; yy < YSize; yy++) {
			Triangles.Add(Vertex);		// Bottom left
			Triangles.Add(Vertex + 1);		// Bottom right
			Triangles.Add(Vertex + YSize + 1);		// Top left
			Triangles.Add(Vertex + 1); // Bottom right
			Triangles.Add(Vertex + YSize + 2); 		// Top right
			Triangles.Add(Vertex + YSize + 1);		// Top left
			Vertex++;
		}
		Vertex++;
	}
}

void MeshTerrain::DestroyRoads() {
	for (auto& Elem : _RoadsByType) {
		Elem.Value.Empty();
	}
	_RoadsByType.Empty();
}

void MeshTerrain::AddRoads(TMap<FString, FPolygon> roadsPaths) {
	for (auto& Elem : roadsPaths) {
		FString UName = Elem.Key;
		FString type = Elem.Value.type;
		if (!_RoadsByType.Contains(type)) {
			_RoadsByType.Add(type, {});
		}
		if (!_RoadsByType[type].Contains(UName)) {
			_RoadsByType[type].Add(UName, Elem.Value);
		}
	}
}

void MeshTerrain::DrawRoads(float stepFactor) {
	HeightMap* heightMap = HeightMap::GetInstance();
	auto [XSize, YSize, MapXSize] = heightMap->GetImageSize();
	float MetersPerPixel = (float)MapXSize / (float)XSize;

	float stepDistance, widthMeters;
	int pixelRange, pixelRangeHalf;
	TMap<FString, FString> pairs;

	for (auto& Elem1 : _RoadsByType) {
		FString type = Elem1.Key;

		// https://forums.unrealengine.com/t/how-to-reade-and-write-pixel-value-of-utexture2d/355103
		TArray<FColor> pixelData = {};
		// Initialize to all empty (black).
		for (int yy = 0; yy < YSize; yy++) {
			for (int xx = 0; xx < XSize; xx++) {
				pixelData.Add(FColor(0,0,0,0));
			}
		}

		int splatsCount = 0;
		for (auto& Elem : _RoadsByType[type]) {
			FString UName = Elem.Key;
			pairs = Lodash::PairsStringToObject(Elem.Value.pairsString);
			widthMeters = pairs.Contains("width") ? DataConvert::Float(pairs["width"]) : 3;
			TArray<FVector> vertices = Elem.Value.vertices;
			stepDistance = widthMeters * stepFactor;
			pixelRange = round(widthMeters / MetersPerPixel);
			pixelRangeHalf = round(pixelRange / 2);

			int verticesCount = vertices.Num();
			// - 1 because it is an open path, so one less edge than number of vertices.
			for (int vv = 0; vv < verticesCount - 1; vv++) {
				int indexNext = vv + 1;
				// Convert to 2D; ignore z.
				FVector2D vertexNext = FVector2D(vertices[indexNext].X, vertices[indexNext].Y);
				FVector2D vertex = FVector2D(vertices[vv].X, vertices[vv].Y);
				FVector2D path = vertexNext - vertex;
				float distanceRemaining = path.Size();
				int maxSteps = (int)ceil(distanceRemaining / stepDistance) + 10;
				int counter = 0;
				FVector2D currentPoint = vertex;
				FVector2D nextPoint;
				FVector pathStep3D = FVector(path.X, path.Y, 0).GetClampedToMaxSize(stepDistance);
				FVector2D pathStep = FVector2D(pathStep3D.X, pathStep3D.Y);

				auto [pixelX1, pixelY1] = heightMap->GetPixelFromXY(currentPoint.X, currentPoint.Y, XSize, YSize, MetersPerPixel);
				while (distanceRemaining > stepDistance) {
					nextPoint = currentPoint + pathStep;

					// Convert meters (world points) to image texture pixels.
					auto [pixelX, pixelY] = heightMap->GetPixelFromXY(currentPoint.X, currentPoint.Y, XSize, YSize, MetersPerPixel);
					// Based on width meters range, find near pixels.
					int xMin = pixelX - pixelRangeHalf;
					int xMax = pixelX + pixelRangeHalf;
					int yMin = pixelY - pixelRangeHalf;
					int yMax = pixelY + pixelRangeHalf;
					if (xMin < 0) {
						xMin = 0;
					}
					if (xMax >= XSize) {
						xMax = XSize - 1;
					}
					if (yMin < 0) {
						yMin = 0;
					}
					if (yMax >= YSize) {
						yMax = YSize - 1;
					}
					for (int yy = yMin; yy <= yMax; yy++) {
						for (int xx = xMin; xx <= xMax; xx++) {
							int arrayIndex = (yy * XSize) + xx;
							pixelData[arrayIndex] = FColor(255,255,255,0);
							splatsCount += 1;
						}
					}

					currentPoint = nextPoint;
					distanceRemaining = (vertexNext - currentPoint).Size();
					counter += 1;
					if (counter > maxSteps) {
						UE_LOG(LogTemp, Warning, TEXT("MeshTerrain.DrawRoads over maxSteps, breaking %s"), *UName);
						break;
					}
				}
			}
		}

		UTexture2D * Texture = UTexture2D::CreateTransient(XSize, YSize, PF_B8G8R8A8);
		void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(TextureData, pixelData.GetData(), 4 * XSize * YSize);
		Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
		Texture->UpdateResource();

		// TODO - causing crash after re-compile (works after fresh restart of Unreal Editor).
		// MaterialInstanceDynamic->SetTextureParameterValue(FName(TEXT("RoadSplatmapTexture")), Texture);
		// MaterialInstanceDynamic->SetTextureParameterValue(FName(TEXT("WalkPathSplatmapTexture")), Texture);
		FString TextureKey = type + "SplatmapTexture";
		MaterialInstanceDynamic->SetTextureParameterValue(FName(TextureKey), Texture);
		// ProceduralMesh->SetMaterial(0, Material);
	}
}
