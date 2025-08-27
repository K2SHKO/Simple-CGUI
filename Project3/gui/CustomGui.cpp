#include "CustomGui.h"
#include <thread>

void CGui::DrawRoundedRect(HDC hdc, RECT& rc, COLORREF color, int radius)
{
    HBRUSH brush = CreateSolidBrush(color);
    HPEN pen = CreatePen(PS_SOLID, 1, color);
    HRGN rgn = CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom, radius, radius);
    SelectObject(hdc, brush);
    FillRgn(hdc, rgn, brush);
    DeleteObject(brush);
    DeleteObject(pen);
    DeleteObject((HGDIOBJ)rgn);
}

void CGui::DrawTextCentered(HDC hdc, RECT& rc, LPCWSTR text, HFONT font, COLORREF color)
{
    SetTextColor(hdc, color);
    SetBkMode(hdc, TRANSPARENT);
    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    DrawTextW(hdc, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, oldFont);
}

void CGui::SwitchToWelcomeView(HWND hwnd)
{
    //isTransitioning = true;
    transitionProgress = 0.0f;

    /*
        It's not optimized version of it but it's working. 
        And it can cause weird switches. 
    */

    std::thread([this, hwnd]()
        {
            const int steps = 30;
            const int delay = 10;
            for (int i = 0; i <= steps; ++i)
            {
                transitionProgress = i / (float)steps;
                InvalidateRect(hwnd, NULL, TRUE);
                Sleep(delay);
            }

            currentView = AppView::Welcome;
            //isTransitioning = false;

            ShowWindow(hwndLogin, SW_HIDE);
            ShowWindow(hwndPassword, SW_HIDE);
            ShowWindow(hwndLabelLogin, SW_HIDE);
            ShowWindow(hwndLabelPassword, SW_HIDE);
            ShowWindow(hwndButton, SW_HIDE);

            InvalidateRect(hwnd, NULL, TRUE);
        }).detach();
}

LRESULT CALLBACK CGui::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        hwndLabelLogin = CreateWindowW(L"STATIC", L"Login",
            WS_CHILD | WS_VISIBLE | SS_LEFT, 0, 0, 200, 22, hwnd, NULL, NULL, NULL);
        hwndLogin = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 0, 0, 200, 28, hwnd, (HMENU)2, NULL, NULL);
        hwndLabelPassword = CreateWindowW(L"STATIC", L"Password",
            WS_CHILD | WS_VISIBLE | SS_LEFT, 0, 0, 200, 22, hwnd, NULL, NULL, NULL);
        hwndPassword = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_PASSWORD, 0, 0, 200, 28, hwnd, (HMENU)3, NULL, NULL);
        hwndButton = CreateWindowExW(0, L"BUTTON", L"Login",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 0, 0, 120, 40, hwnd, (HMENU)1, NULL, NULL);

        hFontTitle = CreateFontW(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            VARIABLE_PITCH, L"Segoe UI");
        hFontLabels = CreateFontW(13, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            VARIABLE_PITCH, L"Segoe UI");
        hFontInputs = CreateFontW(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            VARIABLE_PITCH, L"Segoe UI");
        hFontButton = CreateFontW(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            VARIABLE_PITCH, L"Verdana");

        SendMessage(hwndLabelLogin, WM_SETFONT, (WPARAM)hFontLabels, TRUE);
        SendMessage(hwndLogin, WM_SETFONT, (WPARAM)hFontInputs, TRUE);
        SendMessage(hwndLabelPassword, WM_SETFONT, (WPARAM)hFontLabels, TRUE);
        SendMessage(hwndPassword, WM_SETFONT, (WPARAM)hFontInputs, TRUE);
        SendMessage(hwndButton, WM_SETFONT, (WPARAM)hFontButton, TRUE);

        return 0;

    case WM_SIZE:
    {
        RECT rcClient;
        GetClientRect(hwnd, &rcClient);
        /*
            Variables
        */
        int centerX = rcClient.right / 2;
        int labelHeight = 22;
        int inputHeight = 28;
        int spacing = 8;
        int loginLabelY = rcClient.bottom / 2 - 90;
        int loginY = loginLabelY + labelHeight + spacing;
        int passwordLabelY = loginY + inputHeight + 24;
        int passwordY = passwordLabelY + labelHeight + spacing;
        int btnWidth = 120;
        int btnHeight = 40;
        int btnX = centerX - btnWidth / 2;
        int btnY = passwordY + inputHeight + 30;
        int inputWidth = 240;
        int labelWidth = inputWidth;

        MoveWindow(hwndLabelLogin, centerX - labelWidth / 2, loginLabelY, labelWidth, labelHeight, TRUE);
        MoveWindow(hwndLogin, centerX - inputWidth / 2, loginY, inputWidth, inputHeight, TRUE);
        MoveWindow(hwndLabelPassword, centerX - labelWidth / 2, passwordLabelY, labelWidth, labelHeight, TRUE);
        MoveWindow(hwndPassword, centerX - inputWidth / 2, passwordY, inputWidth, inputHeight, TRUE);
        MoveWindow(hwndButton, btnX, btnY, btnWidth, btnHeight, TRUE);

        closeBtnRect = { rcClient.right - 44, 0, rcClient.right, TITLEBAR_HEIGHT };
        minBtnRect = { rcClient.right - 88, 0, rcClient.right - 44, TITLEBAR_HEIGHT };

        HRGN rgn = CreateRoundRectRgn(0, 0, rcClient.right, rcClient.bottom, 28, 28);
        SetWindowRgn(hwnd, rgn, TRUE);

        return 0;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == 1)
        {
            GetWindowTextW(hwndLogin, m_login, 128);
            GetWindowTextW(hwndPassword, m_password, 128);
			CGui::SwitchToWelcomeView(hwnd);
        }

        return 0;

    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;
        if (pdis->CtlID == 1)
        {
            bool pressed = (pdis->itemState & ODS_SELECTED) != 0;
            HDC hdc = pdis->hDC;
            RECT rc = pdis->rcItem;
            const int radius = 12;
            COLORREF bgColor = pressed ? clrAccentDark : clrAccent;
            COLORREF borderColor = pressed ? RGB(10, 70, 140) : RGB(30, 120, 210);
            COLORREF textColor = RGB(255, 255, 255);
            HRGN rgn = CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom, radius, radius);
            HBRUSH brush = CreateSolidBrush(bgColor);
            HBRUSH borderBrush = CreateSolidBrush(borderColor);
            FillRgn(hdc, rgn, brush);
            FrameRgn(hdc, rgn, borderBrush, 2, 2);
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, textColor);
            SelectObject(hdc, hFontButton);
            wchar_t text[256];
            GetWindowTextW(pdis->hwndItem, text, 256);
            DrawTextW(hdc, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            DeleteObject(brush);
            DeleteObject(borderBrush);
            DeleteObject(rgn);
            return TRUE;
        }
        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rcClient;
        GetClientRect(hwnd, &rcClient);

        TRIVERTEX vert[2] = {
            {0, 0, GetRValue(clrBgStart) << 8, GetGValue(clrBgStart) << 8, GetBValue(clrBgStart) << 8, 0xFF00},
            {rcClient.right, rcClient.bottom, GetRValue(clrBgEnd) << 8, GetGValue(clrBgEnd) << 8, GetBValue(clrBgEnd) << 8, 0xFF00}
        };
        GRADIENT_RECT gRect = { 0, 1 };
        GradientFill(hdc, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V);

        int gridSpacing = 20;
        HPEN darkPen1 = CreatePen(PS_SOLID, 1, RGB(20, 20, 30));
        HPEN darkPen2 = CreatePen(PS_SOLID, 1, RGB(15, 15, 25));
        HPEN oldPen = (HPEN)SelectObject(hdc, darkPen1);

        for (int x = 0; x < rcClient.right; x += gridSpacing)
        {
            SelectObject(hdc, darkPen1); MoveToEx(hdc, x, 0, NULL); LineTo(hdc, x, rcClient.bottom);
            SelectObject(hdc, darkPen2); MoveToEx(hdc, x + 1, 0, NULL); LineTo(hdc, x + 1, rcClient.bottom);
        }

        for (int y = TITLEBAR_HEIGHT; y < rcClient.bottom; y += gridSpacing)
        {
            SelectObject(hdc, darkPen1); MoveToEx(hdc, 0, y, NULL); LineTo(hdc, rcClient.right, y);
            SelectObject(hdc, darkPen2); MoveToEx(hdc, 0, y + 1, NULL); LineTo(hdc, rcClient.right, y + 1);
        }

        SelectObject(hdc, oldPen);
        DeleteObject(darkPen1);
        DeleteObject(darkPen2);

        RECT rcTitleBar = { 0, 0, rcClient.right, TITLEBAR_HEIGHT };
        HBRUSH titleBarBrush = CreateSolidBrush(RGB(50, 50, 70));
        FillRect(hdc, &rcTitleBar, titleBarBrush);
        DeleteObject(titleBarBrush);
        DrawTextCentered(hdc, rcTitleBar, TITLE, hFontTitle, RGB(230, 230, 230));

        HBRUSH btnBrush = CreateSolidBrush(isMinBtnPressed ? RGB(80, 80, 110) : RGB(70, 70, 100));
        FillRect(hdc, &minBtnRect, btnBrush);
        DeleteObject(btnBrush);
        DrawTextCentered(hdc, minBtnRect, L"\u2013", hFontTitle, RGB(220, 220, 220));

        HBRUSH closeBrush = CreateSolidBrush(isCloseBtnPressed ? RGB(200, 60, 60) : RGB(180, 40, 40));
        FillRect(hdc, &closeBtnRect, closeBrush);
        DeleteObject(closeBrush);
        DrawTextCentered(hdc, closeBtnRect, L"X", hFontTitle, RGB(255, 220, 220));

        if (currentView == AppView::Welcome || isTransitioning)
        {
            RECT rc = rcClient;

            RECT welcomeRect = { 0, 20, rcClient.right, 80 };
            HFONT hWelcomeFont = CreateFontW(28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                VARIABLE_PITCH, L"Segoe UI");
            HFONT oldFont = (HFONT)SelectObject(hdc, hWelcomeFont);
            SetTextColor(hdc, RGB(220, 255, 220));
            SetBkMode(hdc, TRANSPARENT);
            DrawTextW(hdc, L"Welcome!", -1, &welcomeRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            SelectObject(hdc, oldFont);
            DeleteObject(hWelcomeFont);

            int panelWidth = 280;
            int panelHeight = 80;
            int panelX = (rcClient.right - panelWidth) / 2;
            int panelY = rcClient.bottom / 2 - 100;

            productRect = { panelX, panelY, panelX + panelWidth, panelY + panelHeight };

            const int cornerRadius = 10;

            HRGN panelRgn = CreateRoundRectRgn(
                productRect.left, productRect.top,
                productRect.right, productRect.bottom,
                cornerRadius, cornerRadius
            );

            HBRUSH panelBrush = CreateSolidBrush(isProductSelected ? RGB(45, 90, 130) : RGB(35, 65, 100));
            FillRgn(hdc, panelRgn, panelBrush);

            HBRUSH borderBrush = CreateSolidBrush(RGB(60, 120, 160));
            FrameRgn(hdc, panelRgn, borderBrush, 1, 1);
            DeleteObject(borderBrush);
            DeleteObject(panelBrush);
            DeleteObject(panelRgn);

            HFONT hProductFont = CreateFontW(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                VARIABLE_PITCH, L"Segoe UI"/*L"Verdana"*/);
            SelectObject(hdc, hProductFont);
            SetTextColor(hdc, RGB(240, 240, 255));
            SetBkMode(hdc, TRANSPARENT);

            RECT nameRect = { panelX + 12, panelY + 10, panelX + panelWidth - 12, panelY + 30 };
            DrawTextW(hdc, L"Notepad.exe", -1, &nameRect, DT_LEFT | DT_SINGLELINE);

            HFONT hDescFont = CreateFontW(13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                VARIABLE_PITCH, L"Segoe UI");
            SelectObject(hdc, hDescFont);
            SetTextColor(hdc, RGB(200, 200, 220));

            RECT descRect = { panelX + 12, panelY + 30, panelX + panelWidth - 12, panelY + panelHeight - 8 };
            DrawTextW(hdc, L"Product1\nAwesomeProduct", -1, &descRect, DT_LEFT | DT_WORDBREAK);

            DeleteObject(hProductFont);
            DeleteObject(hDescFont);

            int btnWidth = 120;
            int btnHeight = 36;
            int btnX = (rcClient.right - btnWidth) / 2;
            int btnY = productRect.bottom + 40;

            injectBtnRect = { btnX, btnY, btnX + btnWidth, btnY + btnHeight };

            if (isProductSelected)
            {
                HBRUSH btnBrush = CreateSolidBrush(RGB(60, 120, 80));
                FillRect(hdc, &injectBtnRect, btnBrush);
                DeleteObject(btnBrush);

                HFONT hBtnFont = CreateFontW(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                    VARIABLE_PITCH, L"Segoe UI");
                SelectObject(hdc, hBtnFont);
                SetTextColor(hdc, RGB(240, 255, 240));

                const wchar_t* label = isInjecting ? L"Starting..." : L"Loaded";
                DrawTextW(hdc, label, -1, &injectBtnRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                DeleteObject(hBtnFont);
            }

            if (isInjecting)
				MessageBoxA(0, "Loaded!!", "", 0); // When the thing is getting loaded;
        }

        RECT statusRect = { 16, rcClient.bottom - 28, 260, rcClient.bottom - 8 };
        DrawText(hdc, L"Status: Online", -1, &statusRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        RECT versionRect = { rcClient.right - 70, rcClient.bottom - 28, rcClient.right - 16, rcClient.bottom - 8 };
        DrawText(hdc, L"v1.4", -1, &versionRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_LBUTTONDOWN:
    {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        if (PtInRect(&closeBtnRect, pt))
        {
            isCloseBtnPressed = true;
            InvalidateRect(hwnd, &closeBtnRect, TRUE);
        }
        else if (PtInRect(&minBtnRect, pt))
        {
            isMinBtnPressed = true;
            InvalidateRect(hwnd, &minBtnRect, TRUE);
        }
        else if (pt.y < TITLEBAR_HEIGHT)
        {
            isDragging = true;
            dragStartPoint = pt;
            SetCapture(hwnd);
        }
        else if (currentView == AppView::Welcome)
        {
            if (PtInRect(&productRect, pt))
            {
                isProductSelected = true;
                InvalidateRect(hwnd, NULL, TRUE);
            }
            else if (isProductSelected && PtInRect(&injectBtnRect, pt) && !isInjecting)
            {
                isInjecting = true;
                InvalidateRect(hwnd, NULL, TRUE);

                std::thread([this, hwnd]()
                    {
                        Sleep(2000);
                        isInjecting = false;
                        InvalidateRect(hwnd, NULL, TRUE);
                    }).detach();
            }
        }
        return 0;
    }
    case WM_LBUTTONUP:
    {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };

        if (isCloseBtnPressed)
        {
            isCloseBtnPressed = false;
            InvalidateRect(hwnd, &closeBtnRect, TRUE);
            if (PtInRect(&closeBtnRect, pt))
                PostMessage(hwnd, WM_CLOSE, 0, 0);
        }
        else if (isMinBtnPressed)
        {
            isMinBtnPressed = false;
            InvalidateRect(hwnd, &minBtnRect, TRUE);
            if (PtInRect(&minBtnRect, pt))
                ShowWindow(hwnd, SW_MINIMIZE);
        }

        if (isDragging)
        {
            isDragging = false;
            ReleaseCapture();
        }

        return 0;
    }
    case WM_MOUSEMOVE:
        if (isDragging)
        {
            POINT ptScreen;
            GetCursorPos(&ptScreen);
            RECT rcWindow;
            GetWindowRect(hwnd, &rcWindow);
            int dx = ptScreen.x - (rcWindow.left + dragStartPoint.x);
            int dy = ptScreen.y - (rcWindow.top + dragStartPoint.y);
            SetWindowPos(hwnd, NULL, rcWindow.left + dx, rcWindow.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }
        return 0;
    case WM_DESTROY:
        DeleteObject(hFontTitle);
        DeleteObject(hFontLabels);
        DeleteObject(hFontInputs);
        DeleteObject(hFontButton);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void CGui::AnimateWindowOpen(HWND hwnd)
{
    RECT rc;
    GetWindowRect(hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    
    /*
        It just makes it lagged (it's opening with longer delay);
    */
    
    for (int i = 1; i <= 30; i++)
    {
#ifdef DEBUG
		printf("Step %d%%\n", (i * 100) / 30);
#endif
#ifdef ANIMATION
        float progress = (float)i / 30;
        int w = (int)(width * progress);
        int h = (int)(height * progress);
        int x = rc.left + (width - w) / 2;
        int y = rc.top + (height - h) / 2;

        SetWindowPos(hwnd, NULL, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
        Sleep(300 / 30);
#endif
    }
    SetWindowPos(hwnd, NULL, rc.left, rc.top, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
}