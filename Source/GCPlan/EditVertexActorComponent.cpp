#include "EditVertexActorComponent.h"

UEditVertexActorComponent::UEditVertexActorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEditVertexActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

