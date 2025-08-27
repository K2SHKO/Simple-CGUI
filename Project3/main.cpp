#include "gui/CustomGui.h"

#define SizeX 400
#define SizeY 400

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    INITCOMMONCONTROLSEX icex = { sizeof(icex), ICC_STANDARD_CLASSES };
    InitCommonControlsEx(&icex);
    CGui gui;
    WNDCLASSW wc = {};
    wc.lpfnWndProc = CGui::StaticWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"Menu";
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(0, L"Menu", L"", WS_POPUP,
        CW_USEDEFAULT, CW_USEDEFAULT, SizeX, SizeY,
        NULL, NULL, hInstance, &gui);
        //NULL, NULL, wc.hInstance, gui);
    if (!hwnd)
        return 0;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN); //1920
    int screenHeight = GetSystemMetrics(SM_CYSCREEN); //1080
    int x = (screenWidth - SizeX) / 2;
    int y = (screenHeight - SizeY) / 2;
    SetWindowPos(hwnd, NULL, x, y, SizeX, SizeY, SWP_NOZORDER | SWP_NOACTIVATE);

    ShowWindow(hwnd, SW_HIDE);
    gui.AnimateWindowOpen(hwnd);
    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}