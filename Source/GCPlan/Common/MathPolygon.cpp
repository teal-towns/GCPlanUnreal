#include "MathPolygon.h"

MathPolygon::MathPolygon() {
}

MathPolygon::~MathPolygon() {
}

// public TArray<FVector> MathPolygon::Bounds(TArray<FVector> Vertices) {
// 	FVector min = FVector::Zero;
// 	FVector max = FVector::Zero;
// 	for (int vv = 0; vv < Vertices.Num(); vv++) {
// 		FVector2 bounds = FVector2(plot.vertices[vv].x, plot.vertices[vv].z);
// 		vertices2D.Add(bounds);
// 		if (min.x == 0 || bounds.x < min.x) {
// 			min.x = bounds.x;
// 		}
// 		if (max.x == 0 || bounds.x > max.x) {
// 			max.x = bounds.x;
// 		}
// 		if (min.y == 0 || bounds.y < min.y) {
// 			min.y = bounds.y;
// 		}
// 		if (max.y == 0 || bounds.y > max.y) {
// 			max.y = bounds.y;
// 		}
// 	}
// 	TArray<FVector2> boundsRect = { min, max };
// 	return boundsRect;
// }

// TODO - convert from C# to C++
// public List<Vector2> PointsTo2D(List<Vector3> points) {
// 	List<Vector2> points2D = new List<Vector2>() {};
// 	for (int ii = 0; ii < points.Count; ii++) {
// 		points2D.Add(new Vector2(points[ii].x, points[ii].z));
// 	}
// 	return points2D;
// }

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

// 	public List<Vector3> BufferVertices(List<Vector3> vertices, Vector3 posCenter, float buffer,
// 	bool setYToCenter = false) {
// 	Vector3 direction;
// 	List<Vector3> verticesNew = new List<Vector3>() {};
// 	for (int ii = 0; ii < vertices.Count; ii++) {
// 		if (setYToCenter) {
// 			vertices[ii] = new Vector3(vertices[ii].x, posCenter.y, vertices[ii].z);
// 		}
// 		direction = vertices[ii] - posCenter;
// 		verticesNew.Add(vertices[ii] + direction.normalized * buffer);
// 	}
// 	return verticesNew;
// 	}

// public float StepAngle(float angle, float degreesStep) {
// 	float steppedAngle;
// 	int remainder = ((int)Mathf.Round(angle) % (int)degreesStep);
// 	if (remainder < 0) {
// 		if (remainder < -1 * degreesStep / 2) {
// 			steppedAngle = angle - (degreesStep + remainder);
// 		} else {
// 			steppedAngle = angle - remainder;
// 		}
// 	} else {
// 		if (remainder > degreesStep / 2) {
// 			steppedAngle = angle + (degreesStep - remainder);
// 		} else {
// 			steppedAngle = angle - remainder;
// 		}
// 	}
// 	return Mathf.Round(steppedAngle);
// }

// // https://www.omnicalculator.com/math/irregular-polygon-area#how-to-find-the-area-of-an-irregular-polygon-shoelace-theorem
// public float PolygonArea(List<Vector2> vertices2D) {
// 	float sum = 0;
// 	for (int ii = 0; ii < vertices2D.Count; ii++) {
// 		int indexNext = ii < vertices2D.Count - 1 ? ii + 1 : 0;
// 		Vector2 vertexNext = vertices2D[indexNext];
// 		sum += vertices2D[ii].x * vertexNext.y - vertices2D[ii].y * vertexNext.x;
// 	}
// 	return sum * 0.5f;
// }

// public Vector3 GetPolygonCenter(List<Vector3> vertices) {
// 	int numPoints = vertices.Count;
// 	float xSum = 0, ySum = 0, zSum = 0;
// 	for (int ii = 0; ii < numPoints; ii++) {
// 		xSum += vertices[ii].x;
// 		ySum += vertices[ii].y;
// 		zSum += vertices[ii].z;
// 	}
// 	Vector3 center = new Vector3(xSum / numPoints, ySum / numPoints, zSum / numPoints);
// 	return center;
// }

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

// public Tuple<int, Vector3> FindNewVertexOnEdge(List<Vector3> vertices, Vector3 posNonEditing,
// 	float distanceError = 1) {
// 	int insertIndex = -1;
// 	Vector3 insertPos = Vector3.zero;
// 	// Insert new vertex IN the right order.
// 	// Point C is on line A to B if the distance from A to B is the same as the sum of AC and CB.
// 	// https://stackoverflow.com/questions/17692922/check-is-a-point-x-y-is-between-two-points-drawn-on-a-straight-line
// 	float distanceAC, distanceCB, distanceAB, diff;
// 	Vector3 pointA, pointB;
// 	Vector3 pointC = posNonEditing;
// 	int indexPrev;
// 	float minDiff = -1;
// 	int minIndex = -1;
// 	float minDistanceVertices = -1;
// 	bool edited = false;
// 	for (int ii = 0; ii < vertices.Count; ii++) {
// 		indexPrev = ii > 0 ? ii - 1 : vertices.Count - 1;
// 		pointA = vertices[indexPrev];
// 		pointB = vertices[ii];
// 		// Move pos down to same y (pick one of the vertices, assume they are the same).
// 		pointC.y = pointA.y;
// 		distanceAB = (pointB - pointA).magnitude;
// 		distanceAC = (pointC - pointA).magnitude;
// 		distanceCB = (pointB - pointC).magnitude;
// 		diff = Mathf.Abs(distanceAB - (distanceAC + distanceCB));
// 		if (minDiff == -1 || diff < minDiff) {
// 			minDiff = diff;
// 			minIndex = ii;
// 			posNonEditing.y = pointC.y;
// 		}
// 		if (minDistanceVertices == -1 || distanceAB < minDistanceVertices) {
// 			minDistanceVertices = distanceAB;
// 		}
// 		if (diff < distanceError) {
// 			posNonEditing.y = pointC.y;
// 			insertIndex = ii;
// 			insertPos = posNonEditing;
// 			edited = true;
// 			break;
// 		}
// 	}
// 	// For some reason we are not always getting a position on the line, so if close enough, use it..
// 	if (!edited && minDiff != -1 && minDiff < minDistanceVertices) {
// 		insertIndex = minIndex;
// 		insertPos = posNonEditing;
// 		edited = true;
// 	}

// 	return new Tuple<int, Vector3>(insertIndex, insertPos);
// }

// // If want an inner radius to take up 15% of the total area, how long is the inner radius?
// // formula: PI * radiusOuter ^ 2 * innerAreaRatio = PI * radiusInner ^ 2
// // sqrt((PI * radiusOuter ^ 2 * innerAreaRatio) / PI) = radiusInner
// public float GetCircleRadiusByArea(float radiusOuter, float innerAreaRatio) {
// 	float radiusInner = (float)Math.Sqrt((Mathf.PI * Mathf.Pow(radiusOuter, 2) * innerAreaRatio) / Mathf.PI);
// 	return radiusInner;
// }

// public Tuple<float, float> GetAverageRadius(List<Vector3> vertices, Vector3 posCenter) {
// 	int numPoints = vertices.Count;
// 	float sum = 0;
// 	float radiusMin = -1;
// 	float radius;
// 	float minX = -1, minZ = -1, maxX = -1, maxZ = -1;
// 	for (int ii = 0; ii < numPoints; ii++) {
// 		radius = (vertices[ii] - posCenter).magnitude;
// 		sum += radius;
// 		if (radiusMin == -1 || radius < radiusMin) {
// 			radiusMin = radius;
// 		}
// 		if (minX == -1 || vertices[ii].x < minX) {
// 			minX = vertices[ii].x;
// 		}
// 		if (maxX == -1 || vertices[ii].x > maxX) {
// 			maxX = vertices[ii].x;
// 		}
// 		if (minZ == -1 || vertices[ii].z < minZ) {
// 			minZ = vertices[ii].z;
// 		}
// 		if (maxZ == -1 || vertices[ii].z > maxZ) {
// 			maxZ = vertices[ii].z;
// 		}
// 	}
// 	float sizeX = maxX - minX;
// 	if (sizeX < radiusMin) {
// 		radiusMin = sizeX;
// 	}
// 	float sizeZ = maxZ - minZ;
// 	if (sizeZ < radiusMin) {
// 		radiusMin = sizeZ;
// 	}
// 	float radiusAverage = sum / numPoints;
// 	return new Tuple<float, float>(radiusAverage, radiusMin);
// }
