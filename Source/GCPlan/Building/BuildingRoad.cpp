#include "BuildingRoad.h"

#include "../BuildingStructsActor.h"
#include "../Common/Lodash.h"
#include "../Common/MathPolygon.h"
#include "../Landscape/HeightMap.h"

BuildingRoad::BuildingRoad() {
}

BuildingRoad::~BuildingRoad() {
}

TMap<FString, FRoadPath> BuildingRoad::BetweenSpaces(TArray<TArray<FVector>> spacesVertices,
	float verticesBuffer) {
	TMap<FString, FRoadPath> roads = {};
	TArray<FString> usedKeys = {};
	HeightMap* heightMap = HeightMap::GetInstance();
	for (int ii = 0; ii < spacesVertices.Num(); ii++) {
		// Unbuffer vertices.
		FVector posCenterGround = MathPolygon::GetPolygonCenter(spacesVertices[ii]);
		spacesVertices[ii] = MathPolygon::BufferVertices(spacesVertices[ii], posCenterGround, -1 * verticesBuffer);
		int verticesCount = spacesVertices[ii].Num();
		for (int vv = 0; vv < verticesCount; vv++) {
			int indexNext = vv < verticesCount - 1 ? vv + 1 : 0;
			FVector vertexNext = spacesVertices[ii][indexNext];
			FVector vertex = spacesVertices[ii][vv];

			// Ensure z is correct for each vertex.
			vertex.Z = heightMap->GetTerrainHeightAtPoint(vertex);
			vertexNext.Z = heightMap->GetTerrainHeightAtPoint(vertexNext);

			FVector edgeCenter = vertex + (vertexNext - vertex) * 0.5;
			FString uName = "BuildingRoad_" + Lodash::ToFixed(edgeCenter.X, 1) + "_" + Lodash::ToFixed(edgeCenter.Y, 1);
			if (!usedKeys.Contains(uName)) {
				roads.Add(uName, FRoadPath(uName, uName, { vertex, vertexNext }, 10, "Road"));
				usedKeys.Add(uName);
			}
		}
	}
	return roads;
}
