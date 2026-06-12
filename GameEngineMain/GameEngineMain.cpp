// GameEngineMain.cpp
#include "framework.h"
#include "Engine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    Engine engine(hInstance);
    if (!engine.Initialize(L"我的游戏引擎", 1280, 720))
        return -1;

    engine.Run();
    return 0;
}