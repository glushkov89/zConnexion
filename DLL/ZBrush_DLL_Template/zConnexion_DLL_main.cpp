#include <cstring>
#include "zConnexion_DLL_main.h"
#include <sstream>
#include <windows.h>
#include <atomic>
#include <memory.h>

#define SHMEMSIZE 4096

static LPVOID lpvMem = NULL;      // pointer to shared memory
static HANDLE hMapObject = NULL;  // handle to file mapping
////From zBrush
//long* mD = NULL;
//long* xT = NULL;
//long* yT = NULL;
//long* zT = NULL;
//long* xR = NULL;
//long* yR = NULL;
//long* zR = NULL;


//VOID SetSharedMem(long* lpszBuf)
//{
//	long* lpszTmp;
//	DWORD dwCount = 1;
//	lpszTmp = (long*)lpvMem;	// Get the address of the shared memory block
//	while (*lpszBuf && dwCount < SHMEMSIZE)	// Copy the null-terminated string into shared memory
//	{
//		*lpszTmp++ = *lpszBuf++;
//		dwCount++;
//	}
//	*lpszTmp = '\0';
//}

//VOID GetSharedMem(LPWSTR lpszBuf, DWORD cchSize)
//{
//	LPWSTR lpszTmp;
//	lpszTmp = (LPWSTR)lpvMem;		// Get the address of the shared memory block
//	while (*lpszTmp && --cchSize) *lpszBuf++ = *lpszTmp++;	// Copy from shared memory into the caller's buffer
//	*lpszBuf = '\0';
//}

float DLL_EXPORT Version(char* pDontCare, double optValue, char* pOptBuffer1, int optBuffer1Size,
	char* pOptBuffer2, int optBuffer2Size, char** zData)
{
	return 1.2f;
}

float DLL_EXPORT zCgetData(char* msg, double num, void* data)
{
	/*Pointers to memory from zBrush*/
	long* mD = (long*)data;
	long* xT = (long*)((char*)data + 4);
	long* yT = (long*)((char*)data + 8);
	long* zT = (long*)((char*)data + 12);
	long* xR = (long*)((char*)data + 16);
	long* yR = (long*)((char*)data + 20);
	long* zR = (long*)((char*)data + 24);

	/*Getting data out of shared memory and into zBrush memory*/
	long* pTmp;
	pTmp = (long*)lpvMem;	// Get the address of the shared memory block
	*mD = *pTmp++;
	*xT = *pTmp++;
	*yT=*pTmp++;
	*zT=*pTmp++;
	*xR=*pTmp++;
	*yR=*pTmp++;
	*zR=*pTmp++;

	return 0.0f;
}

void DLL_EXPORT zCsetData(long mode, long tx, long ty, long tz, long rx, long ry, long rz)
{
	long* pTmp;
	pTmp = (long*)lpvMem;	// Get the address of the shared memory block
	*pTmp++ = mode;
	*pTmp++ = tx;
	*pTmp++ = ty;
	*pTmp++ = tz;
	*pTmp++ = rx;
	*pTmp++ = ry;
	*pTmp++ = rz;

};

// The DLL entry-point function sets up shared memory using a named file-mapping object. 

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	BOOL fInit, fIgnore;

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:	// DLL load due to process initialization or LoadLibrary

		hMapObject = CreateFileMapping(		// Create a named file mapping object
			INVALID_HANDLE_VALUE,   // use paging file
			NULL,                   // default security attributes
			PAGE_READWRITE,         // read/write access
			0,                      // size: high 32-bits
			SHMEMSIZE,              // size: low 32-bits
			TEXT("dllmemfilemap")); // name of map object
		if (hMapObject == NULL)
			return FALSE;

		fInit = (GetLastError() != ERROR_ALREADY_EXISTS);		// The first process to attach initializes memory

		lpvMem = MapViewOfFile(		// Get a pointer to the file-mapped shared memory
			hMapObject,     // object to map view of
			FILE_MAP_WRITE, // read/write access
			0,              // high offset:  map from
			0,              // low offset:   beginning
			0);             // default: map entire file

		if (lpvMem == NULL) return FALSE;
		if (fInit) memset(lpvMem, '\0', SHMEMSIZE);		// Initialize memory if this is the first process
		break;

	case DLL_THREAD_ATTACH:		// The attached process creates a new thread
		break;

	case DLL_THREAD_DETACH:		// The thread of the attached process terminates
		break;

	case DLL_PROCESS_DETACH:	// DLL unload due to process termination or FreeLibrary
		fIgnore = UnmapViewOfFile(lpvMem);		// Unmap shared memory from the process's address space
		fIgnore = CloseHandle(hMapObject);		// Close the process's handle to the file-mapping object
		/*mD = NULL;
		xT = NULL;
		yT = NULL;
		zT = NULL;
		xR = NULL;
		yR = NULL;
		zR = NULL;*/
		break;

	default:
		break;
	}

	return TRUE;
	UNREFERENCED_PARAMETER(hinstDLL);
	UNREFERENCED_PARAMETER(lpvReserved);
}

