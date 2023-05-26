#include "MathVector.h"

#include "math.h"

MathVector::MathVector() {
}

MathVector::~MathVector() {
}

float MathVector::SqrMagnitude(FVector v) {
	return FVector::DotProduct(v, v);
}

float MathVector::Angle(FVector from, FVector to) {
	// sqrt(a) * sqrt(b) = sqrt(a * b) -- valid for real numbers
	float kEpsilonNormalSqrt = 1e-15F;
	float denominator = (float)sqrt(SqrMagnitude(from) * SqrMagnitude(to));
	if (denominator < kEpsilonNormalSqrt)
	    return 0.0;

	float dot = FMath::Clamp(FVector::DotProduct(from, to) / denominator, -1.0, 1.0);
	return ((float)acos(dot)) * 360 / 2 / M_PI;
}

// https://github.com/Unity-Technologies/UnityCsReference/blob/master/Runtime/Export/Math/Vector3.cs#L339
float MathVector::SignedAngle(FVector from, FVector to, FVector axis) {
	float unsignedAngle = Angle(from, to);

	float cross_x = from.Y * to.Z - from.Z * to.Y;
	float cross_y = from.Z * to.X - from.X * to.Z;
	float cross_z = from.X * to.Y - from.Y * to.X;
	float value = axis.X * cross_x + axis.Y * cross_y + axis.Z * cross_z;
	float sign = (value >= 0) ? 1 : -1;
	return unsignedAngle * sign;
}