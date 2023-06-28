#pragma once

#include <mutex>

class LisbonSequence {
public:
	LisbonSequence();
	~LisbonSequence();

	// Singletons should not be cloneable or assignable.
	LisbonSequence(LisbonSequence &other) = delete;
	void operator=(const LisbonSequence &) = delete;

	static LisbonSequence *GetInstance();

	void Start();

private:
	static LisbonSequence *pinstance_;
	static std::mutex mutex_;
};
