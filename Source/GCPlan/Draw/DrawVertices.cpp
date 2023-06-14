#include "DrawVertices.h"

#include "../BuildingStructsActor.h"
#include "../Common/UnrealGlobal.h"
#include "../Landscape/LandNature.h"
#include "../Landscape/SplineRoad.h"
#include "../Landscape/VerticesEdit.h"
#include "../Layout/LayoutPlace.h"
#include "../Layout/LayoutPolygon.h"
#include "../Layout/LayoutPolyLine.h"
#include "../Mesh/InstancedMesh.h"
#include "../Mesh/LoadContent.h"
#include "../Plot/PlotBuild.h"
#include "../Plot/PlotDivide.h"

DrawVertices::DrawVertices() {
}

DrawVertices::~DrawVertices() {
}

void DrawVertices::LoadVertices() {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	SplineRoad* splineRoad = SplineRoad::GetInstance();
	splineRoad->DestroyRoads();

	// PlotData* plotData = PlotData::GetInstance();
	// TMap<FString, FPlot> plots = plotData->LoadAndSubdividePlots();
	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->LoadFromFiles();
	verticesEdit->CheckSubdividePolygons("plot");
	verticesEdit->Hide();
	// verticesEdit->ImportPolygons(ABuildingStructsActor::PlotsToPolygons(plots));

	TMap<FString, FPolygon> polygons = verticesEdit->FilterByTypes({ "plot" });
	PlotDivide::AddRoads(polygons);
	TArray<FLand> lands = PlotBuild::CreateLands(polygons);
	PlotBuild::DrawLands(lands);

	// Roads.
	FString uName;
	TMap<FString, FPolygon> polygonsRoad = verticesEdit->FilterByTypes({ "road" });
	if (polygonsRoad.Num() > 0) {
		TMap<FString, FRoadPath> roads = {};
		for (auto& Elem : polygonsRoad) {
			uName = Elem.Key;
			roads.Add(uName, FRoadPath(uName, uName, Elem.Value.vertices, 10, 	"road"));
		}
		splineRoad->AddRoads(roads);
	}
	// MeshTerrain* meshTerrain = MeshTerrain::GetInstance();
	// meshTerrain->DrawRoads();
	splineRoad->DrawRoads();

	// Place nature on land.
	if (unrealGlobal->_settings->performanceQualityLevel >= 8) {
		LandNature::PlaceNature();
	}
}
