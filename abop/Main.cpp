#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_internal.h"

#include "imgui/imGuIZMOquat.h"

#include <d3d11.h>
#include <tchar.h>

#include <iostream>
#include <fstream>
#include <filesystem>

#include <chrono>

#include <string>
#include <vector>
#include <map>
#include <string>
#include "Stdafx.h"
#include "Utils.hpp"
#include "D3DClass.hpp"
#include "Graphics.hpp"
#include "InputClass.hpp"
#include "LSystem.hpp"
#include "LRule.hpp"
#include "Constant.hpp"

#include <windows.h>

// Data
std::string PATH = "./data/preset/";

static LPCWSTR APPLICATION_NAME = L"The Algorithmic Beauty of Plants";
static FLOAT SCREEN_WIDTH = 1280.0f;
static FLOAT SCREEN_HEIGHT = 800.0f;

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Utils
void ClearCharArray(int size, char* out);
std::vector<std::string> LoadPresetList();
void SavePreset(std::string filename, LSystem* lsystem);
unsigned long long timeSinceEpochMiliisec();
void PrintConstant(std::string& constant, std::string& value);

// Main code
int main(int, char**)
{
#pragma region Init
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

    if (!d3d->Initialize(SCREEN_WIDTH, SCREEN_HEIGHT, true, hwnd, false, 1000.0f, 0.1f))
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

    // rules & ignores
    std::vector<LRule::RuleInfo> ruleInfos = std::vector<LRule::RuleInfo>();
    std::map<char, bool> ignores = std::map<char, bool>();

    // Graphics 초기화
    Graphics* graphics = new Graphics();
    if (!graphics)
    {
        return -1;
    }

    if (graphics->Initialize(hwnd, d3d, lSystem))
    {
        return -1;
    }

    // Show the window
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);
#pragma endregion

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
    
    static bool show_mouse_window = false;

    // Preset load (Init render) 시 아래 변수를 true로 설정해야
    // 다음 frame에서 load 함
    static bool isUpdateRules = true;
    static bool isUpdateIgnores = true;
    static bool isUpdateWord = true;
    static bool isUpdateCamera = true;
    static bool isUpdateLSystemSetting = true;

    bool running = false;
    float runningTime = 0.0f;
    unsigned long long lastTick;
    quat qRot = quat(1.f, 0.f, 0.f, 0.f);

    std::string firstString;

    // Main loop
    bool done = false;
    while (!done)
    {
        lastTick = timeSinceEpochMiliisec();

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

        // Demo Window
        //bool tempp = false;
        //ImGui::ShowDemoWindow(&tempp);

#pragma region UI_Default
        // 1. UI (Default)
        {
            
            style.FrameRounding = 4.f;
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
            
            if (isUpdateCamera)
            {
                // 현재 항상 카메라 위치 업데이트
                DirectX::XMFLOAT3 v = graphics->GetCameraPosition();
                cameraPosition[0] = v.x;
                cameraPosition[1] = v.y;
                cameraPosition[2] = v.z;

                // !!! 각도는 나중에
            }

            ImGui::Begin("Camera Location Window", &show_location_window);

            ImGui::Text(" ");
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 255, 225));
            ImGui::Checkbox("Mouse Drag Window", &show_mouse_window);
            ImGui::PopStyleColor();

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

            // Camera Arm
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 190, 255, 255));
            ImGui::Text("\n<Camera Arm>");
            ImGui::PopStyleColor();

            if (ImGui::Button("Arm On"))
            {
                //graphics->SetCameraPosition(cameraPosition[0], cameraPosition[1], cameraPosition[2]);
            }
            ImGui::SameLine();
            if (ImGui::Button("Arm Off"))
            {

            }
            
            if (show_mouse_window)
            {


                ImGui::Begin("Mouse Drag Window", &show_mouse_window);

                const float w = ImGui::GetContentRegionAvailWidth();
                const float half = w / 2.f;
                const float third = w / 3.f;

                static float resSolid = 1.0;
                static float axesLen = .95;
                static float axesThickness = 1.0;
                vec3 resAxes(axesLen, axesThickness, axesThickness);

                ImGui::PushItemWidth(third);
                ImGui::DragFloat("##res1", &axesLen, 0.01f, 0.0, 1.0, "len %.2f");
                ImGui::SameLine();
                ImGui::DragFloat("##res2", &axesThickness, 0.01f, 0.0, 8.0, "thick %.2f");
                ImGui::SameLine();
                ImGui::DragFloat("##res3", &resSolid, 0.01f, 0.0, 8.0, "solids %.2f");
                ImGui::PopItemWidth();

                imguiGizmo::resizeAxesOf(resAxes);
                imguiGizmo::resizeSolidOf(resSolid);
                if (ImGui::gizmo3D("##gizmo1", qRot, w /*, mode */))
                {
                    // !! 수정해야 함
                    graphics->SetCameraPosition(cameraPosition[0] + qRot.x, cameraPosition[1] + qRot.y, cameraPosition[2] + qRot.z);
                }
                //mat4 modelMatrix = mat4_cast(qRot);

                ImGui::End();
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

#pragma endregion UI_Default

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

#pragma region L-System
        // 2. UI (L-System)
        ImGui::Begin("L-System", &myLsystemMenuBar, ImGuiWindowFlags_MenuBar);

        // L-System : Menu bar
        if (ImGui::BeginMenuBar())
        {
            //if (ImGui::BeginMenu("File"))
            //{
            //    if (ImGui::MenuItem("Open", "Ctrl+O"))
            //    {
            //        // do something
            //    }
            //    if (ImGui::MenuItem("Save As", "Ctrl+S"))
            //    {
            //        ImGui::OpenPopup("SaveAs");
            //    }

            //    ImGui::EndMenu();
            //}
            
            if (ImGui::BeginMenu("Preset"))
            {
                for (std::string& presetName : LoadPresetList())
                {
                    char filename[128];
                    strcpy_s(filename, presetName.c_str());
                    if (ImGui::MenuItem(filename))
                    {
                        lSystem->LoadPreset(presetName);
                        isUpdateRules = true;
                        isUpdateWord = true;
                        isUpdateCamera = true;
                        isUpdateLSystemSetting = true;
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // Multi-line Text 
        int count = 0;
        static char fristString[1024 * 256] = "";
        static char word[1024 * 256] = "";

        if (isUpdateWord)
        {
            ClearCharArray(1024 * 256, word);
            lSystem->GetWord(word);
            isUpdateWord = false;
        }

        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 190, 255, 255));
        ImGui::Text("\n < Auto Render >");
        ImGui::PopStyleColor();
        
        static int frequency = 5;

        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(3 / 7.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(3 / 7.0f, 0.8f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(3 / 7.0f, 0.8f, 0.8f));
        if (ImGui::Button("Start")) // render
        {
            running = true;
            firstString = lSystem->GetWord();
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(5 / 7.0f, 0.5f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(5 / 7.0f, 0.8f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(5 / 7.0f, 0.8f, 0.8f));
        if (ImGui::Button("Pause"))
        {
            running = false;
            runningTime = 0.f;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0, 0.8f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0, 0.8f, 0.8f));
        if (ImGui::Button("Stop"))
        {
            
            // Reset (rule은 유지)
            lSystem->SetWord("");
            lSystem->ClearRule();
            lSystem->ClearState();
            ClearCharArray(1024 * 256, word);
            graphics->UpdateModels();
            running = false;
            runningTime = 0;

            // Render
            lSystem->SetWord(firstString);
            isUpdateWord = true;
            //lSystem->ClearState();
            //graphics->UpdateModels();

        }
        ImGui::PopStyleColor(3);

        if (running)
        {
            if (runningTime > frequency)
            {
                ClearCharArray(1024 * 64, word);
                lSystem->Iterate(1);
                lSystem->GetWord(word);
                lSystem->SetWord(word);
                lSystem->ClearState();
                graphics->UpdateModels();
                runningTime = 0.f;
            }
            runningTime += timeSinceEpochMiliisec() - lastTick;
        }

        ImGui::SameLine();
        ImGui::AlignTextToFramePadding();
        ImGui::Text("frequency :");
        ImGui::SameLine();
        float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
        ImGui::Text("%d", frequency);
        ImGui::SameLine();
        ImGui::PushButtonRepeat(true);
        if (ImGui::ArrowButton("##left", ImGuiDir_Left)) 
        { 
            frequency--;
            if (frequency < 1) { frequency = 1; }
        }
        ImGui::SameLine(0.0f, spacing);
        if (ImGui::ArrowButton("##right", ImGuiDir_Right)) { frequency++; }
        ImGui::PopButtonRepeat();


        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 190, 255, 255));
        ImGui::Text("\n < Manual Render >");
        ImGui::PopStyleColor();

        static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;

        
        if (ImGui::Button("Reset"))
        {
            lSystem->SetWord("");
            lSystem->ClearRule();
            lSystem->ClearState();
            ClearCharArray(1024 * 256, word);
            graphics->UpdateModels();

            isUpdateRules = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Iterate"))
        {
            
            ClearCharArray(1024 * 64, word);
            lSystem->Iterate(1);
            lSystem->GetWord(word);
        }
        ImGui::SameLine();
        if (ImGui::Button("Render"))
        {
            lSystem->SetWord(word);
            lSystem->ClearState();
            graphics->UpdateModels();
        }

        // !!! 나중에 menu bar로 옮기기 (popup이 안되는 이슈)
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(4 / 7.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(4 / 7.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(4 / 7.0f, 0.8f, 0.8f));
        if (ImGui::Button("Save As"))
        {
            ImGui::OpenPopup("SaveAs");
        }

        if (ImGui::BeginPopup("SaveAs", NULL))
        {
            std::string filename = "";
            static char buffer[128];

            ImGui::InputText("Preset Name", buffer, IM_ARRAYSIZE(buffer));

            if (ImGui::Button("Save"))
            {
                filename = buffer;

                // !!! 배포할 때에는 경로를 직접 수정
                SavePreset(filename, lSystem);
                ClearCharArray(128, buffer);

                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                ClearCharArray(128, buffer);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        ImGui::PopStyleColor(3);


        if (ImGui::CollapsingHeader("Word"))
        {
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(4 / 7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(4 / 7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(4 / 7.0f, 0.8f, 0.8f));
            if (ImGui::Button("Save"))
            {
                lSystem->SetWord(word);
            }
            ImGui::PopStyleColor(3);

            // TODO 화면 밖에 나가면 줄바꿈 되도록 수정 예정
            ImGui::InputTextMultiline("words", word, IM_ARRAYSIZE(word),
                ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 4), flags);
        }

        if (ImGui::CollapsingHeader("Rules"))
        {
            static char addIgnore[4] = "";
            // Ignores
            if (ImGui::Button("Add Ignores"))
            {
                ClearCharArray(4, addIgnore);
                ImGui::OpenPopup("AddIgnore");
            }

            if (isUpdateIgnores)
            {
                ignores = lSystem->GetIgnores();
                isUpdateIgnores = false;
            }

            static char ignore[4];

            for (auto& [ig, _] : ignores)
            {
                ImGui::SameLine();
                ignore[0] = ig;

                if (ImGui::Button(ignore))
                {
                    lSystem->DeleteIgnore(ignore[0]);

                    isUpdateIgnores = true;
                }

                ClearCharArray(4, ignore);
            }

            // Ignore pop up
            if (ImGui::BeginPopup("AddIgnore", NULL))
            {
                ImGui::InputText("Ignore", addIgnore, IM_ARRAYSIZE(addIgnore));

                if (ImGui::Button("Add"))
                {
                    // 첫 글자만 인식
                    lSystem->AddIgnore(addIgnore[0]);

                    // update
                    isUpdateIgnores = true;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Close"))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            // Rules
            static char addKey[64] = "";
            static char addValue[128] = "";
            if (ImGui::Button("Add"))
            {
                ClearCharArray(64, addKey);
                ClearCharArray(128, addValue);
                ImGui::OpenPopup("AddRules");
            }
            
            // add rule pop up
            if (ImGui::BeginPopup("AddRules", NULL))
            {
                ImGui::InputText("key", addKey, IM_ARRAYSIZE(addKey));
                ImGui::InputText("value", addValue, IM_ARRAYSIZE(addValue));

                if (ImGui::Button("Add"))
                {
                    lSystem->AddRule(addKey, addValue);

                    // update
                    isUpdateRules = true;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Close"))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            if (isUpdateRules)
            {
                ruleInfos.clear();
                for (auto& [before, rule] : lSystem->GetRules())
                {
                    for (const LRule::RuleInfo& ruleInfo : rule.GetRuleInfos())
                    {
                        ruleInfos.push_back(ruleInfo);
                    }
                }
                
                isUpdateRules = false;
            }

            static char key[64];
            static char value[128];

            int ruleIndex = 0;      
            for (const LRule::RuleInfo& ruleInfo : ruleInfos)
            {
                // 동일한 key로 button 생성 시 처음 button만 인식 됨
                // ruleIndex: key 로 변경
                int j = 0;
                key[j++] = char(ruleIndex++) + '0';
                key[j++] = ':';
                for (int i = 0; i < ruleInfo.before.size(); i++)
                {
                    key[j] = ruleInfo.before[i];
                    j++;
                }

                if (!ruleInfo.condition.empty())
                {
                    key[j++] = '\n';
                    for (int i = 0; i < ruleInfo.condition.size(); i++)
                    {
                        key[j] = ruleInfo.condition[i];
                        j++;
                    }
                }

                for (int i = 0; i < ruleInfo.after.size(); i++)
                {
                    value[i] = ruleInfo.after[i];
                }

                if (ImGui::Button(key))
                {
                    std::string keyStr(key);
                    keyStr = split(keyStr, '\n')[0];
                    lSystem->DeleteRule(keyStr.substr(2, keyStr.size()), ruleInfo.id);

                    isUpdateRules = true;
                }
                ImGui::SameLine();
                ImGui::Text("%s", value);

                ClearCharArray(64, key);
                ClearCharArray(128, value);
            }
        }

        if (ImGui::CollapsingHeader("Settings"))
        {
            static float distance = 1.0f;
            static float angle = 90.0f;
            static float thickness = 1.0f;
            static float nextThickness = 1.0f;

            if (isUpdateLSystemSetting)
            {
                distance = lSystem->GetDistance();
                angle = lSystem->GetAngleChange();
                thickness = lSystem->GetThickness();
                nextThickness = lSystem->GetDeltaThickness();

                isUpdateLSystemSetting = false;
            }

            if (ImGui::InputFloat("Distance", &distance, 1.0f, 1.0f, "%.3f"))
            {
                lSystem->SetDistance(distance);
            }
            if (ImGui::InputFloat("Angle", &angle, 10.0f, 10.0f, "%.3f"))
            {
                lSystem->SetAngleChange(angle);
            }
            if (ImGui::InputFloat("Thickness", &thickness, 1.0f, 1.0f, "%.3f"))
            {
                lSystem->SetThickness(thickness);
            }
            if (ImGui::InputFloat("Next Thickness", &nextThickness, 1.0f, 1.0f, "%.3f"))
            {
                lSystem->SetDeltaThickness(nextThickness);
            }
        }

        static char newConstant[4] = "";
        static char newValue[4] = "";
        static std::string constant;
        static std::string value;
        static bool constant_edit_window = false;

        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 190, 255, 255));
        ImGui::Text("\n<Constant>");
        ImGui::PopStyleColor();
        ImGui::InputText("constant", newConstant, IM_ARRAYSIZE(newConstant));
        ImGui::InputText("value", newValue, IM_ARRAYSIZE(newValue));

        constant = newConstant;
        value = newValue;

        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1 / 7.0f, 0.5f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(1 / 7.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(1 / 7.0f, 0.9f, 0.9f));
        if (ImGui::Button("Add New"))
        {
            if (newConstant && newValue)
            {
                AddConstant(constant, value);
                constant_edit_window = true;
            }

        }ImGui::PopStyleColor(3);


        if (constant_edit_window)
        {
            PrintConstant(constant, value);
            ImGui::SameLine();
            if (ImGui::Button("Delete"))
            {
                DeleteConstant(constant);
                constant_edit_window = false;
            }
        }


        static bool view_code_window = false;

        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 190, 255, 255));
        ImGui::Text("\n\n<Constant>");
        ImGui::PopStyleColor();
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(3 / 7.0f, 0.3f, 0.3f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(3 / 7.0f, 0.5f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(3 / 7.0f, 0.9f, 0.9f));
        if (ImGui::Button("View current L-System code"))
        {
            view_code_window = true;

        }ImGui::PopStyleColor(3);


        if (view_code_window)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
            ImGui::Text("Rendering with this L-System code.");
            ImGui::PopStyleColor();
            ImGui::SameLine();
            if (ImGui::Button("Close"))
            {
                view_code_window = false;
            }

            static float wrap_width = 200.0f;
            ImGui::SliderFloat("width", &wrap_width, 0, 500, "%.0f");

            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImVec2 marker_min = ImVec2(pos.x + wrap_width, pos.y);
            ImVec2 marker_max = ImVec2(pos.x + wrap_width + 10, pos.y + ImGui::GetTextLineHeight());
            ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrap_width);

            ImGui::Text(word, wrap_width);
            draw_list->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0, 255, 0, 255));
            ImGui::PopTextWrapPos();

        }
        ImGui::End();
#pragma endregion L-System

        const float clear_color_with_alpha[4] = 
        { 
            clear_color.x * clear_color.w,
            clear_color.y * clear_color.w,
            clear_color.z * clear_color.w,
            clear_color.w 
        };

        d3d->GetDeviceContext()->OMSetRenderTargets(1, &renderTargetView, NULL);
        d3d->GetDeviceContext()->ClearRenderTargetView(d3d->GetRenderTargetView(), clear_color_with_alpha);

        input->Frame();

        // TODO: TEMP KEYBOARD INPUT
        int F = 0;
        int R = 0;
        int PU = 0;
        int RR = 0;
        int U = 0;

        if (ImGui::GetCurrentContext()->NavWindow == nullptr)
        {
            // 활성화된 윈도우가 없는 경우에만 카메라 키보드 조작
            input->GetCameraMove(F, R, PU, RR, U);
        }

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

void ClearCharArray(int size, char* out)
{
    for (int i = 0; i < size; i++)
    {
        out[i] = 0;
    }
}

std::vector<std::string> LoadPresetList()
{
    std::vector<std::string> v = std::vector<std::string>();


    for (const auto& file : std::filesystem::recursive_directory_iterator(PATH))
    {
        v.push_back(file.path().string());
    }

    return v;
}

void SavePreset(std::string filename, LSystem* lSystem)
{
    std::ofstream file;

    file.open(PATH + filename + ".txt");

    std::string ok;

    ok = "distance:" + std::to_string(lSystem->GetDistance()) + '\n';
    file.write(ok.c_str(), ok.size());
    ok = "angle:" + std::to_string(lSystem->GetAngleChange()) + '\n';
    file.write(ok.c_str(), ok.size());
    ok = "thickness:" + std::to_string(lSystem->GetThickness()) + '\n';
    file.write(ok.c_str(), ok.size());
    ok = "deltaThickness:" + std::to_string(lSystem->GetDeltaThickness()) + '\n';
    file.write(ok.c_str(), ok.size());
    std::string words = lSystem->GetWord();
    RemoveAll(words, ' ');  // 공백 제거
    ok = "word:" + words + '\n';
    file.write(ok.c_str(), ok.size());
    if (lSystem->GetIgnores().size() > 0)
    {
        ok = "ignore:";
        for (auto& [symbol, _] : lSystem->GetIgnores())
        {
            ok += symbol;
        }
        ok += '\n';
        file.write(ok.c_str(), ok.size());
    }
    ok = "rule\n";
    file.write(ok.c_str(), ok.size());

    bool existRule = false;
    for (auto& [_, rule] : lSystem->GetRules())
    {
        for (LRule::RuleInfo ruleInfo : rule.GetRuleInfos())
        {
            existRule = true;
            std::string before = ruleInfo.before;
            RemoveAll(before, ' '); // 공백 제거

            ok = before;
            if (!ruleInfo.condition.empty())
            {
                // condition이 있는 경우
                RemoveAll(ruleInfo.condition, ' '); // 공백 제거
                ok += ":" + ruleInfo.condition;
            }
            RemoveAll(ruleInfo.after, ' '); // 공백 제거
            ok += "->" + ruleInfo.after + '\n';
            file.write(ok.c_str(), ok.size());
        }
    }

    if (existRule)
    {
        ok = "end";
        file.write(ok.c_str(), ok.size());
    }

    file.close();

    return;
}

unsigned long long timeSinceEpochMiliisec()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void PrintConstant(std::string& constant, std::string& value)
{   

    static int slider_i = std::stoi(value);
    static int sliderRange = slider_i * 2;
    static char outConstant[4];
    strcpy_s(outConstant, constant.c_str());

    ImGui::Text("\n[New Constant]");
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(1 / 7.0f, 0.5f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(1 / 7.0f, 0.6f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(1 / 7.0f, 0.7f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(1 / 7.0f, 0.9f, 0.9f));
    ImGui::SliderInt(" ", &slider_i, 0, sliderRange); // Constant Slider
    ImGui::PopStyleColor(4);

    ImGui::Text(">>> constant : %s ", outConstant);
    ImGui::SameLine();
    ImGui::Text(", value : %d", slider_i);


    if (IsConstant(constant))
    {
        if (GetConstant(constant) != std::to_string(slider_i))
        {
            CONSTANT[constant] = std::to_string(slider_i);
        }
    }

}