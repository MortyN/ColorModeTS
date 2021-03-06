#include "Window.h"
#include <commctrl.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include "teamspeak/public_errors.h"
#include "teamspeak/public_errors_rare.h"
#include "teamspeak/public_definitions.h"
#include "teamspeak/public_rare_definitions.h"
#include "teamspeak/clientlib_publicdefinitions.h"
#include "ts3_functions.h"
#include "plugin.h"
#include "UserObj.h"
#include "AppWindow.h"

//defining the wparam values for the msg switch-case, used to see what is clicked in the mainwindow
#define IDI_ICON 101
#define IDC_LISTBOX_TEXT 1000
#define IDC_CHECKBOX_POKETALK 1001
#define IDC_LISTBOXACTIVE_TEXT 1002
#define IDC_BUTTON_ADDLIST 1003
#define IDC_BUTTON_DELETELIST 1004
#define IDC_POKEINPUT 1005
#define IDC_BUTTON_APPLYCUSTOMTEXT 1006
#define IDC_GRPFUNCTIONS 1007
#define IDC_LISTBOX_LOGGING 1008

//functionality for the application, numbers for switch case ease
#define FUNC_POKETALK 3000

Window* window = nullptr;
Window::Window()
{

}
void AddControls(HWND);
void UpdateList(HWND);

HWND rightHwnd;
HWND hWndListBox;	
AppWindow app;

plugin* ts3plugin;

int lastUser;
bool isWinRun = false;
char Buffer[256];
char pokeTextBuf[256];
bool pokeActivated = false;
int selUserAmount = 0;

struct UserObj winUserList[50];
struct UserObj selUserList[50];
struct UserObj emptylist[50];


//windows visual styling, does not include fonts
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

//Initializes ui font on all items
BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam)
{
	HFONT hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(hWnd, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(TRUE, 0));
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,  WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	//Event fired when window is created
	case WM_CREATE:
	{
		window->onCreate();
		AddControls(hwnd);

		//adds clientnames to listbox window
		for (int i = 0; i < lastUser ; i++)
		{
			SendMessage(GetDlgItem(hwnd, IDC_LISTBOX_TEXT), LB_ADDSTRING, 0, (LPARAM)winUserList[i].username);
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
						
						int index = SendMessage((HWND)lparam, LB_GETCARETINDEX, 0, 0);	
						SendMessage((HWND)lparam, LB_GETTEXT, (LPARAM)index, (WPARAM)Buffer);
						//SetWindowText(hwnd, Buffer);
						break;
						
					}
					
				}
				break;
			}
			case IDC_CHECKBOX_POKETALK: 
			{
				//visually activates checkbox mark
				BOOL checked = IsDlgButtonChecked(hwnd, IDC_CHECKBOX_POKETALK);
				if (checked) {
					pokeActivated = false;
					CheckDlgButton(hwnd, IDC_CHECKBOX_POKETALK, BST_UNCHECKED);
					ts3plugin->funcIsActivated(pokeActivated, FUNC_POKETALK);
					EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_APPLYCUSTOMTEXT), false);
					EnableWindow(GetDlgItem(hwnd, IDC_POKEINPUT), false);
					::UpdateWindow(hwnd);
				}
				else {
					pokeActivated = true;
					ts3plugin->funcIsActivated(pokeActivated, FUNC_POKETALK);
					CheckDlgButton(hwnd, IDC_CHECKBOX_POKETALK, BST_CHECKED);
					EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_APPLYCUSTOMTEXT), true);
					EnableWindow(GetDlgItem(hwnd, IDC_POKEINPUT), true);
					::UpdateWindow(hwnd);

				}
				break;
			}

			case IDC_BUTTON_APPLYCUSTOMTEXT:
			{
				char customtext[256];
				GetWindowText(GetDlgItem(hwnd, IDC_POKEINPUT), customtext, 256);
				ts3plugin->pokeText(customtext);
				break;
			}

			case IDC_BUTTON_ADDLIST:
			{
				if (Buffer[0] != 0) {

					SendMessage(GetDlgItem(hwnd, IDC_LISTBOXACTIVE_TEXT), LB_ADDSTRING, 0, (LPARAM)Buffer);
					

					for (int i = 0; i < lastUser; i++)
					{
						if (strcmp(winUserList[i].username, Buffer) == 0) {
							selUserList[selUserAmount] = winUserList[i];
							ts3plugin->userlistActions(selUserList, selUserAmount);
							break;
						}							
					}
					Buffer[0] = '\0';
					selUserAmount++;
					::UpdateWindow(hwnd);
				}	
				break;
			}
			case IDC_BUTTON_DELETELIST:
			{
				
				SendMessage(GetDlgItem(hwnd, IDC_LISTBOXACTIVE_TEXT), LB_RESETCONTENT, 0, 0);
				ts3plugin->userlistActions(emptylist, -1);
				selUserAmount = 0;
				::UpdateWindow(hWndListBox);
				break;
			}


		break;
		}
		return 0;
	}
	break;

	case WM_CLOSE:
		ShowWindow(hwnd, SW_HIDE);
		break;

	/*case WM_DESTROY:
	{

		//event fired when window is destroyed
		window->onDestroy();
		::PostQuitMessage(0);
		break;
	}*/
	default:
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
}

int Window::getUserDetails(UserObj client[], int lastvalue)
{
	lastUser = lastvalue;
	ShowWindow(rightHwnd, SW_SHOW);
	for (int i = 0; i < lastUser; i++)
	{
		winUserList[i] = client[i];
		printf("UserListITEM!: %s\n", winUserList[i].username); //removing this makes it not work???
	}
	//starts the annoyinator (application window)
	if (app.init())
	{
		while (app.isRun()) {
			app.broadcast();
		}
	}
	return 0;
}

std::string curTime()
{
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), " %X : ");
	return ss.str();
}

int Window::logUserPoked(char *clientName)
{
	std::string logmessage;
	std::string action = " Got Poked!";
	logmessage = curTime() + clientName + action;
	

	SendMessage(GetDlgItem(rightHwnd, IDC_LISTBOX_LOGGING), LB_ADDSTRING, 0, (LPARAM)logmessage.c_str());
	//scrolls to bottom automatically
	SendMessage(GetDlgItem(rightHwnd, IDC_LISTBOX_LOGGING), WM_VSCROLL, SB_BOTTOM, 0L);

	::UpdateWindow(rightHwnd);
	return 0;
}

//(HBRUSH)GetStockObject(WHITE_BRUSH); white background

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

	if (!::RegisterClassEx(&wc)) { //if the registration of calls will fail, the function will return false
		return false;
	}

	if (!window) {
		window = this;
	}

	//creation of the window
	m_hwnd=::CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, "MyWindowClass", "The Annoyinator", WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT, 1000, 738, NULL, NULL, NULL, NULL);



	//if the creation fail return false
	if (!m_hwnd){
		return false;
}
	//show window
	::ShowWindow(m_hwnd, SW_SHOW);
	//runs the font initialization
	EnumChildWindows(m_hwnd, EnumChildProc, 0);
	::UpdateWindow(m_hwnd);

	

	//set this flag to true to indicate that the window is initialized and running
	m_is_run = true;

	isWinRun = true;

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


int Window::updateUserDetails(UserObj client[], int lastvalue)
{
	if (isWinRun) {
		lastUser = lastvalue;

		for (int i = 0; i < lastUser; i++)
		{
			winUserList[i] = client[i];
		}
		SendMessage(GetDlgItem(rightHwnd, IDC_LISTBOX_TEXT), LB_RESETCONTENT, 0, 0);
		for (int i = 0; i < lastUser; i++)
		{
			SendMessage(GetDlgItem(rightHwnd, IDC_LISTBOX_TEXT), LB_ADDSTRING, 0, (LPARAM)client[i].username);
		}
		::UpdateWindow(rightHwnd);
	}
		
	return 0;
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

//adding additional ui elements like buttons and lists
void AddControls(HWND hWnd)
{

	rightHwnd = hWnd;
	CreateWindow(TEXT("BUTTON"), TEXT("Add To List"), WS_CHILD | WS_VISIBLE, 320, 150, 100, 50, hWnd, (HMENU)IDC_BUTTON_ADDLIST, NULL, NULL);
	CreateWindow(TEXT("BUTTON"), TEXT("Empty List"), WS_CHILD | WS_VISIBLE, 320, 250, 100, 50, hWnd, (HMENU)IDC_BUTTON_DELETELIST, NULL, NULL);

	//Text above listboxes
	CreateWindow(TEXT("static"), TEXT("Current Channellist:"), WS_VISIBLE | WS_CHILD, 16, 16, 490, 25, hWnd, (HMENU)3, NULL, NULL);
	CreateWindow(TEXT("static"), TEXT("List of Selected Users:"), WS_VISIBLE | WS_CHILD, 436, 16, 490, 25, hWnd, (HMENU)3, NULL, NULL);

	CreateWindow("LISTBOX", NULL, WS_VISIBLE | WS_CHILD | LBS_STANDARD | LBS_NOTIFY,
		430, 40, 300, 400, hWnd,
		(HMENU)IDC_LISTBOXACTIVE_TEXT,
		(HINSTANCE)GetWindowLong
		(hWnd, GWLP_HINSTANCE), NULL);

	CreateWindow("LISTBOX", NULL, WS_VISIBLE | WS_CHILD | LBS_STANDARD | LBS_NOTIFY,
		10, 40, 300, 400,
		hWnd,
		(HMENU)IDC_LISTBOX_TEXT,
		(HINSTANCE)GetWindowLong
		(hWnd, GWLP_HINSTANCE), NULL);

	//creates checkbox
	CreateWindow(TEXT("button"), TEXT("Activate Poke on Talk"),
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		760, 55, 185, 17,
		hWnd, (HMENU)IDC_CHECKBOX_POKETALK, (HINSTANCE)GetWindowLong, NULL);

	CreateWindow(TEXT("static"), TEXT("Enter text to poke with:"), WS_VISIBLE | WS_CHILD, 760, 77, 490, 25, hWnd, (HMENU)3, NULL, NULL);
	CreateWindow(TEXT("EDIT"), TEXT(""),
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_WANTRETURN,
		760, 100, 100, 23, hWnd, (HMENU)IDC_POKEINPUT, (HINSTANCE)GetWindowLong, 0);

	CreateWindow(TEXT("BUTTON"), TEXT("Apply"), WS_CHILD | WS_VISIBLE, 865, 100, 75, 23, hWnd, (HMENU)IDC_BUTTON_APPLYCUSTOMTEXT, NULL, NULL);

	
	//visual groupbox functions
	CreateWindowEx(WS_EX_WINDOWEDGE,
		TEXT("BUTTON"),
		TEXT("Activate functions:"),
		WS_VISIBLE | WS_CHILD | BS_GROUPBOX,// <----BS_GROUPBOX does nothing on the grouping 
		750, 40,
		200, 300,
		hWnd,
		(HMENU)IDC_GRPFUNCTIONS,
		(HINSTANCE)GetWindowLong, NULL);

	//logging window
	CreateWindow(TEXT("static"), TEXT("Log:"), WS_VISIBLE | WS_CHILD, 16, 480, 490, 25, hWnd, (HMENU)3, NULL, NULL);
	CreateWindow("LISTBOX", NULL, WS_VISIBLE | WS_CHILD | LBS_STANDARD | LBS_NOTIFY,
		10, 500, 500, 200,
		hWnd,
		(HMENU)IDC_LISTBOX_LOGGING,
		(HINSTANCE)GetWindowLong
		(hWnd, GWLP_HINSTANCE), NULL);

	EnableWindow(GetDlgItem(hWnd, IDC_POKEINPUT), false);
	EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_APPLYCUSTOMTEXT), false);

}
