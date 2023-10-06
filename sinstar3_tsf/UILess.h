#pragma once
#include <Ctffunc.h>
#include <vector>
#include <xstring>
#include <atl.mini/SComCli.h>

class CSinstar3Tsf;
class UILess
{
public:
	static BOOL _ShowInlinePreedit(CSinstar3Tsf* pTextService, DWORD _tfClientId, SOUI::SComPtr<ITfContext> pContext);
};

enum TextAttributeType
{
	NONE = 0,
	HIGHLIGHTED,
	LAST_TYPE
};

struct STextRange
{
	STextRange() : start(0), end(0) {}
	STextRange(int _start, int _end) : start(_start), end(_end) {}
	int start;
	int end;
};

struct TextAttribute
{
	TextAttribute() : type(NONE) {}
	TextAttribute(int _start, int _end, TextAttributeType _type) : range(_start, _end), type(_type) {}
	STextRange range;
	TextAttributeType type;
};

struct Text
{
	Text() : str(L"") {}
	Text(std::wstring const& _str) : str(_str) {}
	void clear()
	{
		str.clear();
		attributes.clear();
	}
	bool empty() const
	{
		return str.empty();
	}
	std::wstring str;
	std::vector<TextAttribute> attributes;
};
struct CandidateInfo
{
	CandidateInfo()
	{
		currentPage = 0;
		totalPages = 0;
		highlighted = 0;
	}
	void clear()
	{
		currentPage = 0;
		totalPages = 0;
		highlighted = 0;
		candies.clear();
		labels.clear();
	}
	bool empty() const
	{
		return candies.empty();
	}
	UINT currentPage;//当前页
	UINT totalPages;//总数
	UINT highlighted;//选中项
	std::vector<Text> candies;
	std::vector<Text> comments;
	std::vector<Text> labels;
};

struct Context
{
	Context() {}
	void clear()
	{
		preedit.clear();
		aux.clear();
		cinfo.clear();
	}
	bool empty() const
	{
		return preedit.empty() && aux.empty() && cinfo.empty();
	}
	Text preedit;
	Text aux;
	CandidateInfo cinfo;
};


class CCandidateList :
#if __REQUIRED_RPCNDR_H_VERSION__ > 500
	public ITfIntegratableCandidateListUIElement,
	public ITfFnSearchCandidateProvider,//集成搜索栏还要实现它
#endif
	public ITfCandidateListUIElementBehavior	
{
	friend class CSinstar3Tsf;
private:
	SOUI::SComPtr<CSinstar3Tsf> _tsf;

public:
	CCandidateList(CSinstar3Tsf* pTextService);
	~CCandidateList();
#ifndef _Outptr_
#define _Outptr_
#endif
	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, _Outptr_ void** ppvObj);
	STDMETHODIMP_(ULONG) AddRef(void);
	STDMETHODIMP_(ULONG) Release(void);

	// ITfUIElement
	STDMETHODIMP GetDescription(BSTR* pbstr);
	STDMETHODIMP GetGUID(GUID* pguid);
	STDMETHODIMP Show(BOOL showCandidateWindow);
	STDMETHODIMP IsShown(BOOL* pIsShow);

	// ITfCandidateListUIElement
	STDMETHODIMP GetUpdatedFlags(DWORD* pdwFlags);
	STDMETHODIMP GetDocumentMgr(ITfDocumentMgr** ppdim);
	STDMETHODIMP GetCount(UINT* pCandidateCount);
	STDMETHODIMP GetSelection(UINT* pSelectedCandidateIndex);
	STDMETHODIMP GetString(UINT uIndex, BSTR* pbstr);
	STDMETHODIMP GetPageIndex(UINT* pIndex, UINT uSize, UINT* puPageCnt);
	STDMETHODIMP SetPageIndex(UINT* pIndex, UINT uPageCnt);
	STDMETHODIMP GetCurrentPage(UINT* puPage);

	// ITfCandidateListUIElementBehavior methods
	STDMETHODIMP SetSelection(UINT nIndex);
	STDMETHODIMP Finalize(void);
	STDMETHODIMP Abort(void);

#if __REQUIRED_RPCNDR_H_VERSION__ > 500
	// ITfIntegratableCandidateListUIElement methods
	STDMETHODIMP SetIntegrationStyle(GUID guidIntegrationStyle);
	STDMETHODIMP GetSelectionStyle(TfIntegratableCandidateListSelectionStyle* ptfSelectionStyle);
	STDMETHODIMP OnKeyDown(WPARAM wParam, LPARAM lParam, BOOL* pIsEaten);
	STDMETHODIMP ShowCandidateNumbers(BOOL* pIsShow);
	STDMETHODIMP FinalizeExactCompositionString();

	// 通过 ITfFnSearchCandidateProvider 继承
	STDMETHODIMP GetDisplayName(BSTR* pbstrName) override;
	STDMETHODIMP GetSearchCandidates(BSTR bstrQuery, BSTR bstrApplicationId, ITfCandidateList** pplist) override;
	STDMETHODIMP SetResult(BSTR bstrQuery, BSTR bstrApplicationID, BSTR bstrResult) override;
#endif
	void SetUpdatedFlags(DWORD newflags)
	{
		_changed_flags = newflags;
	}
	HRESULT BeginUIElement();
	HRESULT UpdateUIElement();
	HRESULT EndUI();
	SOUI::SComPtr<ITfContext> GetContextDocument();
	bool CanShowUI() {
		return _pbShow == TRUE;
	}
private:
	DWORD _cRef;
	Context _ctx;
	DWORD _ui_id;
	int _idx;
	BOOL _pbShow;
	DWORD _changed_flags;
#if __REQUIRED_RPCNDR_H_VERSION__ > 500
	TfIntegratableCandidateListSelectionStyle _selectionStyle;
#endif
};