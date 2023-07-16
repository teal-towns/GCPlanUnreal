#pragma once

class DrawVertices {
public:
	DrawVertices();
	~DrawVertices();

	static void LoadVertices(TArray<FString> skipTypes = {});
};
