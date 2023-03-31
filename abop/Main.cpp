#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>


#include <iostream>

#include <string>
#include <vector>
#include <string>
#include "Stdafx.h"
#include "D3DClass.hpp"
#include "Graphics.hpp"
#include "InputClass.hpp"
#include "LSystem.hpp"
#include "LRule.hpp"

// Data
static LPCWSTR APPLICATION_NAME = L"The Algorithmic Beauty of Plants";
static FLOAT SCREEN_WIDTH = 1280.0f;
static FLOAT SCREEN_HEIGHT = 800.0f;

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main code
int main(int, char**)
{
    // window 클래스 설정
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

    // D3D 초기화
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

    // Graphics 초기화
    Graphics* graphics = new Graphics();
    if (!graphics)
    {
        return -1;
    }

    // !!! TEMP
    // Simple Tree - Turn Around를 Rotate(2, 180.f) -> Rotate(0, 2 * angleChange_) 로 커스텀

    //lSystem->SetWord("F");
    //lSystem->AddRule('F', "F[-&\\[{-G.+G.+G.-|-G.+G.+G.}]FL][\\++&F[{-G.+G.+G.-|-G.+G.+G.}]L]F[--&/F[{-G.+G.+G.-|-G.+G.+G.}]L][+&F[{-G.+G.+G.-|-G.+G.+G.}]L]");
    //lSystem->AddRule('L', "[++{-G.+G.+G.-|-G.+G.+G.}]S");
    //lSystem->AddRule('S', "[--{-G.+G.+G.-|-G.+G.+G.}]L");
    //lSystem->SetLeafAngleChange(22.5f);
    //lSystem->SetLeafDistance(0.3f);
    //lSystem->SetDistance(2.0f);
    //lSystem->SetDeltaThickness(0.9f);
    //lSystem->SetAngleChange(22.5f);
    //lSystem->Iterate(5);
    // ----------------

    if (graphics->Initialize(hwnd, d3d, lSystem))
    {
        return -1;
    }

    // Init Render
    lSystem->SetWord("F");
    lSystem->AddRule('F', "FF");
    lSystem->AddRule('F', "F[-&\\[{-G.+G.+G.-|-G.+G.+G.}]FL][\\++&F[{-G.+G.+G.-|-G.+G.+G.}]L]F[--&/F[{-G.+G.+G.-|-G.+G.+G.}]L][+&F[{-G.+G.+G.-|-G.+G.+G.}]L]");
    lSystem->AddRule('L', "[++{-G.+G.+G.-|-G.+G.+G.}]S");
    lSystem->AddRule('S', "[--{-G.+G.+G.-|-G.+G.+G.}]L");
    //lSystem->SetLeafAngleChange(22.5f);
    //lSystem->SetLeafDistance(0.3f);
    lSystem->SetAngleChange(22.5f);
    lSystem->SetDistance(1.5f);
    lSystem->SetThickness(0.5f);
    lSystem->SetDeltaThickness(0.9f);
    lSystem->Iterate(4);
    //std::cout << lSystem->GetRules()[0].GetRule() << std::endl;
    graphics->UpdateModels();
    // ----------------

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

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(d3d->GetDevice(), d3d->GetDeviceContext());

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

    static bool show_location_window = false;
    static bool show_console_window = false;
    static bool show_light_window = false;

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

        // UI 설정
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();


        // 1. UI (Default)
        {
            // Demo Window
            //bool tempp = true;
            //ImGui::ShowDemoWindow(&tempp);
            
            ImGui::Begin("DirectX Controller");
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 128));
            ImGui::Text("Welcome, DirectX World! \n\nThis is a library viewer for real-time \ngrowing plant models.");
            ImGui::Text("You can use these models by adding them \nas assets to any graphics development engine.");
            ImGui::PopStyleColor();

            // FPS
            ImGui::Text("\nFPS :");
            if (100 <= io.Framerate)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 255, 255));
                ImGui::SameLine();
                ImGui::Text("\n%.1f", io.Framerate);
                ImGui::PopStyleColor();
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255)); 
                ImGui::SameLine();
                ImGui::Text("\n%.1f", io.Framerate);
                ImGui::PopStyleColor();
            }
            
            // Application average
            ImGui::Text("Application average", 1000.0f / io.Framerate);
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 255, 128));
            ImGui::SameLine();
            ImGui::Text("%.3f", 1000.0f / io.Framerate);
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::Text("ms/frame", 1000.0f / io.Framerate);

            // Background Color
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 190, 255, 255));
            ImGui::Text("\n <Background Color>");
            ImGui::PopStyleColor();
            ImGui::ColorEdit4("", (float*)&clear_color);

            ImGui::Text("\nViewer Windows.");
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 190, 255, 255));
            ImGui::Checkbox("Camera Location Window", &show_location_window);
            ImGui::Checkbox("Console Edit Window", &show_console_window);
            ImGui::Checkbox("Light Location Window", &show_light_window);
            ImGui::PopStyleColor();
        }

        if (show_location_window)
        {
            // Camera
            static float cameraPosition[4] = { 0.0f, 0.0f, -10.0f };
            static float cameraRotation[4] = { 0.0f, 0.0f, 0.0f };
            static float cameraSpeed = 0.3f;
            static float cameraSensitivity = 0.01f;
            
            ImGui::Begin("Camera Location Window", &show_location_window);

            // Camera Position
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 190, 255, 255));
            ImGui::Text("\n<Camera Position>");
            ImGui::PopStyleColor();
            ImGui::Text("(X, Y, Z) :"); 
            ImGui::SameLine();
            if (ImGui::InputFloat3("##positon", cameraPosition))
            {
                graphics->SetCameraPosition(cameraPosition[0], cameraPosition[1], cameraPosition[2]);
            }

            // Camera Rotation
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 190, 255, 255));
            ImGui::Text("\n<Camera Rotation>");
            ImGui::PopStyleColor();
            ImGui::Text("(P, R, Y) :"); 
            ImGui::SameLine();
            if (ImGui::InputFloat3("##rotation", cameraRotation))
            {
                // 카메라 각도 조절
            }

            // Camera Speed
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 190, 255, 255));
            ImGui::Text("\n<Camera Speed>");
            ImGui::PopStyleColor();
            if (ImGui::InputFloat("  ", &cameraSpeed, 0.01f, 0.3f, "%.2f"))
            {
                if (cameraSpeed < 0.01f)
                {
                    cameraSpeed = 0.01f;
                }
                if (cameraSpeed >= 5.0f)
                {
                    cameraSpeed = 5.0f;
                }
                graphics->SetCameraSpeed(cameraSpeed);
            }

            // Camera Sensitivity
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 190, 255, 255));
            ImGui::Text("\n<Camera Sensitivity>");
            ImGui::PopStyleColor();
            if (ImGui::InputFloat(" ", &cameraSensitivity, 0.005f, 0.01f, "%.3f"))
            {
                if (cameraSpeed < 0.001f)
                {
                    cameraSpeed = 0.001f;
                }
                if (cameraSensitivity >= 0.1f)
                {
                    cameraSensitivity = 0.1f;
                }
                graphics->SetCameraSensitivity(cameraSensitivity);
            }

            ImGui::End();

        }

        if (show_console_window)
        {
            ImGui::Begin("Console Edit Window", &show_console_window);

            // 콘솔 클래스 함수 실행 (ex) Draw()

            ImGui::End();
        }

        if (show_light_window)
        {
            ImGui::Begin("Light Location Window", &show_light_window);

            // Light
                //-on / off
                //- shadow ?
                //-light color
                //- position 3
                //- rotation 3

            ImGui::End();
        }

        // Plane 유무 checkbox
        //ImGui::Checkbox("Draw Plane", &show_light_window);

        // wireframe, solid select
        //ImGui::Checkbox("", &show_light_window);

        ImGui::End();


        // Demonstrate the various window flags. Typically you would just use the default!
        static bool no_titlebar = false;
        static bool no_scrollbar = false;
        static bool no_menu = false;
        static bool no_move = false;
        static bool no_resize = false;
        static bool no_collapse = false;
        static bool no_close = false;
        static bool no_nav = false;
        static bool no_background = false;
        static bool no_bring_to_front = false;
        static bool no_docking = false;
        static bool unsaved_document = false;

        bool myLsystemMenuBar = true;

        ImGuiWindowFlags window_flags = 0;
        if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
        if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
        if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
        if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
        if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
        if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
        if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
        if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
        if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
        if (no_docking)         window_flags |= ImGuiWindowFlags_NoDocking;
        if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;
        if (no_close)           myLsystemMenuBar = NULL; // Don't pass our bool* to Begin

        // 2. UI (L-System)
        ImGui::Begin("L-System", &myLsystemMenuBar, ImGuiWindowFlags_MenuBar);

        // L-System : Menu bar
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open", "Ctrl+O"))
                {
                    // do something
                }
                if (ImGui::MenuItem("Save", "Ctrl+S"))
                {
                    // do something
                }
                ImGui::EndMenu();
            }
            
            if (ImGui::BeginMenu("Preset"))
            {
                if (ImGui::MenuItem("2D Example"))
                {
                    // do something
                }
                if (ImGui::MenuItem("3D Example"))
                {
                    // do something
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // L-System : Main window
        //ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 190, 255));
        //ImGui::Text("\n<L-System Algorithm Word>");
        //ImGui::PopStyleColor();

        //// One-line Text Input
        //static char word[128] = "ex) FFFFF";
        //ImGui::InputText(" ", word, IM_ARRAYSIZE(word));
        //ImGui::SameLine();
        //if (ImGui::Button("Render"))
        //{
        //    lSystem->SetWord(word);
        //    lSystem->ClearState();
        //    graphics->UpdateModels();
        //}

        // Multi-line Text Input
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 190, 255));
        ImGui::Text("<L-System Algorithm Word>");
        ImGui::PopStyleColor();

        static char multiText[1024 * 16] = "Input your multi-line text..";
        static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
        // TODO 화면 밖에 나가면 줄바꿈 되도록 수정 예정
        ImGui::InputTextMultiline("##source", multiText, IM_ARRAYSIZE(multiText), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);

        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 190, 255));
        ImGui::Text("<L-System Algorithm Rules>");
        ImGui::PopStyleColor();

        static char multiText2[1024 * 16] = "Input your multi-line text..";
        // TODO 화면 밖에 나가면 줄바꿈 되도록 수정 예정
        ImGui::InputTextMultiline("##rules", multiText2, IM_ARRAYSIZE(multiText), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);
        
        if (ImGui::Button("Clear"))
        {
            lSystem->SetWord("");
            lSystem->ClearState();
            graphics->UpdateModels();
        }
        ImGui::SameLine();
        if (ImGui::Button("Render"))
        {
            lSystem->SetWord(multiText);
            lSystem->ClearState();
            graphics->UpdateModels();
        }
        
        
        // Word

        // Rule
        //ImGui::BeginChild("Scrolling");
        //for (LRule rule : lSystem->GetRules())
        
        //static std::vector<LRule> rules = ;
        //ImGui::Text("%d", rules.size());
        ////for (int i = 0; i < )
        //for (LRule& rule : rules)
        //{
        //    static std::string s = rules[0].GetRule();
        //    ImGui::Text("%s", s);
        //}
        //ImGui::EndChild();

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
