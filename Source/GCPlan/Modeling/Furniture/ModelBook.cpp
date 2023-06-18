#include "ModelBook.h"

#include "../ModelBase.h"
#include "../../Common/Lodash.h"
#include "../../Mesh/DynamicMaterial.h"
#include "../../ModelingStructsActor.h"

ModelBook::ModelBook() {
}

ModelBook::~ModelBook() {
}

AActor* ModelBook::Create() {
	ModelBase* modelBase = ModelBase::GetInstance();
	auto [modelingBase, modelParams] = modelBase->GetInputs("Book", FVector(0.2,0.05,0.3));
	FString name = modelingBase.name;
	FVector size = modelingBase.size;
	TArray<FString> tags = modelingBase.tags;
	if (tags.Contains("books")) {
		return Books(size, tags);
	}

	AActor* actor = modelBase->CreateActor(name);
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	FVector scale = FVector(1,1,1), rotation = FVector(0,0,0), location = FVector(0,0,0);
	FActorSpawnParameters spawnParams;

	modelParams.meshKey = "cube";
	DynamicMaterial* dynamicMaterial = DynamicMaterial::GetInstance();
	FLinearColor color = FLinearColor(Lodash::RandomRangeFloat(0,1), Lodash::RandomRangeFloat(0,1),
		Lodash::RandomRangeFloat(0,1), 1);
	UMaterialInstanceDynamic* material = dynamicMaterial->CreateColor(name + "_book", color);
	modelParams.dynamicMaterial = material;
	scale = size;
	modelBase->CreateActor(name + "_Book", location, rotation, scale, spawnParams, modelParams);

	return actor;
}

AActor* ModelBook::Books(FVector size, TArray<FString> tags, FModelParams modelParams) {
	FString name = Lodash::GetInstanceId("Books_");
	ModelBase* modelBase = ModelBase::GetInstance();
	AActor* actor = modelBase->CreateActor(name);
	FVector scale = FVector(1,1,1), rotation = FVector(0,0,0), location = FVector(0,0,0);
	FActorSpawnParameters spawnParams;
	modelParams.parent = actor->FindComponentByClass<USceneComponent>();

	modelParams.meshKey = "cube";
	DynamicMaterial* dynamicMaterial = DynamicMaterial::GetInstance();
	FLinearColor color;
	UMaterialInstanceDynamic* material;

	FVector averageBookSize = FVector(size.X, 0.05, size.Z);
	FVector bookSize;
	// Go along Y.
	float yStart = -1 * size.Y / 2;
	float yEnd = size.Y / 2;
	float yCurrent = yStart;
	FString bookName;
	while (yCurrent < yEnd) {
		bookName = Lodash::GetInstanceId("Book");
		bookSize = averageBookSize * Lodash::RandomRangeFloat(0.7, 1.3);
		color = FLinearColor(Lodash::RandomRangeFloat(0,1), Lodash::RandomRangeFloat(0,1),
			Lodash::RandomRangeFloat(0,1), 1);
		material = dynamicMaterial->CreateColor(name + bookName, color);
		modelParams.dynamicMaterial = material;
		location.Y = yCurrent;
		location.X = Lodash::RandomRangeFloat(averageBookSize.X * -0.3, averageBookSize.X * 0.3);
		modelBase->CreateActor(name + bookName, location, rotation, bookSize, spawnParams, modelParams);
		// Give a little spacing.
		yCurrent += bookSize.Y + Lodash::RandomRangeFloat(0, averageBookSize.Y * 0.1);
	}
	return actor;
}
