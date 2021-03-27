#include "Window.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include "teamspeak/public_errors.h"
#include "teamspeak/public_errors_rare.h"
#include "teamspeak/public_definitions.h"
#include "teamspeak/public_rare_definitions.h"
#include "teamspeak/clientlib_publicdefinitions.h"
#include "ts3_functions.h"
#include "plugin.h"
#include "AppWindow.h"


#define IDI_ICON         101

#define IDC_LISTBOX_TEXT 1000

Window* window = nullptr;
static struct TS3Functions* ts3Functions;
Window::Window()
{

}
void AddControls(HWND);
HWND hWndListBox;
AppWindow app;


/*std::string tsUser[];*/

int userIds[30];



LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,  WPARAM wparam, LPARAM lparam)
{
	/*int arrSize = *(&tsUser + 1) - tsUser;*/
	switch (msg)
	{
	case WM_CREATE:
	{
		//Event fired when window is created
		
		window->onCreate();
		AddControls(hwnd);

		hWndListBox = CreateWindow(

			"LISTBOX",

			NULL,

			WS_VISIBLE | WS_CHILD | LBS_STANDARD | LBS_NOTIFY,

			10,

			10,

			100,

			50,

			hwnd,

			(HMENU)IDC_LISTBOX_TEXT,

			(HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE),

			NULL);

		char buf[30];
		for (int i = 0; userIds[i]; i++)
		{
			sprintf(buf, "%d", userIds[i]);
			SendMessage(GetDlgItem(hwnd, IDC_LISTBOX_TEXT), LB_ADDSTRING, 0, (LPARAM)buf);
		}
		
		break;
	}

	case WM_COMMAND:

	{

		switch (LOWORD(wparam))

		{

		case IDC_LISTBOX_TEXT:

		{

			switch (HIWORD(wparam))

			{

			case LBN_SELCHANGE:

			{
				//sets the window title as the selected item
				char Buffer[256];

				int index = SendMessage((HWND)lparam, LB_GETCARETINDEX, 0, 0);
				
				SendMessage((HWND)lparam, LB_GETTEXT, (LPARAM)index, (WPARAM)Buffer);

				SetWindowText(hwnd, Buffer);

			}

			}

		}

		break;

		}

		return 0;

	}


		break;


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

int Window::gettext(int ids[])
{

	for (int i = 0; ids[i]; i++)
	{
		userIds[i] = ids[i];
		printf("WINDOW ID!!: %d \n", ids[i]);
		printf("ADDED TO LIST: %d \n", userIds[i]);
	}

	if (app.init())
	{

		while (app.isRun()) {
			app.broadcast();
		}
	}
	printf("RETURNED NANI!??");
	return 0;
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

/*void AddListBoxItems(HWND hWnd) {
	for (int i = 0; userIds[i]; i++)
	{
		SendMessage(GetDlgItem(hWnd, IDC_LISTBOX_TEXT), LB_ADDSTRING, 0, (LPARAM)userIds[i]);
	}
}*/

void AddControls(HWND hWnd)
{

	CreateWindowW(L"Button", L"Click Me", WS_VISIBLE | WS_CHILD, 100, 100, 100, 50, hWnd, NULL, NULL, NULL);
	
}
