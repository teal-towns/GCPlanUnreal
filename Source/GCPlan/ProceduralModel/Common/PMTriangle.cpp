#include "PMTriangle.h"

PMTriangle::PMTriangle() {
}

PMTriangle::~PMTriangle() {
}

TArray<int> PMTriangle::AddQuad(TArray<int> Triangles, int vertexBottomLeftIndex, int vertexTopLeftIndex,
	int vertexTopRightIndex, int vertexBottomRightIndex, FString direction) {
	if (direction == "clockwise") {
		Triangles.Add(vertexBottomLeftIndex);
		Triangles.Add(vertexTopLeftIndex);
		Triangles.Add(vertexTopRightIndex);
		Triangles.Add(vertexBottomRightIndex);
		Triangles.Add(vertexBottomLeftIndex);
		Triangles.Add(vertexTopRightIndex);
	} else {
		Triangles.Add(vertexBottomLeftIndex);
		Triangles.Add(vertexBottomRightIndex);
		Triangles.Add(vertexTopRightIndex);
		Triangles.Add(vertexBottomLeftIndex);
		Triangles.Add(vertexTopRightIndex);
		Triangles.Add(vertexTopLeftIndex);
	}
	return Triangles;
}
