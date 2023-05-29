#include "BuildingRoad.h"

#include "../Common/Lodash.h"
#include "../Common/MathPolygon.h"
#include "../Landscape/HeightMap.h"

BuildingRoad::BuildingRoad() {
}

BuildingRoad::~BuildingRoad() {
}

TMap<FString, TArray<FVector>> BuildingRoad::BetweenSpaces(TArray<TArray<FVector>> spacesVertices) {
	TMap<FString, TArray<FVector>> roadsVertices = {};
	TArray<FString> usedKeys = {};
	HeightMap* heightMap = HeightMap::GetInstance();
	for (int ii = 0; ii < spacesVertices.Num(); ii++) {
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
				roadsVertices.Add(uName, { vertex, vertexNext });
				usedKeys.Add(uName);
				// TODO - add piece if want 3D model too.
				// // Only add if creating new and does not already exist.
				// if (!pieces.ContainsKey(uName) && createNew) {
				// 	usedKeys.Add(uName);
				// 	List<Vector3> verticesRoad = new List<Vector3>() { vertex, vertexNext };
				// 	pieces.Add(uName, new PieceClass(uName, "path", edgeCenter, verticesRoad,
				// 		rot, scale, category_: "lanes2", type_: "road",
				// 		prefabName_: "Road1PartLOD1"));

				// 	// TODO - add roundabouts?
				// }
			}
		}
	}
	return roadsVertices;
}
