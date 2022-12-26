#include "Shader/System.h"
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

int APIENTRY wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int)
{
	System* system = new System;
	if (system == nullptr)
	{
		return 0;
	}
	if (system->Initialize())
	{
		system->Run();
	}

	system->Exit();
	delete system;
	system = nullptr;

	return 0;
}
