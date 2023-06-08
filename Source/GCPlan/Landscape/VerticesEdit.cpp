#include "VerticesEdit.h"

#include "../BuildingStructsActor.h"
#include "../Common/Lodash.h"
#include "../Common/MathPolygon.h"
#include "../Common/MathVector.h"
#include "../Common/UnrealGlobal.h"
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

// void VerticesEdit::DeleteVertex(FString TODO) {
// 	EditVertexDataClass vertexData = _verticesMap[GOName];
// 	string uName = vertexData.uName;
// 	string keyType = vertexData.keyType;
// 	List<EditVertexClass> verticesNames = _verticesWithGONames[uName];
// 	for (int ii = 0; ii < verticesNames.Count; ii++) {
// 		if (GOName == verticesNames[ii].GOName) {
// 			if (_onDeleteVertex.ContainsKey(keyType)) {
// 				_onDeleteVertex[keyType](uName, ii);
// 			}
// 			break;
// 		}
// 	}
// }

// public void MoveVertex(GameObject GO, bool skipY = true) {
// 	string GOName = GO.name;
// 	EditVertexDataClass vertexData = _verticesMap[GOName];
// 	string uName = vertexData.uName;
// 	string keyType = vertexData.keyType;
// 	List<EditVertexClass> verticesNames = _verticesWithGONames[uName];
// 	for (int ii = 0; ii < verticesNames.Count; ii++) {
// 		if (GOName == verticesNames[ii].GOName) {
// 			Vector3 movedPos = GO.transform.position;
// 			if (skipY) {
// 				// Ignore y since was likely moved up for editing.
// 				movedPos.y = verticesNames[ii].pos.y;
// 			}
// 			if (_onMoveVertex.ContainsKey(keyType)) {
// 				_onMoveVertex[keyType](uName, ii, movedPos);
// 			}
// 			break;
// 		}
// 	}
// }

void VerticesEdit::SetType(FString type) {
	_currentType = type;
}

void VerticesEdit::DestroyItems() {
	_items.Empty();
	_itemsActors.Empty();
	_currentUName = "";
	InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
	instancedMesh->ClearInstancesBulk({"VertexWhite", "EdgeBlack", "EdgeBlue", "EdgeRed"});
}

void VerticesEdit::CleanUp() {
	DestroyItems();
	// _listenersDeleteVertex.Empty();
	// _listenersMoveVertex.Empty();
	// _listenersAddVertex.Empty();
	// _listenersDeleteShape.Empty();
	// _listenersMoveShape.Empty();
}

FVerticesEdit VerticesEdit::GetOrCreateCurrent() {
	if (_currentUName != "" && _items.Contains(_currentUName)) {
		return _items[_currentUName];
	}
	_currentUName = Lodash::GetInstanceId("VerticesEdit_");
	TArray<FVector> vertices = {};
	_items.Add(_currentUName, FVerticesEdit(_currentUName, vertices, _currentType));
	_itemsActors.Add(_currentUName, FVerticesEditActor(_currentUName));
	return _items[_currentUName];
}

void VerticesEdit::CheckComputeCenter(FString uName) {
	if (_items[uName].vertices.Num() > 1) {
		InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
		_items[uName].center = MathPolygon::GetPolygonCenter(_items[uName].vertices);
		// UE_LOG(LogTemp, Display, TEXT("center %s index before %d"), *_items[uName].center.ToString(), _itemsActors[uName].centerInstanceIndex); 
		_itemsActors[uName].centerInstanceIndex = instancedMesh->SaveInstance("VertexWhite",
			_itemsActors[uName].centerInstanceIndex, _items[uName].center,
			FRotator(0,0,0), _displayScale);
		// UE_LOG(LogTemp, Display, TEXT("index %d"), _itemsActors[uName].centerInstanceIndex);
	}
}

void VerticesEdit::AddVertex(FVector posAdd) {
	FVerticesEdit item = GetOrCreateCurrent();
	FString uName = item.uName;
	_items[uName].vertices.Add(posAdd);

	// UE_LOG(LogTemp, Display, TEXT("1 uName %s vertices.Num() %d"), *uName, _items[uName].vertices.Num());
	InstancedMesh* instancedMesh = InstancedMesh::GetInstance();
	int index = instancedMesh->CreateInstance("VertexWhite", posAdd, FRotator(0,0,0), _displayScale);
	_itemsActors[uName].verticesInstanceIndices.Add(index);
	// UE_LOG(LogTemp, Display, TEXT("2 verticesInstanceIndices.Num() %d"), _itemsActors[uName].verticesInstanceIndices.Num());
	if (_items[uName].vertices.Num() > 1) {
		int vertexIndex = _items[uName].vertices.Num() - 1;
		DrawEdge(uName, _items[uName].vertices[(vertexIndex - 1)], _items[uName].vertices[vertexIndex]);
	}

	CheckComputeCenter(uName);
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
	int index = instancedMesh->CreateInstance(_meshMap[type], location, rotation, scale);
	_itemsActors[uName].edgesInstanceIndices.Add(index);
}

// void DrawOne(FString uName) {
// 	if (_items.Contains(uName)) {
// 		// TODO
// 		for (int ii = 0; ii < _items[uName].vertices.Len(); ii++) {

// 		}
// 	}
// }

// void VerticesEdit::AddVertexOnEdge(FString GOName, FVector posAdd) {
// 	EditVertexDataClass vertexData = _verticesMap[GOName];
// 	string uName = vertexData.uName;
// 	string keyType = vertexData.keyType;
// 	List<EditVertexClass> verticesNames = _verticesWithGONames[uName];
// 	List<Vector3> vertices = new List<Vector3>() {};
// 	for (int ii = 0; ii < verticesNames.Count; ii++) {
// 		vertices.Add(verticesNames[ii].pos);
// 	}
// 	(int insertIndex, Vector3 insertPos) = _mathPolygon.FindNewVertexOnEdge(
// 		vertices, posAdd);
// 	if (insertIndex > -1) {
// 		if (_onAddVertex.ContainsKey(keyType)) {
// 			_onAddVertex[keyType](uName, insertIndex, insertPos);
// 		}
// 	}
// }

// public void DestroyByGOName(string GOName) {
// 	EditVertexDataClass vertexData = _verticesMap[GOName];
// 	string uName = vertexData.uName;
// 	string keyType = vertexData.keyType;
// 	DestroyOne(keyType, uName);
// 	if (_onDestroyShape.ContainsKey(keyType)) {
// 		_onDestroyShape[keyType](uName);
// 	}
// }

// public void MoveShapeByGOName(string GOName, Vector3 moveDirection) {
// 	EditVertexDataClass vertexData = _verticesMap[GOName];
// 	string uName = vertexData.uName;
// 	string keyType = vertexData.keyType;
// 	if (_onMoveShape.ContainsKey(keyType)) {
// 		_onMoveShape[keyType](uName, moveDirection);
// 	}
// }

// public void TranslateByGOName(string GOName, Vector3 offsetFromOrig, List<string> skipGONames) {
// 	EditVertexDataClass vertexData = _verticesMap[GOName];
// 	string uName = vertexData.uName;
// 	string keyType = vertexData.keyType;
// 	string landName = FormLandName(keyType, uName);
// 	Dictionary<string, MultiplayerGameUserLandGameObjectClass> objs = _landRender.GetObjects(landName);
// 	foreach (var item in objs) {
// 		if (!skipGONames.Contains(item.Key)) {
// 			GameObject GO = _landRender.GetLandGO(landName, item.Key);
// 			// Need to compare from original as the offset is from the original position.
// 			// Keep Y fixed (the raised position for editing, it should never change and the offset should be 0).
// 			Vector3 posOrig = new Vector3(item.Value.position["x"],
// 				GO.transform.position.y, item.Value.position["z"]);
// 			GO.transform.position = posOrig + offsetFromOrig;
// 		}
// 	}
// }

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
