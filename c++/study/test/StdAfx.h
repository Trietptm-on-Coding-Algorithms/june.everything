// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__62B77457_6A90_495C_8C37_075D52BF9318__INCLUDED_)
#define AFX_STDAFX_H__62B77457_6A90_495C_8C37_075D52BF9318__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

// TODO: reference additional headers your program requires here
#include <stdio.h>
#include <wchar.h>
#include <wininet.h>
#include <stdlib.h>
#include <tchar.h>
#include <Wincrypt.h>
#include <locale.h>
#include <tlhelp32.h>
#include <string.h>
#include <iostream.h>
#include "BASE64_API.h"

#pragma comment(lib,"crypt32")
#define STR_MODULE_NAME (L"globalhook.dll")
#define STATUS_SUCCESS  (0x00000000L)

typedef LONG NTSTATUS;
// �ͻ�ID
typedef struct _CLIENT_ID {
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} CLIENT_ID;

// �����߳���Ϣ
typedef struct _THREAD_BASIC_INFORMATION {
	NTSTATUS ExitStatus;
	PVOID TebBaseAddress;
	CLIENT_ID ClientId;
	ULONG AffinityMask;
	LONG Priority;
	LONG BasePriority;
} THREAD_BASIC_INFORMATION;

// �ָ��߳�
typedef NTSTATUS(WINAPI *PFZWRESUMETHREAD)(HANDLE ThreadHandle, PULONG SuspendCount);

// ��ѯ��Ϣ�߳�
typedef NTSTATUS(WINAPI *PFZWQUERYINFORMATIONTHREAD)
(
	HANDLE ThreadHandle,
	ULONG ThreadInformationClass,
	PVOID ThreadInformation,
	ULONG ThreadInformationLength,
	PULONG ReturnLength
	);

typedef DWORD(WINAPI *PFNTCREATETHREADEX)// pfnt create thread ex
(
	PHANDLE                 ThreadHandle,
	ACCESS_MASK             DesiredAccess,
	LPVOID                  ObjectAttributes,
	HANDLE                  ProcessHandle,
	LPTHREAD_START_ROUTINE  lpStartAddress,
	LPVOID                  lpParameter,
	BOOL	                CreateSuspended,
	DWORD                   dwStackSize,
	DWORD                   dw1,
	DWORD                   dw2,
	LPVOID                  Unknown
	);

typedef int (WINAPI *WIDECHARTOMULTIBYTE)(UINT CodePage, //ָ��ִ��ת���Ĵ���ҳ
	DWORD dwFlags, //��������ж���Ŀ��ƣ�����Ӱ��ʹ���˶������ţ��������������ַ�
	LPCWSTR lpWideCharStr, //ָ��Ҫת��Ϊ���ֽ��ַ����Ļ�����
	int cchWideChar, //ָ���ɲ���lpWideCharStrָ��Ļ��������ַ�����
	LPSTR lpMultiByteStr, //ָ����ձ�ת���ַ����Ļ�����
	int cchMultiByte, //ָ���ɲ���lpMultiByteStrָ��Ļ��������ֵ
	LPCSTR lpDefaultChar, //����һ������ת���Ŀ��ַ����������ʹ��pDefaultChar����ָ����ַ�
	LPBOOL pfUsedDefaultChar);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
void DebugLog(const char *format, ...);
BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);
BOOL hook_by_code(LPCSTR szDllName, LPCSTR szFuncName, PROC pfnNew, PBYTE pOrgBytes);
BOOL unhook_by_code(LPCSTR szDllName, LPCSTR szFuncName, PBYTE pOrgBytes);
BOOL IsVistaLater();
BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath);
BOOL MyCreateRemoteThread(HANDLE hProcess, LPTHREAD_START_ROUTINE pThreadProc, LPVOID pRemoteBuf);
NTSTATUS WINAPI NewZwResumeThread(HANDLE ThreadHandle, PULONG SuspendCount);
int WINAPI MyWideCharToMultiByte(UINT CodePage,
	DWORD dwFlags,
	LPCWSTR lpWideCharStr,
	int cchWideChar,
	LPSTR lpMultiByteStr,
	int cchMultiByte,
	LPCSTR lpDefaultChar,
	LPBOOL pfUsedDefaultChar);

// ����Ӧ�û��ɵ�ǰ���̵�����Ŀ¼
#define FILE_PATH "F:\\image.jpg"	//�����ļ�·���Ѽ�����

unsigned long GetFileSize(const char *path);
//char* GetBaes64(const char* path);//LPWSTR
char* GetBaes64(LPWSTR path);
//extern "C" __declspec (dllexport)void __stdcall SetData(WCHAR datas);
INT Save2File(char* datas,char* FileName);
//DWORD GetCurrentDirectory(DWORD nBufferLength, LPTSTR lpBuffer);
LPWSTR GetPath();
LPWSTR GetImageFilePath();
LPWSTR GetImageFilePath(LPWSTR ImggeName);
char* WcharToChar(const wchar_t* wp);
wchar_t* CharToWchar(const char* c);
WCHAR *char2wchar(char* data);
LPWSTR GetCurrentProcessPath(LPWSTR processName);

#endif // !defined(AFX_STDAFX_H__62B77457_6A90_495C_8C37_075D52BF9318__INCLUDED_)
