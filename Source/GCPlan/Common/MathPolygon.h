#pragma once

class MathPolygon {
public:
	MathPolygon();
	~MathPolygon();

	static std::tuple<float, float> GetAverageRadius(TArray<FVector> vertices, FVector posCenter);
	static float GetCircleRadiusByArea(float radiusOuter, float innerAreaRatio);
	static FVector GetPolygonCenter(TArray<FVector> vertices);
	static FVector2D GetPolygonCenter2D(TArray<FVector2D>);
	static bool IsPointInPolygon(FVector2D, TArray<FVector2D>);
	static std::tuple<bool, float, float> GetLineIntersection(float, float, float, float, float,
		float, float, float);
	static TArray<FVector2D> PointsTo2D(TArray<FVector> points);
	static float PolygonArea(TArray<FVector2D> vertices2D);
	static float StepAngle(float, float);
	static TArray<FVector> BufferVertices(TArray<FVector> vertices, FVector posCenter, float buffer,
		bool setZToCenter = false);
};
