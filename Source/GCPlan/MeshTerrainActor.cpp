#include "MeshTerrainActor.h"

#include "ProceduralMeshComponent.h"

#include "Landscape/MeshTerrain.h"

AMeshTerrainActor::AMeshTerrainActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMeshTerrainActor::BeginPlay()
{
	Super::BeginPlay();
}

void AMeshTerrainActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMeshTerrainActor::InitMesh() {
	// https://forums.unrealengine.com/t/when-is-valid-time-to-create-subobjects-components/367184/4
    // ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralMesh");
    // ProceduralMesh->SetupAttachment(GetRootComponent());
	ProceduralMesh = NewObject<UProceduralMeshComponent>(this,UProceduralMeshComponent::StaticClass(), TEXT("ProceduralMeshTerrain"));
	ProceduralMesh->CreationMethod = EComponentCreationMethod::Instance;
    ProceduralMesh->RegisterComponent();
    ProceduralMesh->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
}

void AMeshTerrainActor::EditorInitMesh() {
	InitMesh();
    MeshTerrain* meshTerrain = MeshTerrain::GetInstance();
    meshTerrain->SetProceduralMesh(ProceduralMesh, Material);
    meshTerrain->CreateTerrainFromHeightMap();
}

void AMeshTerrainActor::EditorClearMesh() {
	InitMesh();
	MeshTerrain* meshTerrain = MeshTerrain::GetInstance();
	meshTerrain->SetProceduralMesh(ProceduralMesh, Material);
	meshTerrain->ClearMesh();
}

