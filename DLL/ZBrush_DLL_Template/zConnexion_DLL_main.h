#ifndef __MAIN_H__
#define __MAIN_H__

#include <windows.h>

/*  To use this exported function of dll, include this header
 *  in your project.
 */

#define DLL_EXPORT __declspec(dllexport)
#define SHMEMSIZE 4096

#ifdef __cplusplus
extern "C"
{
#endif

	//ZBrush expects these arguments for a dll function, though it's best to only use the first three
	float DLL_EXPORT Version(char *pDontCare, double optValue, char *pOptBuffer1, int optBuffer1Size,
							 char *pOptBuffer2, int optBuffer2Size, char **zData);

	float DLL_EXPORT zCgetData(char* msg, double num, void* data);

	void DLL_EXPORT zCsetData(long mode, long tx, long ty, long tz, long rx, long ry, long rz);


#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__