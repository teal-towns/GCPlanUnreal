#pragma once

#include <mutex>

#include "../DataStructsActor.h"

class DataFileProject {

private:
	static DataFileProject *pinstance_;
	static std::mutex mutex_;

public:
	DataFileProject();
	~DataFileProject();

	// Singletons should not be cloneable or assignable.
	DataFileProject(DataFileProject &other) = delete;
	void operator=(const DataFileProject &) = delete;

	static DataFileProject *GetInstance();

	static std::tuple<FDataProjectJson, bool> LoadProject(FString fileName);
	static bool SaveProject(FDataProjectJson json, FString fileName);
};
