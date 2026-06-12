#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>

class GraphicsDevice
{
public:
    GraphicsDevice();
    ~GraphicsDevice();

    bool Initialize(HWND hWnd, UINT width, UINT height, bool windowed = true);
    void Shutdown();
    void Resize(UINT width, UINT height);
    void Clear(float r, float g, float b, float a = 1.0f);
    void Present(bool vsync = true);

    ID3D11Device* GetDevice() const { return m_pDevice; }
    ID3D11DeviceContext* GetContext() const { return m_pContext; }

    // 可显式开启/关闭深度测试（默认开启）
    void EnableDepthTest(bool enable);

private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;
    IDXGISwapChain* m_pSwapChain = nullptr;
    ID3D11RenderTargetView* m_pRenderTargetView = nullptr;

    // 深度缓冲相关
    ID3D11Texture2D* m_pDepthStencilBuffer = nullptr;
    ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
    ID3D11DepthStencilState* m_pDepthStencilState = nullptr;

    D3D11_VIEWPORT          m_Viewport = {};
};