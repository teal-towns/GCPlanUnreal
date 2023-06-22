#pragma once

class MathPolygon {
public:
	MathPolygon();
	~MathPolygon();

	static TArray<FVector> PointToBox(FVector point, FVector range, FString plane = "xy");
	static TArray<FVector> MinMaxPoints(TArray<FVector> vertices, FString axis = "x");
	static TArray<FVector> Bounds(TArray<FVector> Vertices);
	static std::tuple<float, float> GetAverageRadius(TArray<FVector> vertices, FVector posCenter);
	static float GetCircleRadiusByArea(float radiusOuter, float innerAreaRatio);
	static FVector GetPolygonCenter(TArray<FVector> vertices);
	static FVector2D GetPolygonCenter2D(TArray<FVector2D>);
	static bool IsPointInPolygon(FVector2D, TArray<FVector2D>);
	static std::tuple<bool, float, float> GetLineIntersection(float, float, float, float, float,
		float, float, float);
	static TArray<FVector2D> PointsTo2D(TArray<FVector> points, FString plane = "xy");
	static float PolygonArea(TArray<FVector2D> vertices2D);
	static float StepAngle(float, float);
	static TArray<FVector> BufferVertices(TArray<FVector> vertices, FVector posCenter, float buffer,
		bool setZToCenter = false);
	static std::tuple<int, FVector> FindNewVertexOnEdge(TArray<FVector> vertices, FVector posNonEditing,
	float distanceError = 1);
};
