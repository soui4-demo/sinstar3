#include "stdafx.h"
#include "ImeModule.h"
#include "UiWnd.h"
#include "../helper/helper.h"
#include "../slog/LogWriter.h"

CImeModule::CImeModule(HINSTANCE hInst, LPCTSTR pszSvrPath,LPCTSTR pszInstallPath):CModuleRef(hInst),m_dwSystemInfoFlags(0)
{
	CUiWnd::RegisterClass(hInst);
	_tcscpy(m_szSvrPath, pszSvrPath);
	_tcscpy(m_szInstallPath,pszInstallPath);
	SECURITY_ATTRIBUTES *psa = Helper_BuildLowIntegritySA();
	m_hMutex = CreateMutex(psa, FALSE, SINSTAR3_MUTEX);
	Helper_FreeSa(psa);
	if (!m_hMutex && GetLastError() == ERROR_ACCESS_DENIED)
	{
		m_hMutex = OpenMutex(SYNCHRONIZE, FALSE, SINSTAR3_MUTEX);
	}
	TCHAR szPath[MAX_PATH];
	_stprintf(szPath,_T("%s\\server\\config.ini"),pszInstallPath);
	int enableLog = GetPrivateProfileInt(_T("log"),_T("enable"),0,szPath);
	if(enableLog){
		TCHAR szExePath[MAX_PATH],szExeName[MAX_PATH];
		GetModuleFileName(NULL,szExePath,MAX_PATH);
		_tsplitpath(szExePath,NULL,NULL,szExeName,NULL);
		_stprintf(szPath,_T("ime_%s"),szExeName);
		SOUI::LogWriter::instance()->setLoggerName(szPath);
		_stprintf(szPath,_T("%s\\log"),pszInstallPath);
		SOUI::LogWriter::instance()->setLoggerPath(szPath);
		SOUI::LogWriter::instance()->enableLog(true);
	}
}

CImeModule::~CImeModule(void)
{
	if(m_hMutex) CloseHandle(m_hMutex);
	CUiWnd::UnregisterClass(GetModule());
}
