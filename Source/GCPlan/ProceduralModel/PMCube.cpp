#include "PMCube.h"

#include "Engine/StaticMeshActor.h"
#include "ProceduralMeshComponent.h"

#include "../Common/UnrealGlobal.h"
// #include "../Modeling/ModelBase.h"
#include "PMBase.h"

PMCube::PMCube() {
}

PMCube::~PMCube() {
}

UStaticMesh* PMCube::CreateFromInputs() {
	PMBase* pmBase = PMBase::GetInstance();
	FProceduralModelBase inputs = pmBase->GetInputs("Cube1", FVector(1,1,1), FVector(2,2,2));
	return Create(inputs.name, inputs.size, inputs.vertices, inputs.tags);
}

UStaticMesh* PMCube::Create(FString name, FVector size, FVector vertices, TArray<FString> tags,
	bool destroyActor) {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	float UVScale = 1;

	PMBase* pmBase = PMBase::GetInstance();

	FVector sizePerVertex = FVector(size.X / (vertices.X - 1), size.Y / (vertices.Y - 1),
		size.Z / (vertices.Z - 1));

	FRotator rotation = FRotator(0,0,0);
	FActorSpawnParameters spawnParams;
	FVector location = FVector(0,0,0);
	FVector scale = FVector(1,1,1);
	AStaticMeshActor* actor;

	// Parent container
	actor = pmBase->CreateActor(name, location, rotation, spawnParams);
	USceneComponent* parent = actor->FindComponentByClass<USceneComponent>();
	UObject* parentObject = (UObject*)actor;
	UProceduralMeshComponent* ProceduralMesh = PMBase::CreateMesh(parentObject, parent, "Cube1");

	float x, y, z;
	TArray<FVector> Vertices = {};
	TArray<FVector2D> UV0 = {};
	TArray<int> Triangles = {};
	int vertexIndex, vertexPastIndex, vertexBottomLeftIndex, vertexBottomRightIndex,
		vertexTopLeftIndex, vertexTopRightIndex;

	// Cube has 6 sides.
	UE_LOG(LogTemp, Display, TEXT("vertices %s"), *vertices.ToString());
	// Front: YZ (X is constant)
	x = size.X * -0.5;
	for (int zz = 0; zz < vertices.Z; zz++) {
		for (int yy = 0; yy < vertices.Y; yy++) {
			Vertices.Add(FVector(x, yy * sizePerVertex.Y * unrealGlobal->GetScale(), zz * sizePerVertex.Z * unrealGlobal->GetScale()));
			UV0.Add(FVector2D((float)yy * UVScale, (float)zz * UVScale));

			// Do 1 quad (6 triangles, 2 vertices) at a time so go every other time.
			if (yy % 2 == 1 && zz < (vertices.Z - 1)) {
				vertexIndex = (zz * vertices.Y) + yy;
				vertexBottomRightIndex = vertexIndex;
				vertexBottomLeftIndex = vertexIndex - 1;
				vertexTopRightIndex = ((zz + 1) * vertices.Y) + yy;
				vertexTopLeftIndex = vertexTopRightIndex - 1;
				Triangles.Add(vertexBottomLeftIndex);
				Triangles.Add(vertexBottomRightIndex);
				Triangles.Add(vertexTopLeftIndex);
				Triangles.Add(vertexBottomRightIndex);
				Triangles.Add(vertexTopRightIndex);
				Triangles.Add(vertexTopLeftIndex);
				UE_LOG(LogTemp, Display, TEXT("Triangles %d %d %d %d %d %d"), vertexBottomLeftIndex, vertexBottomRightIndex, vertexTopLeftIndex, vertexBottomRightIndex, vertexTopRightIndex, vertexTopLeftIndex);
			}
		}
	}

	// TODO - other sides

	PMBase::AddMeshSection(ProceduralMesh, Vertices, UV0, Triangles);

	UStaticMesh* mesh = PMBase::ToStaticMesh(ProceduralMesh);
	if (!destroyActor) {
		PMBase::AddMesh(actor, mesh);
	} else {
		actor->Destroy();
	}
	return mesh;
}
