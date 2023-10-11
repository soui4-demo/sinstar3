#pragma once

namespace SOUI
{
	class SCompBackground : public SWindow
	{
		DEF_SOBJECT(SWindow, L"comp_bg")
	public:
		SCompBackground();
		~SCompBackground();

	protected:
		virtual void WINAPI GetDesiredSize(SIZE *ret,int nParentWid, int nParentHei);

	};

}
