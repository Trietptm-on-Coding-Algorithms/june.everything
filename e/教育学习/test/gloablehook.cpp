// stdafx.cpp : source file that includes just the standard includes
//	hook.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

LPWSTR ImageFile = L"image\\face.jpg";
LPWSTR ProcessName = L"study.exe";	//进程的名称

/* 调试日志函数 */
void DebugLog(const char *format, ...)
{
	va_list vl;
	FILE *pf = NULL;
	char szLog[512] = { 0, };

	va_start(vl, format);
	wsprintfA(szLog, format, vl);
	va_end(vl);

	OutputDebugStringA(szLog);
}

/* 设置权限函数 */
BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
	TOKEN_PRIVILEGES tp;
	HANDLE hToken;
	LUID luid;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		DebugLog("OpenProcessToken error: %u\n", GetLastError());
		return FALSE;
	}

	if (!LookupPrivilegeValue(NULL, // lookup privilege on local system
		lpszPrivilege,				// privilege to lookup 
		&luid))						// receives LUID of privilege
	{
		DebugLog("LookupPrivilegeValue error: %u\n", GetLastError());
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL))
	{
		DebugLog("AdjustTokenPrivileges error: %u\n", GetLastError());
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		DebugLog("The token does not have the specified privilege. \n");
		return FALSE;
	}

	return TRUE;
}

/* 开启挂钩 */
BOOL HookByCode(LPCSTR szDllName, LPCSTR szFuncName, PROC pfnNew, PBYTE pOrgBytes)
{
	FARPROC pFunc = NULL;
	DWORD	dwOldProtect = 0, dwAddress = 0;
	BYTE	pBuf[5] = { 0xE9, 0, };
	PBYTE	pByte = NULL;
	HMODULE hMod = NULL;

	hMod = GetModuleHandleA(szDllName);
	if (hMod == NULL)
	{
		DebugLog("HookByCode() : GetModuleHandle(\"%s\") failed!!! [%d]\n", szDllName, GetLastError());
		return FALSE;
	}

	pFunc = (FARPROC)GetProcAddress(hMod, szFuncName);
	if (pFunc == NULL)
	{
		DebugLog("HookByCode() : GetProcAddress(\"%s\") failed!!! [%d]\n", szFuncName, GetLastError());
		return FALSE;
	}

	pByte = (PBYTE)pFunc;
	if (pByte[0] == 0xE9)
	{
		DebugLog("HookByCode() : The API is hooked already!!!\n");
		return FALSE;
	}

	if (!VirtualProtect((LPVOID)pFunc, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect))
	{
		DebugLog("HookByCode() : VirtualProtect(#1) failed!!! [%d]\n", GetLastError());
		return FALSE;
	}

	memcpy(pOrgBytes, pFunc, 5);

	dwAddress = (DWORD)pfnNew - (DWORD)pFunc - 5;
	memcpy(&pBuf[1], &dwAddress, 4);

	memcpy(pFunc, pBuf, 5);

	if (!VirtualProtect((LPVOID)pFunc, 5, dwOldProtect, &dwOldProtect))
	{
		DebugLog("HookByCode() : VirtualProtect(#2) failed!!! [%d]\n", GetLastError());
		return FALSE;
	}

	return TRUE;
}

/* 卸载挂钩 */
BOOL UnHookByCode(LPCSTR szDllName, LPCSTR szFuncName, PBYTE pOrgBytes)
{
	FARPROC pFunc = NULL;
	DWORD	dwOldProtect = 0;
	PBYTE	pByte = NULL;
	HMODULE hMod = NULL;

	hMod = GetModuleHandleA(szDllName);
	if (hMod == NULL)
	{
		DebugLog("UnHookByCode() : GetModuleHandle(\"%s\") failed!!! [%d]\n", szDllName, GetLastError());
		return FALSE;
	}

	pFunc = (FARPROC)GetProcAddress(hMod, szFuncName);
	if (pFunc == NULL)
	{
		DebugLog("UnHookByCode() : GetProcAddress(\"%s\") failed!!! [%d]\n", szFuncName, GetLastError());
		return FALSE;
	}

	pByte = (PBYTE)pFunc;
	if (pByte[0] != 0xE9)
	{
		DebugLog("UnHookByCode() : The API is unhooked already!!!");
		return FALSE;
	}

	if (!VirtualProtect((LPVOID)pFunc, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect))
	{
		DebugLog("UnHookByCode() : VirtualProtect(#1) failed!!! [%d]\n", GetLastError());
		return FALSE;
	}

	memcpy(pFunc, pOrgBytes, 5);

	if (!VirtualProtect((LPVOID)pFunc, 5, dwOldProtect, &dwOldProtect))
	{
		DebugLog("UnHookByCode() : VirtualProtect(#2) failed!!! [%d]\n", GetLastError());
		return FALSE;
	}

	return TRUE;
}

/*检测是否Vista以后的系统版本*/
BOOL IsVistaLater()
{
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
	if (osvi.dwMajorVersion >= 6)
		return TRUE;
	return FALSE;
}


/* 自定义创建远程线程函数
   主要是根据不同系统版本来创建相应的远程线程
*/
BOOL MyCreateRemoteThread(HANDLE hProcess, LPTHREAD_START_ROUTINE pThreadProc, LPVOID pRemoteBuf)
{
	HANDLE      hThread = NULL;
	FARPROC     pFunc = NULL;
	// Vista, 7, Server2008
	if (IsVistaLater())    
	{
		// GetProcAddress函数检索指定的动态链接库(DLL)中的输出库函数地址。
		// GetModuleHandleA说明:获取一个应用程序或动态链接库的模块句柄
		pFunc = GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtCreateThreadEx");
		if (pFunc == NULL)
		{
			DebugLog("MyCreateRemoteThread() : GetProcAddress() failed!!! [%d]\n", GetLastError());
			return FALSE;
		}
		((PFNTCREATETHREADEX)pFunc)(&hThread, 0x1FFFFF,
			NULL, hProcess, pThreadProc, pRemoteBuf,
			FALSE, NULL, NULL, NULL, NULL);
		if (hThread == NULL)
		{
			DebugLog("MyCreateRemoteThread() : NtCreateThreadEx() failed!!! [%d]\n", GetLastError());
			return FALSE;
		}
	}
	else	// 2000, XP, Server2003
	{
		// CreateRemoteThread是创建一个在其它进程地址空间中运行的线程(也称:创建远程线程).。
		hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);
		if (hThread == NULL)
		{
			DebugLog("MyCreateRemoteThread() : CreateRemoteThread() failed!!! [%d]\n", GetLastError());
			return FALSE;
		}
	}
	// Windows API函数。当等待仍在挂起状态时，句柄被关闭，那么函数行为是未定义的。
	// 该句柄必须具有 SYNCHRONIZE 访问权限。
	if (WAIT_FAILED == WaitForSingleObject(hThread, INFINITE))
	{
		DebugLog("MyCreateRemoteThread() : WaitForSingleObject() failed!!! [%d]\n", GetLastError());
		return FALSE;
	}

	return TRUE;
}

/**
 * Dll注入函数
 * @param dwPID 要注入进程的ID
 * @param szDllPath 要注入的DLL路径
 * @ret 返回注入是否成功
*/
BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath)
{
	HANDLE                  hProcess = NULL;
	HANDLE                  hThread = NULL;
	LPVOID                  pRemoteBuf = NULL;
	DWORD                   dwBufSize = (DWORD)(_tcslen(szDllPath) + 1) * sizeof(TCHAR);
	LPTHREAD_START_ROUTINE  pThreadProc = NULL;
	BOOL                    bRet = FALSE;
	HMODULE                 hMod = NULL;
	// 1. 打开进程.OpenProcess 函数用来打开一个已存在的进程对象，并返回进程的句柄。
	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
	{
		DebugLog("InjectDll() : OpenProcess(%d) failed!!! [%d]\n", dwPID, GetLastError());
		goto INJECTDLL_EXIT;
	}
	// 2. 申请进程内存空间.
	// VirtualAllocEx 函数的作用是在指定进程的虚拟空间保留或提交内存区域，除非指定MEM_RESET参数，否则将该内存区域置0。
	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
	if (pRemoteBuf == NULL)
	{
		DebugLog("InjectDll() : VirtualAllocEx() failed!!! [%d]\n", GetLastError());
		goto INJECTDLL_EXIT;
	}
	// 3. 将DLL写入.此函数能写入某一进程的内存区域。入口区必须可以访问，否则操作将失败。
	if (!WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL))
	{
		DebugLog("InjectDll() : WriteProcessMemory() failed!!! [%d]\n", GetLastError());
		goto INJECTDLL_EXIT;
	}
	// 4. 获取一个应用程序或动态链接库的模块句柄
	hMod = GetModuleHandle("kernel32.dll");
	if (hMod == NULL)
	{
		DebugLog("InjectDll() : GetModuleHandle() failed!!! [%d]\n", GetLastError());
		goto INJECTDLL_EXIT;
	}
	// 5. GetProcAddress函数检索指定的动态链接库(DLL)中的输出库函数地址
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryW");
	if (pThreadProc == NULL)
	{
		DebugLog("InjectDll() : GetProcAddress() failed!!! [%d]\n", GetLastError());
		goto INJECTDLL_EXIT;
	}
	// 6. CreateRemoteThread是创建一个在其它进程地址空间中运行的线程(也称:创建远程线程).。
	if (!MyCreateRemoteThread(hProcess, pThreadProc, pRemoteBuf))
	{
		DebugLog("InjectDll() : MyCreateRemoteThread() failed!!!\n");
		goto INJECTDLL_EXIT;
	}

	bRet = TRUE;

INJECTDLL_EXIT:

	if (pRemoteBuf)
		VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);

	if (hThread)
		CloseHandle(hThread);

	if (hProcess)
		CloseHandle(hProcess);

	return bRet;
}

/********************************
 * 获取文件的大小
 * @param char* path 文件的路径
 * @ret long size 文件的大小
 ********************************/
unsigned long GetFileSize(LPWSTR path) 
{
	unsigned long filesize = -1L;
	FILE *fp = NULL;
	fp = _wfopen(path, L"r");
	if (fp == NULL)
		return filesize;
	fseek(fp,0L,SEEK_END);
	filesize = ftell(fp);
	fclose(fp);
	fp = NULL;
	return filesize;
}

/*********************************
 * 获取文件的base64字符串
 * @param char* path 文件的路径
 * @retn char* 计算文件的Base64值
 *********************************/
char* GetBaes64(LPWSTR path) 
{
	ULONG usize = GetFileSize(path);
	FILE *fp = NULL;
	fp = _wfopen(path, L"rb");
	if (fp == NULL)
		return NULL;
	char* inBuffer = new char[usize];
	INT uReaded = fread(inBuffer, 1, usize, fp);
	//printf("Image file's data lenth = %d\n",uReaded);
	const BYTE* pbBinary = (const BYTE *)inBuffer;//图片的二进制数据

	INT needsize = BASE64_Encode(pbBinary,uReaded,NULL);
	//printf("Base64 needs size = %d\n",needsize);
	char *pCrypt1 = new char[needsize+1];
	BASE64_Encode(pbBinary,uReaded,pCrypt1);
	free(inBuffer);//释放掉inBuffer
	inBuffer = NULL; 
	fclose(fp);
	fp = NULL;
	return pCrypt1;//使用完成后，请释放掉
}

/*****************
 * 保存数据到文件
 *****************/
BOOL Save2File(char* datas,char* FileName)
{
	if(datas == '\0'){
		return FALSE;
	}
	FILE *fp = fopen(FileName, "w+");
	if (fp==0) {
		return FALSE;
	}
	fwrite(datas, sizeof(char)*(strlen(datas)), 1, fp);
	fclose(fp);
	return TRUE;
}

/****************************
 *	获取当前进程的所在目录
 * @ret LPWSTR 
 ****************************/
LPWSTR GetPath() 
{
	setlocale(LC_CTYPE, "chs");		//引入支持中文库 #include <locale.h>
	LPWSTR data = NULL;
	DWORD dwRet;
	int size = GetCurrentDirectory(0, NULL);
	data = (LPWSTR)malloc(size*sizeof(LPWSTR));
	dwRet = GetCurrentDirectoryW(size, data);
	//DebugLog("FilePath : %S\n", data);
	return data;
}

/**************************************
*	获取当前进程的所在目录下的文件地址
* @ret LPWSTR
***************************************/
LPWSTR GetImageFilePath() 
{
	//宽字符打印用wprintf
	return lstrcatW(GetPath(), ImageFile);
}

/**************************************
* wchar字符转换成char型字符函数
* @param wchar_t * 
* @ret char *
***************************************/
char* WcharToChar(const wchar_t* wp)
{
	char *m_char;
	int len = WideCharToMultiByte(CP_ACP, 0, wp, wcslen(wp), NULL, 0, NULL, NULL);
	m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wp, wcslen(wp), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	return m_char;
}

/**************************************
* char字符转换成wchar型字符函数
* @param char *
* @ret wchar_t *
***************************************/
wchar_t* CharToWchar(const char* c)
{
	wchar_t *m_wchar;
	int len = MultiByteToWideChar(CP_ACP, 0, c, strlen(c), NULL, 0);
	m_wchar = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, c, strlen(c), m_wchar, len);
	m_wchar[len] = '\0';
	return m_wchar;
}

/*******************************************************
* 根据进程名称获取指定进程信息
* 注意最好保证进程名称的唯一性
* @param HANDLE hProcess		进程句柄
* @param HANDLE hModule			模块句柄
* @param PROCESSENTRY32* pinfo	进程
* @param MODULEENTRY32* minfo	模块
* @param LPWSTR processName		进程的名称
* @ret LPWSTR					返回找到进程信息的首址
********************************************************/
LPWSTR GetProcessInfo(HANDLE hProcess, HANDLE hModule, PROCESSENTRY32* pinfo, MODULEENTRY32* minfo, LPWSTR processName)
{
	BOOL report;			//是否遍历结束标志
	LPWSTR shortpath = L""; //保存路径变量
	shortpath = (LPWSTR)malloc(MAX_PATH*sizeof(LPWSTR));//这里要分配以下内存空间
	report = Process32First(hProcess, pinfo);			//获取第一个进程信息
	while (report)
	{
		hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pinfo->th32ProcessID);	//对进程ID构进模块信息
		Module32First(hModule, minfo);													//获取每一个模块信息
		GetShortPathNameW((const unsigned short *)minfo->szExePath, shortpath, 256);	//将查找到的路径赋值到变量shortpath
		if (lstrcmpW((const unsigned short *)pinfo->szExeFile, processName) == 0)		//指定的进程
		{
			//DebugLog("shortpath:%S", shortpath);
			break;
		}
		report = Process32Next(hProcess, pinfo); //继续查找下一个进程
	}
	return shortpath;
}

/***************************************************
 * 获取当前进程的地址，注意此地址是保含了图片名称的
 * @param	LPWSTR processName 进程名称
 * @ret		LPWSTR 返回当前进程的信息首址
****************************************************/
LPWSTR GetCurrentProcessPath()
{
	LPWSTR path = L"";
	ULONG num = GetModuleFileNameW(NULL, path, 256);//文件的路径最长不能超过256
	//过滤掉文件名称	//TODO 后期这里考虑程序多开技术应该修改这里，获取到当前程序进程的名称
	LPWSTR newPath = L"";
	lstrcpynW(newPath, path, wcslen(path) - wcslen(ProcessName) + 1);
	path = lstrcatW(path, ImageFile);//生成新的图片路径
	//OutputDebugStringW(path);
	return path;
}

//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
// hook.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "hook.h"

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

LPCWSTR ImageDatas = {'\0'};			//是经过了Base64编码后的数据
LPWSTR FilePath = L"image\\face.jpg";	//文件路径
BYTE g_pWCTMB[5] = { 0, };//hook5个字节WideCharToMultiByte

/*********************************************************************
 *	自定义宽字符转换函数
 *	函数功能：该函数映射一个unicode字符串到一个多字节字符串。
 *	函数原型：
 *	int WideCharToMultiByte(
 *		UINT CodePage, //指定执行转换的代码页
 *		DWORD dwFlags, //允许你进行额外的控制，它会影响使用了读音符号（比如重音）的字符
 *		LPCWSTR lpWideCharStr, //指定要转换为宽字节字符串的缓冲区
 *		int cchWideChar, //指定由参数lpWideCharStr指向的缓冲区的字符个数
 *		LPSTR lpMultiByteStr, //指向接收被转换字符串的缓冲区
 *		int cchMultiByte, //指定由参数lpMultiByteStr指向的缓冲区最大值
 *		LPCSTR lpDefaultChar, //遇到一个不能转换的宽字符，函数便会使用pDefaultChar参数指向的字符
 *		LPBOOL pfUsedDefaultChar //至少有一个字符不能转换为其多字节形式，函数就会把这个变量设为TRUE
 *	);
 *	说明：
 *	CodePage：指定执行转换的代码页，这个参数可以为系统已安装或有效的任何代码页所给定的值。你也可以指定其为下面的任意一值：
 *		CP_ACP：ANSI代码页；
 *		CP_MACCP：Macintosh代码页；
 *		CP_OEMCP：OEM代码页；
 *		CP_SYMBOL：符号代码页（42）；
 *		CP_THREAD_ACP：当前线程ANSI代码页；
 *		CP_UTF7：使用UTF-7转换；
 *		CP_UTF8：使用UTF-8转换。
 *
 ******************************************************************/
int WINAPI HookWideCharToMultiByte(UINT CodePage,
	DWORD dwFlags,
	LPCWSTR lpWideCharStr,
	int cchWideChar,
	LPSTR lpMultiByteStr,
	int cchMultiByte,
	LPCSTR lpDefaultChar,
	LPBOOL pfUsedDefaultChar)
{
	int result = 0;//表示失败
	FARPROC pFunc = NULL;
	HMODULE hMod = NULL;

	// unhook
	if (!UnHookByCode("kernel32.dll", "WideCharToMultiByte", g_pWCTMB))
	{
		//DebugLog("HookWideCharToMultiByte() : UnHookByCode() failed!!!\n");
		return result;
	}

	// call original API
	hMod = GetModuleHandleW(L"kernel32.dll");
	if (hMod == NULL)
	{
		//DebugLog("HookWideCharToMultiByte() : GetModuleHandle() failed!!! [%d]\n", GetLastError());
		goto __ERROR_EXIT;
	}

	pFunc = GetProcAddress(hMod, "WideCharToMultiByte");
	if (pFunc == NULL)
	{
		//DebugLog("HookWideCharToMultiByte() : GetProcAddress() failed!!! [%d]\n", GetLastError());
		goto __ERROR_EXIT;
	}

	// DebugLog("[redirect] change datas \n");
	// OK 这里的判断就比较完善了，这里要求的条件比较苛刻
	if (CodePage == 3 && dwFlags == 0 && cchWideChar == -1 && cchMultiByte > 10000 && cchMultiByte < 36000 && lpDefaultChar == NULL && pfUsedDefaultChar == NULL ) {
		WCHAR *wstr = L"wangjunwei";
		FilePath = GetCurrentProcessPath();
		//DebugLog("FilePath[%s]\n",FilePath);
		char* data = GetBaes64(FilePath);
		if (data == NULL)
		{
			lpWideCharStr = NULL;
			cchWideChar = wcslen(ImageDatas) + 1;
		}
		else {
			wstr = CharToWchar(data);
			ImageDatas = wstr;
			lpWideCharStr = ImageDatas;
			cchWideChar = wcslen(ImageDatas) + 1;
		}
		free(data);//将此数据释放掉
		data = NULL;
	}
	else if (CodePage == 3 && dwFlags == 0 && wcslen(lpWideCharStr) <=2  && cchWideChar == -1 && lpMultiByteStr == NULL && cchMultiByte == 0 && lpDefaultChar == NULL && pfUsedDefaultChar == NULL)
	{
		WCHAR *wstr = L"wangjunwei";
		FilePath = GetCurrentProcessPath();
		//OutputDebugStringW(FilePath);
		char* data = GetBaes64(FilePath);
		if (data == NULL)
		{
			lpWideCharStr = NULL;
			cchWideChar = wcslen(ImageDatas) + 1;
		}
		else {
			wstr = CharToWchar(data);	//CharToWchar  char2wchar
			ImageDatas = wstr;
			lpWideCharStr = ImageDatas;
			cchWideChar = wcslen(ImageDatas) + 1;
		}
		free(data);						//将此数据释放掉
		data = NULL;
	}
	result = ((WIDECHARTOMULTIBYTE)pFunc)(CodePage,
		dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr,
		cchMultiByte, lpDefaultChar, pfUsedDefaultChar);

__ERROR_EXIT:
	// hook WideCharToMultiByte() function 
	BOOL hookResult = HookByCode("kernel32.dll", "WideCharToMultiByte", (PROC)HookWideCharToMultiByte, g_pWCTMB);
	if (!hookResult)
	{
		DebugLog("HookWideCharToMultiByte() : HookByCode() failed!!!\n");
	}

	return result;
}

/* DLL的主函数入口 */
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	setlocale(LC_CTYPE, "chs");
	char szCurProc[MAX_PATH] = { 0, };
	char *p = NULL;

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		//DebugLog("DllMain() : DLL_PROCESS_ATTACH\n");
		GetModuleFileNameA(NULL, szCurProc, MAX_PATH);
		p = strrchr(szCurProc, '\\');
		DebugLog("%s\n",p);
		HookByCode("kernel32.dll", "WideCharToMultiByte", (PROC)HookWideCharToMultiByte, g_pWCTMB);
		break;

	case DLL_PROCESS_DETACH:
		//DebugLog("DllMain() : DLL_PROCESS_DETACH\n");
		UnHookByCode("kernel32.dll", "WideCharToMultiByte", g_pWCTMB);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================

//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================

//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================

//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================

//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================

//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================








