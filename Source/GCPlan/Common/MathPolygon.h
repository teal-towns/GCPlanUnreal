#pragma once

class MathPolygon {
public:
	MathPolygon();
	~MathPolygon();

	static FVector2D GetPolygonCenter2D(TArray<FVector2D>);
	static bool IsPointInPolygon(FVector2D, TArray<FVector2D>);
	static std::tuple<bool, float, float> GetLineIntersection(float, float, float, float, float,
		float, float, float);
};
