#pragma once

class CThreadObject
{
public:
	CThreadObject(const char *pszName="");
	~CThreadObject();

	BOOL BeginThread(LPARAM lParam =0);

	BOOL StopThread();

	void JonThread(DWORD dwWaitEndMS = INFINITE);
	BOOL EndThread(DWORD dwWaitEndMS = INFINITE);

	BOOL IsRunning() const;

	BOOL IsStoped() const;

	uint32_t GetThreadId() const{return m_uId;}
protected:
	//Work Function.
	virtual UINT Run(LPARAM lp) = 0;
	virtual void Quit();

	virtual void OnThreadStart(){}
	virtual void OnThreadStop(){}

	UINT ThreadProc(LPARAM lp);
	static UINT __stdcall StaticTheadProc(LPVOID param);

	HANDLE m_evtStart;	//工作线程通知宿主线程“线程启动”事件
	HANDLE m_evtStop;   //宿主线程通知工作线程“线程结束”事件
	HANDLE m_hThread;   //线程句柄

	LPARAM m_startParam;
	uint32_t m_uId;

	char m_szName[100];
};

