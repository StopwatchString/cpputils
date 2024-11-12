#ifndef DIRECTX_UTILS_H
#define DIRECTX_UTILS_H

#include "glh/glh.h"

#include <D3D11_1.h>
#include <wrl/client.h>  // for Microsoft::WRL::ComPtr template

#include <iostream>
#include <vector>
#include <bit>
#include <assert.h>

struct D3DState
{
    Microsoft::WRL::ComPtr<ID3D11Device1> d3dDevice1                { NULL };
    Microsoft::WRL::ComPtr<ID3D11DeviceContext1> d3dDeviceContext1  { NULL };
    HANDLE hWglD3DDevice                                            { NULL };
};

static D3DState initDirect3D()
{
    D3DState d3dState{};

    // Load the D3D library
    HMODULE hD3D11Lib = LoadLibraryA("d3d11.dll");
    if (hD3D11Lib == NULL) {
        std::cerr << "ERROR initDirect3D() Unable to load d3d11.dll" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Get the function pointer for D3D11CreateDevice from the loaded D3D library
    PFN_D3D11_CREATE_DEVICE D3D11CreateDevicePtr = (PFN_D3D11_CREATE_DEVICE)GetProcAddress(hD3D11Lib, "D3D11CreateDevice");
    if (D3D11CreateDevicePtr == NULL) {
        std::cerr << "ERROR initDirect3D() Could not GetProcAddress of D3D11CreateDevice" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Use D3D11CreateDevice() to make a D3DDevice and D3DDeviceContext
    Microsoft::WRL::ComPtr<ID3D11Device> d3dDevice = nullptr;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3dDeviceContext = nullptr;
    const bool directxDebugFlag = false;
    const UINT deviceFlags =
        D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS | // no separate D3D11 worker thread
        (directxDebugFlag ? D3D11_CREATE_DEVICE_DEBUG : 0) | // useful for diagnosing DX failures
        D3D11_CREATE_DEVICE_SINGLETHREADED;
    const std::vector<D3D_FEATURE_LEVEL> featureLevel = { D3D_FEATURE_LEVEL_11_1 };
    HRESULT hr = D3D11CreateDevicePtr(
        NULL,                      // pAdapter           | Video Adapter, NULL = default
        D3D_DRIVER_TYPE_HARDWARE,  // DriverType         | TYPE_HARDWARE specifies a device that implements D3D in hardware
        NULL,                      // Software           | Indicates what software renderer to use when Driver Type is Software
        deviceFlags,               // Flags              | Runtime layers to enable https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_create_device_flag
        featureLevel.data(),       // pFeatureLevels     | Pointer to an array of D3D Feature Levels https://learn.microsoft.com/en-us/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_feature_level
        featureLevel.size(),       // FeatureLevels      | Number of specified feature levels
        D3D11_SDK_VERSION,         // SDK Version        | Use D3D11_SDK_VERSION
        &d3dDevice,                // ppDevice           | Holds address to pointer to device created
        NULL,                      // pFeatureLevel      | Can check for supported feature levels. NULL means don't check
        &d3dDeviceContext          // ppImmediateContext | Holds address to pointer to the ID3D11DeviceContext
    );
    if (hr != S_OK) {
        std::cerr << "ERROR initDirect3D() D3D11CreateDevicePtr failed!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Cast d3dDevice and d3dDeviceContext into their 11.1 variants
    if (!SUCCEEDED(d3dDevice.As(&d3dState.d3dDevice1))) {
        std::cerr << "ERROR initDirect3D() Could not cast D3D11 device into its D3D11.1 counterpart!" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (!SUCCEEDED(d3dDeviceContext.As(&d3dState.d3dDeviceContext1))) {
        std::cerr << "ERROR initDirect3D() Could not cast D3D11 device context into its D3D11.1 counterpart!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Check that resource sharing is available
    D3D11_FEATURE_DATA_D3D11_OPTIONS opts;
    d3dState.d3dDevice1->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS, &opts, sizeof(opts));
    if (opts.ExtendedResourceSharing != 1) {
        std::cout << "ERROR initDirect3D() DirectX Feature ExtendedResourceSharing is not supported." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Create WGL interop device
    d3dState.hWglD3DDevice = wglDXOpenDeviceNV(d3dState.d3dDevice1.Get());
    if (d3dState.hWglD3DDevice == NULL) {
        std::cerr << "ERROR initDirect3D() wglDXOpenDeviceNV failed!" << std::endl;
        exit(EXIT_FAILURE);
    }

    return d3dState;
}

struct SharedTexture {
    ID3D11Texture2D* d3dTextureShared  { nullptr };
    HANDLE hSharedTexture              { NULL };
    GLuint glTextureShared             { 0 };
    HANDLE hSharedTextureLock           { NULL };
};

static uint32_t ilog2(uint32_t n) {
    return std::bit_width(n) - 1;
}

static SharedTexture createSharedTexture(const D3DState& dxState, int width, int height, bool mipmaps, bool sRGB)
{
    SharedTexture tex{};

    D3D11_TEXTURE2D_DESC texDesc;
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = mipmaps ? ilog2(max(width, height)) : 1;
    texDesc.ArraySize = 1;
    texDesc.Format = sRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;
    texDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_NTHANDLE | D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

    // Initial image data is empty
    //const D3D11_SUBRESOURCE_DATA* initialData = nullptr;

    // Create initial green color data
    std::vector<uint8_t> greenColorData(width * height * 4, 0);
    for (size_t i = 0; i < greenColorData.size(); i += 4) {
        greenColorData[i] = 0;     // Red
        greenColorData[i + 1] = 255; // Green
        greenColorData[i + 2] = 0;   // Blue
        greenColorData[i + 3] = 255; // Alpha
    }

    D3D11_SUBRESOURCE_DATA initialData;
    initialData.pSysMem = greenColorData.data();
    initialData.SysMemPitch = width * 4; // RGBA, so 4 bytes per pixel
    initialData.SysMemSlicePitch = 0;

    HRESULT hr = NULL;

    // Create the DirectX2D texture
    hr = dxState.d3dDevice1->CreateTexture2D(
        &texDesc,              // pDesc        | Texture description object
        &initialData,          // pInitialData | Initial data for texture
        &tex.d3dTextureShared  // ppTexture2D  | Target pointer for created texture
    );
    if (hr != S_OK) {
        std::cerr << "ERROR createSharedTexture() CreateTexture2D() failed!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Get the Resource1 interface from the DirectX2D Texture for CreateSharedHandle()
    IDXGIResource1* pResource = NULL;
    hr = tex.d3dTextureShared->QueryInterface(
        __uuidof(IDXGIResource1),  // Interface to retrieve out of COM object (the texture object)
        (void**)&pResource         // Target to place interface in
    );
    if (hr != S_OK) {
        std::cerr << "ERROR createSharedTexture() QueryInterface failed!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Use CreateSharedHandle() to make a sharable handle betwen OpenGL and DirectX
    // NOTE: This does not create any sort of association between the texture and shared handle yet.
    // The IDXGIResource1 interface was retreived from the texture interface, but that does not inherently link them.
    hr = pResource->CreateSharedHandle(
        NULL,                                                    // pAttributes | Security Attributes
        DXGI_SHARED_RESOURCE_READ | DXGI_SHARED_RESOURCE_WRITE,  // dwAccess    | Requested access rights
        NULL,                                                    // lpName      | Name of resource to share
        &tex.hSharedTexture                                      // pHandle     | 
    );
    if (hr != S_OK) {
        std::cerr << "ERROR createSharedTexture() CreateTexture2D() failed!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // We don't need the Resource1 interface anymore, so release it (decrementing its ref counter)
    pResource->Release();

    // Associate the D3DTexture with the generated resource handle
    if (!wglDXSetResourceShareHandleNV(tex.d3dTextureShared, tex.hSharedTexture)) {
        std::cerr << "ERROR createSharedTexture() wglDXSetResourceShareHandleNV() failed!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Make the OpenGL Texture
    glGenTextures(1, &tex.glTextureShared);

    // This lock synchronizes control of the texture between D3D and OpenGL
    // You *must* lock and unlock this resource when using the texture in OpenGL for any operation.
    // TBD for DirectX operations.
    tex.hSharedTextureLock = wglDXRegisterObjectNV(
        dxState.hWglD3DDevice,    // hDevice  | 
        tex.d3dTextureShared,     // dxObject | 
        tex.glTextureShared,      // name     | 
        GL_TEXTURE_2D,            // type     | 
        WGL_ACCESS_READ_WRITE_NV  // access   | 
    );
    if (tex.hSharedTextureLock == NULL) {
        std::cerr << "wglDXRegisterObjectNV failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (mipmaps) {
        glTextureParameteri(tex.glTextureShared, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(tex.glTextureShared, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        glTextureParameteri(tex.glTextureShared, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(tex.glTextureShared, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    return tex;
}

#endif