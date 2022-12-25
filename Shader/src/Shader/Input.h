#pragma once

class Input
{
public:
	Input();
	Input(const Input&) = default;
	~Input() = default;

	void Initialize();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);
	void ProcessInput(unsigned int msg);

	bool IsKeyDown(unsigned int) const;

private:
	bool mKeys[256];
};
