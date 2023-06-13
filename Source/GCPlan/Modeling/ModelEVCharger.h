#pragma once

class ModelEVCharger
{
public:
    ModelEVCharger();
    ~ModelEVCharger();

    static void Create();
    static std::tuple<TArray<FVector>, TArray<FRotator>> BuildCircularCableLocations(float, float, FVector, float, float);
};
