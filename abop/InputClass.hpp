#pragma once

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	void Initialize();

	void KeyDown(const unsigned int&);
	void KeyUp(const unsigned int&);

	bool IsKeyDown(const unsigned int&);

private:
	bool keys_[256];
};
