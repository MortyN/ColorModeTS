#pragma once

#include <Windows.h>
#include "UserObj.h"

class Window
{
	typedef unsigned __int64 uint64;

public:
	Window();
	//init window
	bool init();
	bool broadcast();
	int getUserDetails(UserObj client[], int lastvalue);
	int updateUserDetails(UserObj client[], int lastvalue);
	//release window
	bool release();
	bool isRun();

	//EVENTS
	virtual void onCreate()=0;
	virtual void onUpdate()=0;
	virtual void onDestroy();



	~Window();

protected:
	HWND m_hwnd;
	bool m_is_run;
};

