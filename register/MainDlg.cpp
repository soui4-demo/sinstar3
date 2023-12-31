// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainDlg.h"	


#define GETPROC2(x,mod) x=GETPROC(x,mod)

CMainDlg::CMainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND"))
{
	m_hCore = LoadLibrary(_T("program\\RegisterCore.dll"));
	SASSERT(m_hCore);
	GETPROC2(Sinstar_InitW,m_hCore);
	GETPROC2(Sinstar_GetErrMsgW,m_hCore);
	GETPROC2(Sinstar_ShowCaller,m_hCore);
	GETPROC2(Sinstar_IsRunning,m_hCore);
	GETPROC2(Sinstar_IsUpdateIME,m_hCore);
	GETPROC2(Sinstar_Update,m_hCore);
	GETPROC2(Sinstar_Uninstall,m_hCore);
	GETPROC2(Sinstar_ForceUninstall,m_hCore);
	GETPROC2(Sinstar_Install,m_hCore);
	GETPROC2(Sinstar_GetCurrentVer,m_hCore);
	GETPROC2(Sinstar_PEVersionW,m_hCore);
	GETPROC2(Sinstar_CheckFiles,m_hCore);
}

CMainDlg::~CMainDlg()
{
	FreeLibrary(m_hCore);
}
const TCHAR szMsgCaption[]=_T("启程输入法");

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	Sinstar_InitW(SApplication::getSingleton().GetAppDir());
	if(!Sinstar_CheckFiles())
	{
		SMessageBox(m_hWnd, Sinstar_GetErrMsgW(), szMsgCaption, MB_OK | MB_ICONSTOP);
		PostQuitMessage(1);
		return 0;
	}

	SStringT newSvrPath = SStringT().Format(_T("%s\\program\\isserver3.exe"), SApplication::getSingleton().GetAppDir().c_str());
	int newVer[4]={0};
	Sinstar_PEVersionW(newSvrPath,newVer);
	SStringT strVer = SStringT().Format(_T("%d.%d.%d.%d"),newVer[0],newVer[1],newVer[2],newVer[3]);
	FindChildByID(R.id.txt_ver)->SetWindowText(strVer);
	SRichEdit *pEdit = FindChildByID2<SRichEdit>(R.id.edit_license);
	pEdit->LoadRtf(SApplication::getSingleton().GetAppDir()+_T("\\license.rtf"));
	FindChildByID(R.id.wnd_btn_container)->EnableWindow(FALSE, TRUE);

	return 0;
}

void CMainDlg::OnClose()
{
	GetNative()->DestroyWindow();
}

void CMainDlg::OnMinimize()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CMainDlg::OnAgree()
{
	BOOL bAgree = FindChildByID(R.id.chk_agree)->IsChecked();
	FindChildByID(R.id.wnd_btn_container)->EnableWindow(bAgree, TRUE);
}

class CDonateDlg : public SHostDialog, public SDpiHandler<CDonateDlg>
{
public:
	CDonateDlg(LPCTSTR pszUires): SHostDialog(pszUires){}

protected:
	BEGIN_MSG_MAP_EX(CDonateDlg)
		CHAIN_MSG_MAP(SDpiHandler<CDonateDlg>)
		CHAIN_MSG_MAP(SHostDialog)
	END_MSG_MAP()
};

void CMainDlg::OnDonate()
{
	CDonateDlg dlgDonate(UIRES.LAYOUT.dlg_donate);
	dlgDonate.DoModal();
}

void CMainDlg::OnInstall()
{
	if(Sinstar_IsRunning())
	{
		if (SMessageBox(GetActiveWindow(), _T("输入法正在使用，不能安装，查看哪些程序在使用吗？"), szMsgCaption, MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
		{
			Sinstar_ShowCaller(TRUE);
		}
		return;
	}
	SetCursor(GETRESPROVIDER->LoadCursor(MAKEINTRESOURCE(IDC_WAIT)));
	BOOL bOK = Sinstar_Install();
	SetCursor(GETRESPROVIDER->LoadCursor(MAKEINTRESOURCE(IDC_ARROW)));
	if(bOK)
	{
		SMessageBox(GetActiveWindow(), _T("安装成功！"), szMsgCaption, MB_OK | MB_ICONINFORMATION);
		OnClose();
	}
	else
		SMessageBox(m_hWnd,Sinstar_GetErrMsgW(),szMsgCaption,MB_OK|MB_ICONSTOP);
}

void CMainDlg::OnUninstall()
{
	if(Sinstar_IsRunning())
	{
		if (SMessageBox(GetActiveWindow(), _T("输入法正在使用，不能卸载，查看哪些程序在使用吗？"), szMsgCaption, MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
		{
			Sinstar_ShowCaller(TRUE);
		}
		return;
	}
	SetCursor(GETRESPROVIDER->LoadCursor(MAKEINTRESOURCE(IDC_WAIT)));
	BOOL bOK = Sinstar_Uninstall();
	SetCursor(GETRESPROVIDER->LoadCursor(MAKEINTRESOURCE(IDC_ARROW)));

	if(bOK)
	{
		SMessageBox(GetActiveWindow(), _T("卸载成功！"), szMsgCaption, MB_OK | MB_ICONINFORMATION);
		OnClose();
	}
	else
		SMessageBox(m_hWnd,Sinstar_GetErrMsgW(),szMsgCaption,MB_OK|MB_ICONSTOP);
}

void CMainDlg::OnUpdate()
{	
	BOOL bUpgradeIme = Sinstar_IsUpdateIME();
	BOOL bRunning = Sinstar_IsRunning();

	if(bRunning && bUpgradeIme)
	{
		if (SMessageBox(GetActiveWindow(), _T("输入法正在使用，不能更新，查看哪些程序在使用吗？"), szMsgCaption, MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
		{
			Sinstar_ShowCaller(TRUE);
		}
		return;
	}

	int oldVer[4];
	if(Sinstar_GetCurrentVer(oldVer))
	{
		if(oldVer[0]==3&&oldVer[1]==0)
		{//3.0
			SMessageBox(GetActiveWindow(), _T("您已经安装了启程输入法3.0,不能直接升级，请先卸载现有版本再安装！"), szMsgCaption, MB_OK | MB_ICONSTOP);
			return;
		}
		SStringT newSvrPath = SStringT().Format(_T("%s\\program\\isserver3.exe"),SApplication::getSingletonPtr()->GetAppDir().c_str());
		int newVer[4];
		Sinstar_PEVersionW(newSvrPath,newVer);
		if(memcmp(newVer,oldVer,sizeof(newVer))<=0)
		{
			SStringT strMsg = SStringT().Format(_T("您已经安装了更新版本，无需升级！\n当前版本:%d.%d.%d.%d\n已安装版本:%d.%d.%d.%d\n需要强制更新吗？"),
				newVer[0],newVer[1],newVer[2],newVer[3],
				oldVer[0],oldVer[1],oldVer[2],oldVer[3]);
			if(IDCANCEL==SMessageBox(m_hWnd,strMsg,szMsgCaption,MB_OKCANCEL|MB_ICONQUESTION))
			{
				return;
			}
		}
	}

	SetCursor(GETRESPROVIDER->LoadCursor(MAKEINTRESOURCE(IDC_WAIT)));
	BOOL bOK = Sinstar_Update();
	SetCursor(GETRESPROVIDER->LoadCursor(MAKEINTRESOURCE(IDC_ARROW)));

	if(bOK)
	{
		SMessageBox(GetActiveWindow(), _T("更新成功！"), szMsgCaption, MB_OK | MB_ICONINFORMATION);
		OnClose();
	}else
	{
		SMessageBox(m_hWnd,Sinstar_GetErrMsgW(),szMsgCaption,MB_OK|MB_ICONSTOP);
	}
}

void CMainDlg::OnForceUninstall()
{
	if(!Sinstar_IsRunning())
	{
		OnUninstall();
		return;
	}

	if (SMessageBox(GetActiveWindow(), _T("确定卸载吗？需要重启才能完全卸载！"), szMsgCaption, MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
		return;

	SetCursor(GETRESPROVIDER->LoadCursor(MAKEINTRESOURCE(IDC_WAIT)));
	BOOL bOK = Sinstar_ForceUninstall();
	SetCursor(GETRESPROVIDER->LoadCursor(MAKEINTRESOURCE(IDC_ARROW)));

	if(bOK)
	{
		SMessageBox(GetActiveWindow(), _T("卸载成功！需要重启才能完全卸载！"), szMsgCaption, MB_OK | MB_ICONINFORMATION);
		OnClose();
	}else
	{
		SMessageBox(GetActiveWindow(),Sinstar_GetErrMsgW(),szMsgCaption,MB_OK|MB_ICONSTOP);
	}
}

