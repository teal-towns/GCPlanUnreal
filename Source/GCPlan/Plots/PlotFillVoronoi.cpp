#include "PlotFillVoronoi.h"

#include "math.h"
#include "Math/Vector.h"
// #include "VectorTypes.h"
#include "Math/Vector2D.h"

// https://forums.unrealengine.com/t/errors-when-including-third-party-library/737189/2
#pragma push_macro("check")   // store 'check' macro current definition
#undef check  // undef to avoid conflicts
THIRD_PARTY_INCLUDES_START
// https://github.com/SirAnthony/cppdelaunay
#include "delaunay/Voronoi.h"
#include "geom/Point.h"
#include "geom/Rectangle.h"
THIRD_PARTY_INCLUDES_END
#pragma pop_macro("check")  // restore definition

#include "../BuildingStructsActor.h"
#include "../Common/Lodash.h"
#include "../Common/MathPolygon.h"
// TODO - figure out how to include a file with just structs
// #include "PlotStructs.h"

PlotFillVoronoi::PlotFillVoronoi() {
}

PlotFillVoronoi::~PlotFillVoronoi() {
}

std::tuple<TArray<TArray<FVector>>, FVector, TArray<FVector2D>> PlotFillVoronoi::Fill(TMap<FString, FPlot> plots, float averageDistance) {
	TArray<TArray<FVector>> spacesVertices = {};
	// Cache plot vertices in 2D format for in polygon checking later.
	TArray<TArray<FVector2D>> plotsVertices2D = {};
	// Index will match plot.
	TArray<float> zVals = {};

	// Convert bounds to min and max rectangle.
	FVector2D min = FVector2D(0,0);
	FVector2D max = FVector2D(0,0);
	for (auto& Elem : plots) {
		FPlot plot = Elem.Value;
		TArray<FVector2D> vertices2D = {};
		for (int vv = 0; vv < plot.vertices.Num(); vv++) {
			FVector2D bounds = FVector2D(plot.vertices[vv].X, plot.vertices[vv].Y);
			vertices2D.Add(bounds);
			if (min.X == 0 || bounds.X < min.X) {
				min.X = bounds.X;
			}
			if (max.X == 0 || bounds.X > max.X) {
				max.X = bounds.X;
			}
			if (min.Y == 0 || bounds.Y < min.Y) {
				min.Y = bounds.Y;
			}
			if (max.Y == 0 || bounds.Y > max.Y) {
				max.Y = bounds.Y;
			}
		}
		plotsVertices2D.Add(vertices2D);
		// Just pick one - could get center to be more accurate?
		zVals.Add(plot.vertices[0].Z);
	}
	TArray<FVector2D> boundsRect = { min, max };

	float plotSizeX = round(max.X - min.X);
	float plotSizeY = round(max.Y - min.Y);
	FVector posCenter = FVector(min.X + (plotSizeX / 2), 0,
		min.Y + (plotSizeY / 2));
	// UE_LOG(LogTemp, Display, TEXT("PFV sizeX %f sizeY %f min %s max %s"), plotSizeX, plotSizeY, *min.ToString(), *max.ToString());

	TArray<FVector2D> pointsTemp = SpawnPoints(boundsRect, averageDistance);
	// UE_LOG(LogTemp, Display, TEXT("PFV pointsTemp %d"), pointsTemp.Num());

	std::vector<Delaunay::Point*> points = {};
	for (int ii = 0; ii < pointsTemp.Num(); ii++) {
		points.push_back(Delaunay::Point::create(pointsTemp[ii].X, pointsTemp[ii].Y));
		// UE_LOG(LogTemp, Display, TEXT("PFV point %s"), *pointsTemp[ii].ToString());
	}
	Delaunay::Rectangle plotBounds = Delaunay::Rectangle(min.X, min.Y, plotSizeX, plotSizeY);
	const std::vector<unsigned> colors = {};
	Delaunay::Voronoi voronoi = Delaunay::Voronoi(points, &colors, plotBounds);
	std::vector<std::vector<Delaunay::Point*>> regions1 = voronoi.regions();
	TArray<TArray<FVector2D>> regions = {};
	for (int ii = 0; ii < regions1.size(); ii++) {
		regions.Add({});
		for (int jj = 0; jj < regions1[ii].size(); jj++) {
			regions[ii].Add(FVector2D(regions1[ii][jj]->x, regions1[ii][jj]->y));
		}
	}
	// UE_LOG(LogTemp, Display, TEXT("PFV regions %d"), regions.Num());

	// Go through all regions and remove any vertices that are not in the plots.
	FVector2D point;
	int totalRegions = regions.Num();
	TArray<FVector> verticesD;
	for (int ii = 0; ii < regions.Num(); ii++) {
		if (regions[ii].Num() > 0) {
			bool valid = false;
			bool atLeastOneInside = false;
			bool done = false;
			TArray<FVector2D> regionVertices2D = {};
			for (int rr = 0; rr < regions[ii].Num(); rr++) {
				regionVertices2D.Add(FVector2D(regions[ii][rr].X, regions[ii][rr].Y));
			}
			// int plotIndex = -1;
			// Check first point and see if in a plot.
			for (int pp = 0; pp < plotsVertices2D.Num(); pp++) {
				auto [valid1, newRegionVertices2D] =
					CheckAdjustVertices(regionVertices2D, plotsVertices2D[pp]);
				if (valid1) {
					TArray<FVector> verticesTemp = {};
					for (int vt = 0; vt < newRegionVertices2D.Num(); vt++) {
						verticesTemp.Add(FVector(newRegionVertices2D[vt].X,
							newRegionVertices2D[vt].Y, zVals[pp]));
					}
					spacesVertices.Add(verticesTemp);
					break;
				}
			}
		}
	}
	// UE_LOG(LogTemp, Display, TEXT("PFV spacesVertices %d"), spacesVertices.Num());
	// for (int ii = 0; ii < spacesVertices.Num(); ii++) {
	// 	UE_LOG(LogTemp, Display, TEXT("PFV spacesVertices ii %d"), ii);
	// 	for (int jj = 0; jj < spacesVertices[ii].Num(); jj++) {
	// 		UE_LOG(LogTemp, Display, TEXT("jj %d vertex %s"), jj, *spacesVertices[ii][jj].ToString());
	// 	}
	// }

	return {spacesVertices, posCenter, boundsRect};
}

// Shape mimics where the points are placed so we do a hexagon grid but use the offsetMaxFactor
// to add some jitter.
TArray<FVector2D> PlotFillVoronoi::SpawnPoints(TArray<FVector2D> boundsRect, float averageDistance,
	float offsetMaxFactor) {
	// Go in a grid until go through all of bounds.
	float curX = boundsRect[0].X;
	float curY = boundsRect[0].Y;
	float offsetMax = averageDistance * offsetMaxFactor;
	float offsetX, offsetY;
	TArray<FVector2D> points = {};
	int counterY = 0;
	while (curY < boundsRect[1].Y + averageDistance) {
		curX = boundsRect[0].X;
		// For hexagon grid, every other one starts at the boundary (offset by half).
		if (counterY % 2 == 1) {
			curX -= averageDistance * 0.5f;
		}
		while (curX < boundsRect[1].X + averageDistance) {
			offsetX = Lodash::RandomRangeFloat(-1 * offsetMax, offsetMax);
			offsetY = Lodash::RandomRangeFloat(-1 * offsetMax, offsetMax);
			points.Add(FVector2D(curX + offsetX, curY + offsetY));
			curX += averageDistance;
		}
		curY += averageDistance;
		counterY += 1;
	}
	return points;
}

std::tuple<bool, TArray<FVector2D>> PlotFillVoronoi::CheckAdjustVertices(TArray<FVector2D> regionVertices2D, TArray<FVector2D> plotVertices2D,
	float minRatioIn) {
	bool valid = true;
	TArray<FVector2D> newRegionVertices2D = {};
	FVector2D regionCenter = MathPolygon::GetPolygonCenter2D(regionVertices2D);
	int maxOut = (int)floor(regionVertices2D.Num() * (1 - minRatioIn));
	int countOut = 0;
	for (int jj = 0; jj < regionVertices2D.Num(); jj++) {
		FVector2D point = regionVertices2D[jj];
		if (!MathPolygon::IsPointInPolygon(point, plotVertices2D)) {
			countOut += 1;
			if (countOut > maxOut) {
				valid = false;
				break;
			}
			// Move point into plot.
			// Draw line from point to region center and find the plot edge line it intersects with.
			// Then move point onto that plot edge line.
			for (int kk = 0; kk < plotVertices2D.Num(); kk++) {
				int indexNext = kk < plotVertices2D.Num() - 1 ? kk + 1 : 0;
				auto [intersects, xIntersect, yIntersect] = MathPolygon::GetLineIntersection(
					regionCenter.X, regionCenter.Y, point.X, point.Y,
					plotVertices2D[kk].X, plotVertices2D[kk].Y,
					plotVertices2D[indexNext].X, plotVertices2D[indexNext].Y);
				if (intersects) {
					newRegionVertices2D.Add(FVector2D(xIntersect, yIntersect));
					break;
				}
			}
		} else {
			newRegionVertices2D.Add(point);
		}
	}

	return {valid, newRegionVertices2D};
}

std::tuple<bool, FString> PlotFillVoronoi::IsValid(TArray<FVector> pathVertices, float minRadiusSkip,
	int minVerticesSkip) {
	FString reason = "";
	FVector posCenterGround = MathPolygon::GetPolygonCenter(pathVertices);
	if (pathVertices.Num() < minVerticesSkip) {
		reason = FString::Printf(TEXT("minVertices: have %d, need %d"), pathVertices.Num(), minRadiusSkip);
		return {false, reason};
	}
	auto [radius, radiusMin] = MathPolygon::GetAverageRadius(pathVertices, posCenterGround);
	if (minRadiusSkip > 0 && radiusMin < minRadiusSkip) {
		reason = FString::Printf(TEXT("minRadius: %f is below %f"), radiusMin, minRadiusSkip);
		return {false, reason};
	}
	return {true, reason};
}
