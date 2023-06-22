#include "MathPolygon.h"

#define _USE_MATH_DEFINES		// for pi
#include "math.h"

MathPolygon::MathPolygon() {
}

MathPolygon::~MathPolygon() {
}

TArray<FVector> MathPolygon::PointToBox(FVector point, FVector range, FString plane) {
	TArray<FVector> vertices;
	if (plane == "xy") {
		vertices = {
			FVector(point.X - range.X / 2, point.Y - range.Y / 2, point.Z),
			FVector(point.X - range.X / 2, point.Y + range.Y / 2, point.Z),
			FVector(point.X + range.X / 2, point.Y + range.Y / 2, point.Z),
			FVector(point.X + range.X / 2, point.Y - range.Y / 2, point.Z)
		};
	} else if (plane == "xz") {
		vertices = {
			FVector(point.X - range.X / 2, point.Y, point.Z - range.Z / 2),
			FVector(point.X - range.X / 2, point.Y, point.Z + range.Z / 2),
			FVector(point.X + range.X / 2, point.Y, point.Z + range.Z / 2),
			FVector(point.X + range.X / 2, point.Y, point.Z - range.Z / 2)
		};
	} else if (plane == "yz") {
		vertices = {
			FVector(point.X, point.Y - range.Y / 2, point.Z - range.Z / 2),
			FVector(point.X, point.Y - range.Y / 2, point.Z + range.Z / 2),
			FVector(point.X, point.Y + range.Y / 2, point.Z + range.Z / 2),
			FVector(point.X, point.Y + range.Y / 2, point.Z - range.Z / 2)
		};
	}
	return vertices;
}

TArray<FVector> MathPolygon::MinMaxPoints(TArray<FVector> vertices, FString axis) {
	FVector min = vertices[0];
	FVector max = vertices[0];
	for (int vv = 0; vv < vertices.Num(); vv++) {
		FVector vertex = vertices[vv];
		if (axis == "x") {
			if (vertex.X < min.X) {
				min = vertex;
			}
			if (vertex.X > max.X) {
				max = vertex;
			}
		} else if (axis == "y") {
			if (vertex.Y < min.Y) {
				min = vertex;
			}
			if (vertex.Y > max.Y) {
				max = vertex;
			}
		} else if (axis == "z") {
			if (vertex.Z < min.Z) {
				min = vertex;
			}
			if (vertex.Z > max.Z) {
				max = vertex;
			}
		}
	}
	return TArray<FVector> { min, max };
}

TArray<FVector> MathPolygon::Bounds(TArray<FVector> Vertices) {
	FVector min = Vertices[0];
	FVector max = Vertices[0];
	for (int vv = 0; vv < Vertices.Num(); vv++) {
		// FVector2D bounds = FVector2D(Vertices[vv].X, Vertices[vv].Y);
		FVector bounds = Vertices[vv];
		if (bounds.X < min.X) {
			min.X = bounds.X;
		}
		if (bounds.X > max.X) {
			max.X = bounds.X;
		}
		if (bounds.Y < min.Y) {
			min.Y = bounds.Y;
		}
		if (bounds.Y > max.Y) {
			max.Y = bounds.Y;
		}
		if (bounds.Z < min.Z) {
			min.Z = bounds.Z;
		}
		if (bounds.Z > max.Z) {
			max.Z = bounds.Z;
		}
	}
	TArray<FVector> boundsRect = { min, max };
	return boundsRect;
}

TArray<FVector2D> MathPolygon::PointsTo2D(TArray<FVector> points, FString plane) {
	TArray<FVector2D> points2D = {};
	for (int ii = 0; ii < points.Num(); ii++) {
		if (plane == "xz") {
			points2D.Add(FVector2D(points[ii].X, points[ii].Z));
		} else if (plane == "yz") {
			points2D.Add(FVector2D(points[ii].Y, points[ii].Z));
		} else {
			points2D.Add(FVector2D(points[ii].X, points[ii].Y));
		}
	}
	return points2D;
}

// https://stackoverflow.com/questions/217578/how-can-i-determine-whether-a-2d-point-is-within-a-polygon
bool MathPolygon::IsPointInPolygon(FVector2D point, TArray<FVector2D> vertices) {
	bool inside = false;
	for (int i = 0, j = vertices.Num() - 1; i < vertices.Num(); j = i++) {
		if ((vertices[i].Y > point.Y) != (vertices[j].Y > point.Y) && point.X <
			(vertices[j].X - vertices[i].X) * (point.Y - vertices[i].Y) /
			(vertices[j].Y - vertices[i].Y) + vertices[i].X) {
			inside = !inside;
		}
	}
	return inside;
}

// https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
std::tuple<bool, float, float> MathPolygon::GetLineIntersection(float p0_x, float p0_y, float p1_x, float p1_y,
	float p2_x, float p2_y, float p3_x, float p3_y) {
	float s1_x = p1_x - p0_x;
	float s1_y = p1_y - p0_y;
	float s2_x = p3_x - p2_x;
	float s2_y = p3_y - p2_y;
	float s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
	float t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);
	if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
		// Collision detected
		float i_x = p0_x + (t * s1_x);
		float i_y = p0_y + (t * s1_y);
		return {true, i_x, i_y};
	}
	return {false, -1, -1};
}

TArray<FVector> MathPolygon::BufferVertices(TArray<FVector> vertices, FVector posCenter, float buffer,
	bool setZToCenter) {
	FVector direction;
	TArray<FVector> verticesNew = {};
	for (int ii = 0; ii < vertices.Num(); ii++) {
		if (setZToCenter) {
			vertices[ii] = FVector(vertices[ii].X, vertices[ii].Y, posCenter.Z);
		}
		direction = vertices[ii] - posCenter;
		verticesNew.Add(vertices[ii] + direction.GetUnsafeNormal() * buffer);
	}
	return verticesNew;
}

float MathPolygon::StepAngle(float angle, float degreesStep) {
	// Nothing to do, and avoid divide by zero errors.
	if (degreesStep < 1 && degreesStep > -1) {
		UE_LOG(LogTemp, Warning, TEXT("MathPolygon.StepAngle degreesStep near zero, skipping. degreesStep %f angle %f"), degreesStep, angle);
		return angle;
	}
	float steppedAngle;
	int remainder = ((int)round(angle) % (int)degreesStep);
	if (remainder < 0) {
		if (remainder < -1 * degreesStep / 2) {
			steppedAngle = angle - (degreesStep + remainder);
		} else {
			steppedAngle = angle - remainder;
		}
	} else {
		if (remainder > degreesStep / 2) {
			steppedAngle = angle + (degreesStep - remainder);
		} else {
			steppedAngle = angle - remainder;
		}
	}
	return round(steppedAngle);
}

// https://www.omnicalculator.com/math/irregular-polygon-area#how-to-find-the-area-of-an-irregular-polygon-shoelace-theorem
float MathPolygon::PolygonArea(TArray<FVector2D> vertices2D) {
	float sum = 0;
	for (int ii = 0; ii < vertices2D.Num(); ii++) {
		int indexNext = ii < vertices2D.Num() - 1 ? ii + 1 : 0;
		FVector2D vertexNext = vertices2D[indexNext];
		sum += vertices2D[ii].X * vertexNext.Y - vertices2D[ii].Y * vertexNext.X;
	}
	return abs(sum * 0.5f);
}

FVector MathPolygon::GetPolygonCenter(TArray<FVector> vertices) {
	int numPoints = vertices.Num();
	float xSum = 0, ySum = 0, zSum = 0;
	for (int ii = 0; ii < numPoints; ii++) {
		xSum += vertices[ii].X;
		ySum += vertices[ii].Y;
		zSum += vertices[ii].Z;
	}
	FVector center = FVector(xSum / numPoints, ySum / numPoints, zSum / numPoints);
	return center;
}

FVector2D MathPolygon::GetPolygonCenter2D(TArray<FVector2D> vertices) {
	int numPoints = vertices.Num();
	float xSum = 0, ySum = 0;
	for (int ii = 0; ii < numPoints; ii++) {
		xSum += vertices[ii].X;
		ySum += vertices[ii].Y;
	}
	FVector2D center = FVector2D(xSum / numPoints, ySum / numPoints);
	return center;
}

std::tuple<int, FVector> MathPolygon::FindNewVertexOnEdge(TArray<FVector> vertices, FVector posNonEditing,
	float distanceError) {
	int insertIndex = -1;
	FVector insertPos = FVector(0,0,0);
	// Insert new vertex IN the right order.
	// Point C is on line A to B if the distance from A to B is the same as the sum of AC and CB.
	// https://stackoverflow.com/questions/17692922/check-is-a-point-x-y-is-between-two-points-drawn-on-a-straight-line
	float distanceAC, distanceCB, distanceAB, diff;
	FVector pointA, pointB;
	FVector pointC = posNonEditing;
	int indexPrev;
	float minDiff = -1;
	int minIndex = -1;
	float minDistanceVertices = -1;
	bool edited = false;
	for (int ii = 0; ii < vertices.Num(); ii++) {
		indexPrev = ii > 0 ? ii - 1 : vertices.Num() - 1;
		pointA = vertices[indexPrev];
		pointB = vertices[ii];
		// Move pos down to same z (pick one of the vertices, assume they are the same).
		pointC.Z = pointA.Z;
		distanceAB = (pointB - pointA).Size();
		distanceAC = (pointC - pointA).Size();
		distanceCB = (pointB - pointC).Size();
		diff = abs(distanceAB - (distanceAC + distanceCB));
		if (minDiff == -1 || diff < minDiff) {
			minDiff = diff;
			minIndex = ii;
			posNonEditing.Z = pointC.Z;
		}
		if (minDistanceVertices == -1 || distanceAB < minDistanceVertices) {
			minDistanceVertices = distanceAB;
		}
		if (diff < distanceError) {
			posNonEditing.Z = pointC.Z;
			insertIndex = ii;
			insertPos = posNonEditing;
			edited = true;
			break;
		}
	}
	// For some reason we are not always getting a position on the line, so if close enough, use it..
	if (!edited && minDiff != -1 && minDiff < minDistanceVertices) {
		insertIndex = minIndex;
		insertPos = posNonEditing;
		edited = true;
	}

	return { insertIndex, insertPos };
}

// If want an inner radius to take up 15% of the total area, how long is the inner radius?
// formula: PI * radiusOuter ^ 2 * innerAreaRatio = PI * radiusInner ^ 2
// sqrt((PI * radiusOuter ^ 2 * innerAreaRatio) / PI) = radiusInner
float MathPolygon::GetCircleRadiusByArea(float radiusOuter, float innerAreaRatio) {
	float radiusInner = (float)sqrt((M_PI * pow(radiusOuter, 2) * innerAreaRatio) / M_PI);
	return radiusInner;
}

std::tuple<float, float> MathPolygon::GetAverageRadius(TArray<FVector> vertices, FVector posCenter) {
	int numPoints = vertices.Num();
	float sum = 0;
	float radiusMin = -1;
	float radius;
	float minX = -1, minY = -1, maxX = -1, maxY = -1;
	for (int ii = 0; ii < numPoints; ii++) {
		radius = (vertices[ii] - posCenter).Size();
		sum += radius;
		if (radiusMin == -1 || radius < radiusMin) {
			radiusMin = radius;
		}
		if (minX == -1 || vertices[ii].X < minX) {
			minX = vertices[ii].X;
		}
		if (maxX == -1 || vertices[ii].X > maxX) {
			maxX = vertices[ii].X;
		}
		if (minY == -1 || vertices[ii].Y < minY) {
			minY = vertices[ii].Y;
		}
		if (maxY == -1 || vertices[ii].Y > maxY) {
			maxY = vertices[ii].Y;
		}
	}
	float sizeX = maxX - minX;
	if (sizeX < radiusMin) {
		radiusMin = sizeX;
	}
	float sizeY = maxY - minY;
	if (sizeY < radiusMin) {
		radiusMin = sizeY;
	}
	float radiusAverage = sum / numPoints;
	return {radiusAverage, radiusMin};
}
