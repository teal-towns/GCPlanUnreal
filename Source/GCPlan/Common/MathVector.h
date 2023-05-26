#pragma once

class MathVector {
public:
	MathVector();
	~MathVector();

	static float SqrMagnitude(FVector);
	static float Angle(FVector, FVector);
	static float SignedAngle(FVector, FVector, FVector);
};
