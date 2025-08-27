#pragma once
#include <windows.h>
#include <commctrl.h>
#include <string>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Msimg32.lib")

//#define DEBUG
#define ANIMATION

class CGui {
public:
	void DrawTextCentered(HDC hdc, RECT& rc, LPCWSTR text, HFONT font, COLORREF color);
	void DrawRoundedRect(HDC hdc, RECT& rc, COLORREF color, int radius);
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void AnimateWindowOpen(HWND hwnd);
	void SwitchToWelcomeView(HWND hwnd);

	static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		CGui* pThis = nullptr;
		if (msg == WM_NCCREATE) {
			CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
			pThis = (CGui*)pCreate->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
		}
		else {
			pThis = (CGui*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		}
		if (pThis) {
			return pThis->WindowProc(hwnd, msg, wParam, lParam);
		}
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
private:

	LPCWSTR TITLE = L"Simple GUI";
	const wchar_t CLASS_NAME[16] = L"Class";
	HWND hwndButton, hwndLogin, hwndPassword, hwndLabelLogin, hwndLabelPassword;
	HFONT hFontTitle, hFontLabels, hFontInputs, hFontButton;

	RECT closeBtnRect, minBtnRect;
	bool isCloseBtnPressed = false;
	bool isMinBtnPressed = false;
	bool isDragging = false;
	POINT dragStartPoint;
	const int TITLEBAR_HEIGHT = 28;

	COLORREF clrBgStart = RGB(10, 10, 20);
	COLORREF clrBgEnd = RGB(18, 18, 28);
	COLORREF clrAccent = RGB(0, 120, 215);
	COLORREF clrAccentDark = RGB(0, 90, 165);
	
	enum class AppView //Pages are located here
	{
		Login,
		Welcome,
		Test
	};
	
	AppView currentView = AppView::Login;
	float transitionProgress = 0.0f;
	bool isTransitioning = false; // not used anymore
	bool isProductSelected = false;

	bool isInjecting = false;
	RECT productRect = {};
	RECT injectBtnRect = {};

	wchar_t m_login[128] = L"";
	wchar_t m_password[128] = L"";
};