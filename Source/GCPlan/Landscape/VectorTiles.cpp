#include "VectorTiles.h"
#include "JsonObjectConverter.h"

#include "../Common/Lodash.h"
#include "../Common/UnrealGlobal.h"
#include "../Draw/DrawVertices.h"
// #include "./VerticesEdit.h"

VectorTiles* VectorTiles::pinstance_{nullptr};
std::mutex VectorTiles::mutex_;

VectorTiles::VectorTiles() {
}

VectorTiles::~VectorTiles() {
}

VectorTiles *VectorTiles::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new VectorTiles();
	}
	return pinstance_;
}

void VectorTiles::Init() {
	if (!_inited) {
		_inited = true;
		this->InitSocketOn();
	}
}

void VectorTiles::InitSocketOn() {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();

	this->DestroySocket();
	FString prefix = "VectorTiles";
	_socketKeys.Add(unrealGlobal->SocketActor->On(prefix, "get-vector-tiles", [this](FString DataString) {
		FDataGetVectorTiles* Data = new FDataGetVectorTiles();
		if (!FJsonObjectConverter::JsonObjectStringToUStruct(DataString, Data, 0, 0)) {
			UE_LOG(LogTemp, Error, TEXT("VectorTiles.On get-vector-tiles json parse error"));
		} else {
			if (Data->valid > 0) {
				// DrawVertices* drawVertices = DrawVertices::GetInstance();
				DrawVertices::LoadPolygons(Data->polygons);
				// verticesEdit->AddSimplified(Data->polygons);
				// for (int ii = 0; ii < Data->polygons.Num(); ii++) {
				// 	UE_LOG(LogTemp, Display, TEXT("polygon_id %s"), *Data->polygons[ii].uName);
				// }
				UE_LOG(LogTemp, Display, TEXT("added polygons"));
			}
		}
	}));
}

void VectorTiles::Destroy() {
	this->DestroySocket();
}

void VectorTiles::DestroySocket() {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
    unrealGlobal->SocketOffRoutes(_socketKeys);
	_socketKeys.Empty();
}

void VectorTiles::GetTiles(float lng, float lat, float xMeters, float yMeters) {
	Init();
    UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	// VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	// verticesEdit->DestroyItems();
	TMap<FString, FString> Data = {
		{ "lng", Lodash::ToFixed(lng, 6) },
        { "lat", Lodash::ToFixed(lat, 6) },
        { "xMeters", Lodash::ToFixed(xMeters, 0) },
        { "yMeters", Lodash::ToFixed(yMeters, 0) },
	};
	unrealGlobal->SocketActor->Emit("get-vector-tiles", Data);
}