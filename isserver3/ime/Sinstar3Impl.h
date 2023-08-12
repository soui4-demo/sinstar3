#pragma once
#include "../include/sinstar-i.h"

#include "ui/InputWnd.h"
#include "ui/StatusWnd.h"
#include "ui/STipWnd.h"

#include "InputState.h"
#include "CmdHandler.h"
#include "../../sinstar3_tsf/UILess.h"

class CSinstar3Sender;

class CSinstar3Impl:
	public CUnknown,
	public ISinstar,
	public IInputListener,
	public IDestroyListener,
	public IInputWndListener
{
	friend class CCmdHandler;// CCmdHandler need access this private members.
	enum {
		UM_ASYNC_COPYDATA = (WM_USER+3000),
	};

public:
	CSinstar3Impl(ITextService *pTxtSvr,HWND hSvr);
	virtual ~CSinstar3Impl(void);

	virtual void OnIMESelect(BOOL bSelect);
	virtual void OnCompositionStarted();
	virtual void OnCompositionStarted(bool bShowUI);
	virtual void OnCompositionChanged();
	virtual void OnCompositionTerminated(bool bClearCtx);
	virtual void OnSetCaretPosition(POINT pt,int nHei);
	virtual void OnSetFocusSegmentPosition(POINT pt,int nHei);
	virtual void ProcessKeyStoke(UINT64 imeContext,UINT vkCode,LPARAM lParam,BOOL bKeyDown, BYTE byKeyState[256], BOOL *pbEaten);
	virtual void TranslateKey(UINT64 imeContext,UINT vkCode,UINT uScanCode,BOOL bKeyDown, BYTE byKeyState[256], BOOL *pbEaten);
	virtual void OnSetFocus(BOOL bFocus,DWORD hActiveWnd);
	virtual int  GetCompositionSegments();
	virtual int  GetCompositionSegmentEnd(int iSeg);
	virtual int	 GetCompositionSegmentAttr(int iSeg);
	virtual void OnOpenStatusChanged(BOOL bOpen);
	virtual void OnConversionModeChanged(EInputMethod uMode);
	virtual void ShowHelp();
	virtual EInputMethod GetDefInputMode();
	virtual void NotifyScaleInfo(HWND hRefWnd);
	virtual void GetCandidateListInfo(Context& _ctx);

	void OnLanguageBarClick(TfLBIClick click, POINT &pt, RECT &prcArea);
protected://IInputListener
	virtual BOOL IsCompositing() const;
	virtual HWND GetHwnd() const;
	virtual void OnInputStart();
	virtual void OnInputResult(const SStringT & strResult,const SStringT & strComp=SStringT() );
	virtual void OnInputEnd();
	virtual void UpdateInputWnd();
	virtual void OnCapital(BOOL bCap);	
	virtual BOOL GoNextCandidatePage();
	virtual BOOL GoPrevCandidatePage();
	virtual short SelectCandidate(short iCand);
	virtual void OpenInputWnd();
	virtual void CloseInputWnd(BOOL bDelay);
	virtual void SetOpenStatus(BOOL bOpen);
	virtual BOOL GetOpenStatus() const;
	virtual void EnableInput(BOOL bEnable);
	virtual BOOL IsInputEnable() const;
	virtual void DelayCaretLeft();
protected:
	BOOL GoPrevPage();
	BOOL GoNextPage();
protected://ICmdListener
	virtual void OnCommand(WORD cmd, LPARAM lp);
	virtual InputContext * GetInputContext();
protected://IDestroyListener
	virtual void OnSkinAwareWndDestroy(CSkinAwareWnd * pWnd);
protected://IInputWndListener
	virtual void OnInputDelayHide();
	virtual void OnSwitchTip(InputContext * pCtx,bool bNext);
public:
	BOOL ChangeSkin(const SStringT & strSkin);
	void OnSkinChanged();
	void UpdateInline();
	void UpdateUI();

	void OpenConfig();
	void ShowTip(LPCTSTR pszTitle, LPCTSTR pszContent,LPCTSTR pszKey);

	void InputSpchar(LPCTSTR pszText);
	void Broadcast(UINT uCmd, LPVOID pData, DWORD nLen);

protected:
	LRESULT OnSvrNotify(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnAsyncCopyData(UINT uMsg, WPARAM wParam, LPARAM lParam);

	BOOL OnCopyData(HWND wnd, PCOPYDATASTRUCT pCopyDataStruct);
	void OnTimer(UINT_PTR id);
	BEGIN_MSG_MAP_EX(CSinstar3Impl)
		MESSAGE_HANDLER_EX(UM_ASYNC_COPYDATA,OnAsyncCopyData)
		MSG_WM_COPYDATA(OnCopyData)
		MESSAGE_HANDLER_EX(UM_SVR_NOTIFY,OnSvrNotify)
		MSG_WM_TIMER(OnTimer)
		CHAIN_MSG_MAP_MEMBER(m_cmdHandler)
	END_MSG_MAP()

public:
	IUNKNOWN_BEGIN(IUnknown)
	IUNKNOWN_END()

protected:
	BOOL IsInputVisible() const;
	BOOL IsStatusVisible() const;
private:
	ITextService	*m_pTxtSvr;
	CInputWnd		*m_pInputWnd;
	CStatusWnd		*m_pStatusWnd;
	STipWnd			*m_pTipWnd;
	CInputState		m_inputState;

	UINT64			m_curImeContext;

	CCmdHandler	    m_cmdHandler;

	SStringT		m_strLoadedFontFile;
	HWND			m_hSvr;
	BOOL			m_bTyping;
	BOOL			m_hasFocus;
	HWND			m_hOwner;
	BOOL			m_bInputEnable;
	BOOL			m_bOpen;
	bool			m_bShowUI;
	bool			m_bPageChanged;
	CSinstar3Sender *m_evtSender;
};

class CSinstar3Sender : public TObjRefImpl<SObject>, public SEventSet, public SNativeWnd{
public:
	CSinstar3Sender (CSinstar3Impl *owner);

public:

	STDMETHOD_(int,GetID)() const{return SENDER_SINSTAR3;}

protected:
	BEGIN_MSG_MAP_EX(CSinstar3Sender)
		CHAIN_MSG_MAP_MEMBER(*m_owner)
		CHAIN_MSG_MAP(SNativeWnd)
	END_MSG_MAP()

protected:
	CSinstar3Impl * m_owner;
};
