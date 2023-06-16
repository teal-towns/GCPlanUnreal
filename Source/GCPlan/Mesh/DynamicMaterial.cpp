#include "DynamicMaterial.h"

#include "Materials/MaterialInstanceDynamic.h"

DynamicMaterial* DynamicMaterial::pinstance_{nullptr};
std::mutex DynamicMaterial::mutex_;

DynamicMaterial::DynamicMaterial() {
}

DynamicMaterial::~DynamicMaterial() {
}

DynamicMaterial *DynamicMaterial::GetInstance() {
	std::lock_guard<std::mutex> lock(mutex_);
	if (pinstance_ == nullptr) {
		pinstance_ = new DynamicMaterial();
	}
	return pinstance_;
}

// void DynamicMaterial::Init() {
// 	if (!_dynamicMaterialBase) {
// 		FString materialPath = "/Script/Engine.MaterialInstanceConstant'/Game/Material/Dynamic_M_Inst.Dynamic_M_Inst'";
// 		UMaterial* material = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL,
// 			*materialPath));
// 		_dynamicMaterialBase = UMaterialInstanceDynamic::Create(Material, NULL);
// 	}
// }
void DynamicMaterial::CleanUp() {
	// TODO - destroy materials?
	_dynamicMaterials.Empty();
}

UMaterialInstanceDynamic* DynamicMaterial::Create(FString name, FString textureBasePath,
	FString textureNormalPath, FLinearColor color, float colorIntensity) {
	FString materialPath = "/Script/Engine.MaterialInstanceConstant'/Game/Material/Dynamic_M_Inst.Dynamic_M_Inst'";
	UMaterialInstance* material = Cast<UMaterialInstance>(StaticLoadObject(UMaterialInstance::StaticClass(), NULL,
		*materialPath));
	UMaterialInstanceDynamic* newMaterial  = UMaterialInstanceDynamic::Create(material, NULL);
	UTexture* texture;
	if (textureBasePath.Len() > 0) {
		texture = Cast<UTexture>(StaticLoadObject(UTexture::StaticClass(), NULL,
			*textureBasePath));
		newMaterial->SetTextureParameterValue(FName("TextureBase"), texture);
	}
	if (textureNormalPath.Len() > 0) {
		texture = Cast<UTexture>(StaticLoadObject(UTexture::StaticClass(), NULL,
			*textureNormalPath));
		newMaterial->SetTextureParameterValue(FName("TextureNormal"), texture);
	}
	newMaterial->SetScalarParameterValue(FName("ColorIntensity"), colorIntensity);
	newMaterial->SetVectorParameterValue(FName("Color"), color);
	_dynamicMaterials.Add(name, newMaterial);
	return newMaterial;
}
