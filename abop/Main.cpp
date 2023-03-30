// Dear ImGui: standalone example application for DirectX 11
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>

//
#include <iostream>

#include <string>
#include <vector>
#include <string>
#include "Stdafx.h"
#include "D3DClass.hpp"
#include "Graphics.hpp"
#include "InputClass.hpp"
#include "LSystem.hpp"

// Data
static LPCWSTR APPLICATION_NAME = L"The Algorithmic Beauty of Plants";
static FLOAT SCREEN_WIDTH = 1280.0f;
static FLOAT SCREEN_HEIGHT = 800.0f;
//static ID3D11Device* g_pd3dDevice = NULL;
//static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
//static IDXGISwapChain* g_pSwapChain = NULL;
//static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
//
//// Forward declarations of helper functions
//bool CreateDeviceD3D(HWND hWnd);
//void CleanupDeviceD3D();
//void CreateRenderTarget();
//void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main code
int main(int, char**)
{
    // window ?대옒???ㅼ젙
    //WNDCLASSEXW wc =
    //{
    //    sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L,
    //    GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
    //    L"ImGui Example", NULL
    //};
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASSEXW wc;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = APPLICATION_NAME;
    wc.cbSize = sizeof(WNDCLASSEX);

    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowW
    (
        wc.lpszClassName, APPLICATION_NAME, WS_OVERLAPPEDWINDOW,
        100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, wc.hInstance, NULL
    );

    // D3D 珥덇린??
    D3DClass* d3d = new D3DClass();
    if (!d3d)
    {
        return -1;
    }

    if (!d3d->Initialize(1280, 800, true, hwnd, false, 1000.0f, 0.1f))
    {
        std::cout << "Could not initialize Direct3D" << std::endl;
        d3d->Shutdown();
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return -1;
    }

    ID3D11RenderTargetView* renderTargetView = NULL;
    renderTargetView = d3d->GetRenderTargetView();

    LSystem* lSystem = new LSystem();
    if (!lSystem)
    {
        return -1;
    }

    // Graphics 珥덇린??
    Graphics* graphics = new Graphics();
    if (!graphics)
    {
        return -1;
    }

    // !!! TEMP
    // Simple Tree - Turn Around를 Rotate(2, 180.f) -> Rotate(0, 2 * angleChange_) 로 커스텀

    lSystem->SetWord("F");
    lSystem->AddRule('F', "F[-&\\[{-G.+G.+G.-|-G.+G.+G.}]FL][\\++&F[{-G.+G.+G.-|-G.+G.+G.}]L]F[--&/F[{-G.+G.+G.-|-G.+G.+G.}]L][+&F[{-G.+G.+G.-|-G.+G.+G.}]L]");
    lSystem->AddRule('L', "[++{-G.+G.+G.-|-G.+G.+G.}]S");
    lSystem->AddRule('S', "[--{-G.+G.+G.-|-G.+G.+G.}]L");
    lSystem->SetLeafAngleChange(22.5f);
    lSystem->SetLeafDistance(0.3f);
    lSystem->SetDistance(2.0f);
    lSystem->SetDeltaThickness(0.9f);
    lSystem->SetAngleChange(22.5f);
    lSystem->Iterate(5);
    // ----------------

    if (graphics->Initialize(hwnd, d3d, lSystem))
    {
        return -1;
    }

    graphics->UpdateModels();

    // Show the window
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);


#pragma region "Setup ImGui"
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
    //io.ConfigViewportsNoDefaultParent = true;
    //io.ConfigDockingAlwaysTabBar = true;
    //io.ConfigDockingTransparentPayload = true;
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(d3d->GetDevice(), d3d->GetDeviceContext());

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);
#pragma endregion

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Input 초기화
    InputClass* input = new InputClass;
    if (!input)
    {
        return -1;
    }

    if (!input->Initialize(hInstance, hwnd, SCREEN_WIDTH, SCREEN_HEIGHT))
    {
        std::cout << "Could not initialize the input object." << std::endl;
        return -1;
    }

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;

        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // UI ?ㅼ젙
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();

        // Rendering
        //d3d->BeginScene
        //(
        //    clear_color.x* clear_color.w,
        //    clear_color.y* clear_color.w,
        //    clear_color.z* clear_color.w,
        //    clear_color.w
        //);

        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };

        d3d->GetDeviceContext()->OMSetRenderTargets(1, &renderTargetView, NULL);
        d3d->GetDeviceContext()->ClearRenderTargetView(d3d->GetRenderTargetView(), clear_color_with_alpha);
        
        // Input (!!! TEMP)
        input->Frame();
        int F, R, PU, RR, U;
        input->GetCameraMove(F, R, PU, RR, U);
        graphics->Frame(F, R, PU, RR, U);

        // TODO: Zbuffer?
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        d3d->EndScene();
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    input->Shutdown();
    input = nullptr;

    delete lSystem;
    lSystem = nullptr;

    graphics->Shutdown();
    graphics = nullptr;

    d3d->Shutdown();
    d3d = nullptr;

    DestroyWindow(hwnd);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    //case WM_SIZE:
    //    if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
    //    {
    //        CleanupRenderTarget();
    //        g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
    //        CreateRenderTarget();
    //    }
    //    return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_DPICHANGED:
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
        {
            //const int dpi = HIWORD(wParam);
            //printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
            const RECT* suggested_rect = (RECT*)lParam;
            SetWindowPos(hWnd, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        break;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}
