#include "StdAfx.h"
#include "SkinMananger.h"
#include <helper/SMenu.h>
#include <helper/SAutoBuf.h>

CSkinMananger::CSkinMananger():m_nMaxCtxID(R.id.menu_skin)
{
}

CSkinMananger::~CSkinMananger(void)
{
}

int CSkinMananger::InitSkinMenu(HMENU hMenu, const SStringT &strSkinPath, int nStartID, const SStringT &strCurSkin)
{
	WIN32_FIND_DATA findData;
	HANDLE hFind = FindFirstFile(strSkinPath + _T("\\*.*"), &findData);
	SMenu smenu(hMenu);
	int nID = nStartID;
	//enum sub folder
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do {
			if (findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				if (_tcscmp(findData.cFileName, _T(".")) != 0 && _tcscmp(findData.cFileName, _T("..")) != 0)
				{					
					HMENU hSubMenu = CreatePopupMenu();
					if (smenu.AppendMenu(MF_STRING|MF_POPUP ,(UINT_PTR)hSubMenu , findData.cFileName))
					{
						m_nMaxCtxID += 1000;
						SetMenuContextHelpId(hSubMenu,m_nMaxCtxID);
						SStringT strSubSkinDir = strSkinPath + _T("\\") + findData.cFileName;
						m_mapCtxId2Path[m_nMaxCtxID] = strSubSkinDir;
					}else
					{
						DestroyMenu(hSubMenu);
					}
				}
			}
		} while (FindNextFile(hFind, &findData));
		FindClose(hFind);
	}

	//enum skins.
	hFind = FindFirstFile(strSkinPath + _T("\\*.sskn"), &findData);
	if (hFind != INVALID_HANDLE_VALUE)
	{		
		do {

			SStringT strFullPath = strSkinPath + _T("\\") + findData.cFileName;
			//SLOG_INFO("skin "<<strFullPath<<" id is "<<nID);
			SStringT strDesc;
			if(ExtractSkinInfo(strFullPath,strDesc))
			{
				nID++;
				m_mapSkin[nID] = strFullPath;
				BOOL bInsertSucess= smenu.AppendMenu( MF_STRING , nID, strDesc);

				if (bInsertSucess&&(strFullPath == strCurSkin))
				{
					CheckMenuItem(hMenu,nID, MF_BYCOMMAND | MF_CHECKED);
				}
			}
		} while (FindNextFile(hFind, &findData));
		FindClose(hFind);
	}
	m_nMaxMenuID = smax(m_nMaxMenuID,nID);
	return nID;
}

static const int KMinVer[4]={0,0,0,0};
static const int KMaxVer[4]={4,0,0,0};

struct EnumSkinFileCtx{
	IResProvider *pResProvider;
	bool bVerify;
	BOOL bGood;
	SStringT errReport;
};

static BOOL CALLBACK EnumSkinFileCallback(LPCTSTR pszType, LPCTSTR pszName, LPARAM lp){
	EnumSkinFileCtx * ctx = (EnumSkinFileCtx*)lp;
	if(ctx->pResProvider->GetRawBufferSize(pszType,pszName)!=0)
		return TRUE;
	SStringT resId = SStringT().Format(_T("%s:%s"),pszType,pszName);
	if(!ctx->errReport.IsEmpty())
		ctx->errReport+=L"\n";
	ctx->errReport+=resId;
	ctx->bGood=FALSE;
	return ctx->bVerify;
}

bool CSkinMananger::VerifySkin(const SStringT & strSkinPath,SStringW &strErrReport)
{
	return _ExtractSkinInfo(strSkinPath,strErrReport,true);
}

bool CSkinMananger::ExtractSkinInfo(const SStringT & strSkinPath,SStringW &strDesc)
{
	return _ExtractSkinInfo(strSkinPath,strDesc,false);
}

SStringT CSkinMananger::SkinPathFromID(int nSkinID) const
{
	SStringT strSkinPath;
	if (const SMap<int, SStringT>::CPair * p = m_mapSkin.Lookup(nSkinID))
	{
		strSkinPath = p->m_value;
	}
	return strSkinPath;
}


SOUI::SStringT CSkinMananger::SkinPathFromCtxID(int nCtxID) const
{
	SStringT strSkinPath;
	if (const SMap<int, SStringT>::CPair * p = m_mapCtxId2Path.Lookup(nCtxID))
	{
		strSkinPath = p->m_value;
	}
	return strSkinPath;
}

bool CSkinMananger::ExtractSkinOffset(IResProvider * pResProvider,SkinInfo & info)
{
	int nSize = (int)pResProvider->GetRawBufferSize(_T("uidef"), _T("xml_init"));

	SAutoBuf buffer(nSize);
	pResProvider->GetRawBuffer(_T("uidef"), _T("xml_init"), buffer, nSize);

	pugi::xml_document xmlDoc;
	xmlDoc.load_buffer_inplace(buffer, nSize);
	pugi::xml_node xmlOffset = xmlDoc.child(L"uidef").child(L"offset");
	info.ptOffset.x = xmlOffset.attribute(L"x").as_int();
	info.ptOffset.y = xmlOffset.attribute(L"y").as_int();

	pugi::xml_node comp_layout= xmlDoc.child(L"uidef").child(L"comp_layout");

	const LPCWSTR kDefCompLayout = L"LAYOUT:wnd_composition";
	info.horzLayout = comp_layout.attribute(L"horizontal").as_string(kDefCompLayout);
	info.vertLayout = comp_layout.attribute(L"vertical").as_string(kDefCompLayout);
	return true;
}

void CSkinMananger::ClearMap()
{
	m_mapSkin.RemoveAll();
	m_mapCtxId2Path.RemoveAll();
	m_nMaxCtxID = R.id.menu_skin;
	m_nMaxMenuID=0;
}

bool CSkinMananger::ExtractPreview(const SStringT & strSkinPath,IBitmapS ** ppImg)
{
	IResProvider *pResProvider = NULL;
	g_ComMgr2->CreateResProvider_ZIP((IObjRef**)&pResProvider);
	ZIPRES_PARAM param;
	ZipFile(&param,SApplication::getSingleton().GetRenderFactory(), strSkinPath);
	pResProvider->Init((WPARAM)&param,0);

	int nSize = (int)pResProvider->GetRawBufferSize(_T("uidef"),_T("xml_init"));
	if(nSize == 0)
		return false;
	bool bRet = false;
	SAutoBuf buffer(nSize);
	pResProvider->GetRawBuffer(_T("uidef"),_T("xml_init"),buffer,nSize);
	pugi::xml_document xmlDoc;
	if(xmlDoc.load_buffer_inplace(buffer,nSize))
	{
		pugi::xml_node xmlPreview= xmlDoc.child(L"UIDEF").child(L"preview");
		if(xmlPreview){
			SStringW strSrc = xmlPreview.attribute(L"src").as_string();
			SStringWList arrSrc;
			if(2==SplitString(strSrc,L':',arrSrc)){
				IBitmapS *pImg = pResProvider->LoadImage(arrSrc[0],arrSrc[1]);
				if(pImg){
					*ppImg = pImg;
					bRet = true;
				}
			}
		}
	}
	pResProvider->Release();

	return bRet;
}

bool CSkinMananger::_ExtractSkinInfo(const SStringT & strSkinPath,SStringW &strResult,bool bVerify)
{
	IResProvider *pResProvider = NULL;
	g_ComMgr2->CreateResProvider_ZIP((IObjRef**)&pResProvider);
	ZIPRES_PARAM param;
	ZipFile(&param,NULL, strSkinPath);
	pResProvider->Init((WPARAM)&param,0);
	int nSize = (int)pResProvider->GetRawBufferSize(_T("uidef"),_T("xml_init"));
	if(nSize==0){
		pResProvider->Release();
		return false;
	}
	//verify skin resource
	EnumSkinFileCtx ctxEnumFile ={pResProvider,TRUE,bVerify};
	pResProvider->EnumResource(EnumSkinFileCallback,(LPARAM)&ctxEnumFile);
	if(!ctxEnumFile.bGood)
	{
		if(bVerify)
		{
			strResult = L"以下资源ID没有找到:\n";
			strResult += S_CT2W(ctxEnumFile.errReport);
		}
		pResProvider->Release();
		return false;
	}
	SAutoBuf buffer(nSize);
	pResProvider->GetRawBuffer(_T("uidef"),_T("xml_init"),buffer,nSize);
	pResProvider->Release();

	pugi::xml_document xmlDoc;
	xmlDoc.load_buffer_inplace(buffer,nSize);

	pugi::xml_node xmlDesc = xmlDoc.child(L"uidef").child(L"desc");
	SStringW strVer = xmlDesc.attribute(L"version").as_string(L"1.0");
	SStringWList lstVer;
	SplitString(strVer,L'.',lstVer);
	int nVer[4]={0};
	bool bValid = true;
	for(size_t i =0;i<lstVer.GetCount() && i<4;i++)
	{
		nVer[i]=_wtoi(lstVer[i]);
		if(nVer[i]<KMinVer[i])
		{
			bValid=false;
			break;
		}
		if(nVer[i]<KMaxVer[i])
			break;
		if(nVer[i]>KMaxVer[i])
		{
			bValid=false;
			break;
		}
	}

	if(!bValid)
	{
		if(bVerify){
			strResult.Format(L"不支持的皮肤版本：%s\n最小版本:%d.%d.%d.%d\n最大版本:%d.%d.%d.%d"
				,strVer.c_str()
				,KMinVer[0],KMinVer[1],KMinVer[2],KMinVer[3]
				,KMaxVer[0],KMaxVer[1],KMaxVer[2],KMaxVer[3]
			);
		}
		return false;
	}
	strResult = S_CW2T(SStringW(xmlDesc.attribute(L"name").value())+L":"+xmlDesc.attribute(L"version").value()+L"["+xmlDesc.attribute(L"author").value()+L"]");
	return true;
}
