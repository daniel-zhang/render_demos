#ifndef WINDOW_BASE_H
#define WINDOW_BASE_H

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>

// Base class that wraps the nasty details to create a window
template <typename Derivative>
class WindowBase
{
protected:
	HWND mHwnd;

public:
	WindowBase():mHwnd(NULL){}
	virtual ~WindowBase(){}
	BOOL create(
		PCWSTR lpWindowName,
		DWORD dwStyle,
		DWORD dwExStyle = 0,			// We can achieve double-buffering by setting this
		int x = CW_USEDEFAULT,			// Window position default
		int y = CW_USEDEFAULT,			// Window position default
		int nWidth = CW_USEDEFAULT,		// Window position default
		int nHeight = CW_USEDEFAULT,	// Window position default
		HWND hWndParent = 0,
		HMENU hMenu = 0,
		HCURSOR hCursor = NULL)
	{
		// Register the window class
		WNDCLASS wc = {0};
		wc.lpfnWndProc   = Derivative::winProc;
		wc.hInstance     = GetModuleHandle(NULL);	// Get the current handle(in a single-threaded environment)

		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.lpszClassName = getClassName();
		wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor       = LoadCursor(0, IDC_ARROW);
		wc.lpszMenuName  = 0;
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);// Set default background as white.

		RegisterClass(&wc);

		CreateWindowEx(
			dwExStyle, getClassName(), lpWindowName, dwStyle, x, y,
			nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), this);

		return (mHwnd ? TRUE : FALSE);
	}

	static LRESULT CALLBACK winProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		Derivative* pThis = NULL;
		if (uMsg == WM_NCCREATE)
		{
			//Extract "this" pointer. 
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			pThis = reinterpret_cast<Derivative*>(pCreate->lpCreateParams);

			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

			//mHwnd is better assigned here, so our handleMsg() can process WM_NCCREATE
			//and WM_CREATE messages.
			pThis->mHwnd = hwnd;
		}
		else
		{
			pThis = (Derivative*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		}

		if (pThis)
		{
			return pThis->handleMsg(uMsg, wParam, lParam);
		}
		else
		{
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}

	virtual LRESULT  handleMsg( UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
	virtual PCWSTR  getClassName() const = 0;
	virtual PCWSTR  getWindowCaption() const = 0;
};

#endif