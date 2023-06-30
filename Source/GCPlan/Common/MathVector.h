#pragma once

class MathVector {
public:
	MathVector();
	~MathVector();

	static float SqrMagnitude(FVector);
	static float Angle(FVector, FVector);
	static float SignedAngle(FVector, FVector, FVector);
	static float ConstrainDegrees(float degrees);
	static FVector ConstrainRotation(FVector rotation);
	static FVector RotateVector(FVector vector, FVector rotation);
	static TArray<FVector> RotateVertices(TArray<FVector> vertices, FVector rotation,
		FVector offset = FVector(0,0,0));
	static TArray<FVector> RotateAround(TArray<FVector> vertices, FVector rotation, FVector center);
	static TArray<FVector> BeizerCurvePoints(FVector start, FVector end, FVector control, int count);
};
