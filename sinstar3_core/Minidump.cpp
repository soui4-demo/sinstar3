#include "stdafx.h"
#include "Minidump.h"
#include <windows.h>
#include <DbgHelp.h>

#pragma comment(lib, "DbgHelp.lib")

static WCHAR s_dumpFile[MAX_PATH]=L"minidump.dmp";

// ����Dump�ļ�  
//   
void CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)
{
    // ����Dump�ļ�  
    //  
    HANDLE hDumpFile = CreateFileW(lpstrDumpFilePathName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // Dump��Ϣ  
    //  
    MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
    dumpInfo.ExceptionPointers = pException;
    dumpInfo.ThreadId = GetCurrentThreadId();
    dumpInfo.ClientPointers = FALSE;

    // д��Dump�ļ�����  
    //  
    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);

    CloseHandle(hDumpFile);
}

// ����Unhandled Exception�Ļص�����  
//  
LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
    // ���ﵯ��һ������Ի����˳�����  
    //  
    //FatalAppExit(-1, _T("*** Unhandled Exception! ***"));
	CreateDumpFile(s_dumpFile, pException);
    return EXCEPTION_EXECUTE_HANDLER;
}

void CMinidump::Enable()
{
    // ���ô���Unhandled Exception�Ļص�����  
    //   
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
}

void CMinidump::SetDumpFile(LPCWSTR pszDumpFile)
{
	wcscpy_s(s_dumpFile,MAX_PATH,pszDumpFile);
}