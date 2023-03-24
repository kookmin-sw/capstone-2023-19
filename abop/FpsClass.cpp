#include "Stdafx.h"
#include <mmsystem.h>
#include "FpsClass.hpp"

FpsClass::FpsClass()
{

}

FpsClass::FpsClass(const FpsClass& other)
{

}
FpsClass::~FpsClass() 
{

}

void FpsClass::Initialize()
{
	startTime_ = timeGetTime();

	return;
}

void FpsClass::Frame()
{
	count_++;

	if (timeGetTime() >= (startTime_ + 1000))
	{
		fps_ = count_;
		count_ = 0;

		startTime_ = timeGetTime();
	}
}

int FpsClass::GetFps()
{
	return fps_;
}