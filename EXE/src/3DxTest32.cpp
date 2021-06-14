/*----------------------------------------------------------------------
 *  3DxTest32.cpp  -- Basic Win32 Program to initinalize a 3D mouse, read
 *                  its data and print it out.
 *
 *  Originally written by Elio Querze
 *
 *  NOTE: MUST LINK WITH SIAPP.LIB
 *
 */

 //static char CvsId[]="(C) 1997-2015 3Dconnexion: $Id: 3DxTest32.cpp 14661 2018-01-15 10:32:16Z jwick $";


 /* Standard Win32 Includes */
#include "stdafx.h"
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <crtdbg.h>


/* SpaceWare Specific Includes */
#include "spwmacro.h"  /* Common macros used by SpaceWare functions. */
#include "si.h"        /* Required for any SpaceWare support within an app.*/
#include "siapp.h"     /* Required for siapp.lib symbols */
#include "virtualkeys.hpp"

/* Program Specific Includes */
#include "3DxTest32.h"
#include "zConnexion_DLL_main.h"

/* Global variables */
HDC          hdc;         /* Handle to Device Context used to draw on screen */
HWND         hWndMain;    /* Handle to Main Window */

SiHdl        devHdl;      /* Handle to 3D Mouse Device */
TCHAR devicename[100] = _T("");

INPUT ip;						/*Sending keystroke to zBrush*/
//const int VirtKeyCode = 0x4D;	/*0x4D -- M key*/
const int VirtKeyCode = 0x7B;	/*0x4D -- F12 key*/
int counter = 0;

/*----------------------------------------------------------------------
* Function: WinMain()
*
* Description:
*    This is the main window function and we use it to initialize data
*    and then call our loop function. This is a std. Win32 function.
*
* Args:
*    HINSTANCE hInstance         // handle to current instance
*    HINSTANCE hPrevInstance     // handle to previous instance
*    LPSTR     lpszCmdLine       // pointer to command line
*    int       nCmdShow          // show state of window
*
* Return Value:
*    int       Returns the return value of DispatchLoopNT
*
*----------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	int  res;			/* SbInits result..if>0 it worked, if=0 it didnt work   */
	int  hsize, vsize;	/* size of window to be created,for each Dimension */
	int posx, posy;		/* Window position */

	/* Set Window Size & position */
	hsize = 280;
	vsize = 310;
	posx = 540;
	posy = 540;

	/*Initialise INPUT structure for sending keystrokes*/
	ip.type = INPUT_KEYBOARD;
	ip.ki.wVk = VirtKeyCode;

	/* create our apps window */
	CreateSPWindow(posx, posy, hsize, vsize, _T("zConnexion"));

	/* update screen */
	InvalidateRect(hWndMain, NULL, FALSE);

	/* Initialize 3D mouse */
	res = SbInit();

	/* if 3D mouse was not detected then print error, close win., exit prog. */
	if (res < 1)
	{
		MessageBox(hWndMain,
			_T("Sorry - No supported 3Dconnexion device available.\n"),
			NULL, MB_OK);
		if (hWndMain != NULL)
		{
			DestroyWindow(hWndMain);    /* destroy window */
		}

		ExitProcess(1);                /* exit program */
	}

	//zCsetData(ZCONNEXION_EVENT_MOTION, 350, -20, 1, 0, -0, +20);

	/* Function To be Repeated */
	return(DispatchLoopNT());
};


/*--------------------------------------------------------------------------
* Function: SbInit()
*
* Description:
*    This function initializes the 3D mouse and opens ball for use.
*
*
* Args: None
*
*
* Return Value:
*    int  res         result of SiOpen, =0 if Fail =1 if it Works
*
*--------------------------------------------------------------------------*/
int SbInit()
{
	int res;						/* result of SiOpen, to be returned  */
	SiOpenDataEx oData;			/* OS Independent data to open ball  */

	/*init the SpaceWare input library */
	//if (SiInitialize() == SPW_ERROR)
	if (SiInitialize() == SPW_DLL_LOAD_ERROR)  /*This function initializes the 3DxWare Input Library. Since most input functions require the library to be initialized, call this function before you call any other input functions. */
	{
		MessageBox(hWndMain, _T("Error: Could not load SiAppDll dll files"),
			NULL, MB_ICONEXCLAMATION);
	}

	SiOpenWinInitEx(&oData, hWndMain);    /* init Win. platform specific data  */

	// Tell the driver we want to receive V3DCMDs instead of V3DKeys
	SiOpenWinAddHintBoolEnum(&oData, SI_HINT_USESV3DCMDS, SPW_TRUE);

	// Tell the driver we need a min driver version of 17.5.5.  
	// This could be used to tell the driver that it needs to be upgraded before it can run this application correctly.
	SiOpenWinAddHintStringEnum(&oData, SI_HINT_DRIVERVERSION, L"17.5.5");

	/* open data, which will check for device type and return the device handle
	to be used by this function */
	if ((devHdl = SiOpenEx(L"zConnexion", SI_ANY_DEVICE, SI_NO_MASK, SI_EVENT, &oData)) == NULL)
	{
		SiTerminate();  /* called to shut down the SpaceWare input library */
		res = 0;        /* could not open device */
		return res;
	}
	else
	{
		if (SiGrabDevice(devHdl, SPW_TRUE) == SPW_NO_ERROR)
		{
			SiDeviceName devName;
			SiGetDeviceName(devHdl, &devName);
			_stprintf_s(devicename, SPW_NUM_ELEMENTS_IN(devicename), _T("%S"), devName.name);

			res = 1;        /* opened device succesfully */
			return res;
		}
		else {
			SiTerminate();  /* called to shut down the SpaceWare input library */
			res = 0;        /* could not open device */
			return res;
		}
	}

}


/*--------------------------------------------------------------------------
* Function: DispatchLoopNT()
*
* Description:
*    This function contains the main message loop which constantly checks for
*    3D mouse Events and handles them apropriately.
*
* Args: None
*
*
* Return Value:
*    int  msg.wparam                  // event passed to window
*
*--------------------------------------------------------------------------*/
int DispatchLoopNT()
{
	MSG            msg;      /* incoming message to be evaluated */
	BOOL           handled;  /* is message handled yet */
	SiSpwEvent     Event;    /* SpaceWare Event */
	SiGetEventData EData;    /* SpaceWare Event Data */

	handled = SPW_FALSE;     /* init handled */

	/* start message loop */
	while (GetMessage(&msg, NULL, 0, 0))
	{
		handled = SPW_FALSE;

		/* init Window platform specific data for a call to SiGetEvent */
		SiGetEventWinInit(&EData, msg.message, msg.wParam, msg.lParam);

		/* check whether msg was a 3D mouse event and process it */
		if (SiGetEvent(devHdl, SI_AVERAGE_EVENTS, &EData, &Event) == SI_IS_EVENT)
		{
			if (Event.type == SI_MOTION_EVENT)
			{
				SbMotionEvent(&Event);        /* process 3D mouse motion event */
			}
			else if (Event.type == SI_ZERO_EVENT)
			{
				SbZeroEvent();                /* process 3D mouse zero event */
			}
			else if (Event.type == SI_DEVICE_CHANGE_EVENT)
			{
				HandleDeviceChangeEvent(&Event); /* process 3D mouse device change event */
			}
			else if (Event.type == SI_CMD_EVENT)
			{
				HandleV3DCMDEvent(&Event); /* V3DCMD_* events */
			}
			else if (Event.type == SI_APP_EVENT)
			{
				HandleAppEvent(&Event); /* AppCommand* events */
			}

			handled = SPW_TRUE;              /* 3D mouse event handled */

			//zC_Send_Keystroke();
		}

		/* not a 3D mouse event, let windows handle it */
		if (handled == SPW_FALSE)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return((int)msg.wParam);
}


/*--------------------------------------------------------------------------
* Function: HandleNTEvent
*
* Description:  This is a std. Win32 function to handle various window events
*
*
*
* Args: HWND hWnd                    // handle to window
*       unsigned msg                 // message to process
*       WPARAM wParam                // 32 bit msg parameter
*       LPARAM lParam                // 32 bit msg parameter
*
* Return Value:
*    int  msg.wparam                 // program done
*
*--------------------------------------------------------------------------*/
LRESULT WINAPI HandleNTEvent(HWND hWnd, unsigned msg, WPARAM wParam,
	LPARAM lParam)
{
	PAINTSTRUCT ps;           /* used to paint the client area of a window */
	LONG addr;                /* address of our window */

	addr = GetClassLong(hWnd, 0);  /* get address */

	switch (msg)
	{
	case WM_ACTIVATEAPP:
		hdc = GetDC(hWnd);
		/* print buffers */
		TextOut(hdc, 0, 20, devicename, (int)_tcslen(devicename));
		TextOut(hdc, 15, 100, _T("TX: 0"), 5);
		TextOut(hdc, 15, 120, _T("TY: 0"), 5);
		TextOut(hdc, 15, 140, _T("TZ: 0"), 5);
		TextOut(hdc, 15, 160, _T("RX: 0"), 5);
		TextOut(hdc, 15, 180, _T("RY: 0"), 5);
		TextOut(hdc, 15, 200, _T("RZ: 0"), 5);
		TextOut(hdc, 15, 220, _T(" P: 0"), 5);

		/*release our window handle */
		ReleaseDC(hWnd, hdc);
	case WM_KEYDOWN:
	case WM_KEYUP:
		/* user hit a key to close program */
		if (wParam == VK_ESCAPE)
		{
			SendMessage(hWndMain, WM_CLOSE, 0, 0l);
		}
		break;

	case WM_PAINT:
		/* time to paint the window */
		if (addr)
		{
			hdc = BeginPaint(hWndMain, &ps);
			EndPaint(hWndMain, &ps);
		}

		break;

	case WM_CLOSE:
		/* cleanup the object info created */

		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return (0);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);

}


/*--------------------------------------------------------------------------
* Function: CreateSPWindow
*
* Description:  This creates the window for our app
*
*
*
* Args:  int  atx        // horiz. start point to put window
*        int  aty        // vert.  start point to put window
*        int  hi         // hight of window
*        int  wid        // width of window
*        char *string    // window caption
*
* Return Value:
*    NONE
*
*--------------------------------------------------------------------------*/
void CreateSPWindow(int atx, int aty, int hi, int wid, TCHAR* string)
{
	WNDCLASS  wndclass;     /* our own instance of the window class */
	HINSTANCE hInst;        /* handle to our instance */

	hInst = NULL;             /* init handle */

	/* Register display window class */
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = (WNDPROC)HandleNTEvent;
	wndclass.cbClsExtra = 8;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInst;
	wndclass.hIcon = NULL;
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = _T("zConnexion");

	RegisterClass(&wndclass);

	/* create the window */
	hWndMain = CreateWindow(_T("zConnexion"),           /*Window class name*/
		string,              /*Window caption*/
		WS_OVERLAPPEDWINDOW, /*Window Style*/
		atx, aty, wid, hi,
		NULL,	             /*parent window handle*/
		NULL,                /*window menu handle*/
		hInst,		         /*program instance handle*/
		NULL);               /*creation parameters*/

	  /* display the window */
	ShowWindow(hWndMain, SW_SHOW);

	/* get handle of our window to draw on */
	hdc = GetDC(hWndMain);

	/* print buffers */
	TextOut(hdc, 0, 20, devicename, (int)_tcslen(devicename));
	TextOut(hdc, 15, 100, _T("TX: 0"), 5);
	TextOut(hdc, 15, 120, _T("TY: 0"), 5);
	TextOut(hdc, 15, 140, _T("TZ: 0"), 5);
	TextOut(hdc, 15, 160, _T("RX: 0"), 5);
	TextOut(hdc, 15, 180, _T("RY: 0"), 5);
	TextOut(hdc, 15, 200, _T("RZ: 0"), 5);
	TextOut(hdc, 15, 220, _T(" P: 0"), 5);

	/*release our window handle */
	ReleaseDC(hWndMain, hdc);

	UpdateWindow(hWndMain);

} /* end of CreateWindow */


/*----------------------------------------------------------------------
* Function: SbMotionEvent()
*
* Description:
*    This function receives motion information and prints out the info
*    on screen.
*
*
* Args:
*    SiSpwEvent *pEvent   Containts Data from a 3D mouse Event
*
* Return Value:
*    NONE
*
*----------------------------------------------------------------------*/
void SbMotionEvent(SiSpwEvent* pEvent)
{
	zCsetData
	(
		ZCONNEXION_EVENT_MOTION,
		pEvent->u.spwData.mData[SI_TX],
		pEvent->u.spwData.mData[SI_TY],
		pEvent->u.spwData.mData[SI_TZ],
		pEvent->u.spwData.mData[SI_RX],
		pEvent->u.spwData.mData[SI_RY],
		pEvent->u.spwData.mData[SI_RZ]
	);

	zCSendKeystroke();

	//TCHAR buff0[30];                            /* text buffer for TX */
	//TCHAR buff1[30];                            /* text buffer for TY */
	//TCHAR buff2[30];                            /* text buffer for TZ */
	//TCHAR buff3[30];                            /* text buffer for RX */
	//TCHAR buff4[30];                            /* text buffer for RY */
	//TCHAR buff5[30];                            /* text buffer for RZ */
	//TCHAR buff6[30];                            /* text buffer for Period */

	//int len0, len1, len2, len3, len4, len5, len6;	   /* length of each buffer */

	///* put the actual ball data into the buffers */
	//len0 = _stprintf_s(buff0, 30, _T("TX: %d         "), pEvent->u.spwData.mData[SI_TX]);
	//len1 = _stprintf_s(buff1, 30, _T("TY: %d         "), pEvent->u.spwData.mData[SI_TY]);
	//len2 = _stprintf_s(buff2, 30, _T("TZ: %d         "), pEvent->u.spwData.mData[SI_TZ]);
	//len3 = _stprintf_s(buff3, 30, _T("RX: %d         "), pEvent->u.spwData.mData[SI_RX]);
	//len4 = _stprintf_s(buff4, 30, _T("RY: %d         "), pEvent->u.spwData.mData[SI_RY]);
	//len5 = _stprintf_s(buff5, 30, _T("RZ: %d         "), pEvent->u.spwData.mData[SI_RZ]);
	//len6 = _stprintf_s(buff6, 30, _T(" P: %d         "), pEvent->u.spwData.period);

	///* get handle of our window to draw on */
	//hdc = GetDC(hWndMain);

	///* print buffers */
	//TCHAR* buf = _T("Motion Event              ");
	//TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
	//TextOut(hdc, 0, 20, devicename, (int)_tcslen(devicename));
	//TextOut(hdc, 15, 100, buff0, len0);
	//TextOut(hdc, 15, 120, buff1, len1);
	//TextOut(hdc, 15, 140, buff2, len2);
	//TextOut(hdc, 15, 160, buff3, len3);
	//TextOut(hdc, 15, 180, buff4, len4);
	//TextOut(hdc, 15, 200, buff5, len5);
	//TextOut(hdc, 15, 220, buff6, len6);

	///* Dump to debugger output buffer to get a running log */
	//_RPT3(_CRT_WARN, "%S %S %S", buff0, buff1, buff2);
	//_RPT3(_CRT_WARN, " %S %S %S", buff3, buff4, buff5);
	//_RPT1(_CRT_WARN, " %S\n", buff6);

	//// Also dump to stdout for a searchable log
	//printf("%d %d %d %d %d %d\n",
	//	pEvent->u.spwData.mData[SI_TX],
	//	pEvent->u.spwData.mData[SI_TY],
	//	pEvent->u.spwData.mData[SI_TZ],
	//	pEvent->u.spwData.mData[SI_RX],
	//	pEvent->u.spwData.mData[SI_RY],
	//	pEvent->u.spwData.mData[SI_RZ]);

	/*release our window handle */
	ReleaseDC(hWndMain, hdc);
}

/*----------------------------------------------------------------------
* Function: SbZeroEvent()
*
* Description:
*    This function clears the previous data, no motion data was received
*
*
*
* Args:
*    NONE
*
* Return Value:
*    NONE
*
*----------------------------------------------------------------------*/
void SbZeroEvent()
{
	/* get handle of our window to draw on */
	hdc = GetDC(hWndMain);

	/* print null data */
	TextOut(hdc, 0, 0, _T("Zero Event                  "), 28);
	TextOut(hdc, 0, 20, devicename, (int)_tcslen(devicename));
	TextOut(hdc, 15, 100, _T("TX: 0          "), 15);
	TextOut(hdc, 15, 120, _T("TY: 0          "), 15);
	TextOut(hdc, 15, 140, _T("TZ: 0          "), 15);
	TextOut(hdc, 15, 160, _T("RX: 0          "), 15);
	TextOut(hdc, 15, 180, _T("RY: 0          "), 15);
	TextOut(hdc, 15, 200, _T("RZ: 0          "), 15);
	TextOut(hdc, 15, 220, _T(" P: 0          "), 15);

	/*release our window handle */
	ReleaseDC(hWndMain, hdc);
}

void HandleDeviceChangeEvent(SiSpwEvent* pEvent)
{
	TCHAR buf[100];
	hdc = GetDC(hWndMain);

	switch (pEvent->u.deviceChangeEventData.type)
	{
	case SI_DEVICE_CHANGE_CONNECT:
		_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("Device ID %d connected"), pEvent->u.deviceChangeEventData.devID);
		TextOut(hdc, 0, 20, buf, (int)_tcslen(buf));
		break;
	case SI_DEVICE_CHANGE_DISCONNECT:
		_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("Device ID %d disconnected"), pEvent->u.deviceChangeEventData.devID);
		TextOut(hdc, 0, 20, buf, (int)_tcslen(buf));
		break;
	default:
		_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("Unknown deviceChangeEvent type: %d"), pEvent->u.deviceChangeEventData.type);
		TextOut(hdc, 0, 20, buf, (int)_tcslen(buf));
		break;
	}
	ReleaseDC(hWndMain, hdc);
}

void HandleV3DCMDEvent(SiSpwEvent* pEvent)
{
	//MessageBox(NULL,L"SI_CMD_EVENT recieved",L"Message", MB_OK);
	//TCHAR buf[100];
	//hdc = GetDC(hWndMain);
	switch (pEvent->u.cmdEventData.functionNumber)
	{
	//case V3DCMD_MENU_OPTIONS:
	//	/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_MENU_OPTIONS(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
	//	TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
	//	break;
	case V3DCMD_VIEW_FIT:
		zCsetData(ZCONNEXION_EVENT_FIT,0,0,0,0,0,0);
		/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_FIT(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
		break;
	//case V3DCMD_KEY_F1:
	//	/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_KEY_F1(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
	//	TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
	//	break;
	//case V3DCMD_KEY_F2:
	///*	_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_KEY_F2(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
	//	TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
	//	break;
	//case V3DCMD_KEY_F3:
	//	_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_KEY_F3(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
	//	TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
	//	break;
	//case V3DCMD_KEY_F4:
	//	_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_KEY_F4(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
	//	TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
	//	break;
	//case V3DCMD_KEY_F5:
	//	_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_KEY_F5(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
	//	TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
	//	break;
	//case V3DCMD_KEY_F6:
	//	_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_KEY_F6(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
	//	TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
	//	break;
	//case V3DCMD_KEY_F7:
	//	_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_KEY_F7(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
	//	TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
	//	break;
	//case V3DCMD_KEY_F8:
	//	_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_KEY_F8(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
	//	TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
	//	break;
	//case V3DCMD_KEY_F9:
	//	_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_KEY_F9(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
	//	TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
	//	break;
	//case V3DCMD_KEY_F10:
	//	_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_KEY_F10(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
	//	TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
	//	break;
	//case V3DCMD_KEY_F11:
	//	_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_KEY_F11(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
	//	TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
	//	break;
	//case V3DCMD_KEY_F12:
	//	_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_KEY_F12(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
	//	TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
	//	break;
	case V3DCMD_VIEW_TOP:
		zCsetData(ZCONNEXION_EVENT_TOP, 0, 0, 0, 0, 0, 0);
		/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_TOP(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
		break;
	case V3DCMD_VIEW_LEFT:
		zCsetData(ZCONNEXION_EVENT_LEFT, 0, 0, 0, 0, 0, 0);
		/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_LEFT(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
		break;
	case V3DCMD_VIEW_RIGHT:
		zCsetData(ZCONNEXION_EVENT_RIGHT, 0, 0, 0, 0, 0, 0);
		/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_RIGHT(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
		break;
	case V3DCMD_VIEW_FRONT:
		zCsetData(ZCONNEXION_EVENT_FRONT, 0, 0, 0, 0, 0, 0);
		/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_FRONT(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
		break;
	case V3DCMD_VIEW_BOTTOM:
		zCsetData(ZCONNEXION_EVENT_BOTTOM, 0, 0, 0, 0, 0, 0);
		/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_BOTTOM(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
		break;
	case V3DCMD_VIEW_BACK:
		zCsetData(ZCONNEXION_EVENT_BACK, 0, 0, 0, 0, 0, 0);
		/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_BACK(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
		break;
	case V3DCMD_VIEW_ROLLCW:
		zCsetData(ZCONNEXION_EVENT_RST, 0, 0, 0, 0, 0, -90);
		/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_ROLLCW(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
		break;
	case V3DCMD_VIEW_ROLLCCW:
		zCsetData(ZCONNEXION_EVENT_RST, 0, 0, 0, 0, 0, 90);
		/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_ROLLCCW(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
		break;
	case V3DCMD_VIEW_SPINCW:
		zCsetData(ZCONNEXION_EVENT_RST, 0, 0, 0, 0, -90, 0);
		/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_ROLLCW(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
		break;
	case V3DCMD_VIEW_SPINCCW:
		zCsetData(ZCONNEXION_EVENT_RST, 0, 0, 0, 0, 90, 0);
		/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_ROLLCCW(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
		break;
	case V3DCMD_VIEW_TILTCW:
		zCsetData(ZCONNEXION_EVENT_RST, 0, 0, 0, -90, 0, 0);
		/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_ROLLCW(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
		break;
	case V3DCMD_VIEW_TILTCCW:
		zCsetData(ZCONNEXION_EVENT_RST, 0, 0, 0, -90, 0, 0);
		/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_ROLLCCW(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
		break;
	case V3DCMD_VIEW_ISO1:
		zCsetData(ZCONNEXION_EVENT_ISO1, 0, 0, 0, 0, 0, 0);
		/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_ISO1(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
		break;
	case V3DCMD_VIEW_ISO2:
		zCsetData(ZCONNEXION_EVENT_ISO2, 0, 0, 0, 0, 0, 0);
		/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_ISO2(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
		break;
	case V3DCMD_VIEW_1:
		zCsetData(ZCONNEXION_EVENT_RV1, 0, 0, 0, 0, 0, 0);
		/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_1(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
		break;
	case V3DCMD_VIEW_2:
		zCsetData(ZCONNEXION_EVENT_RV2, 0, 0, 0, 0, 0, 0);
		/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_2(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
		break;
	case V3DCMD_VIEW_3:
		zCsetData(ZCONNEXION_EVENT_RV3, 0, 0, 0, 0, 0, 0);
	/*	_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_3(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
		break;
	case V3DCMD_SAVE_VIEW_1:
		zCsetData(ZCONNEXION_EVENT_SV1, 0, 0, 0, 0, 0, 0);
		/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_SAVE_VIEW_1(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
		break;
	case V3DCMD_SAVE_VIEW_2:
		zCsetData(ZCONNEXION_EVENT_SV2, 0, 0, 0, 0, 0, 0);
		/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_SAVE_VIEW_2(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
		break;
	case V3DCMD_SAVE_VIEW_3:
		zCsetData(ZCONNEXION_EVENT_SV3, 0, 0, 0, 0, 0, 0);
		/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_SAVE_VIEW_3(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
		break;
	case V3DCMD_VIEW_QZ_IN:
		zCsetData(ZCONNEXION_EVENT_QZIN, 0, 0, 0, 0, 0, 0);
		/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_QZ_IN(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
		break;
	case V3DCMD_VIEW_QZ_OUT:
		zCsetData(ZCONNEXION_EVENT_QZOUT, 0, 0, 0, 0, 0, 0);
		/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_VIEW_QZ_OUT(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
		break;
	/*case V3DCMD_MOTIONMACRO_ZOOM:
		_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_MOTIONMACRO_ZOOM(ZoomFrom=%d, ZoomTo=%d, Scale=%f)"), pEvent->u.cmdEventData.iArgs[0], pEvent->u.cmdEventData.iArgs[1], pEvent->u.cmdEventData.fArgs[0]);
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
		break;
	case V3DCMD_MOTIONMACRO_ZOOMIN_CENTERTOCENTER:
		_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_MOTIONMACRO_ZOOMIN_CENTERTOCENTER(Scale=%f)"), pEvent->u.cmdEventData.fArgs[0]);
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
		break;
	case V3DCMD_MOTIONMACRO_ZOOMIN_CURSORTOCENTER:
		_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_MOTIONMACRO_ZOOMIN_CURSORTOCENTER(Scale=%f)"), pEvent->u.cmdEventData.fArgs[0]);
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
		break;
	case V3DCMD_MOTIONMACRO_ZOOMIN_CURSORTOCURSOR:
		_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_MOTIONMACRO_ZOOMIN_CURSORTOCURSOR(Scale=%f)"), pEvent->u.cmdEventData.fArgs[0]);
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
		break;
	case V3DCMD_MOTIONMACRO_ZOOMOUT_CENTERTOCENTER:
		_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_MOTIONMACRO_ZOOMOUT_CENTERTOCENTER(Scale=%f)"), pEvent->u.cmdEventData.fArgs[0]);
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
		break;
	case V3DCMD_MOTIONMACRO_ZOOMOUT_CURSORTOCENTER:
		_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_MOTIONMACRO_ZOOMOUT_CURSORTOCENTER(Scale=%f)"), pEvent->u.cmdEventData.fArgs[0]);
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
		break;
	case V3DCMD_MOTIONMACRO_ZOOMOUT_CURSORTOCURSOR:
		_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("V3DCMD_MOTIONMACRO_ZOOMOUT_CURSORTOCURSOR(Scale=%f)"), pEvent->u.cmdEventData.fArgs[0]);
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));
		break;*/
	default:
		/*_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("Unhandled V3DCMD: number = %d"), pEvent->u.cmdEventData.functionNumber);
		TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
		break;
	}
	zCSendKeystroke();
	ReleaseDC(hWndMain, hdc);
}

void HandleAppEvent(SiSpwEvent* pEvent)
{
	MessageBox(NULL, L"SI_APP_EVENT recieved", L"Message", MB_OK);
	/*TCHAR buf[100];
	hdc = GetDC(hWndMain);
	_stprintf_s(buf, SPW_NUM_ELEMENTS_IN(buf), _T("AppCmd: %S %s"), pEvent->u.appCommandData.id.appCmdID, pEvent->u.appCommandData.pressed ? _T(" pressed") : _T(" released"));
	TextOut(hdc, 0, 0, buf, (int)_tcslen(buf));*/
}

void zCSendKeystroke()
{
	ip.ki.dwFlags = 0;
	SendInput(1, &ip, sizeof(INPUT));
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));
};