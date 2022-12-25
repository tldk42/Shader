#include "Input.h"

Input::Input()
	: mKeys()
{
}

void Input::Initialize()
{
	for (auto& mKey : mKeys)
	{
		mKey = false;
	}
}

void Input::KeyDown(const unsigned input)
{
	mKeys[input] = true;
}

void Input::KeyUp(const unsigned input)
{
	mKeys[input] = false;
}

void Input::ProcessInput(unsigned msg)
{
}

bool Input::IsKeyDown(const unsigned input) const
{
	return mKeys[input];
}
