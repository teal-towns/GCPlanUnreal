#pragma once

#include <mutex>

#include "../BuildingStructsActor.h"

class VerticesEdit {

private:
	static VerticesEdit *pinstance_;
	static std::mutex mutex_;

	TMap<FString, FVerticesEdit> _items = {};
	TMap<FString, FVerticesEditActor> _itemsActors = {};
	// static AStaticMeshActor* _verticesEditParentActor;
	FString _currentUName = "";
	FString _currentType = "plot";
	TMap<FString, FString> _meshMap = {
		{ "plot", "EdgeBlue" },
		{ "building", "EdgeRed" },
		{ "road", "EdgeBlack" }
	};
	FVector _displayScale = FVector(20,20,20);
	FVector _displayScaleEdge = FVector(10,10,10);

	// TMap<FString, std::function<void(FString, int)>> _listenersDeleteVertex = {};
	// TMap<FString, std::function<void(FString, int, FVector)>> _listenersMoveVertex = {};
	// TMap<FString, std::function<void(FString, int, FVector)>> _listenersAddVertex = {};
	// TMap<FString, std::function<void(FString)>> _listenersDeleteShape = {};
	// TMap<FString, std::function<void(FString, FVector)>> _listenersMoveShape = {};

public:
	VerticesEdit();
	~VerticesEdit();

	// Singletons should not be cloneable or assignable.
	VerticesEdit(VerticesEdit &other) = delete;
	void operator=(const VerticesEdit &) = delete;

	static VerticesEdit *GetInstance();

	void SetType(FString);
	void DestroyItems();
	void CleanUp();
	FVerticesEdit GetOrCreateCurrent();
	void CheckComputeCenter(FString uName);
	void AddVertex(FVector posAdd);
	void DrawEdge(FString uName, FVector start, FVector end);

	// void AddOnDeleteVertex(FString key, std::function<void(FString, int)> callback);
	// void RemoveOnDeleteVertex(FString key);
	// void AddOnMoveVertex(FString key, std::function<void(FString, int, FVector)> callback);
	// void RemoveOnMoveVertex(FString key);
	// void AddOnAddVertex(FString key, std::function<void(FString, int, FVector)> callback);
	// void RemoveOnAddVertex(FString key);
	// void AddOnDeleteShape(FString key, std::function<void(FString)> callback);
	// void RemoveOnDeleteShape(FString key);
	// void AddOnMoveShape(FString key, std::function<void(FString, FVector)> callback);
	// void RemoveOnMoveShape(FString key);
};
