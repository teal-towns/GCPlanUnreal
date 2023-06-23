#pragma once

class PMCylinder {
public:
	PMCylinder();
	~PMCylinder();

	static UStaticMesh* CreateFromInputs();
	static UStaticMesh* Create(FString name, FVector size, bool destroyActor = false);
};
