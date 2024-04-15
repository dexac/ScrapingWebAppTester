
#include <windows.h>
#include <winuser.h>
#include <tchar.h>
#include <WebView2.h>
#include <stdlib.h>
#include <string>
#include <wrl.h>
#include <wil/com.h>

#define MyWindow _declspec(dllexport)
using namespace Microsoft::WRL;

extern "C" {
	static wil::com_ptr<ICoreWebView2> webviewWindow;
	static wil::com_ptr<ICoreWebView2Controller> webviewController;

	static TCHAR szWindowClass[] = _T("DesktopApp");
	static TCHAR szTitle[] = _T("Test sample");

	HINSTANCE hInst;

	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	MyWindow HWND CreateWind(HWND hParent) {

		HINSTANCE hInstance = GetModuleHandle(NULL);

		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = szWindowClass;
		wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

		if (!RegisterClassEx(&wcex))
		{
			MessageBox(NULL,
				_T("Call to RegisterClassEx failed!"),
				_T("Windows Desktop Guided Tour"),
				NULL);

			return hParent;
		}

		// Store instance handle in our global variable
		hInst = hInstance;

		HWND hWnd = CreateWindow(
			szWindowClass,
			szTitle,
			WS_POPUP,
			CW_USEDEFAULT, CW_USEDEFAULT,
			1200, 900,
			NULL,
			NULL,
			hInstance,
			NULL
		);

		if (!hWnd)
		{
			MessageBox(NULL,
				_T("Call to CreateWindow failed!"),
				_T("Windows Desktop Guided Tour"),
				NULL);

			return hParent;
		}

		ShowWindow(hWnd,
			SW_SHOW);
		UpdateWindow(hWnd);

		HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr,
			Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
				[hWnd](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {

					env->CreateCoreWebView2Controller(hWnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
						[hWnd](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
							if (controller != nullptr) {
								webviewController = controller;
								webviewController->get_CoreWebView2(&webviewWindow);
							}

							ICoreWebView2Settings* Settings;
							webviewWindow->get_Settings(&Settings);
							Settings->put_IsScriptEnabled(TRUE);
							Settings->put_AreDefaultScriptDialogsEnabled(TRUE);
							Settings->put_IsWebMessageEnabled(TRUE);

							RECT bounds;
							GetClientRect(hWnd, &bounds);
							webviewController->put_Bounds(bounds);

							webviewWindow->Navigate(L"https://www.google.com/");

							webviewWindow->OpenDevToolsWindow();

							return S_OK;
						}).Get());
					return S_OK;
				}).Get());

	 

		return hWnd;
	}	

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		TCHAR greeting[] = _T("Windows desktop!");

		switch (message)
		{
		case WM_SIZE:
			if (webviewController != nullptr) {
				RECT bounds;
				GetClientRect(hWnd, &bounds);
				webviewController->put_Bounds(bounds);
			};
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		}

		return 0;
	}
}
