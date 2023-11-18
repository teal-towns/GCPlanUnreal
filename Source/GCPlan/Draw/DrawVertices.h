#pragma once

#include "../BuildingStructsActor.h"
#include "../DataStructsActor.h"

class DrawVertices {
public:
	DrawVertices();
	~DrawVertices();

	static void LoadPolygons(TArray<FPolygonSimplified> polygonsSimplified, bool destroyAll = true);
	static void DrawPolygon(FPolygon polygon);
	static void LoadVertices(TArray<FString> skipTypes = {});
};
