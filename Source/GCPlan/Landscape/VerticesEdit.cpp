#include "VerticesEdit.h"

#include "../BuildingStructsActor.h"
#include "../Common/Lodash.h"
#include "../Common/MathPolygon.h"
#include "../Common/MathVector.h"
#include "../Common/UnrealGlobal.h"
#include "../Landscape/HeightMap.h"
#include "../Mesh/InstancedMesh.h"
#include "../ProceduralModel/PMBase.h"

VerticesEdit* VerticesEdit::pinstance_{nullptr};
std::mutex VerticesEdit::mutex_;
// AStaticMeshActor* VerticesEdit::_verticesEditParentActor;

VerticesEdit::VerticesEdit() {
}

VerticesEdit::~VerticesEdit() {
}

VerticesEdit *VerticesEdit::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new VerticesEdit();

		// PMBase* pmBase = PMBase::GetInstance();
		// FActorSpawnParameters spawnParams;
		// _verticesEditParentActor = pmBase->CreateActor("VerticesEdit", FVector(0,0,0), FRotator(0,0,0), spawnParams);
	}
	return pinstance_;
}

void VerticesEdit::SetType(FString type) {
	_currentType = type;
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

	pinstance_ = nullptr;

	_listenersSavePolygon.Empty();

	// _listenersDeleteVertex.Empty();
	// _listenersMoveVertex.Empty();
	// _listenersAddVertex.Empty();
	// _listenersDeleteShape.Empty();
	// _listenersMoveShape.Empty();
}

void VerticesEdit::Hide() {
	_itemsActors.Empty();
	InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
	instancedMesh->ClearInstancesBulk({"VertexWhite", "EdgeBlack", "EdgeBlue", "EdgeRed"});
}

// void VerticesEdit::Show() {
// 	for (auto& Elem : _items) {
// 		DrawItem(Elem.Value.uName);
// 	}
// }

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
		SavePolygon(_lastSelectedObject->uName);
	}
}

void VerticesEdit::OnMouseDown(FVector worldLocation) {
	// UE_LOG(LogTemp, Display, TEXT("mouse down %s"), *worldLocation.ToString());
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
	// UE_LOG(LogTemp, Display, TEXT("mouse up %s"), *worldLocation.ToString());
	if (_selectedObject) {
		// UE_LOG(LogTemp, Display, TEXT("mouse up %s %s %d"), *_selectedObject->uName, *_selectedObject->objectType, _selectedObject->objectIndex);
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
	}
	// Reset.
	_mouseDown = false;
	_mouseDownStartLocation = FVector(0,0,0);
	_timeMouseDown = -1;
	StopDrag();
	_selectedObject = nullptr;
}

void VerticesEdit::OnMouseMove(FVector worldLocation) {
	if (worldLocation != _lastWorldLocation) {
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
	// if (_dragging) {
	// 	UE_LOG(LogTemp, Display, TEXT("StopDrag"));
	// }
	_dragging = false;
}

void VerticesEdit::StartDrag() {
	// UE_LOG(LogTemp, Display, TEXT("StartDrag"));
	_dragging = true;
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
			if (!found && vv < verticesCount - 1 || Elem.Value.closedLoop > 0) {
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
	if (found) {
		// UE_LOG(LogTemp, Display, TEXT("selectedObject %s %s %d"), *selectedObject->uName, *selectedObject->objectType, selectedObject->objectIndex); 
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

FPolygon VerticesEdit::GetOrCreateCurrent() {
	if (_currentUName != "" && _items.Contains(_currentUName)) {
		return _items[_currentUName];
	}
	_currentUName = Lodash::GetInstanceId("VerticesEdit_");
	TArray<FVector> vertices = {};
	_items.Add(_currentUName, FPolygon(_currentUName, _currentUName, vertices, FVector(0,0,0), _currentType));
	_itemsActors.Add(_currentUName, FVerticesEditActor(_currentUName));
	return _items[_currentUName];
}

void VerticesEdit::AddVertex(FVector posAdd) {
	FPolygon item = GetOrCreateCurrent();
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

void VerticesEdit::AddVertexOnEdge(FString uName, FVector worldLocation) {
	auto [insertIndex, insertPos] = MathPolygon::FindNewVertexOnEdge(_items[uName].vertices, worldLocation);
	// UE_LOG(LogTemp, Display, TEXT("add on edge %s %s %s"), *uName, insertIndex, *insertPos.ToString(), *worldLocation.ToString());
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

		// FString msg = "";
		// for (int ii = 0; ii < _itemsActors[uName].verticesInstanceIndices.Num(); ii++) {
		// 	msg += FString::FromInt(_itemsActors[uName].verticesInstanceIndices[ii]) + " ";
		// }
		// UE_LOG(LogTemp, Display, TEXT("MoveVertex instances %d %s"), index, *msg);

		_itemsActors[uName].verticesInstanceIndices[index] = instancedMesh->SaveInstance("VertexWhite",
			_itemsActors[uName].verticesInstanceIndices[index], newLocation,
			FRotator(0,0,0), _displayScale);
		// UE_LOG(LogTemp, Display, TEXT("MoveVertex new index val %d %s"), _itemsActors[uName].verticesInstanceIndices[index], *newLocation.ToString());

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
		instancedMesh->RemoveInstances(_meshMapEdge[_items[uName].type], removeIndices);
		_itemsActors.Remove(uName);
	}
}

void VerticesEdit::DeletePolygon(FString uName) {
	HidePolygon(uName);
	_items.Remove(uName);
	if (_selectedObject && _selectedObject->uName == uName) {
		_selectedObject = nullptr;
		_lastSelectedObject = nullptr;
		_currentUName = "";
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
	// UE_LOG(LogTemp, Display, TEXT("DrawEdge edgeLength %f angleZ %f angleY %f"), edgeLength, angleZ, angleY);
	FRotator rotation = FRotator(angleY, angleZ, 0);
	int index = instancedMesh->CreateInstance(_meshMapEdge[type], location, rotation, scale);
	_itemsActors[uName].edgesInstanceIndices.Add(index);
}

void VerticesEdit::RemoveAllEdges(FString uName) {
	if (_items.Contains(uName) && _meshMapEdge.Contains(_items[uName].type)) {
		InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
		instancedMesh->RemoveInstances(_meshMapEdge[_items[uName].type], _itemsActors[uName].edgesInstanceIndices);
		_itemsActors[uName].edgesInstanceIndices.Empty();
	} else {
		UE_LOG(LogTemp, Warning, TEXT("VerticesEdit.RemoveAllEdges missing _items uName or _meshMapEdge type %s"), *uName);
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
		if (ii == verticesCount - 1 && !_items[uName].closedLoop) {
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
		// UE_LOG(LogTemp, Display, TEXT("DrawEdge edgeLength %f angleZ %f angleY %f"), edgeLength, angleZ, angleY);
		rotation = FRotator(angleY, angleZ, 0);
		index = instancedMesh->CreateInstance(_meshMapEdge[type], location, rotation, scale);
		_itemsActors[uName].edgesInstanceIndices.Add(index);
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
		if (!_meshMapEdge.Contains(Elem.Value.type)) {
			UE_LOG(LogTemp, Warning, TEXT("VerticesEdit.ImportPolygons invalid type %s uName %s"), *Elem.Value.type, *uName);
		} else {
			_items.Add(uName, FPolygon(uName, uName, Elem.Value.vertices, Elem.Value.posCenter, Elem.Value.type));
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

void VerticesEdit::SavePolygon(FString uName) {
	FString type = _items[uName].type;
	for (auto& Elem : _listenersSavePolygon) {
		Elem.Value(uName, type);
	}
}

void VerticesEdit::AddOnSavePolygon(FString key, std::function<void(FString, FString)> callback) {
	if (!_listenersSavePolygon.Contains(key)) {
		_listenersSavePolygon.Add(key, callback);
	}
}

void VerticesEdit::RemoveOnSavePolygon(FString key) {
	if (_listenersSavePolygon.Contains(key)) {
		_listenersSavePolygon.Remove(key);
	}
}



// void VerticesEdit::AddOnDeleteVertex(FString key, std::function<void(FString, int)> callback) {
// 	if (!_listenersDeleteVertex.Contains(key)) {
// 		_listenersDeleteVertex.Add(key, callback);
// 	}
// }

// void VerticesEdit::RemoveOnDeleteVertex(FString key) {
// 	if (_listenersDeleteVertex.Contains(key)) {
// 		_listenersDeleteVertex.Remove(key);
// 	}
// }

// void VerticesEdit::AddOnMoveVertex(FString key, std::function<void(FString, int, FVector)> callback) {
// 	if (!_listenersMoveVertex.Contains(key)) {
// 		_listenersMoveVertex.Add(key, callback);
// 	}
// }

// void VerticesEdit::RemoveOnMoveVertex(FString key) {
// 	if (_listenersMoveVertex.Contains(key)) {
// 		_listenersMoveVertex.Remove(key);
// 	}
// }

// void VerticesEdit::AddOnAddVertex(FString key, std::function<void(FString, int, FVector)> callback) {
// 	if (!_listenersAddVertex.Contains(key)) {
// 		_listenersAddVertex.Add(key, callback);
// 	}
// }

// void VerticesEdit::RemoveOnAddVertex(FString key) {
// 	if (_listenersAddVertex.Contains(key)) {
// 		_listenersAddVertex.Remove(key);
// 	}
// }

// void VerticesEdit::AddOnDeleteShape(FString key, std::function<void(FString)> callback) {
// 	if (!_listenersDeleteShape.Contains(key)) {
// 		_listenersDeleteShape.Add(key, callback);
// 	}
// }

// void VerticesEdit::RemoveOnDeleteShape(FString key) {
// 	if (_listenersDeleteShape.Contains(key)) {
// 		_listenersDeleteShape.Remove(key);
// 	}
// }

// void VerticesEdit::AddOnMoveShape(FString key, std::function<void(FString, FVector)> callback) {
// 	if (!_listenersMoveShape.Contains(key)) {
// 		_listenersMoveShape.Add(key, callback);
// 	}
// }

// void VerticesEdit::RemoveOnMoveShape(FString key) {
// 	if (_listenersMoveShape.Contains(key)) {
// 		_listenersMoveShape.Remove(key);
// 	}
// }
