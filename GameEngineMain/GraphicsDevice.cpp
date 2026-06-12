#include "framework.h"      // 如有预编译头
#include "GraphicsDevice.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

GraphicsDevice::GraphicsDevice() {}
GraphicsDevice::~GraphicsDevice()
{
    Shutdown();
}

bool GraphicsDevice::Initialize(HWND hWnd, UINT width, UINT height, bool windowed)
{
    // ----- 修正：使用窗口的实际客户区尺寸，防止拉伸变形 -----
    RECT clientRect;
    if (hWnd)
    {
        GetClientRect(hWnd, &clientRect);
        width = clientRect.right - clientRect.left;
        height = clientRect.bottom - clientRect.top;
    }
    if (width == 0 || height == 0) return false;

    // 1. 创建交换链和设备
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = windowed ? TRUE : FALSE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL featureLevel;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        nullptr, 0, D3D11_SDK_VERSION,
        &sd, &m_pSwapChain, &m_pDevice, &featureLevel, &m_pContext);
    if (FAILED(hr)) return false;

    // 2. 后台缓冲区 + 渲染目标视图
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
    if (FAILED(hr)) return false;
    hr = m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr)) return false;

    // 3. 深度模板缓冲区
    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    hr = m_pDevice->CreateTexture2D(&depthDesc, nullptr, &m_pDepthStencilBuffer);
    if (FAILED(hr)) return false;

    hr = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, nullptr, &m_pDepthStencilView);
    if (FAILED(hr)) return false;

    // 4. 绑定渲染目标 + 深度缓冲
    m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

    // 5. 视口
    m_Viewport.Width = (FLOAT)width;
    m_Viewport.Height = (FLOAT)height;
    m_Viewport.MinDepth = 0.0f;
    m_Viewport.MaxDepth = 1.0f;
    m_Viewport.TopLeftX = 0;
    m_Viewport.TopLeftY = 0;
    m_pContext->RSSetViewports(1, &m_Viewport);

    // 6. 深度模板状态（开启深度测试）
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    m_pDevice->CreateDepthStencilState(&dsDesc, &m_pDepthStencilState);
    m_pContext->OMSetDepthStencilState(m_pDepthStencilState, 0);

    return true;
}

void GraphicsDevice::Shutdown()
{
    if (m_pContext) m_pContext->ClearState();

    if (m_pDepthStencilState) { m_pDepthStencilState->Release(); m_pDepthStencilState = nullptr; }
    if (m_pDepthStencilView) { m_pDepthStencilView->Release();  m_pDepthStencilView = nullptr; }
    if (m_pDepthStencilBuffer) { m_pDepthStencilBuffer->Release(); m_pDepthStencilBuffer = nullptr; }

    if (m_pRenderTargetView) { m_pRenderTargetView->Release(); m_pRenderTargetView = nullptr; }
    if (m_pSwapChain) { m_pSwapChain->Release();        m_pSwapChain = nullptr; }
    if (m_pContext) { m_pContext->Release();          m_pContext = nullptr; }
    if (m_pDevice) { m_pDevice->Release();           m_pDevice = nullptr; }
}

void GraphicsDevice::Resize(UINT width, UINT height)
{
    if (!m_pSwapChain || !m_pDevice || !m_pContext) return;
    if (width == 0 || height == 0) return;

    m_pContext->OMSetRenderTargets(0, nullptr, nullptr);

    if (m_pRenderTargetView) { m_pRenderTargetView->Release(); m_pRenderTargetView = nullptr; }
    if (m_pDepthStencilView) { m_pDepthStencilView->Release();  m_pDepthStencilView = nullptr; }
    if (m_pDepthStencilBuffer) { m_pDepthStencilBuffer->Release(); m_pDepthStencilBuffer = nullptr; }

    HRESULT hr = m_pSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    if (FAILED(hr)) {
        OutputDebugString(L"ResizeBuffers failed\n");
        return;
    }

    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
    if (FAILED(hr) || !pBackBuffer) return;
    hr = m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr)) return;

    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    hr = m_pDevice->CreateTexture2D(&depthDesc, nullptr, &m_pDepthStencilBuffer);
    if (FAILED(hr)) return;
    hr = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, nullptr, &m_pDepthStencilView);
    if (FAILED(hr)) return;

    m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

    m_Viewport.Width = (FLOAT)width;
    m_Viewport.Height = (FLOAT)height;
    m_pContext->RSSetViewports(1, &m_Viewport);
}

void GraphicsDevice::Clear(float r, float g, float b, float a)
{
    if (m_pContext && m_pRenderTargetView)
    {
        float color[4] = { r, g, b, a };
        m_pContext->ClearRenderTargetView(m_pRenderTargetView, color);
        if (m_pDepthStencilView)
            m_pContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }
}

void GraphicsDevice::Present(bool vsync)
{
    if (m_pSwapChain)
        m_pSwapChain->Present(vsync ? 1 : 0, 0);
}

void GraphicsDevice::EnableDepthTest(bool enable)
{
    // 可扩展深度测试开关，暂不实现
}