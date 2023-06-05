#pragma once

class PMCylinder {
public:
	PMCylinder();
	~PMCylinder();

	static UStaticMesh* CreateFromInputs();
	static UStaticMesh* Create(FString name, FVector size, FVector vertices = FVector(10,10,10),
		TArray<FString> tags = {}, bool destroyActor = false);
};
