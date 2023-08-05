#pragma once
#include <helper/SAdapterBase.h>
#include "../FindCaller/FindCaller.h"

namespace SOUI
{

	class CTaskAdapter : public SMcAdapterBase
	{
	public:
		CTaskAdapter();
		~CTaskAdapter();

		bool AddTask(TASKINFO ti);
		TASKINFO* GetTask(int iTask);
		BOOL DelTask(int iTask);
		void RemoveAll();
	protected:
		virtual int WINAPI getCount() override;

		virtual void WINAPI getView(int position, IWindow * pItem, IXmlNode * xmlTemplate) override;

		//删除一行，提供外部调用。
		void DeleteItem(int iPosition);

		SStringW WINAPI GetColumnName(int iCol) const override;

		BOOL WINAPI OnSort(int iCol, UINT * stFlags, int nCols) override;

		static int __cdecl SortCmp(void *context, const void * p1, const void * p2);
	protected:
		SArray<TASKINFO> m_arrTasks;
	};
}
