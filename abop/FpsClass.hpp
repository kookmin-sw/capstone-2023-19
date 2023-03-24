#pragma once

#pragma comment(lib, "winmm.lib")

class FpsClass
{
public:
	FpsClass();
	FpsClass(const FpsClass&);
	~FpsClass();

	void Initialize();
	void Frame();
	int GetFps();

private:
	int fps_ = 0, count_ = 0;
	unsigned long startTime_;
};