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
	UMaterialInstanceDynamic* CreateTextureColor(FString name, FString textureBasePath = "",
		FString textureNormalPath = "", FLinearColor color = FLinearColor(1,1,1,1),
		float tiling = 1);
	UMaterialInstanceDynamic* CreateTexture(FString name, FString textureBasePath,
		FString textureNormalPath, float tiling = 1);
	UMaterialInstanceDynamic* CreateColor(FString name, FLinearColor color);
	static FLinearColor GetColor(FString key);
};
