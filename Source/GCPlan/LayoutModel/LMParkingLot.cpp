#include "LMParkingLot.h"
#include "LayoutModelBase.h"
#include "../Building/BuildingRoom.h"
#include "../Common/DataConvert.h"
#include "../Common/Lodash.h"
#include "../Common/MathPolygon.h"
#include "../Common/MathVector.h"
#include "../Landscape/VerticesEdit.h"
#include "../Modeling/ModelBase.h"
#include "../Mesh/DynamicMaterial.h"
#include "../Mesh/LoadContent.h"
#include "../ModelingStructsActor.h"
#include "../BuildingStructsActor.h"

LMParkingLot::LMParkingLot()
{
}

LMParkingLot::~LMParkingLot()
{
}

TMap<FString, FPolygon> LMParkingLot::CreateFromInputs()
{
    ModelBase *modelBase = ModelBase::GetInstance();
    LayoutModelBase *layoutModelBase = LayoutModelBase::GetInstance();
    auto [layoutParams, modelParams] = layoutModelBase->GetInputs("ParkingLot", FVector(150, 50, 0));
    FString name = layoutParams.name;
    FVector size = layoutParams.size;
    return Create(size, modelParams, FModelCreateParams());
}

TMap<FString, FPolygon> LMParkingLot::Create(FVector size, FModelParams modelParams,
                                             FModelCreateParams createParamsIn)
{
    FString uName, type, pairsString, scaleString;
    TArray<FVector> vertices;
    TMap<FString, FPolygon> polygons = {};
    float parkingStripDepth = 7;
    float treeStripDepth = 1;
    float rowGap = 15;
    int rowCount = floor((size.Y + rowGap) / (parkingStripDepth + treeStripDepth + rowGap));
    UE_LOG(LogTemp, Warning, TEXT("LMParkingLot.Create rowCount %d"), rowCount);
    float treeGap = 15;
    int treeCountInRow = floor(size.X / treeGap);
    float centerY = -(size.Y / 2 - parkingStripDepth / 2 - treeStripDepth / 2 - rowGap / 2);
    FModelCreateParams createParamsRow;
    createParamsRow.rotation = createParamsIn.rotation;

    for (int ii = 0; ii < rowCount; ii++)
    {
        createParamsRow.offset = createParamsIn.offset + FVector(0, centerY, 0);
        FVector rowSize = FVector(size.X, parkingStripDepth, size.Z);
        LMParkingLot::CreateRow(rowSize, modelParams, createParamsRow);

        for (int jj = 0; jj < treeCountInRow; jj++)
        {
            uName = Lodash::GetInstanceId("pineS");
            pairsString = "mesh=pineS";
            vertices = {MathVector::RotateVector(FVector(-(size.X / 2 - treeGap / 2) + jj * treeGap, -6, 0), createParamsRow.rotation) + createParamsRow.offset};
            vertices = ModelBase::Vertices(vertices, createParamsIn);
            polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0, 0, 0), "pine", "point", pairsString));
        }
        centerY += parkingStripDepth + treeStripDepth + rowGap;
    }

    VerticesEdit *verticesEdit = VerticesEdit::GetInstance();
    verticesEdit->AddAndSave(polygons);
    return polygons;
}

TMap<FString, FPolygon> LMParkingLot::CreateRow(FVector size, FModelParams modelParams,
                                                FModelCreateParams createParamsIn)
{
    FString uName, type, pairsString, scaleString;
    TArray<FVector> vertices;
    TMap<FString, FPolygon> polygons = {};
    uName = Lodash::GetInstanceId("Floor");
    FString scaleX = FString::SanitizeFloat(size.X);
    FString scaleY = FString::SanitizeFloat(size.Y);
    FString scaleZ = FString::SanitizeFloat(size.Z);
    scaleString = "scale=" + scaleX + "," + scaleY + "," + scaleZ;
    UE_LOG(LogTemp, Warning, TEXT("LMParkingLot.CreateRow scaleString %s"), *scaleString);
    pairsString = "mesh=cube&mat=smoothConcrete&" + scaleString;
    vertices = {MathVector::RotateVector(FVector(0, 0, 0), createParamsIn.rotation) + createParamsIn.offset};
    vertices = ModelBase::Vertices(vertices, createParamsIn);
    polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0, 0, 0), "floor", "point", pairsString));

    float parameterMargin = 10;
    float solarChargingSectionLength = size.X - parameterMargin * 2;
    float solarChargingStationGap = 2;
    float singleStationLength = 20;
    int solarChargingStationCount = floor((solarChargingSectionLength + solarChargingStationGap) / (solarChargingStationGap + singleStationLength));

    float evChargerDepth = 1;

    float chargingStationStartX = -(size.X / 2 - parameterMargin - singleStationLength / 2);
    for (int ii = 0; ii < solarChargingStationCount; ii++)
    {
        uName = Lodash::GetInstanceId("SolarChargingStation");
        pairsString = "mesh=solarChargingStation";
        vertices = {MathVector::RotateVector(FVector(chargingStationStartX, 0, 0), createParamsIn.rotation) + createParamsIn.offset};
        vertices = ModelBase::Vertices(vertices, createParamsIn);
        polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0, 0, 0), "solarChargingStation", "point", pairsString));

        float slot1X = chargingStationStartX - singleStationLength / 4;
        float slot2X = chargingStationStartX + singleStationLength / 4;
        uName = Lodash::GetInstanceId("EVCharger");
        pairsString = "mesh=evCharger";
        vertices = {MathVector::RotateVector(FVector(slot1X, -(size.Y / 2), 0), createParamsIn.rotation) + createParamsIn.offset};
        polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0, 0, 0), "evCharger", "point", pairsString));

        uName = Lodash::GetInstanceId("EVCharger");
        pairsString = "mesh=evCharger";
        vertices = {MathVector::RotateVector(FVector(slot2X, -(size.Y / 2), 0), createParamsIn.rotation) + createParamsIn.offset};
        polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0, 0, 0), "evCharger", "point", pairsString));

        float slot1CarProb = Lodash::RandomRangeFloat(0, 1);
        float slot2CarProb = Lodash::RandomRangeFloat(0, 1);

        if (slot1CarProb > 0.5)
        {
            uName = Lodash::GetInstanceId("Car");
            pairsString = "mesh=mercedes";
            vertices = {MathVector::RotateVector(FVector(slot1X - 2, 5, 0), createParamsIn.rotation) + createParamsIn.offset}; // note: this car mesh seems not centered in the model
            polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0, 0, 0), "car", "point", pairsString));
        }

        if (slot2CarProb > 0.5)
        {
            uName = Lodash::GetInstanceId("Car");
            pairsString = "mesh=mercedes";
            vertices = {MathVector::RotateVector(FVector(slot2X - 2, 5, 0), createParamsIn.rotation) + createParamsIn.offset};
            polygons.Add(uName, FPolygon(uName, uName, vertices, FVector(0, 0, 0), "car", "point", pairsString));
        }

        chargingStationStartX += solarChargingStationGap + singleStationLength;
    }

    VerticesEdit *verticesEdit = VerticesEdit::GetInstance();
    verticesEdit->AddAndSave(polygons);
    return polygons;
}
