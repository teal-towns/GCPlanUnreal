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

UMaterialInstanceDynamic* DynamicMaterial::CreateTextureColor(FString name, FString textureBasePath,
	FString textureNormalPath, FLinearColor color, float tiling) {
	FString materialPath = "/Script/Engine.MaterialInstanceConstant'/Game/Material/Dynamic/DynamicTextureColor_M_Inst.DynamicTextureColor_M_Inst'";
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
	// newMaterial->SetScalarParameterValue(FName("ColorIntensity"), colorIntensity);
	newMaterial->SetVectorParameterValue(FName("Color"), color);
	newMaterial->SetScalarParameterValue(FName("Tiling"), tiling);
	_dynamicMaterials.Add(name, newMaterial);
	return newMaterial;
}

UMaterialInstanceDynamic* DynamicMaterial::CreateTexture(FString name, FString textureBasePath,
	FString textureNormalPath, float tiling) {
	FString materialPath = "/Script/Engine.MaterialInstanceConstant'/Game/Material/Dynamic/DynamicTexture_M_Inst.DynamicTexture_M_Inst'";
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
	newMaterial->SetScalarParameterValue(FName("Tiling"), tiling);
	_dynamicMaterials.Add(name, newMaterial);
	return newMaterial;
}

UMaterialInstanceDynamic* DynamicMaterial::CreateColor(FString name, FLinearColor color) {
	FString materialPath = "/Script/Engine.MaterialInstanceConstant'/Game/Material/Dynamic/DynamicColor_M_Inst.DynamicColor_M_Inst'";
	UMaterialInstance* material = Cast<UMaterialInstance>(StaticLoadObject(UMaterialInstance::StaticClass(), NULL,
		*materialPath));
	UMaterialInstanceDynamic* newMaterial  = UMaterialInstanceDynamic::Create(material, NULL);
	newMaterial->SetVectorParameterValue(FName("Color"), color);
	_dynamicMaterials.Add(name, newMaterial);
	return newMaterial;
}

FLinearColor DynamicMaterial::GetColor(FString key) {
	TMap<FString, FLinearColor> colorKeys = {
		{ "black", FLinearColor(0,0,0,1) },
		{ "blue", FLinearColor(0,0,0.25,1) },
		{ "green", FLinearColor(0,0.2,0,1) },
		{ "grey", FLinearColor(0.25,0.25,0.25,1) },
		{ "orange", FLinearColor(0.27,0.09,0,1) },
		{ "purple", FLinearColor(0.05,0,0.15,1) },
		{ "red", FLinearColor(0.4,0,0,1) },
		{ "white", FLinearColor(1,1,1,1) },
		{ "yellow", FLinearColor(0.3,0.3,0,1) },

		{ "beige", FLinearColor(1,0.94,0.77,1) }
	};
	if (colorKeys.Contains(key)) {
		return colorKeys[key];
	}
	return FLinearColor(1,1,1,1);
}
