#include <windows.h>
#include <commctrl.h>
#include "ironhack.h"

DWORD Hack(TCHAR * title)
{
	HWND hWnd;
	DWORD pid, code;
	HANDLE proc;
	
	hWnd = FindWindow(NULL, title);
	if (hWnd == NULL)
		return FALSE;
	GetWindowThreadProcessId(hWnd, &pid);
	proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (proc == NULL)
		return FALSE;
	MessageBox(hWnd, "Injected", title, 0);
	while (GetExitCodeProcess(proc, &code)) {
		if (code != STILL_ACTIVE)
			break;
		if (GetKeyState(VK_LBUTTON) & 0x100);
	}
	
	return TRUE;
}

BOOL CALLBACK SetFont(HWND child, LPARAM font)
{
	SendMessage(child, WM_SETFONT, font, TRUE);
	
	return TRUE;
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, HWND combobox)
{
	int length;
	
	length = GetWindowTextLength(hWnd) + 1;
	
	TCHAR title[length];
	
	GetWindowText(hWnd, title, length);
	if (!IsWindowVisible(hWnd) || length - 1 == 0)
		return TRUE;
	SendMessage(combobox, CB_ADDSTRING, 0, title);
	
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	static HWND button;
	static HWND combobox;
	DWORD i, code;
	HANDLE thread;
	TCHAR title[1024];
	
	switch (Msg) {
		case WM_CREATE:
			button   = CreateWindowEx(0, WC_BUTTON, "Inject", WS_VISIBLE | WS_CHILD, 60, 125, 80, 20, hWnd, ID_HACK, NULL, NULL);
			combobox = CreateWindowEx(0, WC_COMBOBOX, NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST, 20, 95, 160, 20, hWnd, NULL, NULL, NULL);
			EnumChildWindows(hWnd, SetFont, GetStockObject(DEFAULT_GUI_FONT));
			EnumWindows(EnumWindowsProc, combobox);
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case ID_HACK:
					i = SendMessage(combobox, CB_GETCURSEL, 0, 0);
					SendMessage(combobox, CB_GETLBTEXT, i, title);
					thread = CreateThread(NULL, 0, &Hack, title, 0, NULL);
					Sleep(1000);
					GetExitCodeThread(thread, &code);
					if (code == 0)
						MessageBox(0, "Failed", title, MB_ICONERROR);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
	
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	HWND hWnd;
	WNDCLASSEX wndClass;
	MSG Msg;
	
	ZeroMemory(&wndClass, sizeof(wndClass));
	
	wndClass.cbSize        = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc   = WndProc;
	wndClass.hInstance     = hInstance;
	wndClass.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(ICON));
	wndClass.hIconSm       = LoadIcon(hInstance, MAKEINTRESOURCE(ICON));
	wndClass.hbrBackground = CreatePatternBrush(LoadBitmap(hInstance, MAKEINTRESOURCE(BACKGROUND)));
	wndClass.lpszClassName = "Window Class";
	
	RegisterClassEx(&wndClass);
	
	hWnd = CreateWindowEx(0, "Window Class", "ironhack", WS_VISIBLE | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 200, 210, NULL, NULL, hInstance, NULL);
	
	PlaySound(MAKEINTRESOURCE(LULLABY), hInstance, SND_RESOURCE | SND_LOOP | SND_ASYNC);
	
	while (GetMessage(&Msg, NULL, 0, 0)) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	
	return 0;
}
