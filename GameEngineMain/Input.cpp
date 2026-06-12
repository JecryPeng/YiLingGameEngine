#include "Input.h"
#include <vector>

HWND Input::m_hWnd = nullptr;
BYTE Input::m_KeyState[256] = {};
BYTE Input::m_LastKeyState[256] = {};

// 原始输入累加器
int Input::m_RawMouseAccumX = 0;
int Input::m_RawMouseAccumY = 0;

// 本帧鼠标移动量（供外部读取）
int Input::m_MouseDeltaX = 0;
int Input::m_MouseDeltaY = 0;

bool Input::m_CursorLocked = false;

void Input::SetWindow(HWND hwnd)
{
    m_hWnd = hwnd;
}

void Input::Update()
{
    // 备份键盘状态
    memcpy(m_LastKeyState, m_KeyState, sizeof(m_KeyState));
    if (!GetKeyboardState(m_KeyState))
    {
        OutputDebugString(L"Input::Update: GetKeyboardState failed\n");
        // 即使失败，也继续进行，但键盘状态可能无效
        // 如果你想更严格，可以在这里直接 return，但那样会跳过鼠标更新
    }

    // 使用原始输入累加值作为本帧鼠标移动量
    m_MouseDeltaX = m_RawMouseAccumX;
    m_MouseDeltaY = m_RawMouseAccumY;

    // 清空累加器，准备下一帧
    m_RawMouseAccumX = 0;
    m_RawMouseAccumY = 0;

    // 根据锁定状态显示或隐藏光标（原始输入模式下无需移动光标）
    //if (m_CursorLocked)
    //    ShowCursor(FALSE);
    //else
    //    ShowCursor(TRUE);
}

bool Input::GetKey(KeyCode key)
{
    int vk = static_cast<int>(key);
    return (m_KeyState[vk] & 0x80) != 0;
}

bool Input::GetKeyDown(KeyCode key)
{
    int vk = static_cast<int>(key);
    return ((m_KeyState[vk] & 0x80) != 0) && ((m_LastKeyState[vk] & 0x80) == 0);
}

bool Input::GetKeyUp(KeyCode key)
{
    int vk = static_cast<int>(key);
    return ((m_KeyState[vk] & 0x80) == 0) && ((m_LastKeyState[vk] & 0x80) != 0);
}

bool Input::GetMouseButton(int button)
{
    int vk = (button == 0) ? VK_LBUTTON : (button == 1) ? VK_RBUTTON : VK_MBUTTON;
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

bool Input::GetMouseButtonDown(int button)
{
    static bool prev[3] = { false, false, false };
    bool curr = GetMouseButton(button);
    bool down = curr && !prev[button];
    prev[button] = curr;
    return down;
}

bool Input::GetMouseButtonUp(int button)
{
    static bool prev[3] = { false, false, false };
    bool curr = GetMouseButton(button);
    bool up = !curr && prev[button];
    prev[button] = curr;
    return up;
}

int Input::GetMouseDeltaX()
{
    return m_MouseDeltaX;
}

int Input::GetMouseDeltaY()
{
    return m_MouseDeltaY;
}

void Input::SetCursorLocked(bool locked)
{
    if (m_CursorLocked == locked) return;
    m_CursorLocked = locked;

    // 状态切换时，立即更新光标可见性
    if (locked)
    {
        ShowCursor(FALSE);
        // 清空累加器，避免切换瞬间产生跳跃
        m_RawMouseAccumX = 0;
        m_RawMouseAccumY = 0;
    }
    else
    {
        ShowCursor(TRUE);
    }
}

// 由 WndProc 在收到 WM_INPUT 时调用，累积原始鼠标移动量
void Input::ProcessRawMouseInput(LPARAM lParam)
{
    UINT size = 0;
    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));
    if (size == 0) return;

    std::vector<BYTE> buffer(size);
    if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer.data(), &size, sizeof(RAWINPUTHEADER)) != size)
        return;

    RAWINPUT* raw = (RAWINPUT*)buffer.data();
    if (raw->header.dwType == RIM_TYPEMOUSE)
    {
        // 累加鼠标移动量（硬件单位，不受系统光标加速影响）
        m_RawMouseAccumX += raw->data.mouse.lLastX;
        m_RawMouseAccumY += raw->data.mouse.lLastY;
    }
}