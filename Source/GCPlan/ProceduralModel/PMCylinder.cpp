#include "PMCylinder.h"

#include "ProceduralMeshComponent.h"

#include "../Common/UnrealGlobal.h"

#include "PMBase.h"

PMCylinder::PMCylinder() {
}

PMCylinder::~PMCylinder() {
}

UStaticMesh* PMCylinder::CreateFromInputs() {
	PMBase* pmBase = PMBase::GetInstance();
	FProceduralModelBase inputs = pmBase->GetInputs("Cylinder1", FVector(1,1,1), FVector(10,10,10));
	return Create(inputs.name, inputs.size, inputs.vertices, inputs.tags);
}

UStaticMesh* PMCylinder::Create(FString name, FVector size, FVector vertices, TArray<FString> tags, bool destroyActor) {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	float UVScale = 1;

	PMBase* pmBase = PMBase::GetInstance();

	FRotator rotation = FRotator(0,0,0);
	FActorSpawnParameters spawnParams;
	FVector location = FVector(0,0,0);
	FVector scale = FVector(1,1,1);
	AStaticMeshActor* actor;

	// Parent container
	actor = pmBase->CreateActor(name, location, rotation, spawnParams);
	USceneComponent* parent = actor->FindComponentByClass<USceneComponent>();
	UObject* parentObject = (UObject*)actor;
	UProceduralMeshComponent* ProceduralMesh = PMBase::CreateMesh(parentObject, parent, "Cylinder1");

	// Make cylinder that goes straight in Z direction.
	float radius = (size.X * 0.5 + size.Y * 0.5) / 2;
	float anglePerVertex = 360 / vertices.X;
	float zSizePerVertex = size.Z / (vertices.Z - 1);
	FVector centerToEdge = FVector(1,0,0).GetClampedToMaxSize(radius);
	FVector xyLine;
	int vertexIndex, vertexPastIndex, vertexBottomLeftIndex, vertexBottomRightIndex,
		vertexTopLeftIndex, vertexTopRightIndex;
	FVector center = FVector(0,0,0);
	TArray<FVector> Vertices = {};
	TArray<FVector2D> UV0 = {};
	TArray<int> Triangles = {};
	float extrudeFactor = 1;
	TArray<float> extrudeFactors = {1, 1, 1.1, 1.2, 1.3, 1.3, 1.2, 1.1, 1, 1};
	for (int zz = 0; zz < vertices.Z; zz++) {
		for (int xy = 0; xy < vertices.X; xy++) {
			xyLine = centerToEdge.RotateAngleAxis(anglePerVertex * xy, FVector(0,0,1)).GetClampedToMaxSize(radius) * extrudeFactors[zz];
			Vertices.Add(FVector(center.X + xyLine.X * unrealGlobal->GetScale(), center.Y + xyLine.Y * unrealGlobal->GetScale(), zz * zSizePerVertex * unrealGlobal->GetScale()));

			UV0.Add(FVector2D((float)xy * UVScale, (float)zz * UVScale));
			// UE_LOG(LogTemp, Display, TEXT("UVs %f %f %d %d %d"), (float)xy * UVScale, (float)zz * UVScale, zz, xy, UVScale);
			// As with triangles, need to add last UVs to connect to start
			if (xy == vertices.X - 1) {
				UV0.Add(FVector2D((float)(xy + 1) * UVScale, (float)(zz + 1) * UVScale));
				// UE_LOG(LogTemp, Display, TEXT("UVs %f %f %d %d %d"), (float)(xy + 1) * UVScale, (float)(zz + 1) * UVScale, zz, xy, UVScale);
			}

			// Do 1 quad (6 triangles, 2 vertices) at a time so go every other time.
			if (zz < (vertices.Z - 1)) {
				if (xy > 0) {
					vertexIndex = (zz * vertices.X) + xy;
					vertexBottomRightIndex = vertexIndex;
					vertexBottomLeftIndex = vertexIndex - 1;
					vertexTopRightIndex = ((zz + 1) * vertices.X) + xy;
					vertexTopLeftIndex = vertexTopRightIndex - 1;
					// Reverse order (otherwise inside is visible and outside is not).
					Triangles.Add(vertexBottomLeftIndex);
					Triangles.Add(vertexTopLeftIndex);
					Triangles.Add(vertexBottomRightIndex);

					Triangles.Add(vertexBottomRightIndex);
					Triangles.Add(vertexTopLeftIndex);
					Triangles.Add(vertexTopRightIndex);
					// UE_LOG(LogTemp, Display, TEXT("Triangles %d %d %d %d %d %d"), vertexBottomLeftIndex, vertexBottomRightIndex, vertexTopLeftIndex, vertexBottomRightIndex, vertexTopRightIndex, vertexTopLeftIndex);
				}
				// Do the last to the first, since we start with 1 instead of 0.
				if (xy == vertices.X - 1) {
					vertexIndex = (zz * vertices.X) + 0;
					vertexBottomRightIndex = vertexIndex;
					vertexBottomLeftIndex = (zz * vertices.X) + xy;
					vertexTopRightIndex = ((zz + 1) * vertices.X) + 0;
					vertexTopLeftIndex = ((zz + 1) * vertices.X) + xy;
					// Reverse order (otherwise inside is visible and outside is not).
					Triangles.Add(vertexBottomLeftIndex);
					Triangles.Add(vertexTopLeftIndex);
					Triangles.Add(vertexBottomRightIndex);

					Triangles.Add(vertexBottomRightIndex);
					Triangles.Add(vertexTopLeftIndex);
					Triangles.Add(vertexTopRightIndex);
					// UE_LOG(LogTemp, Display, TEXT("Triangles %d %d %d %d %d %d"), vertexBottomLeftIndex, vertexBottomRightIndex, vertexTopLeftIndex, vertexBottomRightIndex, vertexTopRightIndex, vertexTopLeftIndex);
				}
			}
		}
	}

	PMBase::AddMeshSection(ProceduralMesh, Vertices, UV0, Triangles);

	UStaticMesh* mesh = PMBase::ToStaticMesh(ProceduralMesh);
	if (!destroyActor) {
		PMBase::AddMesh(actor, mesh);
	} else {
		actor->Destroy();
	}
	return mesh;
}
