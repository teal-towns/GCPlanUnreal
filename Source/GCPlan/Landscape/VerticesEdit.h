#pragma once

#include <mutex>

#include "../BuildingStructsActor.h"

class VerticesEdit {

private:
	static VerticesEdit *pinstance_;
	static std::mutex mutex_;

	TMap<FString, FPolygon> _items = {};
	TMap<FString, FVerticesEditActor> _itemsActors = {};
	// static AStaticMeshActor* _verticesEditParentActor;
	FString _currentUName = "";
	FString _currentType = "plot";
	FString _mode = "";
	FVerticesEditSelectedObject* _selectedObject = nullptr;
	// For trash.
	FVerticesEditSelectedObject* _lastSelectedObject = nullptr;

	bool _dragging = false;
	bool _mouseDown = false;
	FVector _mouseDownStartLocation = FVector(0,0,0);
	FVector _lastWorldLocation = FVector(0,0,0);
	double _timeMouseDown = -1;
	double _timeDragDelay = 0.25;

	TMap<FString, FString> _meshMapEdge = {
		{ "plot", "EdgeBlue" },
		// { "building", "EdgeRed" },
		{ "road", "EdgeBlack" }
	};
	FVector _displayScale = FVector(20,20,20);
	FVector _displayScaleEdge = FVector(10,10,10);

	TMap<FString, std::function<void(FString, FPolygon)>> _listenersSavePolygon = {};
	TMap<FString, std::function<void(FString, FString)>> _listenersDeletePolygon = {};

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
	void Hide();
	// void Show();
	void SetMode(FString mode = "");
	void Trash();
	FPolygon GetOrCreateCurrent();
	void CheckComputeCenter(FString uName);
	void AddVertex(FVector posAdd);
	void AddVertexOnEdge(FString uName, FVector worldLocation);
	void DeleteVertex(FString uName, int index);
	void MoveVertex(FString uName, int index, FVector newLocation);
	void HidePolygon(FString uName);
	void DeletePolygon(FString uName);
	void MovePolygon(FString uName, FVector moveOffset, bool saveAsFinal = false);

	void DrawEdge(FString uName, FVector start, FVector end);
	void RemoveAllEdges(FString uName);
	void DrawAllEdges(FString uName);

	void DrawItem(FString uName);
	void ImportPolygons(TMap<FString, FPolygon> polygons);
	TMap<FString, FPolygon> ExportPolygonsByType(FString type);
	void SavePolygon(FString uName);

	FVerticesEditSelectedObject* GetItemByLocation(FVector worldLocation);
	void OnMouseDown(FVector);
	void OnMouseUp(FVector);
	void OnMouseMove(FVector);
	void StartDrag();
	void StopDrag();

	void AddOnSavePolygon(FString key, std::function<void(FString, FPolygon)> callback);
	void RemoveOnSavePolygon(FString key);
	void AddOnDeletePolygon(FString key, std::function<void(FString, FString)> callback);
	void RemoveOnDeletePolygon(FString key);

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
