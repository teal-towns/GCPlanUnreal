#pragma once

class PMCube {
public:
	PMCube();
	~PMCube();

	static UStaticMesh* CreateFromInputs();
	static UStaticMesh* Create(FString name, FVector size, FVector vertices = FVector(2,2,2),
		TArray<FString> tags = {}, bool destroyActor = false);
};
