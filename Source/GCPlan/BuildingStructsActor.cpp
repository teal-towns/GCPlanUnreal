#include "BuildingStructsActor.h"

#include "Common/DataConvert.h"
#include "Common/MathPolygon.h"

ABuildingStructsActor::ABuildingStructsActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABuildingStructsActor::BeginPlay()
{
	Super::BeginPlay();
}


// FPlot ABuildingStructsActor::PolygonToPlot(FPolygon obj) {
// 	TArray<FVector2D> vertices2D = MathPolygon::PointsTo2D(obj.vertices);
// 	float squareMeters = MathPolygon::PolygonArea(vertices2D);
// 	return FPlot(obj._id, obj.uName, obj.vertices, obj.posCenter, "", 100, squareMeters);
// }
// FPolygon ABuildingStructsActor::PlotToPolygon(FPlot obj) {
// 	return FPolygon(obj._id, obj.uName, obj.vertices, obj.posCenter, "plot", "polygon");
// }
// TMap<FString, FPlot> ABuildingStructsActor::PolygonsToPlots(TMap<FString, FPolygon> objs) {
// 	TMap<FString, FPlot> objsOut = {};
// 	for (auto& Elem : objs) {
// 		objsOut.Add(Elem.Key, PolygonToPlot(Elem.Value));
// 	}
// 	return objsOut;
// }
// TMap<FString, FPolygon> ABuildingStructsActor::PlotsToPolygons(TMap<FString, FPlot> objs) {
// 	TMap<FString, FPolygon> objsOut = {};
// 	for (auto& Elem : objs) {
// 		objsOut.Add(Elem.Key, PlotToPolygon(Elem.Value));
// 	}
// 	return objsOut;
// }

// FPlot ABuildingStructsActor::UpdatePlotFromPolygon(FPolygon polygon, FPlot plot) {
// 	plot.vertices = polygon.vertices;
// 	plot.posCenter = polygon.posCenter;
// 	TArray<FVector2D> vertices2D = MathPolygon::PointsTo2D(plot.vertices);
// 	plot.squareMeters = MathPolygon::PolygonArea(vertices2D);
// 	return plot;
// }


// FPlotSimplified ABuildingStructsActor::PlotToSimplified(FPlot plot) {
// 	return FPlotSimplified(plot._id, plot.uName, DataConvert::VectorsToStrings(plot.vertices),
// 		DataConvert::VectorToString(plot.posCenter), plot.buildPattern, plot.averagePlotDistance,
// 		plot.squareMeters, plot.parentPlotUName, plot.childPlotUNames, plot.verticesBuffer);
// }

// FPlot ABuildingStructsActor::PlotFromSimplified(FPlotSimplified plotSimplified) {
// 	return FPlot(plotSimplified._id, plotSimplified.uName, DataConvert::StringsToVectors(plotSimplified.vertices),
// 		DataConvert::StringToVector(plotSimplified.posCenter), plotSimplified.buildPattern,
// 		plotSimplified.averagePlotDistance, plotSimplified.squareMeters, plotSimplified.parentPlotUName,
// 		plotSimplified.childPlotUNames, plotSimplified.verticesBuffer);
// }

// TMap<FString, FPlotSimplified> ABuildingStructsActor::PlotsToSimplified(TMap<FString, FPlot> plots) {
// 	TMap<FString, FPlotSimplified> plotsSimplified = {};
// 	for (auto& Elem : plots) {
// 		plotsSimplified.Add(Elem.Key, PlotToSimplified(Elem.Value));
// 	}
// 	return plotsSimplified;
// }

// TMap<FString, FPlot> ABuildingStructsActor::PlotsFromSimplified(TMap<FString, FPlotSimplified> plotsSimplified) {
// 	TMap<FString, FPlot> plots = {};
// 	for (auto& Elem : plotsSimplified) {
// 		plots.Add(Elem.Key, PlotFromSimplified(Elem.Value));
// 	}
// 	return plots;
// }

FPolygonSimplified ABuildingStructsActor::PolygonToSimplified(FPolygon polygon) {
	return FPolygonSimplified(polygon._id, polygon.uName, DataConvert::VectorsToStrings(polygon.vertices),
		DataConvert::VectorToString(polygon.posCenter), polygon.type, polygon.shape,
		polygon.tags, polygon.jsonDataString, polygon.squareMeters,
		polygon.parentUName, polygon.childUNames, polygon.verticesBuffer, polygon.averageChildDiameter);
}

FPolygon ABuildingStructsActor::PolygonFromSimplified(FPolygonSimplified polygonSimplified) {
	return FPolygon(polygonSimplified._id, polygonSimplified.uName, DataConvert::StringsToVectors(polygonSimplified.vertices),
		DataConvert::StringToVector(polygonSimplified.posCenter), polygonSimplified.type,
		polygonSimplified.shape, polygonSimplified.tags,
		polygonSimplified.jsonDataString, polygonSimplified.squareMeters, polygonSimplified.parentUName,
		polygonSimplified.childUNames, polygonSimplified.verticesBuffer, polygonSimplified.averageChildDiameter);
}

TMap<FString, FPolygonSimplified> ABuildingStructsActor::PolygonsToSimplified(TMap<FString, FPolygon> polygons) {
	TMap<FString, FPolygonSimplified> polygonsSimplified = {};
	for (auto& Elem : polygons) {
		polygonsSimplified.Add(Elem.Key, PolygonToSimplified(Elem.Value));
	}
	return polygonsSimplified;
}

TMap<FString, FPolygon> ABuildingStructsActor::PolygonsFromSimplified(TMap<FString, FPolygonSimplified> polygonsSimplified) {
	TMap<FString, FPolygon> polygons = {};
	for (auto& Elem : polygonsSimplified) {
		polygons.Add(Elem.Key, PolygonFromSimplified(Elem.Value));
	}
	return polygons;
}
