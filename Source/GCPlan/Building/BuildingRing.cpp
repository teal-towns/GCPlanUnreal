#include "BuildingRing.h"

#include "math.h"

#include "../BuildingStructsActor.h"
#include "../Common/Lodash.h"
#include "../Common/MathPolygon.h"
#include "BuildingPolyLine.h"

BuildingRing::BuildingRing() {
}

BuildingRing::~BuildingRing() {
}

FBuildingBlueprint BuildingRing::Create(TArray<FVector> pathVertices, TArray<int> heightFloorsOrder,
    int residentCount, int minRingWidthUnits, int maxRingWidthUnits,
    float unitDiameter, float unitHeight,
    int numRings, float residentToUnitCountFactor,
    FString crossDirection, bool placeOnSides, float maxBuiltRatio,
    FString uName) {
	int unitCount = (int)round(residentCount * residentToUnitCountFactor);
    return CreateOneRing(unitCount, pathVertices, heightFloorsOrder,
        minRingWidthUnits, maxRingWidthUnits,
        unitDiameter, unitHeight, crossDirection, placeOnSides,
        maxBuiltRatio, uName);
};

// We assume a mix between a circle and a unitagon for ring shape with 4 key height points
// of maxFloors, minFloors, maxFloorsSecondary, minFloorsSecondary.
// So we use these 4 points as a square / rectangle, each one quarter of the circumference:
// Side 1: minFloors --> maxFloors
// Side 2: maxFloors --> minFloorsSecondary
// Side 3: minFloorsSecondary --> maxFloorsSecondary
// Side 4: maxFloorsSecondary --> minFloors
// We assume a smooth descent from each point
FBuildingBlueprint BuildingRing::CreateOneRing(int unitCount,
	TArray<FVector> pathVertices, TArray<int> heightFloorsOrder,
	int minRingWidthUnits, int maxRingWidthUnits, float unitDiameter,
	float unitHeight, FString crossDirection, bool placeOnSides,
	float maxBuiltRatio, FString uName) {
	FBuildingBlueprint blueprint = FBuildingBlueprint();
	if (uName.Len() == 0) {
		uName = Lodash::GetInstanceId();
	}
	blueprint.land.land_id = "Ring_" + uName;

	FVector posCenterGround = MathPolygon::GetPolygonCenter(pathVertices);
	blueprint.posCenterGround = posCenterGround;
	auto [radius, radiusMin] = MathPolygon::GetAverageRadius(pathVertices, posCenterGround);
	float radiusInner = 0;
	if (crossDirection == "outToIn") {
		auto [maxRingWidthUnits1, radiusInner1] = GetMaxRingsByAreaOuter(radius, unitDiameter,
			maxRingWidthUnits, maxBuiltRatio);
		maxRingWidthUnits = maxRingWidthUnits1;
		radiusInner = radiusInner1;
	}
	blueprint.stats.averageRadiusOuter = radius;
	blueprint.stats.averageRadiusInner = radiusInner;

	FBuildData buildData = FBuildData(posCenterGround, posCenterGround);
	auto [blueprint1, buildData1] = BuildingPolyLine::BuildAlongPolyLine(blueprint, buildData, pathVertices,
		heightFloorsOrder, unitDiameter, unitHeight, minRingWidthUnits, minRingWidthUnits, maxRingWidthUnits,
		-1, 1, -1, unitCount, -1, -1, "hex", 1, 0, crossDirection, placeOnSides);
	blueprint = blueprint1;
	buildData = buildData1;

	return blueprint;
}

// Set 40% to 60% build area max compared to inner open space.
// Assumes build rings inward from starting radius.
std::tuple<int, float> BuildingRing::GetMaxRingsByAreaOuter(float radius, float unitDiameter, int maxRings,
    float maxBuiltRatio) {
    float areaOuter = M_PI * pow(radius, 2);
    float areaOpen;
    float radiusOpen = radius;
    for (int ii = 1; ii <= maxRings; ii++) {
        radiusOpen -= unitDiameter;
        areaOpen = M_PI * pow(radiusOpen, 2);
        if ((areaOuter - areaOpen) / areaOuter > maxBuiltRatio) {
            return {ii, radiusOpen};
        }
    }
    return {maxRings, radiusOpen};
}
