#pragma once
#include "SMutexView.h"

namespace SOUI
{
	class SStatusBackground : public SMutexView
	{
		DEF_SOBJECT(SMutexView,L"status_bg")
	public:
		SStatusBackground(void);
		~SStatusBackground(void);

	protected:
		virtual void WINAPI GetDesiredSize(SIZE *ret,int nParentWid, int nParentHei);

	};

}
