/*----------------------------------------------------------------------
*  sbtest32.h  -- Basic Program to init 3D mouse, read its data and
*                  print it out.
*
*  Originally written by Elio Querze
*
*
*----------------------------------------------------------------------
*
* Copyright notice:
* Copyright (c) 1998-2015 3Dconnexion. All rights reserved.
*
* This file and source code are an integral part of the "3Dconnexion
* Software Developer Kit", including all accompanying documentation,
* and is protected by intellectual property laws. All use of the
* 3Dconnexion Software Developer Kit is subject to the License
* Agreement found in the "LicenseAgreementSDK.txt" file.
* All rights not expressly granted by 3Dconnexion are reserved.
*
*/

static char SbTestCvsId[] = "(C) 1998-2015 3Dconnexion: $Id: 3DxTEST32.H 13020 2016-05-25 09:51:06Z jwick $";
/* Global Variables */

#ifndef SBTEST32_H
#define SBTEST32_H

/* zConnexion motion aliases*/
#define ZCONNEXION_EVENT_MOTION	0
#define ZCONNEXION_EVENT_FIT	1
#define ZCONNEXION_EVENT_TOP	2
#define ZCONNEXION_EVENT_LEFT	3
#define ZCONNEXION_EVENT_RIGHT	4
#define ZCONNEXION_EVENT_FRONT	5
#define ZCONNEXION_EVENT_BOTTOM	6
#define ZCONNEXION_EVENT_BACK	7
#define ZCONNEXION_EVENT_RST	8
#define ZCONNEXION_EVENT_ISO1	9
#define ZCONNEXION_EVENT_ISO2	10
#define ZCONNEXION_EVENT_QZIN	11
#define ZCONNEXION_EVENT_QZOUT	12
#define ZCONNEXION_EVENT_SV1	15
#define ZCONNEXION_EVENT_SV2	16
#define ZCONNEXION_EVENT_SV3	17
#define ZCONNEXION_EVENT_RV1	18
#define ZCONNEXION_EVENT_RV2	19
#define ZCONNEXION_EVENT_RV3	20

/* Functions --See cpp file for additional details */

LRESULT  WINAPI HandleNTEvent(HWND hWnd, unsigned msg, WPARAM wParam, LPARAM lParam);
int   DispatchLoopNT();
void  CreateSPWindow(int, int, int, int, TCHAR*);
int   SbInit();
void  SbMotionEvent(SiSpwEvent* pEvent);
void  SbZeroEvent();
void  SbButtonPressEvent(int buttonnumber);
void  SbButtonReleaseEvent(int buttonnumber);
void  HandleDeviceChangeEvent(SiSpwEvent* pEvent);
void  HandleV3DCMDEvent(SiSpwEvent* pEvent);
void  HandleAppEvent(SiSpwEvent* pEvent);
void  zCSendKeystroke();
#endif
