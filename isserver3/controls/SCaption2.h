#pragma once

namespace SOUI
{
	DEF_EVT_EXT(EventDragMove,EVENT_DRAGMOVE,{
		CPoint ptMove;
	})

	class SCaption2 : public SWindow
	{
		DEF_SOBJECT(SWindow,L"caption2")
	public:
		SCaption2(void);
		~SCaption2(void);

	protected:
		
		void OnLButtonDown(UINT nFlags,CPoint pt);

		void OnLButtonUp(UINT nFlags,CPoint pt);

		void OnMouseMove(UINT nFlags,CPoint pt);

		SOUI_MSG_MAP_BEGIN()
			MSG_WM_LBUTTONDOWN(OnLButtonDown)
			MSG_WM_LBUTTONUP(OnLButtonUp)
			MSG_WM_MOUSEMOVE(OnMouseMove)
		SOUI_MSG_MAP_END()

		BOOL	m_bDraging;
		CPoint  m_ptClick;
	};

}
