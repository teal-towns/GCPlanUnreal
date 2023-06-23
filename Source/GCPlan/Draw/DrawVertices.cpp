#include "DrawVertices.h"

#include "../BuildingStructsActor.h"
#include "../Building/BuildingRoom.h"
#include "../Common/DataConvert.h"
#include "../Common/Lodash.h"
#include "../Common/UnrealGlobal.h"
#include "../Landscape/LandNature.h"
#include "../Landscape/SplineRoad.h"
#include "../Landscape/VerticesEdit.h"
#include "../Layout/LayoutPlace.h"
#include "../Layout/LayoutPolygon.h"
#include "../Layout/LayoutPolyLine.h"
#include "../Mesh/InstancedMesh.h"
#include "../Mesh/LoadContent.h"
#include "../Modeling/ModelBase.h"
#include "../Modeling/Furniture/ModelDesk.h"
#include "../Modeling/Furniture/ModelPlanterBox.h"
#include "../Modeling/Furniture/ModelTable.h"
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
			roads.Add(uName, FRoadPath(uName, uName, Elem.Value.vertices, 10, "road"));
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

	InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
	TArray<FString> types;
	TMap<FString, FString> pairs;
	FString type, meshKey;
	FVector location;
	// Buildings - assume all are points.
	// types = { "couch", "chair", "desk", "planterBox", "room", "table" };
	// polygons = verticesEdit->FilterByTypes(types);
	polygons = verticesEdit->FilterByShapes({ "point" });
	for (auto& Elem : polygons) {
		pairs = Lodash::PairsStringToObject(Elem.Value.pairsString);
		location = Elem.Value.vertices[0];
		pairs.Add("loc", DataConvert::VectorToString(location));
		if (pairs.Contains("mesh")) {
			meshKey = ModelBase::InstancedMeshFromPairs(pairs);
			if (meshKey.Len() > 0) {
				auto [location1, rotation, scale] = ModelBase::PairsToTransform(pairs);
				instancedMesh->CreateInstance(meshKey, location,
					DataConvert::VectorToRotator(rotation), scale);
			}
		} else {
			type = Elem.Value.type;
			if (type == "desk") {
				ModelDesk::Build(pairs);
			} else if (type == "planterBox") {
				ModelPlanterBox::Build(pairs);
			} else if (type == "room") {
				BuildingRoom::Build(pairs);
			} else if (type == "table") {
				ModelTable::Build(pairs);
			}
		}
	}

	// Plants
	LoadContent* loadContent = LoadContent::GetInstance();
	LayoutPolygon* layoutPolygon = LayoutPolygon::GetInstance();
	FPlaceParams placeParams;
	TArray<FString> meshNames, meshTypes;
	types = { "tree", "bush", "flower" };
	polygons = verticesEdit->FilterByTypes(types);
	for (auto& Elem : polygons) {
		pairs = Lodash::PairsStringToObject(Elem.Value.pairsString);
		meshNames = {};
		if (pairs.Contains("meshTypes")) {
			pairs["meshTypes"].ParseIntoArray(meshTypes, TEXT(","), true);
			meshNames = loadContent->GetMeshNamesByTypes(meshTypes);
		} else if (pairs.Contains("meshes")) {
			pairs["meshes"].ParseIntoArray(meshNames, TEXT(","), true);
		}
		if (!pairs.Contains("mesh")) {
			pairs.Add("mesh", "");
		}
		if (meshNames.Num() > 0) {
			for (int ii = 0; ii < meshNames.Num(); ii++) {
				pairs["mesh"] = meshNames[ii];
				ModelBase::InstancedMeshFromPairs(pairs);
			}

			placeParams.offsetAverage = pairs.Contains("placeOffsetAverage") ?
				DataConvert::Float(pairs["placeOffsetAverage"]) : 10;
			placeParams.rotMinX = pairs.Contains("placeRotMinX") ?
				DataConvert::Float(pairs["placeRotMinX"]) : 0;
			placeParams.rotMaxX = pairs.Contains("placeRotMaxX") ?
				DataConvert::Float(pairs["placeRotMaxX"]) : 0;
			placeParams.rotMinY = pairs.Contains("placeRotMinY") ?
				DataConvert::Float(pairs["placeRotMinY"]) : 0;
			placeParams.rotMaxY = pairs.Contains("placeRotMaxY") ?
				DataConvert::Float(pairs["placeRotMaxY"]) : 0;
			placeParams.rotMinZ = pairs.Contains("placeRotMinZ") ?
				DataConvert::Float(pairs["placeRotMinZ"]) : 0;
			placeParams.rotMaxZ = pairs.Contains("placeRotMaxZ") ?
				DataConvert::Float(pairs["placeRotMaxZ"]) : 360;
			placeParams.plane = pairs.Contains("placePlane") ? pairs["placePlane"] : "xy";
			layoutPolygon->PlaceInPolygon(Elem.Value.vertices, meshNames, placeParams);
		}
	}
}
