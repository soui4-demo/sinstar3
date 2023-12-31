#include "StdAfx.h"
#include "SMutexView.h"

namespace SOUI
{
	SMutexView::SMutexView(void)
	{
		m_bDisplay = 0;
		m_bVisible = 0;
	}

	SMutexView::~SMutexView(void)
	{
	}

	void SMutexView::OnShowWindow(BOOL bShow, UINT nStatus)
	{
		BOOL isVisible = IsVisible(FALSE);
		SWindow::OnShowWindow(bShow,nStatus);
		if(bShow && !isVisible && nStatus == NormalShow)
		{//hide other show sibling
			SWindow *pParent = GetParent();
			if(!pParent) return;
			SWindow *pSib= pParent->GetWindow(GSW_FIRSTCHILD);
			while(pSib)
			{
				if(pSib != this && pSib->IsVisible(FALSE) && pSib->IsClass(SMutexView::GetClassName()))
				{
					pSib->SetVisible(FALSE,TRUE);
				}
				pSib = pSib->GetWindow(GSW_NEXTSIBLING);
			}
		}
	}

}
