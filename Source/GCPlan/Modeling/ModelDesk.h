#pragma once

class ModelDesk {
public:
	ModelDesk();
	~ModelDesk();

	static void Create();
};

class ModelMonitor {
public:
	ModelMonitor();
	~ModelMonitor();

	static void Create(FVector defLocation);
};

class ModelKeyboard {
public:
	ModelKeyboard();
	~ModelKeyboard();

	static void Create(FVector defLocation);
};

class ModelMouse {
public:
	ModelMouse();
	~ModelMouse();

	static void Create(FVector defLocation);
};