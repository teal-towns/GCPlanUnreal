#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildingStructsActor.generated.h"

UCLASS()
class GCPLAN_API ABuildingStructsActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABuildingStructsActor();

protected:
	virtual void BeginPlay() override;

public:	
	// virtual void Tick(float DeltaTime) override;

	// static FPolygonSimplified PolygonToSimplified(FPolygon polygon);
	// static FPolygon PolygonFromSimplified(FPolygonSimplified polygonSimplified);
	// static TMap<FString, FPolygonSimplified> PolygonsToSimplified(TMap<FString, FPolygon> polygons);
	// static TMap<FString, FPolygon> PolygonsFromSimplified(TMap<FString, FPolygonSimplified> polygonSimplified);

	static FPlot PolygonToPlot(FPolygon obj);
	static FPolygon PlotToPolygon(FPlot obj);
	static TMap<FString, FPlot> PolygonsToPlots(TMap<FString, FPolygon> objs);
	static TMap<FString, FPolygon> PlotsToPolygons(TMap<FString, FPlot> objs);

	static FPlot UpdatePlotFromPolygon(FPolygon polygon, FPlot plot);

	static FPlotSimplified PlotToSimplified(FPlot plot);
	static FPlot PlotFromSimplified(FPlotSimplified plotSimplified);
	static TMap<FString, FPlotSimplified> PlotsToSimplified(TMap<FString, FPlot> plots);
	static TMap<FString, FPlot> PlotsFromSimplified(TMap<FString, FPlotSimplified> plotSimplified);

};

USTRUCT()
struct FMapStringFloat {
	GENERATED_BODY()

	UPROPERTY()
	TMap<FString, float> v;

	FMapStringFloat() {};
	FMapStringFloat(TMap<FString, float> v_) {
		v = v_;
	};
};

USTRUCT()
struct FBuildingBlueprintJsonData {
	GENERATED_BODY()

	UPROPERTY()
	TArray<FMapStringFloat> openEdgePositions;
	UPROPERTY()
	int floor;
	UPROPERTY()
	FString edgeType;

	FBuildingBlueprintJsonData() {};
	FBuildingBlueprintJsonData(TArray<FMapStringFloat> openEdgePositions_, int floor_, FString edgeType_) {
		openEdgePositions = openEdgePositions_, floor = floor_, edgeType = edgeType_;
	};
};

USTRUCT()
struct FBuildData {
	GENERATED_BODY()

	UPROPERTY()
	FVector posCenterGround;
	UPROPERTY()
	FVector posCurrentGround;
	UPROPERTY()
	int unitsCreated = 0;
	UPROPERTY()
	int unitLengthsCreatedOriginRing = 0;
	UPROPERTY()
	TArray<FVector2D> filledCoords = {};

	FBuildData() {};
	FBuildData(FVector posCenterGround_, FVector posCurrentGround_) {
		posCenterGround = posCenterGround_, posCurrentGround = posCurrentGround_;
	};
};

USTRUCT()
struct FBuildingStats {
	GENERATED_BODY()

	UPROPERTY()
	int unitCount;
	UPROPERTY()
	int ringWidthUnits;
	UPROPERTY()
	int roofUnitCount = 0;
	UPROPERTY()
	float averageRadiusInner = 0;
	UPROPERTY()
	float averageRadiusOuter = 0;
	UPROPERTY()
	int residentCount = 0;
};

USTRUCT()
struct FLandGameObject {
	GENERATED_BODY()

	UPROPERTY()
	FString clone_name;
	UPROPERTY()
	TMap<FString, float> position;
	UPROPERTY()
	TMap<FString, float> rotation;
	UPROPERTY()
	TMap<FString, float> scale;
	UPROPERTY()
	TArray<FString> child_renames;
	UPROPERTY()
	FString jsonDataString = "";

	FLandGameObject() {};
	FLandGameObject(FString clone_name_, FVector position_, FRotator rotation_,
		FVector scale_, TArray<FString> child_renames_, FString jsonDataString_) {
		clone_name = clone_name_, child_renames = child_renames_, jsonDataString = jsonDataString_;
		// compile errors - can we use static functions in constructors, or no?
		// position = DataConvert::VectorToDict(position_);
		// rotation = DataConvert::RotatorToDict(rotation_);
		// scale = DataConvert::VectorToDict(scale_);
		position = TMap<FString, float> {
			{ "x", (float)position_.X },
			{ "y", (float)position_.Y },
			{ "z", (float)position_.Z },
		};
		rotation = TMap<FString, float> {
			{ "x", (float)rotation_.Roll },
			{ "y", (float)rotation_.Pitch },
			{ "z", (float)rotation_.Yaw },
		};
		scale = TMap<FString, float> {
			{ "x", (float)scale_.X },
			{ "y", (float)scale_.Y },
			{ "z", (float)scale_.Z },
		};
	};
};

USTRUCT()
struct FLand {
	GENERATED_BODY()

	UPROPERTY()
	TMap<FString, FLandGameObject> game_objects = {};
	UPROPERTY()
	FString land_id = "";
};

USTRUCT()
struct FBuildingBlueprint {
	GENERATED_BODY()

	UPROPERTY()
	int valid = 0;
	UPROPERTY()
	FString msg = "";
	UPROPERTY()
	FLand land;
	UPROPERTY()
	FBuildingStats stats;
	UPROPERTY()
	FVector posCenterGround = FVector(0,0,0);

	FBuildingBlueprint() {};
	FBuildingBlueprint(FLand land_, int valid_ = 0) {
		land = land_, valid = valid_;
	};
};

USTRUCT()
struct FPlot {
	GENERATED_BODY()

	UPROPERTY()
	FString _id;
	UPROPERTY()
	FString uName;
	UPROPERTY()
	TArray<FVector> vertices;
	UPROPERTY()
	FVector posCenter;
	UPROPERTY()
	FString buildPattern;
	UPROPERTY()
	float averagePlotDistance;
	UPROPERTY()
	float squareMeters;
	UPROPERTY()
	FString parentPlotUName;
	UPROPERTY()
	TArray<FString> childPlotUNames;
	UPROPERTY()
	float verticesBuffer;

	FPlot() {};
	FPlot(FString id_, FString uName_, TArray<FVector> vertices_, FVector posCenter_,
		FString buildPattern_ = "", float averagePlotDistance_ = 100, float squareMeters_ = -1,
		FString parentPlotUName_ = "", TArray<FString> childPlotUNames_ = {}, float verticesBuffer_ = -25) {
		_id = id_;
		uName = uName_;
		vertices = vertices_;
		posCenter = posCenter_;
		buildPattern = buildPattern_;
		averagePlotDistance = averagePlotDistance_;
		squareMeters = squareMeters_;
		parentPlotUName = parentPlotUName_;
		childPlotUNames = childPlotUNames_;
		verticesBuffer = verticesBuffer_;
	};
};

USTRUCT()
struct FPlotSimplified {
	GENERATED_BODY()

	UPROPERTY()
	FString _id;
	UPROPERTY()
	FString uName;
	UPROPERTY()
	TArray<FMapStringFloat> vertices;
	UPROPERTY()
	FMapStringFloat posCenter;
	UPROPERTY()
	FString buildPattern;
	UPROPERTY()
	float averagePlotDistance;
	UPROPERTY()
	float squareMeters;
	UPROPERTY()
	FString parentPlotUName;
	UPROPERTY()
	TArray<FString> childPlotUNames;
	UPROPERTY()
	float verticesBuffer;

	FPlotSimplified() {};
	FPlotSimplified(FString id_, FString uName_, TArray<FMapStringFloat> vertices_, FMapStringFloat posCenter_,
		FString buildPattern_ = "", float averagePlotDistance_ = 100, float squareMeters_ = -1,
		FString parentPlotUName_ = "", TArray<FString> childPlotUNames_ = {}, float verticesBuffer_ = -25) {
		_id = id_;
		uName = uName_;
		vertices = vertices_;
		posCenter = posCenter_;
		buildPattern = buildPattern_;
		averagePlotDistance = averagePlotDistance_;
		squareMeters = squareMeters_;
		parentPlotUName = parentPlotUName_;
		childPlotUNames = childPlotUNames_;
		verticesBuffer = verticesBuffer_;
	};
};

USTRUCT()
struct FRoadPath {
	GENERATED_BODY()

	UPROPERTY()
	FString _id;
	UPROPERTY()
	FString uName;
	UPROPERTY()
	TArray<FVector> vertices;
	UPROPERTY()
	float widthMeters;
	UPROPERTY()
	FString type;
	UPROPERTY()
	float laneCount;

	FRoadPath() {};
	FRoadPath(FString id_, FString uName_, TArray<FVector> vertices_, float widthMeters_, FString type_, float laneCount_ = 1) {
		_id = id_;
		uName = uName_;
		vertices = vertices_;
		widthMeters = widthMeters_;
		type = type_;
		laneCount = laneCount_;
	};
};

USTRUCT()
struct FPolygon {
	GENERATED_BODY()

	UPROPERTY()
	FString _id;
	UPROPERTY()
	FString uName;
	UPROPERTY()
	TArray<FVector> vertices;
	UPROPERTY()
	FVector posCenter;
	UPROPERTY()
	FString type;
	UPROPERTY()
	int closedLoop;
	UPROPERTY()
	FString jsonDataString;

	FPolygon() {};
	FPolygon(FString id_, FString uName_, TArray<FVector> vertices_, FVector posCenter_,
		FString type_ = "", int closedLoop_ = -1, FString jsonDataString_ = "") {
		_id = id_;
		uName = uName_;
		vertices = vertices_;
		posCenter = posCenter_;
		type = type_;
		if (closedLoop_ == -1) {
			closedLoop = (type == "road") ? 0 : 1;
		} else {
			closedLoop = closedLoop_;
		}
		jsonDataString = jsonDataString_;
	};
};

// USTRUCT()
// struct FPolygonSimplified {
// 	GENERATED_BODY()

// 	UPROPERTY()
// 	FString _id;
// 	UPROPERTY()
// 	FString uName;
// 	UPROPERTY()
// 	TArray<FMapStringFloat> vertices;
// 	UPROPERTY()
// 	FMapStringFloat posCenter;
// 	UPROPERTY()
// 	FString type;
// 	UPROPERTY()
// 	int closedLoop;
// 	UPROPERTY()
// 	FString jsonDataString;

// 	FPolygonSimplified() {};
// 	FPolygonSimplified(FString id_, FString uName_, TArray<FMapStringFloat> vertices_, FMapStringFloat posCenter_,
// 		FString type_ = "", int closedLoop_ = 1, FString jsonDataString_ = "") {
// 		_id = id_;
// 		uName = uName_;
// 		vertices = vertices_;
// 		posCenter = posCenter_;
// 		type = type_;
// 		closedLoop = closedLoop_;
// 		jsonDataString = jsonDataString_;
// 	};
// };

USTRUCT()
struct FVerticesEditSelectedObject {
	GENERATED_BODY()

	UPROPERTY()
	FString uName;
	UPROPERTY()
	FString objectType;
	UPROPERTY()
	int objectIndex;

	// FVerticesEditSelectedObject() {};
	FVerticesEditSelectedObject(FString uName_ = "", FString objectType_ = "", int objectIndex_ = -1) {
		uName = uName_;
		objectType = objectType_;
		objectIndex = objectIndex_;
	};
};

// USTRUCT()
// struct FVerticesEdit {
// 	GENERATED_BODY()

// 	UPROPERTY()
// 	FString uName;
// 	UPROPERTY()
// 	TArray<FVector> vertices;
// 	UPROPERTY()
// 	FString type;
// 	UPROPERTY()
// 	FVector center;
// 	UPROPERTY()
// 	int closedLoop;

// 	FVerticesEdit() {};
// 	FVerticesEdit(FString uName_, TArray<FVector> vertices_, FString type_,
// 		FVector center_ = FVector(0,0,0)) {
// 		uName = uName_;
// 		vertices = vertices_;
// 		type = type_;
// 		center = center_;
// 		closedLoop = (type == "road") ? 0 : 1;
// 	};
// };

USTRUCT()
struct FVerticesEditActor {
	GENERATED_BODY()

	UPROPERTY()
	FString uName;
	UPROPERTY()
	TArray<int> verticesInstanceIndices;
	UPROPERTY()
	TArray<int> edgesInstanceIndices;
	UPROPERTY()
	int centerInstanceIndex;

	FVerticesEditActor() {};
	FVerticesEditActor(FString uName_, TArray<int> verticesInstanceIndices_ = {},
		TArray<int> edgesInstanceIndices_ = {}, int centerInstanceIndex_ = -1) {
		uName = uName_;
		verticesInstanceIndices = verticesInstanceIndices_;
		edgesInstanceIndices = edgesInstanceIndices_;
		centerInstanceIndex = centerInstanceIndex_;
	};
};
