#include "PMPrism.h"

#include "Engine/StaticMeshActor.h"
#include "ProceduralMeshComponent.h"
#include "../Common/UnrealGlobal.h"

#include "PMBase.h"
#include "../Modeling/ModelBase.h"

PMPrism::PMPrism()
{
}

PMPrism::~PMPrism()
{
}

UStaticMesh *PMPrism::CreateFromInputs()
{
	PMBase *pmBase = PMBase::GetInstance();
	auto [inputs, modelParams] = pmBase->GetInputs("Prism1", FVector(1, 1, 1), 4, 0.0);
	return Create(inputs.name, inputs.sidesSegmentCount, inputs.size.Z, inputs.size.X, inputs.topOffsetWidth, inputs.closeTop, inputs.closeBottom);
}

std::tuple<TArray<FVector>, TArray<int32>, TArray<FVector>, TArray<FProcMeshTangent>, TArray<FVector2D>> PMPrism::InitializeMesh(int32 sidesSegmentCount, bool closeTop, bool closeBottom, bool doubleSided)
{
	int32 VertexCount = sidesSegmentCount * 4;		 // 4 vertices per face
	int32 TriangleCount = sidesSegmentCount * 2 * 3; // 2 triangles per face * 3 vertices per triangle

	// Count vertices for caps if set
	VertexCount += (sidesSegmentCount - 2) * 3 * (int(closeTop) + int(closeBottom));
	TriangleCount += (sidesSegmentCount - 2) * 3 * (int(closeTop) + int(closeBottom));

	// Count extra vertices if double sided
	if (doubleSided)
	{
		VertexCount = VertexCount * 2;
		TriangleCount = TriangleCount * 2;
	}

	TArray<FVector> Vertices;
	Vertices.AddUninitialized(VertexCount);
	TArray<FVector> Normals;
	Normals.AddUninitialized(VertexCount);
	TArray<FProcMeshTangent> Tangents;
	Tangents.AddUninitialized(VertexCount);
	TArray<FVector2D> UV0s;
	UV0s.AddUninitialized(VertexCount);

	TArray<int32> Triangles;
	Triangles.AddUninitialized(TriangleCount);

	return {Vertices, Triangles, Normals, Tangents, UV0s};
}

UStaticMesh *PMPrism::Create(FString name, int32 sidesSegmentCount, float height,
							 float width, float topOffsetWidth, bool closeTop, bool closeBottom, bool destroyActor)
{

	int32 VertexIndex = 0;
	int32 TriangleIndex = 0;
	bool doubleSided = true;
	auto [Vertices, Triangles, Normals, Tangents, UV0s] = InitializeMesh(sidesSegmentCount, closeTop, closeBottom, doubleSided);

	ModelBase* modelBase = ModelBase::GetInstance();
	PMBase *pmBase = PMBase::GetInstance();
	UnrealGlobal *unrealGlobal = UnrealGlobal::GetInstance();
	float scale = unrealGlobal->GetScale();

	FVector rotation = FVector(0,0,0);
	FActorSpawnParameters spawnParams;
	FVector location = FVector(0, 0, 0);
	AStaticMeshActor *actor;

	// Parent container
	actor = modelBase->CreateActor(name, location, rotation, FVector(1,1,1), spawnParams);
	USceneComponent *parent = actor->FindComponentByClass<USceneComponent>();
	UObject *parentObject = (UObject *)actor;
	UProceduralMeshComponent *ProceduralMesh = PMBase::CreateMesh("Prism1", parentObject);

	// -------------------------------------------------------
	// Make a column
	const float AngleBetweenQuads = (2.0f / static_cast<float>(sidesSegmentCount)) * PI;
	const float VMapPerQuad = 1.0f / static_cast<float>(sidesSegmentCount);
	const FVector Offset = FVector(0, 0, height * scale);

	// Start by building up vertices that make up the column sides
	for (int32 QuadIndex = 0; QuadIndex < sidesSegmentCount; QuadIndex++)
	{
		const float Angle = static_cast<float>(QuadIndex) * AngleBetweenQuads;
		const float NextAngle = static_cast<float>(QuadIndex + 1) * AngleBetweenQuads;

		// Set up the vertices
		const FVector P0 = FVector(FMath::Cos(Angle) * width, FMath::Sin(Angle) * width, 0.f) * scale;
		const FVector P1 = FVector(FMath::Cos(NextAngle) * width, FMath::Sin(NextAngle) * width, 0.f) * scale;
		const FVector P2 = FVector(FMath::Cos(NextAngle) * (width + topOffsetWidth), FMath::Sin(NextAngle) * (width + topOffsetWidth), 0.f) * scale + Offset;
		const FVector P3 = FVector(FMath::Cos(Angle) * (width + topOffsetWidth), FMath::Sin(Angle) * (width + topOffsetWidth), 0.f) * scale + Offset;

		// Set up the quad triangles
		int32 VertIndex1 = VertexIndex++;
		int32 VertIndex2 = VertexIndex++;
		int32 VertIndex3 = VertexIndex++;
		int32 VertIndex4 = VertexIndex++;

		Vertices[VertIndex1] = P0;
		Vertices[VertIndex2] = P1;
		Vertices[VertIndex3] = P2;
		Vertices[VertIndex4] = P3;

		// Now create two triangles from those four vertices
		// The order of these (clockwise/counter-clockwise) dictates which way the normal will face.
		Triangles[TriangleIndex++] = VertIndex4;
		Triangles[TriangleIndex++] = VertIndex3;
		Triangles[TriangleIndex++] = VertIndex1;

		Triangles[TriangleIndex++] = VertIndex3;
		Triangles[TriangleIndex++] = VertIndex2;
		Triangles[TriangleIndex++] = VertIndex1;

		// UVs.  Note that Unreal UV origin (0,0) is top left
		UV0s[VertIndex1] = FVector2D(1.0f - (VMapPerQuad * QuadIndex), 1.0f);
		UV0s[VertIndex2] = FVector2D(1.0f - (VMapPerQuad * (QuadIndex + 1)), 1.0f);
		UV0s[VertIndex3] = FVector2D(1.0f - (VMapPerQuad * (QuadIndex + 1)), 0.0f);
		UV0s[VertIndex4] = FVector2D(1.0f - (VMapPerQuad * QuadIndex), 0.0f);

		// Normals
		const FVector NormalCurrent = FVector::CrossProduct(Vertices[VertIndex1] - Vertices[VertIndex3], Vertices[VertIndex2] - Vertices[VertIndex3]).GetSafeNormal();

		// If not smoothing we just set the vertex normal to the same normal as the polygon they belong to
		Normals[VertIndex1] = Normals[VertIndex2] = Normals[VertIndex3] = Normals[VertIndex4] = NormalCurrent;

		// Tangents (perpendicular to the surface)
		const FVector SurfaceTangent = (P0 - P1).GetSafeNormal();
		Tangents[VertIndex1] = Tangents[VertIndex2] = Tangents[VertIndex3] = Tangents[VertIndex4] = FProcMeshTangent(SurfaceTangent, false);

		// -------------------------------------------------------
		// If double sided, create extra polygons but face the normals the other way.
		if (doubleSided)
		{
			VertIndex1 = VertexIndex++;
			VertIndex2 = VertexIndex++;
			VertIndex3 = VertexIndex++;
			VertIndex4 = VertexIndex++;

			Vertices[VertIndex1] = P0;
			Vertices[VertIndex2] = P1;
			Vertices[VertIndex3] = P2;
			Vertices[VertIndex4] = P3;

			// Reverse the poly order to face them the other way
			Triangles[TriangleIndex++] = VertIndex4;
			Triangles[TriangleIndex++] = VertIndex1;
			Triangles[TriangleIndex++] = VertIndex3;

			Triangles[TriangleIndex++] = VertIndex3;
			Triangles[TriangleIndex++] = VertIndex1;
			Triangles[TriangleIndex++] = VertIndex2;

			// UVs  (Unreal 1,1 is top left)
			UV0s[VertIndex1] = FVector2D(1.0f - (VMapPerQuad * QuadIndex), 1.0f);
			UV0s[VertIndex2] = FVector2D(1.0f - (VMapPerQuad * (QuadIndex + 1)), 1.0f);
			UV0s[VertIndex3] = FVector2D(1.0f - (VMapPerQuad * (QuadIndex + 1)), 0.0f);
			UV0s[VertIndex4] = FVector2D(1.0f - (VMapPerQuad * QuadIndex), 0.0f);

			// Just simple (unsmoothed) normal for these
			Normals[VertIndex1] = Normals[VertIndex2] = Normals[VertIndex3] = Normals[VertIndex4] = NormalCurrent;

			// Tangents (perpendicular to the surface)
			const FVector SurfaceTangentDbl = (P0 - P1).GetSafeNormal();
			Tangents[VertIndex1] = Tangents[VertIndex2] = Tangents[VertIndex3] = Tangents[VertIndex4] = FProcMeshTangent(SurfaceTangentDbl, false);
		}

		// -------------------------------------------------------
		// Caps are closed here by triangles that start at 0, then use the points along the circle for the other two corners.
		// A method uses a vertex in the center of the circle uses more polygons.
		if (QuadIndex != 0 && QuadIndex != sidesSegmentCount - 1 && (closeBottom || closeTop))
		{
			VertIndex1 = VertexIndex++;
			VertIndex2 = VertexIndex++;
			VertIndex3 = VertexIndex++;
			if (closeBottom)
			{
				FVector BottomCapVertex0 = FVector(FMath::Cos(0.f) * width, FMath::Sin(0.f) * width, 0.f) * scale;
				FVector BottomCapVertex1 = FVector(FMath::Cos(Angle) * width, FMath::Sin(Angle) * width, 0.f) * scale;
				FVector BottomCapVertex2 = FVector(FMath::Cos(NextAngle) * width, FMath::Sin(NextAngle) * width, 0.f) * scale;

				Vertices[VertIndex1] = BottomCapVertex0;
				Vertices[VertIndex2] = BottomCapVertex1;
				Vertices[VertIndex3] = BottomCapVertex2;

				Triangles[TriangleIndex++] = VertIndex1;
				Triangles[TriangleIndex++] = VertIndex2;
				Triangles[TriangleIndex++] = VertIndex3;

				UV0s[VertIndex1] = FVector2D(0.5f - (FMath::Cos(0.f) / 2.0f), 0.5f - (FMath::Sin(0.f) / 2.0f));
				UV0s[VertIndex2] = FVector2D(0.5f - (FMath::Cos(-Angle) / 2.0f), 0.5f - (FMath::Sin(-Angle) / 2.0f));
				UV0s[VertIndex3] = FVector2D(0.5f - (FMath::Cos(-NextAngle) / 2.0f), 0.5f - (FMath::Sin(-NextAngle) / 2.0f));

				FVector CapNormalCurrent = FVector::CrossProduct(Vertices[VertIndex1] - Vertices[VertIndex3], Vertices[VertIndex2] - Vertices[VertIndex3]).GetSafeNormal();
				Normals[VertIndex1] = Normals[VertIndex2] = Normals[VertIndex3] = CapNormalCurrent;

				// Tangents (perpendicular to the surface)
				FVector SurfaceTangentCap = (P0 - P1).GetSafeNormal();
				Tangents[VertIndex1] = Tangents[VertIndex2] = Tangents[VertIndex3] = FProcMeshTangent(SurfaceTangentCap, false);

				if (doubleSided)
				{
					VertIndex1 = VertexIndex++;
					VertIndex2 = VertexIndex++;
					VertIndex3 = VertexIndex++;
					Vertices[VertIndex1] = BottomCapVertex0;
					Vertices[VertIndex2] = BottomCapVertex2;
					Vertices[VertIndex3] = BottomCapVertex1;

					Triangles[TriangleIndex++] = VertIndex3;
					Triangles[TriangleIndex++] = VertIndex1;
					Triangles[TriangleIndex++] = VertIndex2;

					UV0s[VertIndex1] = FVector2D(0.5f - (FMath::Cos(0.f) / 2.0f), 0.5f - (FMath::Sin(0.f) / 2.0f));
					UV0s[VertIndex2] = FVector2D(0.5f - (FMath::Cos(-Angle) / 2.0f), 0.5f - (FMath::Sin(-Angle) / 2.0f));
					UV0s[VertIndex3] = FVector2D(0.5f - (FMath::Cos(-NextAngle) / 2.0f), 0.5f - (FMath::Sin(-NextAngle) / 2.0f));
					Normals[VertIndex1] = Normals[VertIndex2] = Normals[VertIndex3] = CapNormalCurrent;
					Tangents[VertIndex1] = Tangents[VertIndex2] = Tangents[VertIndex3] = FProcMeshTangent(SurfaceTangentCap, false);
				}
			}
			if (closeTop)
			{
				FVector TopCapVertex0 = FVector(FMath::Cos(0.f) * (width + topOffsetWidth), FMath::Sin(0.f) * (width + topOffsetWidth), 0.f) * scale;
				FVector TopCapVertex1 = FVector(FMath::Cos(Angle) * (width + topOffsetWidth), FMath::Sin(Angle) * (width + topOffsetWidth), 0.f) * scale;
				FVector TopCapVertex2 = FVector(FMath::Cos(NextAngle) * (width + topOffsetWidth), FMath::Sin(NextAngle) * (width + topOffsetWidth), 0.f) * scale;
				TopCapVertex0 = TopCapVertex0 + Offset;
				TopCapVertex1 = TopCapVertex1 + Offset;
				TopCapVertex2 = TopCapVertex2 + Offset;

				Vertices[VertIndex1] = TopCapVertex0;
				Vertices[VertIndex2] = TopCapVertex1;
				Vertices[VertIndex3] = TopCapVertex2;

				Triangles[TriangleIndex++] = VertIndex3;
				Triangles[TriangleIndex++] = VertIndex2;
				Triangles[TriangleIndex++] = VertIndex1;

				UV0s[VertIndex1] = FVector2D(0.5f - (FMath::Cos(0.f) / 2.0f), 0.5f - (FMath::Sin(0.f) / 2.0f));
				UV0s[VertIndex2] = FVector2D(0.5f - (FMath::Cos(Angle) / 2.0f), 0.5f - (FMath::Sin(Angle) / 2.0f));
				UV0s[VertIndex3] = FVector2D(0.5f - (FMath::Cos(NextAngle) / 2.0f), 0.5f - (FMath::Sin(NextAngle) / 2.0f));

				FVector CapNormalCurrent = FVector::CrossProduct(Vertices[VertIndex1] - Vertices[VertIndex3], Vertices[VertIndex2] - Vertices[VertIndex3]).GetSafeNormal();
				Normals[VertIndex1] = Normals[VertIndex2] = Normals[VertIndex3] = CapNormalCurrent;

				// Tangents (perpendicular to the surface)
				FVector SurfaceTangentCap = (P0 - P1).GetSafeNormal();
				Tangents[VertIndex1] = Tangents[VertIndex2] = Tangents[VertIndex3] = FProcMeshTangent(SurfaceTangentCap, false);

				if (doubleSided)
				{
					VertIndex1 = VertexIndex++;
					VertIndex2 = VertexIndex++;
					VertIndex3 = VertexIndex++;
					Vertices[VertIndex1] = TopCapVertex0;
					Vertices[VertIndex2] = TopCapVertex2;
					Vertices[VertIndex3] = TopCapVertex1;

					Triangles[TriangleIndex++] = VertIndex3;
					Triangles[TriangleIndex++] = VertIndex1;
					Triangles[TriangleIndex++] = VertIndex2;

					UV0s[VertIndex1] = FVector2D(0.5f - (FMath::Cos(0.f) / 2.0f), 0.5f - (FMath::Sin(0.f) / 2.0f));
					UV0s[VertIndex2] = FVector2D(0.5f - (FMath::Cos(Angle) / 2.0f), 0.5f - (FMath::Sin(Angle) / 2.0f));
					UV0s[VertIndex3] = FVector2D(0.5f - (FMath::Cos(NextAngle) / 2.0f), 0.5f - (FMath::Sin(NextAngle) / 2.0f));

					Normals[VertIndex1] = Normals[VertIndex2] = Normals[VertIndex3] = CapNormalCurrent;
					Tangents[VertIndex1] = Tangents[VertIndex2] = Tangents[VertIndex3] = FProcMeshTangent(SurfaceTangentCap, false);
				}
			}
		}
	}

	PMBase::AddMeshSection(ProceduralMesh, Vertices, UV0s, Triangles, Normals, Tangents);
	UStaticMesh *mesh = PMBase::ToStaticMesh(ProceduralMesh);
	if (!destroyActor)
	{
		PMBase::AddMesh(actor, mesh);
	}
	else {
		if (IsValid(actor)) {
			actor->Destroy();
		}
	}
	return mesh;
}
