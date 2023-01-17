#include "Stdafx.h"
#include "InputClass.hpp"

InputClass::InputClass()
{

}

InputClass::InputClass(const InputClass& other)
{

}

InputClass::~InputClass()
{

}


void InputClass::Initialize()
{
	for (int i = 0; i < 256; i++)
	{
		// 키 배열 초기화
		this->keys_[i] = false;
	}
}

void InputClass::KeyDown(const unsigned int& input)
{
	// 눌릴 때만 true로
	this->keys_[input] = true;
}

void InputClass::KeyUp(const unsigned int& input)
{
	this->keys_[input] = false;
}

bool InputClass::IsKeyDown(const unsigned int& input)
{
	return this->keys_[input];
}
