#include "PlotDivide.h"

#include "../BuildingStructsActor.h"
#include "../Building/BuildingRoad.h"
#include "../Common/Lodash.h"
#include "../Common/MathPolygon.h"
#include "../Landscape/MeshTerrain.h"
#include "../Landscape/SplineRoad.h"
#include "PlotFillVoronoi.h"

PlotDivide::PlotDivide() {
}

PlotDivide::~PlotDivide() {
}

std::tuple<TMap<FString, FPolygon>, int> PlotDivide::SubdividePlots(TMap<FString, FPolygon> polygons,
	float minRadiusSkip, float minSquareMetersSkip, bool addRoads) {
	MeshTerrain* meshTerrain = MeshTerrain::GetInstance();
	SplineRoad* splineRoad = SplineRoad::GetInstance();

	int countNew = 0;
	if (addRoads) {
		// meshTerrain->DestroyRoads();
		splineRoad->DestroyRoads();
	}

	TMap<FString, FPolygon> newPolygons = {};
	FString uName, parentUName;
	FVector center;
	TArray<FVector> vertices;
	TArray<TArray<FVector>> verticesTemp;
	FPolygon polygonTemp;
	float averageChildDiameter, newAverageChildDiameter, squareMeters;
	TArray<FVector> bounds;
	TMap<FString, FRoadPath> roads;
	TArray<FVector2D> vertices2D;
	for (auto& Elem : polygons) {
		polygonTemp = Elem.Value;
		parentUName = Elem.Key;
		if (polygonTemp.squareMeters < 0) {
			vertices2D = MathPolygon::PointsTo2D(polygonTemp.vertices);
			polygonTemp.squareMeters = MathPolygon::PolygonArea(vertices2D);
		}
		if (polygonTemp.squareMeters > minSquareMetersSkip) {
			verticesTemp = { Elem.Value.vertices };
			averageChildDiameter = Elem.Value.averageChildDiameter;

			auto [spacesVertices, posCenter, boundsRect] = PlotFillVoronoi::Fill(verticesTemp, averageChildDiameter);
			// UE_LOG(LogTemp, Display, TEXT("PlotDivide.SubdividePlots spacesVertices.Num %d %s %s"), spacesVertices.Num(), *boundsRect[0].ToString(), *boundsRect[1].ToString());

			// Buffer vertices and remove any spaces that are not valid.
			if (polygonTemp.verticesBuffer != 0) {
				spacesVertices = PlotFillVoronoi::BufferAndRemoveVertices(spacesVertices, polygonTemp.verticesBuffer,
					minRadiusSkip);
			}

			if (spacesVertices.Num() > 0) {
				bounds = MathPolygon::Bounds(spacesVertices[0]);
				newAverageChildDiameter = bounds[1].X - bounds[0].X;
				for (int ii = 0; ii < spacesVertices.Num(); ii++) {
					uName = Lodash::GetInstanceId("Plot");
					center = MathPolygon::GetPolygonCenter(spacesVertices[ii]);
					vertices2D = MathPolygon::PointsTo2D(spacesVertices[ii]);
					squareMeters = MathPolygon::PolygonArea(vertices2D);
					newPolygons.Add(uName, FPolygon(uName, uName, spacesVertices[ii], center,
						polygonTemp.type, polygonTemp.shape, polygonTemp.tags, "", squareMeters, parentUName, {},
						polygonTemp.verticesBuffer, newAverageChildDiameter));
					// Add this as a child of the existing one too.
					polygons[parentUName].childUNames.Add(uName);
					countNew += 1;
				}

				if (addRoads) {
					roads = BuildingRoad::BetweenSpaces(spacesVertices, polygonTemp.verticesBuffer);
					// meshTerrain->AddRoads(roads);
					splineRoad->AddRoads(roads);
				}
			}
		}
		// Return the parent too, as it may have been updated with children.
		newPolygons.Add(parentUName, polygons[parentUName]);
	}
	return { newPolygons, countNew };
}

bool PlotDivide::AddRoads(TMap<FString, FPolygon> polygons) {
	SplineRoad* splineRoad = SplineRoad::GetInstance();
	splineRoad->DestroyRoads();
	TArray<TArray<FVector>> spacesVertices = {};
	float verticesBuffer = -999;
	for (auto& Elem : polygons) {
		// Only do final ones.
		if (Elem.Value.childUNames.Num() < 1) {
			spacesVertices.Add(Elem.Value.vertices);
			if (verticesBuffer == -999) {
				verticesBuffer = Elem.Value.verticesBuffer;
			}
		}
	}
	TMap<FString, FRoadPath> roads = BuildingRoad::BetweenSpaces(spacesVertices, verticesBuffer);
	splineRoad->AddRoads(roads);
	return true;
}
