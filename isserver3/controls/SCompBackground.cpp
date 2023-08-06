#include "stdafx.h"
#include "SCompBackground.h"

namespace SOUI
{
	SCompBackground::SCompBackground()
	{
	}


	SCompBackground::~SCompBackground()
	{
	}

	void SCompBackground::GetDesiredSize(SIZE *szRet,int nParentWid, int nParentHei)
	{
		__baseCls::GetDesiredSize(szRet,nParentWid, nParentHei);
		if (m_pBgSkin)
		{
			CSize szBg = m_pBgSkin->GetSkinSize();
			if (!GetLayoutParam()->IsSpecifiedSize(Vert) && szRet->cy < szBg.cy)
			{
				szRet->cy = szBg.cy;
			}
			if (!GetLayoutParam()->IsSpecifiedSize(Horz) && szRet->cx < szBg.cx)
			{
				szRet->cx = szBg.cx;
			}
		}
	}

}
