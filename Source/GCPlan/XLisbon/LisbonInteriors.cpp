#include "LisbonInteriors.h"

#include "../BuildingStructsActor.h"
#include "../Common/DataConvert.h"
#include "../Common/Lodash.h"
#include "../Landscape/VerticesEdit.h"
#include "../LayoutModel/LMLights.h"
#include "../LayoutModel/LMRoomPlants.h"
#include "../LayoutModel/LMWall.h"
#include "../LayoutModel/OfficeRoom/LMCafeteria.h"
#include "../LayoutModel/OfficeRoom/LMConferenceRoom.h"
#include "../LayoutModel/OfficeRoom/LMLobby.h"
#include "../LayoutModel/OfficeRoom/LMOfficeDesks.h"
#include "../LayoutModel/OfficeRoom/LMServerRoom.h"
#include "../Mesh/LoadContent.h"
#include "../Modeling/ModelBase.h"

LisbonInteriors* LisbonInteriors::pinstance_{nullptr};
std::mutex LisbonInteriors::mutex_;

LisbonInteriors::LisbonInteriors() {
}

LisbonInteriors::~LisbonInteriors() {
}

LisbonInteriors *LisbonInteriors::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new LisbonInteriors();
	}
	return pinstance_;
}

TMap<FString, FPolygon> LisbonInteriors::Rooms(float zOffset) {
	FModelCreateParams createParamsIn;
	FVector rotation = FVector(0,0,0), location = FVector(0,0,0), scale = FVector(1,1,1);
	FString uName, type, pairsString, scaleString;
	TArray<FVector> vertices;
	TMap<FString, FPolygon> polygons = {};
	LoadContent* loadContent = LoadContent::GetInstance();

	bool addLights = false;

	// Hardcoded - varies by project (usually these are manually drawn / set via UI).
	FModelParams modelParams;
	FModelCreateParams createParams;
	// createParams.rotation = FVector(0,0,30);
	createParamsIn.offset = FVector(0,0,zOffset);
	FVector roomSize = FVector(10,12,4);
	FVector roomSizePrev;
	FVector roomPos = FVector(0,0,0);
	FLayoutWall wallParams;
	wallParams.meshesByTags["door"] = { "doorWood1" };
	wallParams.doorRotation = FVector(0,0,180);

	FLayoutLights lightsParams;

	FString wallMaterial = "white";
	FString floorMaterial = "marbleTile";
	wallMaterial = "concretePanelWhite";
	// floorMaterial = "greyWood";
	floorMaterial = "concreteFloor";
	FString floorMaterialDefault = floorMaterial;

	// Lobby
	// Walls
	uName = Lodash::GetInstanceId("Room");
	floorMaterial = "marbleTile";
	pairsString = "meshRule=roomCube&mat=" + wallMaterial + "&bottomMat=" + floorMaterial + "&skipBack=1&skipFront=1&scale=" +
		DataConvert::VectorToString(roomSize) + ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { location + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "room", "point", pairsString));
	// Back wall & door
	wallParams.doorLeft = roomSize.Y - 1.5;
	wallParams.offset = roomPos + FVector(roomSize.X / -2,0,0);
	wallParams.wallRotation = FVector(0,90,0);
	wallParams.rotation = FVector(0,0,0);
	wallParams.materialKey = wallMaterial;
	LMWall::Wall(FVector(roomSize.Z, roomSize.Y, 0.01), modelParams, createParamsIn, wallParams);
	// Front wall & door
	wallParams.doorLeft = roomSize.Y - 1.5;
	wallParams.offset = roomPos + FVector(roomSize.X / 2,0,0);
	wallParams.wallRotation = FVector(0,90,0);
	wallParams.rotation = FVector(0,0,0);
	wallParams.materialKey = wallMaterial;
	LMWall::Wall(FVector(roomSize.Z, roomSize.Y, 0.01), modelParams, createParamsIn, wallParams);
	lightsParams.offset = roomPos;
	if (addLights) {
		LMLights::Ceiling(roomSize, modelParams, createParamsIn, lightsParams);
	}
	wallParams.materialKey = wallMaterial;

	createParams.offset = createParamsIn.offset + roomPos;
	// LMLobby::Create(roomSize, modelParams, createParams);
	uName = Lodash::GetInstanceId("Room");
	location = roomPos + FVector(1,0,0);
	scale = FVector(1.2,1.2,1);
	pairsString = "mesh=buildingLobby&scale=" +
		DataConvert::VectorToString(scale) + ModelBase::AddRotationString(createParamsIn.rotation + FVector(0,0,-90));
	vertices = { location + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "roomPart", "point", pairsString));
	FWallPlanterBox plantParams;
	plantParams.walls = { "left", "back" };
	FString meshes = Lodash::Join(loadContent->GetMeshNamesByTags({"indoorBush"}), ",");
	plantParams.pairsStringPlants = "meshes=" + meshes + "&placeOffsetAverage=0.3";
	LMRoomPlants::WallPlanterBox(roomSize, modelParams, createParamsIn, plantParams);

	roomSizePrev = roomSize;


	// Cafeteria - in front of lobby
	roomSize = FVector(10,12,4);
	roomPos = roomPos + FVector(roomSizePrev.X / 2 + roomSize.X / 2, 0, 0);
	// Walls
	uName = Lodash::GetInstanceId("Room");
	location = roomPos;
	floorMaterial = "concreteFloor";
	pairsString = "meshRule=roomCube&mat=" + wallMaterial + "&bottomMat=" + floorMaterial + "&skipBack=1&skipLeft=1&scale=" +
		DataConvert::VectorToString(roomSize) + ModelBase::AddRotationString(createParamsIn.rotation);
	floorMaterial = floorMaterialDefault;
	vertices = { location + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "room", "point", pairsString));
	// Shared back wall, so already have it.
	// Left wall & door
	wallParams.doorLeft = 0.5;
	wallParams.offset = roomPos + FVector(0, roomSize.Y / -2, 0);
	wallParams.wallRotation = FVector(0,90,-90);
	wallParams.rotation = FVector(0,0,-90);
	wallParams.doorRotation = FVector(0,0,90);
	LMWall::Wall(FVector(roomSize.Z, roomSize.X, 0.01), modelParams, createParamsIn, wallParams);
	lightsParams.offset = roomPos;
	if (addLights) {
		LMLights::Ceiling(roomSize, modelParams, createParamsIn, lightsParams);
	}
	wallParams.doorRotation = FVector(0,0,180);
	// Wallpaper
	uName = Lodash::GetInstanceId("Wallpaper");
	scale = FVector(5.5, 0.01, roomSize.Z);
	location = roomPos + FVector(0,roomSize.Y / -2 + 0.01,0);
	pairsString = "mesh=cube&mat=wallpaperTropical1&scale=" +
		DataConvert::VectorToString(scale) + ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { location + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "roomPart", "point", pairsString));

	createParams.offset = createParamsIn.offset + roomPos;
	LMCafeteria::Create(roomSize, modelParams, createParams);
	roomSizePrev = roomSize;


	// Office desks - to left of cafeteria
	roomSize = FVector(11,12,4);
	roomPos = roomPos + FVector(0, roomSizePrev.Y / -2 + roomSize.Y / -2, 0);
	// Walls
	uName = Lodash::GetInstanceId("Room");
	location = roomPos;
	floorMaterial = "concreteFloorBlack";
	pairsString = "meshRule=roomCube&mat=" + wallMaterial + "&bottomMat=" + floorMaterial + "&skipRight=1&skipFront=1&scale=" +
		DataConvert::VectorToString(roomSize) + ModelBase::AddRotationString(createParamsIn.rotation);
	floorMaterial = floorMaterialDefault;
	vertices = { location + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "room", "point", pairsString));
	// Shared right wall, so already have it.
	// Front wall & door
	wallParams.materialKey = wallMaterial;
	wallParams.doorLeft = roomSize.Y - 1.5;
	wallParams.offset = roomPos + FVector(roomSize.X / 2,0,0);
	wallParams.wallRotation = FVector(0,90,0);
	wallParams.rotation = FVector(0,0,0);
	LMWall::Wall(FVector(roomSize.Z, roomSize.Y, 0.01), modelParams, createParamsIn, wallParams);
	lightsParams.offset = roomPos;
	lightsParams.lightIntensity = 2500;
	if (addLights) {
		LMLights::Ceiling(roomSize, modelParams, createParamsIn, lightsParams);
	}
	lightsParams.lightIntensity = 5000;
	wallParams.materialKey = wallMaterial;
	// Wallpaper
	uName = Lodash::GetInstanceId("Wallpaper");
	scale = FVector(0.01, 6, roomSize.Z);
	location = roomPos + FVector(roomSize.X / 2 - 0.01, 0,0);
	pairsString = "mesh=cube&mat=greenWall1&scale=" +
		DataConvert::VectorToString(scale) + ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { location + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "roomPart", "point", pairsString));

	createParams.offset = createParamsIn.offset + roomPos;
	LMOfficeDesks::Desks(roomSize, modelParams, createParams);
	roomSizePrev = roomSize;


	// Lounge - front of office
	roomSize = FVector(10,12,4);
	roomPos = roomPos + FVector(roomSizePrev.X / 2 + roomSize.X / 2, 0, 0);
	// Walls
	uName = Lodash::GetInstanceId("Room");
	location = roomPos;
	floorMaterial = "greyWood";
	pairsString = "meshRule=roomCube&mat=" + wallMaterial + "&bottomMat=" + floorMaterial + "&skipBack=1&skipLeft=1&scale=" +
		DataConvert::VectorToString(roomSize) + ModelBase::AddRotationString(createParamsIn.rotation);
	floorMaterial = floorMaterialDefault;
	vertices = { location + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "room", "point", pairsString));
	// Shared back wall, so already have it.
	wallParams.materialKey = wallMaterial;
	// Left wall & door
	wallParams.doorLeft = roomSize.X - 2;
	wallParams.offset = roomPos + FVector(0, roomSize.Y / -2,0);
	wallParams.wallRotation = FVector(0,90,-90);
	wallParams.rotation = FVector(0,0,-90);
	wallParams.doorRotation = FVector(0,0,90);
	LMWall::Wall(FVector(roomSize.Z, roomSize.X, 0.01), modelParams, createParamsIn, wallParams);
	lightsParams.offset = roomPos;
	lightsParams.lightIntensity = 2500;
	if (addLights) {
		LMLights::Ceiling(roomSize, modelParams, createParamsIn, lightsParams);
	}
	lightsParams.lightIntensity = 5000;
	wallParams.doorRotation = FVector(0,0,180);
	wallParams.materialKey = wallMaterial;
	// Wallpaper
	uName = Lodash::GetInstanceId("Wallpaper");
	scale = FVector(0.01, 6, roomSize.Z);
	location = roomPos + FVector(roomSize.X / 2 - 0.01, 0,0);
	pairsString = "mesh=cube&mat=greenWall2&scale=" +
		DataConvert::VectorToString(scale) + ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { location + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "roomPart", "point", pairsString));
	// Wallpaper
	uName = Lodash::GetInstanceId("Wallpaper");
	scale = FVector(6, 0.01, roomSize.Z);
	location = roomPos + FVector(roomSize.X / -2 + scale.X / 2, roomSize.Y / -2 + 0.01,0);
	pairsString = "mesh=cube&mat=wallpaperTropical2&scale=" +
		DataConvert::VectorToString(scale) + ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { location + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "roomPart", "point", pairsString));
	// Wallpaper
	uName = Lodash::GetInstanceId("Wallpaper");
	scale = FVector(4, 0.01, roomSize.Z);
	location = roomPos + FVector(roomSize.X / 2 - scale.X / 2, roomSize.Y / 2 - 0.01,0);
	pairsString = "mesh=cube&mat=greenWall3&scale=" +
		DataConvert::VectorToString(scale) + ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { location + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "roomPart", "point", pairsString));

	createParams.offset = createParamsIn.offset + roomPos;
	LMLobby::TwoTables(roomSize, modelParams, createParams);
	roomSizePrev = roomSize;


	// Conference room - left of lounge
	roomSize = FVector(10,12,4);
	roomPos = roomPos + FVector(0, roomSizePrev.Y / -2 + roomSize.Y / -2, 0);
	// Walls
	uName = Lodash::GetInstanceId("Room");
	location = roomPos;
	// floorMaterial = "concreteFloorBlack";
	floorMaterial = "marbleTile";
	pairsString = "meshRule=roomCube&mat=" + wallMaterial + "&bottomMat=" + floorMaterial + "&skipRight=1&scale=" +
		DataConvert::VectorToString(roomSize) + ModelBase::AddRotationString(createParamsIn.rotation);
	floorMaterial = floorMaterialDefault;
	vertices = { location + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "room", "point", pairsString));
	// Shared right wall, so already have it.
	lightsParams.offset = roomPos;
	if (addLights) {
		LMLights::Ceiling(roomSize, modelParams, createParamsIn, lightsParams);
	}

	createParams.offset = createParamsIn.offset + roomPos;
	LMConferenceRoom::Create(roomSize, modelParams, createParams);
	roomSizePrev = roomSize;


	// Server room (not directly connected)
	roomSize = FVector(10,12,4);
	roomPos = roomPos + FVector(roomSizePrev.X / 2 + roomSize.X / 2, 0, 0) + FVector(1,0,0);
	// Walls
	uName = Lodash::GetInstanceId("Room");
	location = roomPos;
	pairsString = "meshRule=roomCube&mat=white&bottomMat=black&scale=" + DataConvert::VectorToString(roomSize) +
		ModelBase::AddRotationString(createParamsIn.rotation);
	vertices = { location + createParamsIn.offset };
	vertices = ModelBase::Vertices(vertices, createParamsIn);
	polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0,0,0), "room", "point", pairsString));
	lightsParams.offset = roomPos;
	if (addLights) {
		LMLights::Ceiling(roomSize, modelParams, createParamsIn, lightsParams);
	}

	createParams.offset = createParamsIn.offset + roomPos;
	LMServerRoom::Create(roomSize, modelParams, createParams);

	VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	verticesEdit->AddAndSave(polygons);
	return polygons;
}
