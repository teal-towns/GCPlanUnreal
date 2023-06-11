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

std::tuple<TMap<FString, FPlot>, int> PlotDivide::SubdividePlots(TMap<FString, FPlot> Plots, float minRadiusSkip,
	float minSquareMetersSkip, bool addRoads) {
	MeshTerrain* meshTerrain = MeshTerrain::GetInstance();
	SplineRoad* splineRoad = SplineRoad::GetInstance();

	int countNew = 0;
	if (addRoads) {
		// meshTerrain->DestroyRoads();
		splineRoad->DestroyRoads();
	}

	TMap<FString, FPlot> newPlots = {};
	FString uName, parentPlotUName;
	FVector center;
	TMap<FString, FPlot> PlotsTemp;
	FPlot plotTemp;
	float plotDistance, newAveragePlotDistance, squareMeters;
	TArray<FVector> bounds;
	TMap<FString, FRoadPath> roads;
	TArray<FVector2D> vertices2D;
	for (auto& Elem : Plots) {
		plotTemp = Elem.Value;
		parentPlotUName = Elem.Key;
		if (plotTemp.squareMeters < 0) {
			vertices2D = MathPolygon::PointsTo2D(plotTemp.vertices);
			plotTemp.squareMeters = MathPolygon::PolygonArea(vertices2D);
		}
		if (plotTemp.squareMeters > minSquareMetersSkip) {
			PlotsTemp = { { Elem.Key, Elem.Value } };
			plotDistance = Elem.Value.averagePlotDistance;

			auto [spacesVertices, posCenter, boundsRect] = PlotFillVoronoi::Fill(PlotsTemp, plotDistance);
			UE_LOG(LogTemp, Display, TEXT("LPA spacesVertices.Num %d %s %s"), spacesVertices.Num(), *boundsRect[0].ToString(), *boundsRect[1].ToString());

			// Buffer vertices and remove any spaces that are not valid.
			if (plotTemp.verticesBuffer != 0) {
				spacesVertices = PlotFillVoronoi::BufferAndRemoveVertices(spacesVertices, plotTemp.verticesBuffer,
					minRadiusSkip);
			}

			if (spacesVertices.Num() > 0) {
				bounds = MathPolygon::Bounds(spacesVertices[0]);
				newAveragePlotDistance = bounds[1].X - bounds[0].X;
				for (int ii = 0; ii < spacesVertices.Num(); ii++) {
					uName = Lodash::GetInstanceId("Plot");
					center = MathPolygon::GetPolygonCenter(spacesVertices[ii]);
					vertices2D = MathPolygon::PointsTo2D(spacesVertices[ii]);
					squareMeters = MathPolygon::PolygonArea(vertices2D);
					newPlots.Add(uName, FPlot(uName, uName, spacesVertices[ii], center,
						plotTemp.buildPattern, newAveragePlotDistance, squareMeters, parentPlotUName, {}));
					// Add this as a child of the existing plot too.
					Plots[parentPlotUName].childPlotUNames.Add(uName);
					countNew += 1;
				}

				if (addRoads) {
					roads = BuildingRoad::BetweenSpaces(spacesVertices, plotTemp.verticesBuffer);
					// meshTerrain->AddRoads(roads);
					splineRoad->AddRoads(roads);
				}
			}
		}
		// Return the parent plot too, as it may have been updated with children.
		newPlots.Add(parentPlotUName, Plots[parentPlotUName]);
	}
	return { newPlots, countNew };
}

bool PlotDivide::AddRoads(TMap<FString, FPlot> Plots) {
	SplineRoad* splineRoad = SplineRoad::GetInstance();
	splineRoad->DestroyRoads();
	TArray<TArray<FVector>> spacesVertices = {};
	float verticesBuffer = -999;
	for (auto& Elem : Plots) {
		// Only do final plots.
		if (Elem.Value.childPlotUNames.Num() < 1) {
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
