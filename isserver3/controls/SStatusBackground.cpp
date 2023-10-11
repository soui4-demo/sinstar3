#include "StdAfx.h"
#include "SStatusBackground.h"

namespace SOUI
{
	SStatusBackground::SStatusBackground(void)
	{
	}

	SStatusBackground::~SStatusBackground(void)
	{
	}

	void SStatusBackground::GetDesiredSize(SIZE *szRet,int nParentWid, int nParentHei)
	{
		if (!m_pBgSkin)
		{
			return __baseCls::GetDesiredSize(szRet,nParentWid, nParentHei);
		}
		*szRet = m_pBgSkin->GetSkinSize();
	}
}
