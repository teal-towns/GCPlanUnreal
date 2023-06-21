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
	FString _currentType = "";
	FString _currentShape = "";
	FString _currentPairsString = "";
	TArray<FString> _currentFilterTypes = {};
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

	FVector _displayScale = FVector(20,20,20);
	FVector _displayScaleEdge = FVector(10,10,10);

	TMap<FString, std::function<void(FString, FPolygon)>> _listenersSavePolygon = {};
	TMap<FString, std::function<void(FString, FString)>> _listenersDeletePolygon = {};

public:
	VerticesEdit();
	~VerticesEdit();

	// Singletons should not be cloneable or assignable.
	VerticesEdit(VerticesEdit &other) = delete;
	void operator=(const VerticesEdit &) = delete;

	static VerticesEdit *GetInstance();

	void SetType(FString);
	void SetShape(FString);
	void SetPairsString(FString);
	void SetFilterTypes(TArray<FString>);
	TMap<FString, FPolygon> FilterByTypes(TArray<FString> types);
	void DestroyItems();
	void CleanUp();
	void Hide();
	void SetMode(FString mode = "");
	void Trash();
	std::tuple<FPolygon, bool> GetOrCreateCurrent();
	void CheckComputeCenter(FString uName);
	FString GetEdgeMesh(FString);
	void AddVertex(FVector posAdd);
	void AddVertexOnEdge(FString uName, FVector worldLocation);
	void DeleteVertex(FString uName, int index);
	void MoveVertex(FString uName, int index, FVector newLocation);
	void HidePolygon(FString uName);
	void DeletePolygon(FString uName, bool deleteChildren = true, bool save = true);
	void MovePolygon(FString uName, FVector moveOffset, bool saveAsFinal = false);

	void DrawEdge(FString uName, FVector start, FVector end);
	void RemoveAllEdges(FString uName);
	void DrawAllEdges(FString uName);

	void DrawAll();
	void DrawItemsByTypes(TArray<FString> types);
	void DrawItem(FString uName);
	void ImportPolygons(TMap<FString, FPolygon> polygons);
	TMap<FString, FPolygon> ExportPolygonsByType(FString type);
	void LoadFromFiles();
	void RemoveChildren(FString type, bool save = true);
	FString GetParentPairsString(FString type, FString childUName);
	int CheckSubdividePolygons(FString type, bool save = true);
	void AddAndSave(TMap<FString, FPolygon> polygons);
	void SaveToFile(FString type);
	void SavePolygon(FString uName);
	FPolygon GetByUName(FString uName);

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
};
