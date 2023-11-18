#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EditVertexActorComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GCPLAN_API UEditVertexActorComponent : public UActorComponent
{
	GENERATED_BODY()
public:	
	UEditVertexActorComponent();

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	
};
