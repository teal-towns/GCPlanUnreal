#pragma once

class ModelEVCharger
{
public:
    ModelEVCharger();
    ~ModelEVCharger();

    static void Create();
    static std::tuple<TArray<FVector>, TArray<FVector>> BuildCircularCableLocations(float, float, FVector, float, float);
};
