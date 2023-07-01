#include "MathVector.h"

#define _USE_MATH_DEFINES		// for pi
#include "math.h"

#include "./Lodash.h"

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

float MathVector::ConstrainDegrees(float degrees) {
	while (degrees >= 360) {
		degrees -= 360;
	}
	while (degrees <= -360) {
		degrees += 360;
	}
	return degrees;
}

FVector MathVector::ConstrainRotation(FVector rotation) {
	rotation.X = ConstrainDegrees(rotation.X);
	rotation.Y = ConstrainDegrees(rotation.Y);
	rotation.Z = ConstrainDegrees(rotation.Z);
	return rotation;
}

FVector MathVector::RotateVector(FVector vector, FVector rotation) {
	FRotator rotator = FRotator(rotation.Y, rotation.Z, rotation.X);
	return rotator.RotateVector(vector);
}

TArray<FVector> MathVector::RotateVertices(TArray<FVector> vertices, FVector rotation,
	FVector offset) {
	FRotator rotator = FRotator(rotation.Y, rotation.Z, rotation.X);
	for (int ii = 0; ii < vertices.Num(); ii++) {
		vertices[ii] = rotator.RotateVector(vertices[ii]) + offset;
	}
	return vertices;
}

TArray<FVector> MathVector::RotateAround(TArray<FVector> vertices, FVector rotation, FVector center) {
	FVector diff;
	FRotator rotator = FRotator(rotation.Y, rotation.Z, rotation.X);
	for (int ii = 0; ii < vertices.Num(); ii++) {
		diff = vertices[ii] - center;
		vertices[ii] = center + rotator.RotateVector(diff);
	}
	return vertices;
}

// https://youtu.be/xVF9pnarOX4?t=319
TArray<FVector> MathVector::BeizerCurvePoints(FVector start, FVector end, FVector control, int count) {
	TArray<FVector> points = {};
	FVector startControlPoint, endControlPoint, startEndDiff, startEndPoint;
	FVector startControlDiff = control - start;
	FVector endControlDiff = end - control;
	float ratioPerIteration = (float)(1 / (float)count);
	float startControlLength = startControlDiff.Size();
	float endControlLength = endControlDiff.Size();
	float startEndLength;
	FVector crossAxisLine;
	float sign = 0;
	for (int ii = 0; ii < count; ii++) {
		startControlPoint = start + startControlDiff.GetClampedToMaxSize(startControlLength * ratioPerIteration * ii);
		endControlPoint = control + endControlDiff.GetClampedToMaxSize(endControlLength * ratioPerIteration * ii);
		startEndDiff = endControlPoint - startControlPoint;
		startEndLength = startEndDiff.Size();
		startEndPoint = startControlPoint + startEndDiff.GetClampedToMaxSize(startEndLength * ratioPerIteration * ii);
		// Give some jitter
		// if (randomOffset != 0) {
		// 	sign = Lodash::RandomRangeInt(0,1) == 0 ? 1 : -1;
		// 	crossAxisLine = startEndDiff.RotateAngleAxis(sign * 90, FVector(0,0,1));
		// 	startEndPoint += crossAxisLine.GetClampedToMaxSize(randomOffset);
		// }
		points.Add(startEndPoint);
		// UE_LOG(LogTemp, Display, TEXT("ii %d point %s startControlPoint %s endControlPoint %s"), ii, *startEndPoint.ToString(), *startControlPoint.ToString(), *endControlPoint.ToString());
	}
	// Add end point.
	points.Add(end);
	return points;
}
