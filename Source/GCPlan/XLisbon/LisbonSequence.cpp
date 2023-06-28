#include "LisbonSequence.h"

#include "../Common/Lodash.h"
#include "../Common/UnrealGlobal.h"
#include "../Draw/DrawHighlight.h"
#include "../Landscape/VerticesEdit.h"
#include "../Modeling/ModelBase.h"
#include "../Move/MoveObject.h"

LisbonSequence* LisbonSequence::pinstance_{nullptr};
std::mutex LisbonSequence::mutex_;

LisbonSequence::LisbonSequence() {
}

LisbonSequence::~LisbonSequence() {
}

LisbonSequence *LisbonSequence::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new LisbonSequence();
	}
	return pinstance_;
}

void LisbonSequence::Start() {
	UnrealGlobal* unrealGlobal = UnrealGlobal::GetInstance();
	ModelBase* modelBase = ModelBase::GetInstance();

	// // Move trains
	// MoveObject* moveObject = MoveObject::GetInstance();
	// VerticesEdit* verticesEdit = VerticesEdit::GetInstance();
	// verticesEdit->LoadFromFiles();
	// verticesEdit->Hide();
	// TMap<FString, FPolygon> polygons = verticesEdit->FilterByTypes({ "train" });
	// UE_LOG(LogTemp, Display, TEXT("LisbonSequence Start polygons %d"), polygons.Num());
	// AActor* actor;
	// float speed = 0.5;
	// FVector end = FVector(-401,-149,3);
	// for (auto& Elem : polygons) {
	// 	actor = modelBase->GetActorByName(Elem.Key, true);
	// 	UE_LOG(LogTemp, Display, TEXT("polygon name %s"), *Elem.Key);
	// 	if (actor) {
	// 		UE_LOG(LogTemp, Display, TEXT("yes actor"));
	// 		moveObject->Move(actor, end, speed);
	// 	}
	// }

	// // UI text
	// // if (unrealGlobal->_canvasTextWidget) {
	// // 	unrealGlobal->_canvasTextWidget->SetText(Lodash::GetInstanceId("TextHere_"));
	// // }

	// // 3D text
	// FModelCreateParams createParams;
	// createParams.offset = FVector(0,0,5);
	// createParams.rotation = FVector(0,0,-45);
	// DrawHighlight* drawHighlight = DrawHighlight::GetInstance();
	// drawHighlight->CreateOne("key1", FVector(5,5,5), FModelParams(), createParams, {}, Lodash::GetInstanceId("testing1"));
}
