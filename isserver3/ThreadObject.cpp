#include "stdafx.h"
#include "ThreadObject.h"
#include <process.h>
#include <assert.h>

// we have undocumented Win32 APIs to set thread name.
const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)
static void SetThreadName(const char *threadName)
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = ::GetCurrentThreadId();
	info.dwFlags = 0;
#pragma warning(push)
#pragma warning(disable: 6320 6322)
	__try
	{
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR *)&info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}

#pragma warning(pop)
}

CThreadObject::CThreadObject(const char *pszName) :m_hThread(0),m_startParam(0),m_uId(0)
{
	strcpy_s(m_szName,100,pszName);
	m_evtStart = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_evtStop = CreateEvent(NULL, TRUE, FALSE, NULL);
}


CThreadObject::~CThreadObject()
{
	EndThread(1000);
	CloseHandle(m_evtStart);
	CloseHandle(m_evtStop);
}


UINT CThreadObject::ThreadProc(LPARAM lp)
{
	SetThreadName(m_szName);
	OnThreadStart();
	SetEvent(m_evtStart);	//标记线程启动
	ResetEvent(m_evtStop);  //清除线程结束标志
	UINT uRet = Run(lp);		//执行线程
	OnThreadStop();
	ResetEvent(m_evtStart); //清除线程启动标志
	return uRet;
}


UINT __stdcall CThreadObject::StaticTheadProc(LPVOID param)
{
	CThreadObject * _this = (CThreadObject*)param;
	return _this->ThreadProc(_this->m_startParam);
}

BOOL CThreadObject::BeginThread(LPARAM lParam)
{
	if (IsRunning()) return FALSE;
	m_startParam = lParam;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, &CThreadObject::StaticTheadProc, this, 0, &m_uId);
	if (m_hThread == NULL) return FALSE;
	DWORD dwRet = WaitForSingleObject(m_evtStart, INFINITE);
	return dwRet == WAIT_OBJECT_0;
}

BOOL CThreadObject::StopThread()
{
	if (!IsRunning()) return FALSE;
	Quit();
	return TRUE;
}

void CThreadObject::Quit()
{
	assert(m_hThread);
	SetEvent(m_evtStop);
}

void CThreadObject::JonThread(DWORD dwWaitEndMS)
{
	assert(m_hThread);
	if (WAIT_OBJECT_0 != WaitForSingleObject(m_hThread, dwWaitEndMS))
	{
		TerminateThread(m_hThread, (DWORD)-1);
	}
	CloseHandle(m_hThread);
	m_hThread = 0;
}

BOOL CThreadObject::EndThread(DWORD dwWaitEndMS)
{
	if (!StopThread())
		return TRUE;
	JonThread(dwWaitEndMS);
	return TRUE;
}

BOOL CThreadObject::IsRunning() const
{
	return WaitForSingleObject(m_evtStart, 0) == WAIT_OBJECT_0;
}

BOOL CThreadObject::IsStoped() const
{
	return WaitForSingleObject(m_evtStop, 0) == WAIT_OBJECT_0;	
}

