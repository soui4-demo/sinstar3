// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainDlg.h"	

#include <helper/SplitString.h>
#include "../FindCaller/FindCaller.h"
#include "../FindCaller/fc_const.h"
#include "../sinstar3_proxy/SimpleWnd.h"
#include "../include/reg.h"
#include "../include/global.h"

CMainDlg::CMainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND")), m_hWndCallerX64(0), m_hWndCallerX86(0)
{
	m_bLayoutInited = FALSE;
	m_taskAdapter = new CTaskAdapter;
}

CMainDlg::~CMainDlg()
{
	delete m_taskAdapter;
}

BOOL CMainDlg::StartProcess(LPCTSTR pszFileName)
{
	STARTUPINFO         si = { 0 };
	PROCESS_INFORMATION pi;
	DWORD dwWaitRet = 1;
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	SStringT strPath = SApplication::getSingleton().GetAppDir() + _T("\\") + pszFileName;
	//以后台启动方式创建服务器进程
	if (!CreateProcess(strPath, NULL, NULL, NULL, FALSE, CREATE_NEW_PROCESS_GROUP, NULL, NULL, &si, &pi))
	{
	return FALSE;
	}
	dwWaitRet = WaitForInputIdle(pi.hProcess, 1000);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return dwWaitRet == 0;
}

int CMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(FALSE);

	const TCHAR * KFindCallerX86 = _T("findcaller.exe");
	const TCHAR * KFindCallerX64 = _T("x64\\findcaller.exe");

	if (StartProcess(KFindCallerX64))
	{
		m_hWndCallerX64 = FindWindowEx(HWND_MESSAGE, 0, KFindCallerClass, KTitle_X64);
	}
	if (StartProcess(KFindCallerX86))
	{
		m_hWndCallerX86 = FindWindowEx(HWND_MESSAGE, 0, KFindCallerClass, KTitle_X86);
	}

	return 0;
}

BOOL Is64OS()
{
	SYSTEM_INFO si;
	GetNativeSystemInfo(&si);

	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
		si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
} 


void CMainDlg::Refresh(BOOL isX64,const SStringT & strFileName)
{
	SStringTList strDlls;
	SplitString(strFileName,_T('|'),strDlls);

	if(isX64)
	{
		for(UINT i=0;i<strDlls.GetCount();i++)
		{
			SStringT strPath = strDlls[i];
			SendFCCmd(m_hWndCallerX64, FC_REFRESH, (strPath.GetLength()+1) * sizeof(TCHAR), (LPVOID)(LPCTSTR)strPath);
		}
	}else
	{
		for(UINT i=0;i<strDlls.GetCount();i++)
		{
			SStringT strPath = strDlls[i];
			SendFCCmd(m_hWndCallerX86, FC_REFRESH, (strPath.GetLength()+1) * sizeof(TCHAR), (LPVOID)(LPCTSTR)strPath);
		}
	}
}

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	m_bLayoutInited = TRUE;

	SMCListView *pListView = FindChildByID2<SMCListView>(R.id.mclv_tasks);
	SASSERT(pListView);
	pListView->SetAdapter(m_taskAdapter);

	if (lParam)
	{
		m_strTarget = *(SStringT*)lParam;
	}
	else
	{
		m_strTarget = _T("sinstar3_ime.ime|sinstar3_tsf.dll");
	}

	FindChildByID(R.id.btn_refresh)->FireCommand();
	return 0;
}

void CMainDlg::ParseRefreshAck(BOOL bX64, const BYTE * pBuf , DWORD dwLen)
{
	const BYTE * pEnd = pBuf + dwLen;
	if (dwLen == 0) return;

	DWORD dwCount = 0;
	memcpy(&dwCount, pBuf, sizeof(DWORD));
	pBuf += sizeof(DWORD);
	
	SSLOGI()<<"ParseRefreshAck, count:" << dwCount;

	for (DWORD i = 0; i < dwCount && pBuf<pEnd; i++)
	{
		TASKINFO ti;
		memcpy(&ti.pid, pBuf, sizeof(DWORD));
		pBuf += sizeof(DWORD);
		short sLen = 0;
		memcpy(&sLen, pBuf, sizeof(short));
		pBuf += sizeof(short);
		memcpy(ti.szName, pBuf, sLen * sizeof(TCHAR));
		pBuf += sLen * sizeof(TCHAR);
		ti.szName[sLen] = 0;
		memcpy(&sLen, pBuf, sizeof(short));
		pBuf += sizeof(short);
		memcpy(ti.szPath, pBuf, sLen * sizeof(TCHAR));
		pBuf += sLen * sizeof(TCHAR);
		ti.szPath[sLen] = 0;

		m_taskAdapter->AddTask(ti);

		SSLOGI()<<"taskInfo, x64="<<bX64<< " pid=" << ti.pid << " name=" << ti.szName << " path=" << ti.szPath;
	}
	m_taskAdapter->notifyDataSetChanged();
}

LRESULT CMainDlg::SendFCCmd(HWND hRecv, DWORD fcCmd, DWORD dwLen, LPVOID pData)
{
	if (!::IsWindow(hRecv))
		return -1;
	COPYDATASTRUCT cds = { 0 };
	cds.dwData = fcCmd;
	cds.cbData = dwLen;
	cds.lpData = pData;
	return ::SendMessage(hRecv, WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);
}

BOOL CMainDlg::OnCopyData(HWND wnd, PCOPYDATASTRUCT pCopyDataStruct)
{
	if (pCopyDataStruct->dwData == CDT_RET_PROCPATH)
	{
		PROC_INFO *pInfo = (PROC_INFO*)pCopyDataStruct->lpData;
		TASKINFO ti;
		ti.pid = pInfo->dwProcID;
		wcscpy(ti.szPath, pInfo->szPath);
		_wsplitpath(ti.szPath, NULL, NULL, ti.szName, NULL);
		m_taskAdapter->AddTask(ti);
		return TRUE;
	}
	if (wnd != m_hWndCallerX64
		&& wnd != m_hWndCallerX86)
		return FALSE;
	
	BOOL bX64 = wnd == m_hWndCallerX64;
	switch (pCopyDataStruct->dwData)
	{
	case ACK_REFRESH:
		{
			BYTE * pBuf = (BYTE*)(pCopyDataStruct->lpData);
			ParseRefreshAck(bX64, pBuf, pCopyDataStruct->cbData);
		}
		break;
	}
	return TRUE;
}

void CMainDlg::OnDestroy()
{
	SASSERT(IsWindow());
	if (m_hWndCallerX64 && ::IsWindow(m_hWndCallerX64))
	{
		SendFCCmd(m_hWndCallerX64, FC_QUIT, 0, NULL);
	}
	if (m_hWndCallerX86 && ::IsWindow(m_hWndCallerX86))
	{
		SendFCCmd(m_hWndCallerX86, FC_QUIT, 0, NULL);
	}

	__super::OnDestroy();
}

#include "../sinstar3_proxy/SimpleWnd.h"

void CMainDlg::OnRefresh()
{
	m_taskAdapter->RemoveAll();
	HWND hAfter = NULL;
	while(true)
	{
		HWND hFind = FindWindowEx(NULL,hAfter,CLS_SINSTAR3_IME_WND,NULL);
		if(!hFind)
			break;
		OnFindImeHost(hFind);
		hAfter = hFind;
	}
	hAfter = NULL;
	while(true)
	{
		HWND hFind = FindWindowEx(HWND_MESSAGE,hAfter,CLS_SINSTAR3_IME_WND,NULL);
		if(!hFind)
			break;
		OnFindImeHost(hFind);
		hAfter = hFind;
	}
	m_taskAdapter->notifyDataSetChanged();

	if(Is64OS())
	{
		Refresh(TRUE, m_strTarget);
		Refresh(FALSE, m_strTarget);
	}else
	{
		Refresh(FALSE, m_strTarget);
	}
}


typedef struct tagEWPARAM
{
	DWORD dwProcID;
	HWND hWnd;
	DWORD dwThreadID;
}EWPARAM;

BOOL CALLBACK MyEnumWindowsProc(
								HWND hWnd,      // handle to parent window
								LPARAM lParam   // application-defined value
								)
{
	DWORD dwProcID=0;
	EWPARAM *p=(EWPARAM*)lParam;
	DWORD dwThreadID = GetWindowThreadProcessId(hWnd,&dwProcID);
	if(dwProcID==p->dwProcID && !GetParent(hWnd) && !(GetWindowLong(hWnd,GWL_EXSTYLE)&WS_EX_TOOLWINDOW) && !(GetWindowLong(hWnd,GWL_STYLE)&WS_DISABLED))
	{
		p->hWnd=hWnd;
		p->dwThreadID = dwThreadID;
		return FALSE;
	}
	return TRUE;
}

BOOL GetProcessInfo(DWORD dwProcID,HWND &hWnd, DWORD &dwThreadID)
{
	EWPARAM ewp={ dwProcID,0};
	BOOL bRet = EnumWindows(MyEnumWindowsProc,(LPARAM)&ewp);
	hWnd = ewp.hWnd;
	dwThreadID = ewp.dwThreadID;
	return bRet;
}


BOOL Kill_Process(DWORD dwProcID)
{
	BOOL bRet=FALSE;
	HANDLE hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwProcID);
	if(hProcess)
	{
		bRet=TerminateProcess(hProcess,1);
		CloseHandle(hProcess);
	}
	return bRet;
}


void CMainDlg::OnActivateTask()
{
	SMCListView *pListView = FindChildByID2<SMCListView>(R.id.mclv_tasks);
	int curSel = pListView->GetSel();
	if (curSel != -1)
	{
		TASKINFO *ti = m_taskAdapter->GetTask(curSel);
		HWND hWnd=0;
		DWORD dwThreadID=0;
		GetProcessInfo(ti->pid,hWnd,dwThreadID);
		DWORD dwCurId = GetCurrentThreadId();
		AttachThreadInput(dwCurId,dwThreadID,TRUE);
		if (::IsIconic(hWnd))
		{
			::ShowWindow(hWnd, SW_RESTORE);
		}
		::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		::SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		AttachThreadInput(dwCurId, dwThreadID, FALSE);

		OnMinimize();
	}

}


void CMainDlg::OnKillTask()
{
	SMCListView *pListView = FindChildByID2<SMCListView>(R.id.mclv_tasks);
	int curSel = pListView->GetSel();
	if (curSel != -1)
	{
		TASKINFO *ti =m_taskAdapter->GetTask(curSel);
		if (Kill_Process(ti->pid))
		{
			m_taskAdapter->DelTask(curSel);
		}
	}
}

//TODO:消息映射
void CMainDlg::OnClose()
{
	GetNative()->DestroyWindow();
}

void CMainDlg::OnMaximize()
{
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
}
void CMainDlg::OnRestore()
{
	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
}
void CMainDlg::OnMinimize()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CMainDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);
	if (!m_bLayoutInited) return;
	
	SWindow *pBtnMax = FindChildByName(L"btn_max");
	SWindow *pBtnRestore = FindChildByName(L"btn_restore");
	if(!pBtnMax || !pBtnRestore) return;
	
	if (nType == SIZE_MAXIMIZED)
	{
		pBtnRestore->SetVisible(TRUE);
		pBtnMax->SetVisible(FALSE);
	}
	else if (nType == SIZE_RESTORED)
	{
		pBtnRestore->SetVisible(FALSE);
		pBtnMax->SetVisible(TRUE);
	}
}

void CMainDlg::OnFindImeHost(HWND hWnd)
{
	::PostMessage(hWnd, UM_GETPROCPATH, 0, (LPARAM)m_hWnd);
}

