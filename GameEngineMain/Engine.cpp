#include "framework.h"
#include "Engine.h"
#include "Transform.h"
#include <DirectXMath.h>
#include <chrono>
#include "Camera.h"
#include "Input.h"
#include "FreeLookCamera.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Mesh.h"
#include "ResourceManager.h"

using namespace DirectX;

Engine::Engine(HINSTANCE hInstance) : m_hInstance(hInstance) {}
Engine::~Engine()
{
    ReleaseEngineResources();
    m_Graphics.Shutdown();
}

bool Engine::Initialize(const std::wstring& title, int width, int height)
{
    m_WindowClass = L"MyEngineWindowClass";
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = m_hInstance;
    wc.lpszClassName = m_WindowClass.c_str();
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    RegisterClassExW(&wc);

    m_hWnd = CreateWindowW(m_WindowClass.c_str(), title.c_str(),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, width, height,
        nullptr, nullptr, m_hInstance, nullptr);
    if (!m_hWnd) return false;

    Input::SetWindow(m_hWnd);

    RAWINPUTDEVICE rid;
    rid.usUsagePage = 0x01;
    rid.usUsage = 0x02;
    rid.dwFlags = 0;
    rid.hwndTarget = m_hWnd;
    RegisterRawInputDevices(&rid, 1, sizeof(rid));

    SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);
    if (!m_Graphics.Initialize(m_hWnd, width, height)) return false;

    ShowWindow(m_hWnd, SW_SHOW);
    if (!InitEngineResources()) return false;
    TestCreateScene();
    UpdateWindow(m_hWnd);
    return true;
}

bool Engine::InitEngineResources()
{
    ID3D11Device* device = m_Graphics.GetDevice();
    if (!device) return false;

    D3D11_RASTERIZER_DESC rsDesc = {};
    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.CullMode = D3D11_CULL_BACK;
    rsDesc.FrontCounterClockwise = TRUE;
    rsDesc.DepthClipEnable = TRUE;
    if (FAILED(device->CreateRasterizerState(&rsDesc, &m_pRasterizerState)))
        return false;

    std::vector<D3D11_INPUT_ELEMENT_DESC> layoutDesc =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,    D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,   D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    ResourceManager& rm = ResourceManager::Instance();
    m_pDefaultMaterial = rm.GetOrCreateDefaultMaterial(device, layoutDesc, &m_pVsBlob);
    if (!m_pDefaultMaterial) return false;

    m_pCubeMesh = rm.GetOrCreateCubeMesh(device, layoutDesc, m_pVsBlob);
    if (!m_pCubeMesh) return false;

    // VS Blob 不再需要，释放
    if (m_pVsBlob)
    {
        m_pVsBlob->Release();
        m_pVsBlob = nullptr;
    }

    return true;
}

void Engine::Run()
{
    m_isRunning = true;
    auto prevTime = std::chrono::high_resolution_clock::now();
    while (m_isRunning)
    {
        auto curTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = curTime - prevTime;
        prevTime = curTime;
        float dt = elapsed.count();

        ProcessMessages();
        Input::Update();
        Update(dt);
        Render();
    }
}

void Engine::ProcessMessages()
{
    MSG msg = {};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_QUIT) m_isRunning = false;
    }
}

void Engine::Update(float dt)
{
    for (auto obj : m_SceneObjects)
    {
        if (!obj || !obj->active) continue;
        for (auto comp : obj->GetAllComponents())
            if (comp) comp->Update(dt);
    }

    float speeds[] = { 45.0f, -30.0f, 20.0f };
    int i = 0;
    for (auto obj : m_SceneObjects)
    {
        if (!obj || !obj->active) continue;
        if (obj->GetComponent<Camera>()) continue;
        if (i >= 3) break;
        Transform* t = obj->GetTransform();
        if (t) t->rotation.y += speeds[i++] * dt;
    }
}

void Engine::Render()
{
    m_Graphics.Clear(0.0f, 0.2f, 0.4f);
    if (!m_pActiveCamera) return;

    ID3D11DeviceContext* ctx = m_Graphics.GetContext();
    if (!ctx) return;
    ctx->RSSetState(m_pRasterizerState);

    DirectX::XMMATRIX viewProj = m_pActiveCamera->GetViewProjection();

    XMFLOAT3 lightDir = { 0.5f, -1.0f, -0.5f };
    XMVECTOR v = XMLoadFloat3(&lightDir);
    v = XMVector3Normalize(v);
    XMStoreFloat3(&lightDir, v);
    XMFLOAT4 lightColor = { 1.0f, 1.0f, 1.0f, 1.0f };

    for (auto obj : m_SceneObjects)
    {
        if (!obj || !obj->active) continue;
        if (obj->GetComponent<Camera>()) continue;

        Transform* trans = obj->GetTransform();
        MeshRenderer* rend = obj->GetComponent<MeshRenderer>();
        if (trans && rend && rend->mesh && rend->material)
        {
            XMMATRIX world = trans->GetWorldMatrix();
            rend->Render(ctx, world, viewProj, lightDir, lightColor);
        }
    }

    m_Graphics.Present(true);
}

void Engine::OnSize(UINT width, UINT height, WPARAM wParam)
{
    if (wParam == SIZE_MINIMIZED || width == 0 || height == 0) return;
    m_Graphics.Resize(width, height);
    if (m_pActiveCamera)
        m_pActiveCamera->UpdateAspectRatio((float)width, (float)height);
}

void Engine::ReleaseEngineResources()
{
    if (m_pRasterizerState) { m_pRasterizerState->Release(); m_pRasterizerState = nullptr; }
    if (m_pVsBlob) { m_pVsBlob->Release(); m_pVsBlob = nullptr; }
    ResourceManager::Instance().Clear();
    m_pDefaultMaterial = nullptr;
    m_pCubeMesh = nullptr;
}

void Engine::TestCreateScene()
{
    if (!m_pCubeMesh || !m_pDefaultMaterial) return;

    // 立方体1
    GameObject* c1 = new GameObject();
    c1->name = L"Cube1";
    c1->GetTransform()->position = { 0,0,0 };
    MeshRenderer* r1 = c1->AddComponent<MeshRenderer>();
    r1->mesh = m_pCubeMesh;
    r1->material = m_pDefaultMaterial;
    m_SceneObjects.push_back(c1);

    // 立方体2
    GameObject* c2 = new GameObject();
    c2->name = L"Cube2";
    c2->GetTransform()->position = { 2.5f, 0, 0 };
    c2->GetTransform()->scale = { 0.7f, 0.7f, 0.7f };
    MeshRenderer* r2 = c2->AddComponent<MeshRenderer>();
    r2->mesh = m_pCubeMesh;
    r2->material = m_pDefaultMaterial;
    m_SceneObjects.push_back(c2);

    // 立方体3
    GameObject* c3 = new GameObject();
    c3->name = L"Cube3";
    c3->GetTransform()->position = { -2.0f, 1.5f, 1.0f };
    c3->GetTransform()->scale = { 1.2f, 1.2f, 1.2f };
    MeshRenderer* r3 = c3->AddComponent<MeshRenderer>();
    r3->mesh = m_pCubeMesh;
    r3->material = m_pDefaultMaterial;
    m_SceneObjects.push_back(c3);

    // 摄像机
    GameObject* camObj = new GameObject();
    camObj->name = L"MainCamera";
    camObj->GetTransform()->position = { 2.0f, 2.5f, -4.0f };
    XMVECTOR eye = XMVectorSet(2.0f, 2.5f, -4.0f, 0);
    XMVECTOR target = XMVectorZero();
    XMVECTOR dir = XMVector3Normalize(target - eye);
    float pitch = XMConvertToDegrees(asinf(XMVectorGetY(dir)));
    float yaw = XMConvertToDegrees(atan2f(XMVectorGetX(dir), XMVectorGetZ(dir)));
    camObj->GetTransform()->rotation = { pitch, yaw, 0 };

    Camera* cam = camObj->AddComponent<Camera>();
    RECT cr;
    GetClientRect(m_hWnd, &cr);
    cam->UpdateAspectRatio((float)(cr.right - cr.left), (float)(cr.bottom - cr.top));
    camObj->AddComponent<FreeLookCamera>();
    m_SceneObjects.push_back(camObj);
    SetActiveCamera(cam);
}

void Engine::OnDestroy() { PostQuitMessage(0); }

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Engine* pEngine = (Engine*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    switch (message)
    {
    case WM_SIZE: if (pEngine) pEngine->OnSize(LOWORD(lParam), HIWORD(lParam), wParam); break;
    case WM_DESTROY: if (pEngine) pEngine->OnDestroy(); break;
    case WM_INPUT: Input::ProcessRawMouseInput(lParam); return 0;
    default: return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}