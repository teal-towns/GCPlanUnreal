#pragma once

#include "../BuildingStructsActor.h"
#include "../Modeling/ModelBase.h"

class LMParkingLot
{
public:
    LMParkingLot();
    ~LMParkingLot();

    static TMap<FString, FPolygon> CreateFromInputs();
    static TMap<FString, FPolygon> Create(FVector size, FModelParams modelParams = FModelParams(),
                                          FModelCreateParams createParamsIn = FModelCreateParams());
    static TMap<FString, FPolygon> CreateStrip(FVector size, FModelParams modelParams = FModelParams(),
                                               FModelCreateParams createParamsIn = FModelCreateParams());
};
