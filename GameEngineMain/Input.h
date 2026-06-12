#pragma once
#include <Windows.h>

enum class KeyCode
{
    W = 'W', A = 'A', S = 'S', D = 'D',
    Q = 'Q', E = 'E', R = 'R',
    Up = VK_UP, Down = VK_DOWN, Left = VK_LEFT, Right = VK_RIGHT,
    Shift = VK_SHIFT, Control = VK_CONTROL, Alt = VK_MENU,
    Space = VK_SPACE, Enter = VK_RETURN, Escape = VK_ESCAPE,
    MouseLeft = VK_LBUTTON, MouseRight = VK_RBUTTON, MouseMiddle = VK_MBUTTON
};

class Input
{
public:
    static void SetWindow(HWND hwnd);
    static void Update();

    static bool GetKey(KeyCode key);
    static bool GetKeyDown(KeyCode key);
    static bool GetKeyUp(KeyCode key);

    static bool GetMouseButton(int button);      // 0=左, 1=右, 2=中
    static bool GetMouseButtonDown(int button);
    static bool GetMouseButtonUp(int button);

    static int GetMouseDeltaX();
    static int GetMouseDeltaY();

    static void SetCursorLocked(bool locked);

    static void ProcessRawMouseInput(LPARAM lParam);  // 由 WndProc 调用

private:
    static void CenterCursor();

    static HWND m_hWnd;
    static BYTE m_KeyState[256];
    static BYTE m_LastKeyState[256];
    static int m_MouseDeltaX, m_MouseDeltaY;
    static bool m_CursorLocked;
    static int m_RawMouseAccumX, m_RawMouseAccumY;    // 累加原始移动量
};