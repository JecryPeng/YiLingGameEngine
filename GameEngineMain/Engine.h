#pragma once
#include "GraphicsDevice.h"
#include <Windows.h>
#include <string>
#include <vector>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "GameObject.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"

#pragma comment(lib, "d3dcompiler.lib")

class Engine
{
public:
    Engine(HINSTANCE hInstance);
    ~Engine();
    bool Initialize(const std::wstring& title, int width, int height);
    void Run();
    void RequestQuit() { m_isRunning = false; }
    GraphicsDevice& GetGraphics() { return m_Graphics; }
    HWND GetWindowHandle() const { return m_hWnd; }
    void SetActiveCamera(Camera* cam) { m_pActiveCamera = cam; }
    void OnSize(UINT width, UINT height, WPARAM wParam);
    void OnDestroy();

private:
    void ProcessMessages();
    void Update(float dt);
    void Render();
    bool InitEngineResources();
    void ReleaseEngineResources();
    void TestCreateScene();

    HINSTANCE       m_hInstance;
    HWND            m_hWnd = nullptr;
    GraphicsDevice  m_Graphics;
    bool            m_isRunning = false;
    std::wstring    m_WindowClass;
    std::vector<GameObject*> m_SceneObjects;

    // ‰÷»æ◊ ‘¥
    ID3D11RasterizerState* m_pRasterizerState = nullptr;
    ID3DBlob* m_pVsBlob = nullptr;
    Camera* m_pActiveCamera = nullptr;
    Material* m_pDefaultMaterial = nullptr;
    Mesh* m_pCubeMesh = nullptr;

};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);