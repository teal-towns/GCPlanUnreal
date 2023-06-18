#pragma once

class PMCylinder {
public:
	PMCylinder();
	~PMCylinder();

	static UStaticMesh* CreateFromInputs();
	static UStaticMesh* Create(FString name, FVector size,
		TArray<FString> tags = {}, bool destroyActor = false);
};
