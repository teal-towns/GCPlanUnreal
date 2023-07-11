#include "VerticesEdit.h"

#include "../BuildingStructsActor.h"
#include "../Common/DataConvert.h"
#include "../Common/Lodash.h"
#include "../Common/MathPolygon.h"
#include "../Common/MathVector.h"
#include "../Common/UnrealGlobal.h"
#include "../Data/DataFileProject.h"
#include "../Landscape/HeightMap.h"
#include "../Mesh/InstancedMesh.h"
#include "../Plot/PlotDivide.h"
#include "../ProceduralModel/PMBase.h"

VerticesEdit* VerticesEdit::pinstance_{nullptr};
std::mutex VerticesEdit::mutex_;

VerticesEdit::VerticesEdit() {
}

VerticesEdit::~VerticesEdit() {
}

VerticesEdit *VerticesEdit::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new VerticesEdit();
	}
	return pinstance_;
}

void VerticesEdit::SetType(FString type) {
	_currentType = type;
}

void VerticesEdit::SetShape(FString shape) {
	_currentShape = shape;
}

void VerticesEdit::SetPairsString(FString pairsString) {
	_currentPairsString = pairsString;
}

void VerticesEdit::SetFilterTypes(TArray<FString> filterTypes) {
	_currentFilterTypes = filterTypes;
	Hide();
	if (filterTypes.Num() < 1) {
		DrawAll();
	} else {
		DrawItemsByTypes(_currentFilterTypes);
	}
}

TMap<FString, FPolygon> VerticesEdit::FilterByTypes(TArray<FString> types) {
	TMap<FString, FPolygon> items = {};
	for (auto& Elem : _items) {
		if (types.Contains(Elem.Value.type)) {
			items.Add(Elem.Key, Elem.Value);
		}
	}
	return items;
}

TMap<FString, FPolygon> VerticesEdit::FilterByShapes(TArray<FString> shapes) {
	TMap<FString, FPolygon> items = {};
	for (auto& Elem : _items) {
		if (shapes.Contains(Elem.Value.shape)) {
			items.Add(Elem.Key, Elem.Value);
		}
	}
	return items;
}

void VerticesEdit::DestroyItems() {
	VerticesEdit::Hide();
	_items.Empty();
	_currentUName = "";
	_selectedObject = nullptr;
	_lastSelectedObject = nullptr;
}

void VerticesEdit::CleanUp() {
	DestroyItems();
	_dragging = false;
	_mouseDown = false;
	_mouseDownStartLocation = FVector(0,0,0);
	_lastWorldLocation = FVector(0,0,0);
	_timeMouseDown = -1;
	_mode = "";
	_currentType = "";
	_currentShape = "";
	_currentPairsString = "";
	_currentFilterTypes = {};

	pinstance_ = nullptr;

	_listenersSavePolygon.Empty();
	_listenersDeletePolygon.Empty();
}

void VerticesEdit::Hide() {
	_itemsActors.Empty();
	InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
	instancedMesh->ClearInstancesBulk({"VertexWhite", "EdgeBlack", "EdgeBlue", "EdgeRed"});
}

void VerticesEdit::SetMode(FString mode) {
	// If done adding validate and either remove or re-render (close).
	if (_mode == "add" && mode != "add" && _currentUName != "") {
		if (_items[_currentUName].vertices.Num() < 2) {
			DeletePolygon(_currentUName);
		} else {
			DrawAllEdges(_currentUName);
			SavePolygon(_currentUName);
		}
	}
	_currentUName = "";
	_mode = mode;
}

void VerticesEdit::Trash() {
	if (_lastSelectedObject) {
		if (_lastSelectedObject->objectType == "vertex") {
			DeleteVertex(_lastSelectedObject->uName, _lastSelectedObject->objectIndex);
		} else if (_lastSelectedObject->objectType == "center") {
			DeletePolygon(_lastSelectedObject->uName);
		}
	}
}

void VerticesEdit::OnMouseDown(FVector worldLocation) {
	_mouseDown = true;
	_mouseDownStartLocation = worldLocation;
	_timeMouseDown = FPlatformTime::Seconds();
	// Reset.
	StopDrag();
	_selectedObject = nullptr;
	_lastSelectedObject = nullptr;
	GetItemByLocation(worldLocation);
}

void VerticesEdit::OnMouseUp(FVector worldLocation) {
	if (_selectedObject && _mode != "add") {
		if (_selectedObject->objectType == "edge") {
			AddVertexOnEdge(_selectedObject->uName, worldLocation);
		}
		// Set final location (on move we update displayed instances but NOT actual data,
		// so we can compute the offset from the original location). Once done, actually move it.
		if (_dragging && _selectedObject->objectType == "center") {
			FVector moveOffset = worldLocation - _mouseDownStartLocation;
			MovePolygon(_selectedObject->uName, moveOffset, true);
		}
		SavePolygon(_selectedObject->uName);
	} else if (_mode == "add") {
		AddVertex(worldLocation);
		if (_currentShape == "point") {
			SavePolygon(_currentUName);
			_currentUName = "";
		}
	}
	// Reset.
	_mouseDown = false;
	_mouseDownStartLocation = FVector(0,0,0);
	_timeMouseDown = -1;
	StopDrag();
	_selectedObject = nullptr;
}

void VerticesEdit::OnMouseMove(FVector worldLocation) {
	if (worldLocation != _lastWorldLocation && _mode != "add") {
		_lastWorldLocation = worldLocation;
		if (_dragging && _selectedObject) {
			if (_selectedObject->objectType == "vertex") {
				MoveVertex(_selectedObject->uName, _selectedObject->objectIndex, worldLocation);
			} else if (_selectedObject->objectType == "center") {
				FVector moveOffset = worldLocation - _mouseDownStartLocation;
				MovePolygon(_selectedObject->uName, moveOffset, false);
			}
		}
		if (_mouseDown && !_dragging && _timeMouseDown > 0 && (FPlatformTime::Seconds() - _timeMouseDown) > _timeDragDelay) {
			StartDrag();
		}
	}
}

void VerticesEdit::StopDrag() {
	_dragging = false;
}

void VerticesEdit::StartDrag() {
	_dragging = true;
}

void VerticesEdit::SetDisplayScale(FVector displayScale, FVector displayScaleEdge) {
	_displayScale = displayScale;
	_displayScaleEdge = displayScaleEdge;
}

FVerticesEditSelectedObject* VerticesEdit::GetItemByLocation(FVector worldLocation) {
	float xMin = worldLocation.X - _displayScale.X;
	float xMax = worldLocation.X + _displayScale.X;
	float yMin = worldLocation.Y - _displayScale.Y;
	float yMax = worldLocation.Y + _displayScale.Y;
	bool found = false;
	FVerticesEditSelectedObject* selectedObject = new FVerticesEditSelectedObject();
	FVector point, pointNext, pathLine, perpendicularLine, pointTemp;
	TArray<FVector2D> polygon2D = {};
	int verticesCount, indexNext;
	float halfWidth = _displayScale.X / 2;
	float degrees;
	for (auto& Elem : _items) {
		// Ensure visible.
		if (_itemsActors.Contains(Elem.Key)) {
			point = Elem.Value.posCenter;
			if (point.X >= xMin && point.X <= xMax && point.Y >= yMin && point.Y <= yMax) {
				selectedObject = new FVerticesEditSelectedObject(Elem.Key, "center");
				found = true;
				break;
			}
			verticesCount = Elem.Value.vertices.Num();
			for (int vv = 0; vv < verticesCount; vv++) {
				point = Elem.Value.vertices[vv];
				if (point.X >= xMin && point.X <= xMax && point.Y >= yMin && point.Y <= yMax) {
					selectedObject = new FVerticesEditSelectedObject(Elem.Key, "vertex", vv);
					found = true;
					break;
				}
				// Edges
				if (!found && vv < verticesCount - 1 || Elem.Value.shape == "polygon") {
					indexNext = (vv < verticesCount - 1) ? vv + 1 : 0;
					pointNext = Elem.Value.vertices[indexNext];
					// Make rectangle (width along line in between vertices).
					pathLine = pointNext - point;
					// 45 instead of 90 since we want to not overlap vertex (to make it easy to click vertex instead of edge).
					degrees = 45;
					perpendicularLine = pathLine.RotateAngleAxis(degrees, FVector(0,0,1)).GetClampedToMaxSize(halfWidth);
					polygon2D.Empty();
					pointTemp = point + perpendicularLine;
					polygon2D.Add(FVector2D(pointTemp.X, pointTemp.Y));
					pointTemp = point - perpendicularLine;
					polygon2D.Add(FVector2D(pointTemp.X, pointTemp.Y));
					pointTemp = pointNext - perpendicularLine;
					polygon2D.Add(FVector2D(pointTemp.X, pointTemp.Y));
					pointTemp = pointNext + perpendicularLine;
					polygon2D.Add(FVector2D(pointTemp.X, pointTemp.Y));
					if (MathPolygon::IsPointInPolygon(FVector2D(worldLocation.X, worldLocation.Y), polygon2D)) {
						selectedObject = new FVerticesEditSelectedObject(Elem.Key, "edge", vv);
						found = true;
						// Do not break on edge since want to prioritize vertices and edges and vertices overlap so allow
						// checking more vertices.
						// break;
					}
				}
			}
			if (found) {
				break;
			}
		}
	}
	if (found) {
		_selectedObject = selectedObject;
		_lastSelectedObject = selectedObject;
	}
	return selectedObject;
}

void VerticesEdit::CheckComputeCenter(FString uName) {
	if (_items[uName].vertices.Num() > 1) {
		InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
		FVector center = MathPolygon::GetPolygonCenter(_items[uName].vertices);
		HeightMap* heightMap = HeightMap::GetInstance();
		center.Z = heightMap->GetTerrainHeightAtPoint(FVector(center.X, center.Y, 0));
		_items[uName].posCenter = center;
		_itemsActors[uName].centerInstanceIndex = instancedMesh->SaveInstance("VertexWhite",
			_itemsActors[uName].centerInstanceIndex, _items[uName].posCenter,
			FRotator(0,0,0), _displayScale);
	}
}

std::tuple<FPolygon, bool> VerticesEdit::GetOrCreateCurrent() {
	if (_currentUName != "" && _items.Contains(_currentUName)) {
		return { _items[_currentUName], true };
	}
	if (_currentType.Len() < 1 || _currentShape.Len() < 1) {
		UE_LOG(LogTemp, Warning, TEXT("VerticesEdit.GetOrCreateCurrent missing type [%s] and / or shape [%s], skipping"),
			*_currentType, *_currentShape);
		return { FPolygon(), false };
	}
	_currentUName = Lodash::GetInstanceId("VerticesEdit_");
	TArray<FVector> vertices = {};
	_items.Add(_currentUName, FPolygon(_currentUName, _currentUName, vertices, FVector(0,0,0), _currentType,
		_currentShape, _currentPairsString));
	_itemsActors.Add(_currentUName, FVerticesEditActor(_currentUName));
	return { _items[_currentUName], true };
}

FString VerticesEdit::GetEdgeMesh(FString type) {
	return "EdgeBlue";
}

void VerticesEdit::AddVertex(FVector posAdd) {
	auto [item, valid] = GetOrCreateCurrent();
	if (valid) {
		FString uName = item.uName;
		_items[uName].vertices.Add(posAdd);

		InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
		int index = instancedMesh->CreateInstance("VertexWhite", posAdd, FRotator(0,0,0), _displayScale);
		_itemsActors[uName].verticesInstanceIndices.Add(index);
		if (_items[uName].vertices.Num() > 1) {
			int vertexIndex = _items[uName].vertices.Num() - 1;
			DrawEdge(uName, _items[uName].vertices[(vertexIndex - 1)], _items[uName].vertices[vertexIndex]);
		}

		CheckComputeCenter(uName);
	}
}

void VerticesEdit::AddVertexOnEdge(FString uName, FVector worldLocation) {
	auto [insertIndex, insertPos] = MathPolygon::FindNewVertexOnEdge(_items[uName].vertices, worldLocation);
	if (insertIndex > -1) {
		HeightMap* heightMap = HeightMap::GetInstance();
		insertPos.Z = heightMap->GetTerrainHeightAtPoint(FVector(insertPos.X, insertPos.Y, 0));
		_items[uName].vertices.Insert(insertPos, insertIndex);

		InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
		int index = instancedMesh->CreateInstance("VertexWhite", insertPos, FRotator(0,0,0), _displayScale);
		_itemsActors[uName].verticesInstanceIndices.Insert(index, insertIndex);

		DrawAllEdges(uName);
	}
}

void VerticesEdit::DeleteVertex(FString uName, int index) {
	_items[uName].vertices.RemoveAt(index);
	InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
	TArray<int> removeIndices = { _itemsActors[uName].verticesInstanceIndices[index] };
	instancedMesh->RemoveInstances("VertexWhite", removeIndices);
	_itemsActors[uName].verticesInstanceIndices.RemoveAt(index);
	DrawAllEdges(uName);
	CheckComputeCenter(uName);
}

void VerticesEdit::MoveVertex(FString uName, int index, FVector newLocation) {
	if (index < 0 || index > _items[uName].vertices.Num() - 1) {
		UE_LOG(LogTemp, Warning, TEXT("MoveVertex bad index %d %d"), index, _items[uName].vertices.Num());
	} else {
		_items[uName].vertices[index] = newLocation;

		InstancedMesh* instancedMesh = InstancedMesh::GetInstance();

		_itemsActors[uName].verticesInstanceIndices[index] = instancedMesh->SaveInstance("VertexWhite",
			_itemsActors[uName].verticesInstanceIndices[index], newLocation,
			FRotator(0,0,0), _displayScale);

		DrawAllEdges(uName);
		CheckComputeCenter(uName);
	}
}

void VerticesEdit::HidePolygon(FString uName) {
	if (_itemsActors.Contains(uName)) {
		InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
		// vertices (including center)
		TArray<int> removeIndices = _itemsActors[uName].verticesInstanceIndices;
		removeIndices.Add(_itemsActors[uName].centerInstanceIndex);
		instancedMesh->RemoveInstances("VertexWhite", removeIndices);
		// edges
		removeIndices = _itemsActors[uName].edgesInstanceIndices;
		instancedMesh->RemoveInstances(GetEdgeMesh(_items[uName].type), removeIndices);
		_itemsActors.Remove(uName);
	}
}

void VerticesEdit::DeletePolygon(FString uName, bool deleteChildren, bool save) {
	FString parentUName;
	FString type = _items[uName].type;
	HidePolygon(uName);

	// Remove from associated ones too.
	parentUName = _items[uName].parentUName;
	if (parentUName.Len() && _items.Contains(parentUName) &&
		_items[parentUName].childUNames.Contains(uName)) {
		_items[parentUName].childUNames.Remove(uName);
	}
	if (deleteChildren && _items[uName].childUNames.Num() > 0) {
		for (int ii = 0; ii < _items[uName].childUNames.Num(); ii++) {
			DeletePolygon(_items[uName].childUNames[ii], true);
		}
	}

	_items.Remove(uName);

	if (_selectedObject && _selectedObject->uName == uName) {
		_selectedObject = nullptr;
		_lastSelectedObject = nullptr;
		_currentUName = "";
	}

	if (save) {
		SaveToFile(type);
	}

	for (auto& Elem : _listenersDeletePolygon) {
		Elem.Value(uName, type);
	}
}

void VerticesEdit::MovePolygon(FString uName, FVector moveOffset, bool saveAsFinal) {
	HeightMap* heightMap = HeightMap::GetInstance();
	InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
	FVector newPoint;
	for (int ii = 0; ii < _items[uName].vertices.Num(); ii++) {
		newPoint = _items[uName].vertices[ii] + moveOffset;
		newPoint.Z = heightMap->GetTerrainHeightAtPoint(FVector(newPoint.X, newPoint.Y, 0));
		if (saveAsFinal) {
			_items[uName].vertices[ii] = newPoint;
		}

		_itemsActors[uName].verticesInstanceIndices[ii] = instancedMesh->SaveInstance("VertexWhite",
			_itemsActors[uName].verticesInstanceIndices[ii], newPoint,
			FRotator(0,0,0), _displayScale);
	}
	newPoint = _items[uName].posCenter + moveOffset;
	newPoint.Z = heightMap->GetTerrainHeightAtPoint(FVector(newPoint.X, newPoint.Y, 0));
	if (saveAsFinal) {
		_items[uName].posCenter = newPoint;
	}

	_itemsActors[uName].centerInstanceIndex = instancedMesh->SaveInstance("VertexWhite",
		_itemsActors[uName].centerInstanceIndex, newPoint,
		FRotator(0,0,0), _displayScale);

	DrawAllEdges(uName);
}

void VerticesEdit::DrawEdge(FString uName, FVector start, FVector end) {
	InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
	FString type = _items[uName].type;
	FVector pathLine = end - start;
	FVector midpoint = start + pathLine * 0.5;
	FVector location = midpoint;
	float edgeLength = pathLine.Size();
	FVector scale = FVector(edgeLength, _displayScaleEdge.Y, _displayScaleEdge.Z);
	float angleZ = MathVector::SignedAngle(FVector(1,0,0), pathLine, FVector(0,0,1));
	// TODO - this is incorrect
	// float angleY = MathVector::SignedAngle(FVector(1,0,0), pathLine, FVector(0,1,0));
	float angleY = 0;
	FRotator rotation = FRotator(angleY, angleZ, 0);
	int index = instancedMesh->CreateInstance(GetEdgeMesh(type), location, rotation, scale);
	_itemsActors[uName].edgesInstanceIndices.Add(index);
}

void VerticesEdit::RemoveAllEdges(FString uName) {
	if (_items.Contains(uName) && GetEdgeMesh(_items[uName].type) != "") {
		InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
		instancedMesh->RemoveInstances(GetEdgeMesh(_items[uName].type), _itemsActors[uName].edgesInstanceIndices);
		_itemsActors[uName].edgesInstanceIndices.Empty();
	} else {
		UE_LOG(LogTemp, Warning, TEXT("VerticesEdit.RemoveAllEdges missing _items uName or type %s"), *uName);
	}
}

void VerticesEdit::DrawAllEdges(FString uName) {
	RemoveAllEdges(uName);

	InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
	FString type = _items[uName].type;
	int index, indexNext;
	FVector pathLine, midpoint, location, scale, start, end;
	FRotator rotation;
	float edgeLength, angleZ, angleY;
	int verticesCount = _items[uName].vertices.Num();
	for (int ii = 0; ii < verticesCount; ii++) {
		if (ii == verticesCount - 1 && _items[uName].shape != "polygon") {
			break;
		}
		indexNext = ii < verticesCount - 1 ? ii + 1 : 0;
		end = _items[uName].vertices[indexNext];
		start = _items[uName].vertices[ii];
		pathLine = end - start;
		midpoint = start + pathLine * 0.5;
		location = midpoint;
		edgeLength = pathLine.Size();
		scale = FVector(edgeLength, _displayScaleEdge.Y, _displayScaleEdge.Z);
		angleZ = MathVector::SignedAngle(FVector(1,0,0), pathLine, FVector(0,0,1));
		// TODO - this is incorrect
		// angleY = MathVector::SignedAngle(FVector(1,0,0), pathLine, FVector(0,1,0));
		angleY = 0;
		rotation = FRotator(angleY, angleZ, 0);
		index = instancedMesh->CreateInstance(GetEdgeMesh(type), location, rotation, scale);
		_itemsActors[uName].edgesInstanceIndices.Add(index);
	}
}

void VerticesEdit::DrawAll() {
	for (auto& Elem : _items) {
		DrawItem(Elem.Key);
	}
}

void VerticesEdit::DrawItemsByTypes(TArray<FString> types) {
	for (auto& Elem : _items) {
		if (types.Contains(Elem.Value.type)) {
			DrawItem(Elem.Key);
		}
	}
}

void VerticesEdit::DrawItem(FString uName) {
	FPolygon item = _items[uName];
	InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
	TArray<int> verticesInstanceIndices;
	int centerInstanceIndex, index;
	FVector point;
	centerInstanceIndex = instancedMesh->CreateInstance("VertexWhite", item.posCenter,
		FRotator(0,0,0), _displayScale);

	verticesInstanceIndices.Empty();
	for (int ii = 0; ii < item.vertices.Num(); ii++) {
		point = item.vertices[ii];
		index = instancedMesh->CreateInstance("VertexWhite", point, FRotator(0,0,0), _displayScale);
		verticesInstanceIndices.Add(index);
	}

	_itemsActors.Add(uName, FVerticesEditActor(uName, verticesInstanceIndices, {}, centerInstanceIndex));

	DrawAllEdges(uName);
}

void VerticesEdit::ImportPolygons(TMap<FString, FPolygon> polygons) {
	FString uName;
	for (auto& Elem : polygons) {
		uName = Elem.Key;
		if (GetEdgeMesh(Elem.Value.type) == "") {
			UE_LOG(LogTemp, Warning, TEXT("VerticesEdit.ImportPolygons invalid type %s uName %s"), *Elem.Value.type, *uName);
		} else {
			_items.Add(uName, FPolygon(uName, uName, Elem.Value.vertices, Elem.Value.posCenter, Elem.Value.type,
				Elem.Value.shape, Elem.Value.pairsString, Elem.Value.jsonDataString, Elem.Value.squareMeters,
				Elem.Value.parentUName, Elem.Value.childUNames, Elem.Value.verticesBuffer,
				Elem.Value.averageChildDiameter, Elem.Value.skip));
			DrawItem(uName);
		}
	}
}

TMap<FString, FPolygon> VerticesEdit::ExportPolygonsByType(FString type) {
	TMap<FString, FPolygon> polygons = {};
	for (auto& Elem : _items) {
		if (Elem.Value.type == type) {
			polygons.Add(Elem.Key, Elem.Value);
		}
	}
	return polygons;
}

void VerticesEdit::LoadFromFiles() {
	FString pathPrefix = "VerticesEdit";
	DataConvert* dataConvert = DataConvert::GetInstance();
	DataFileProject* dataFileProject = DataFileProject::GetInstance();
	TArray<FString> fileNames = dataConvert->GetDirectoryFiles(pathPrefix);
	FString uName;
	for (int ii = 0; ii < fileNames.Num(); ii++) {
		auto [data, valid] = dataFileProject->LoadProject(pathPrefix + "/" + fileNames[ii]);
		if (valid) {
			for (auto& Elem : data.polygons) {
				uName = Elem.Key;
				_items.Add(uName, FPolygon(uName, uName, Elem.Value.vertices, Elem.Value.posCenter, Elem.Value.type,
					Elem.Value.shape, Elem.Value.pairsString, Elem.Value.jsonDataString, Elem.Value.squareMeters,
					Elem.Value.parentUName, Elem.Value.childUNames, Elem.Value.verticesBuffer,
					Elem.Value.averageChildDiameter, Elem.Value.skip));
				DrawItem(uName);
			}
		}
	}
}

void VerticesEdit::RemoveChildren(FString type, bool save) {
	TArray<FString> keysToRemove = {};
	for (auto& Elem : _items) {
		if (Elem.Value.type == type) {
			if (Elem.Value.parentUName == "") {
				// Empty out children.
				_items[Elem.Key].childUNames.Empty();
			} else {
				keysToRemove.Add(Elem.Key);
			}
		}
	}
	int count = keysToRemove.Num();
	for (int ii = (count - 1); ii >= 0; ii--) {
		_items.Remove(keysToRemove[ii]);
	}
	if (save) {
		SaveToFile(type);
	}
}

FString VerticesEdit::GetParentPairsString(FString type, FString childUName) {
	FString pairsString = "";
	if (_items.Contains(childUName)) {
		FString uName = _items[childUName].parentUName;
		while (uName != "" && _items.Contains(uName)) {
			if (_items[uName].pairsString.Len() > 0) {
				return _items[uName].pairsString;
			}
		}
	}
	return pairsString;
}

int VerticesEdit::CheckSubdividePolygons(FString type, bool save) {
	int countNew = 0;
	TMap<FString, FPolygon> newItems = {};
	for (auto& Elem : _items) {
		if (Elem.Value.type == type && Elem.Value.parentUName == "" && Elem.Value.childUNames.Num() < 1) {
			auto [polygons1, countNew1] = PlotDivide::SubdividePlots({ { Elem.Key, Elem.Value} });
			_items[Elem.Key] = polygons1[Elem.Key];
			for (auto& Elem1 : polygons1) {
				// Can not update polygons directly since it we are looping through them.
				newItems.Add(Elem1.Key, Elem1.Value);
				countNew += 1;
			}
		}
	}
	for (auto& Elem : newItems) {
		_items.Add(Elem.Key, Elem.Value);
	}
	if (save && countNew > 0) {
		SaveToFile(type);
	}
	return countNew;
}

void VerticesEdit::AddAndSave(TMap<FString, FPolygon> polygons) {
	TArray<FString> types = {};
	FString uName;
	for (auto& Elem : polygons) {
		uName = Elem.Key;
		_items.Add(uName, FPolygon(uName, uName, Elem.Value.vertices, Elem.Value.posCenter, Elem.Value.type,
			Elem.Value.shape, Elem.Value.pairsString, Elem.Value.jsonDataString, Elem.Value.squareMeters,
			Elem.Value.parentUName, Elem.Value.childUNames, Elem.Value.verticesBuffer,
			Elem.Value.averageChildDiameter, Elem.Value.skip));
		if (!types.Contains(Elem.Value.type)) {
			types.Add(Elem.Value.type);
		}
	}
	for (int ii = 0; ii < types.Num(); ii++) {
		SaveToFile(types[ii]);
	}
}

void VerticesEdit::SaveToFile(FString type) {
	DataFileProject* dataFileProject = DataFileProject::GetInstance();
	DataConvert* dataConvert = DataConvert::GetInstance();
	TMap<FString, FPolygon> items = FilterByTypes({ type });
	FDataProjectJson* json = new FDataProjectJson(items);
	dataConvert->CheckCreateDirectory("VerticesEdit");
	FString fileName = "VerticesEdit/" + type + ".json";
	dataFileProject->SaveProject(*json, fileName);
}

void VerticesEdit::SavePolygon(FString uName) {
	SaveToFile(_items[uName].type);

	for (auto& Elem : _listenersSavePolygon) {
		Elem.Value(uName, _items[uName]);
	}
}

FPolygon VerticesEdit::GetByUName(FString uName) {
	if (_items.Contains(uName)) {
		return _items[uName];
	}
	return FPolygon();
}

void VerticesEdit::AddOnSavePolygon(FString key, std::function<void(FString, FPolygon)> callback) {
	if (!_listenersSavePolygon.Contains(key)) {
		_listenersSavePolygon.Add(key, callback);
	}
}

void VerticesEdit::RemoveOnSavePolygon(FString key) {
	if (_listenersSavePolygon.Contains(key)) {
		_listenersSavePolygon.Remove(key);
	}
}

void VerticesEdit::AddOnDeletePolygon(FString key, std::function<void(FString, FString)> callback) {
	if (!_listenersDeletePolygon.Contains(key)) {
		_listenersDeletePolygon.Add(key, callback);
	}
}

void VerticesEdit::RemoveOnDeletePolygon(FString key) {
	if (_listenersDeletePolygon.Contains(key)) {
		_listenersDeletePolygon.Remove(key);
	}
}
