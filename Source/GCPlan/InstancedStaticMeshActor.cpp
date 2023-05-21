#include "InstancedStaticMeshActor.h"
#include "Components/InstancedStaticMeshComponent.h"

AInstancedStaticMeshActor::AInstancedStaticMeshActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	InstancedStaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedStaticMeshComponent"));
	SetRootComponent(InstancedStaticMeshComponent);
	InstancedStaticMeshComponent->SetMobility(EComponentMobility::Static);

	// This apparently can only be called from a constructor.. Otherwise get a runtime error / crash.
	// We could get ALL meshes we want to use, but we would need a separate singleton actor for that since
	// we do not want to get all meshes for every actor (since each actor will only use ONE mesh).
	// Instead create a blueprint copy of this actor for each mesh.
	// SetMesh("/Script/Engine.StaticMesh'/Game/Buildings/Hex/HexModule/HexModule.HexModule'");
}

void AInstancedStaticMeshActor::BeginPlay()
{
	Super::BeginPlay();

	GetName();
	// CreateInstance(FVector(1000, 0, 0), FRotator(0,0,0), FVector(1,1,1));
	// CreateInstance(FVector(0, 1000, 0), FRotator(0,30,0), FVector(1,1,1));
	// CreateInstance(FVector(3000, 1000, 0), FRotator(0,60,0), FVector(2,2,2));
}

void AInstancedStaticMeshActor::CreateInstance(FVector Translation, FRotator Rotation, FVector Scale) {
	InstancedStaticMeshComponent->AddInstance(FTransform(Rotation, Translation, Scale));
}

// void AInstancedStaticMeshActor::SetMesh(FString MeshPath) {
// 	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset((TEXT("%s"), *MeshPath));
// 	UStaticMesh* Asset = MeshAsset.Object;
// 	InstancedStaticMeshComponent->SetStaticMesh(Asset);
// }

FString AInstancedStaticMeshActor::GetName() {
	FString MeshName = InstancedStaticMeshComponent->GetStaticMesh()->GetName();
	// UE_LOG(LogTemp, Display, TEXT("MeshName %s"), *MeshName);
	return MeshName;
}

void AInstancedStaticMeshActor::ClearInstances() {
	InstancedStaticMeshComponent->ClearInstances();
}
