#include "BuildingStructsActor.h"

#include "Common/DataConvert.h"

ABuildingStructsActor::ABuildingStructsActor()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ABuildingStructsActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// void ABuildingStructsActor::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
// }

// FPolygonSimplified ABuildingStructsActor::PolygonToSimplified(FPolygon polygon) {
// 	return FPolygonSimplified(polygon._id, polygon.uName, DataConvert::VectorsToDicts(polygon.vertices),
// 		DataConvert::VectorToDict(polygon.posCenter), polygon.type, polygon.closedLoop);
// }

// FPolygon ABuildingStructsActor::PolygonFromSimplified(FPolygonSimplified polygonSimplified) {
// 	return FPolygon(polygonSimplified._id, polygonSimplified.uName, DataConvert::DictsToVectors(polygonSimplified.vertices),
// 		DataConvert::DictToVector(polygonSimplified.posCenter), polygonSimplified.type,
// 		polygonSimplified.closedLoop);
// }

// TMap<FString, FPolygonSimplified> ABuildingStructsActor::PolygonsToSimplified(TMap<FString, FPolygon> polygons) {
// 	TMap<FString, FPolygonSimplified> polygonsSimplified = {};
// 	for (auto& Elem : polygons) {
// 		polygonsSimplified.Add(Elem.Key, PolygonToSimplified(Elem.Value));
// 	}
// 	return polygonsSimplified;
// }

// TMap<FString, FPolygon> ABuildingStructsActor::PolygonsFromSimplified(TMap<FString, FPolygonSimplified> polygonsSimplified) {
// 	TMap<FString, FPolygon> polygons = {};
// 	for (auto& Elem : polygonsSimplified) {
// 		polygons.Add(Elem.Key, PolygonFromSimplified(Elem.Value));
// 	}
// 	return polygons;
// }


FPlot ABuildingStructsActor::PolygonToPlot(FPolygon obj) {
	return FPlot(obj._id, obj.uName, obj.vertices, obj.posCenter);
}
FPolygon ABuildingStructsActor::PlotToPolygon(FPlot obj) {
	return FPolygon(obj._id, obj.uName, obj.vertices, obj.posCenter, "plot");
}
TMap<FString, FPlot> ABuildingStructsActor::PolygonsToPlots(TMap<FString, FPolygon> objs) {
	TMap<FString, FPlot> objsOut = {};
	for (auto& Elem : objs) {
		objsOut.Add(Elem.Key, PolygonToPlot(Elem.Value));
	}
	return objsOut;
}
TMap<FString, FPolygon> ABuildingStructsActor::PlotsToPolygons(TMap<FString, FPlot> objs) {
	TMap<FString, FPolygon> objsOut = {};
	for (auto& Elem : objs) {
		objsOut.Add(Elem.Key, PlotToPolygon(Elem.Value));
	}
	return objsOut;
}


FPlotSimplified ABuildingStructsActor::PlotToSimplified(FPlot plot) {
	return FPlotSimplified(plot._id, plot.uName, DataConvert::VectorsToDicts(plot.vertices),
		DataConvert::VectorToDict(plot.posCenter), plot.buildPattern, plot.averagePlotDistance);
}

FPlot ABuildingStructsActor::PlotFromSimplified(FPlotSimplified plotSimplified) {
	return FPlot(plotSimplified._id, plotSimplified.uName, DataConvert::DictsToVectors(plotSimplified.vertices),
		DataConvert::DictToVector(plotSimplified.posCenter), plotSimplified.buildPattern,
		plotSimplified.averagePlotDistance);
}

TMap<FString, FPlotSimplified> ABuildingStructsActor::PlotsToSimplified(TMap<FString, FPlot> plots) {
	TMap<FString, FPlotSimplified> plotsSimplified = {};
	for (auto& Elem : plots) {
		plotsSimplified.Add(Elem.Key, PlotToSimplified(Elem.Value));
	}
	return plotsSimplified;
}

TMap<FString, FPlot> ABuildingStructsActor::PlotsFromSimplified(TMap<FString, FPlotSimplified> plotsSimplified) {
	TMap<FString, FPlot> plots = {};
	for (auto& Elem : plotsSimplified) {
		plots.Add(Elem.Key, PlotFromSimplified(Elem.Value));
	}
	return plots;
}
