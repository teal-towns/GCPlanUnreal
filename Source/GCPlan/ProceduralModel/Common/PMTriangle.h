#pragma once

class PMTriangle {
public:
	PMTriangle();
	~PMTriangle();

	static TArray<int> AddQuad(TArray<int> Triangles, int vertexBottomLeftIndex, int vertexTopLeftIndex,
		int vertexTopRightIndex, int vertexBottomRightIndex, FString direction = "clockwise");
};
