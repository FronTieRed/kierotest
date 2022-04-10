#include "includes.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

void SetupConsole(){
    AllocConsole();

    FILE* pConFile;
    freopen_s(&pConFile, "CONIN$", "r", stdin);
    freopen_s(&pConFile, "CONOUT$", "w", stdout);
    freopen_s(&pConFile, "CONOUT$", "w", stderr);

    SetConsoleTitleA("WarHook");
}

void InitImGui(){
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool init = false;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags){
    if(!init){
        if(SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**) &pDevice))){
            pDevice->GetImmediateContext(&pContext);
            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            window = sd.OutputWindow;
            ID3D11Texture2D* pBackBuffer;
            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &pBackBuffer);
            pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
            pBackBuffer->Release();
            oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
            InitImGui();
            init = true;
        }else{
            return oPresent(pSwapChain, SyncInterval, Flags);
        }
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    
    ImGui::Begin("ImGui test window");
    ImGui::Text("This is some useful text.");
    ImGui::TextColored(ImVec4(255,0,0,0), "This text is colored!");
    ImGui::End();

    ImGui::Render();

    pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    return oPresent(pSwapChain, SyncInterval, Flags);
}

DWORD WINAPI CheatThread(){
    bool init_hook = false;

    do
    {
        SetupConsole();

        if (kiero::init(kiero::RenderType::Auto) == kiero::Status::Success)
        {
            kiero::bind(8, (void**)& oPresent, hkPresent);
            init_hook = true;
        }
    } while (!init_hook);
    return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID)
{
    if(dwReason = DLL_PROCESS_ATTACH){
        DisableThreadLibraryCalls(hMod);
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)CheatThread, hMod, 0, nullptr);
    }

    if(dwReason = DLL_PROCESS_DETACH){
        kiero::shutdown();
    }
    return TRUE;
}