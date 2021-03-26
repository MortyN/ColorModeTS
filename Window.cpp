#include "Window.h"

Window* window = nullptr;

Window::Window()
{

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,  WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_CREATE:
	{
		//Event fired when window is created

		window->onCreate();
		break;
	}
	case WM_DESTROY:
	{
		//event fired when window is destroyed
		window->onDestroy();
		::PostQuitMessage(0);
		break;
	}
	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
}

bool Window::init()
{
	WNDCLASSEX wc;
	wc.cbClsExtra = NULL;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = NULL;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = NULL;
	wc.lpszClassName = "MyWindowClass";
	wc.lpszMenuName = "";
	wc.style = NULL;
	wc.lpfnWndProc = &WndProc;


	if (!::RegisterClassEx(&wc)) { //if the registration of calss will fail, the function will return false
		return false;
	}

	if (!window) {
		window = this;
	}

	//creation of the window
	m_hwnd=::CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, "MyWindowClass", "Epic plugin", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768, NULL, NULL, NULL, NULL);

	//if the creation fail return false
	if (!m_hwnd){
		return false;
}
	//show window

	::ShowWindow(m_hwnd, SW_SHOW);
	::UpdateWindow(m_hwnd);

	

	//set this flag to true to indicate that the window is initialized and running
	m_is_run = true;

	return true;
}

bool Window::broadcast()
{
	MSG msg;

	while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	window->onUpdate();

	Sleep(0);

	return false;
}

bool Window::release()
{
	//Destroy the window
	if (!::DestroyWindow(m_hwnd)) {
		return false;
	}
	return true;
}

bool Window::isRun()
{
	return m_is_run;
}

void Window::onDestroy()
{
	m_is_run = false;
}

Window::~Window()
{

}
