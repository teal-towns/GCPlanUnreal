#pragma once

#include <mutex>

class DynamicMaterial {
private:
	static DynamicMaterial *pinstance_;
	static std::mutex mutex_;

	TMap<FString, UMaterialInstanceDynamic*> _dynamicMaterials = {};

public:
	DynamicMaterial();
	~DynamicMaterial();

	// Singletons should not be cloneable or assignable.
	DynamicMaterial(DynamicMaterial &other) = delete;
	void operator=(const DynamicMaterial &) = delete;
	static DynamicMaterial *GetInstance();

	void CleanUp();
	// void Init();
	UMaterialInstanceDynamic* Create(FString name, FString textureBasePath = "",
		FString textureNormalPath = "", FLinearColor color = FLinearColor(255,255,255), float colorIntensity = 1);

};
