#include "stdafx.h"
#include "BuildIndexProgWnd.h"

namespace SOUI
{
	CBuildIndexProgWnd::CBuildIndexProgWnd():SHostWnd(UIRES.LAYOUT.wnd_build_index_prog)
	{
	}


	CBuildIndexProgWnd::~CBuildIndexProgWnd()
	{
	}

	void CBuildIndexProgWnd::OnFinalMessage(HWND hWnd)
	{
		__super::OnFinalMessage(hWnd);
		delete this;
	}

	void CBuildIndexProgWnd::SetPage(int iPage,int nMax)
	{
		STabCtrl *pTab = FindChildByID2<STabCtrl>(R.id.tab_stage);
		pTab->SetCurSel(iPage);
		SWindow *pPage = sobj_cast<SWindow>(pTab->GetPage(iPage));
		SProgress *pProg = pPage->FindChildByID2<SProgress>(R.id.prog_build_index);
		pProg->SetRange(0, nMax);
		pProg->SetValue(0);
		pTab->Update();
	}

	void CBuildIndexProgWnd::SetProg(int nValue)
	{
		STabCtrl *pTab = FindChildByID2<STabCtrl>(R.id.tab_stage);
		int iCurPage = pTab->GetCurSel();
		SWindow *pPage = sobj_cast<SWindow>(pTab->GetPage(iCurPage));
		pPage->FindChildByID2<SProgress>(R.id.prog_build_index)->SetValue(nValue);
		pTab->Update();
		SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	}

}
