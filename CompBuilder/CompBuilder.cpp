// dui-demo.cpp : main source file
//

#include "stdafx.h"
#include "MainDlg.h"
#include <SouiFactory.h>
//从PE文件加载，注意从文件加载路径位置
#define SYS_NAMED_RESOURCE _T("soui-sys-resource.dll")

#define DELAYIMP_INSECURE_WRITABLE_HOOKS
#include <Delayimp.h>

ROBJ_IN_CPP

extern "C" FARPROC WINAPI DliHook(
	unsigned        dliNotify,
	PDelayLoadInfo  pdli
) {

	FARPROC fp = NULL;
	switch (dliNotify) {
	case dliNotePreLoadLibrary:
		fp = (FARPROC)(HMODULE)NULL;
		break;
	case dliFailLoadLib:
		{
			char szPath[MAX_PATH];
			sprintf(szPath, "..\\program\\%s", pdli->szDll);
			HMODULE hMod = LoadLibraryA(szPath);
			fp = (FARPROC)(HMODULE)hMod;
		}
		break;

	case dliNotePreGetProcAddress:
		fp = (FARPROC)NULL;
		break;

	case dliFailGetProc:
		fp = (FARPROC)NULL;
		break;
	}
	return(fp);
}

// Tell __delayLoadHelper2 to call my hook function
PfnDliHook __pfnDliNotifyHook2 = DliHook;
PfnDliHook __pfnDliFailureHook2 = DliHook;

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int /*nCmdShow*/)
{
	HRESULT hRes = OleInitialize(NULL);
	SASSERT(SUCCEEDED(hRes));

	int nRet = 0;

	SComMgr *pComMgr = new SComMgr;
	SouiFactory souiFac;
	TCHAR szCurrentDir[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szCurrentDir, sizeof(szCurrentDir));

	LPTSTR lpInsertPos = _tcsrchr(szCurrentDir, _T('\\'));
#ifdef _DEBUG
	_tcscpy(lpInsertPos + 1, _T("..\\..\\CompBuilder"));
#else
	_tcscpy(lpInsertPos+1,_T("..\\program"));
	pComMgr->SetDllPath(szCurrentDir);
#endif
	SetCurrentDirectory(szCurrentDir);
	{
		BOOL bLoaded = FALSE;
		CAutoRefPtr<SOUI::IImgDecoderFactory> pImgDecoderFactory;
		CAutoRefPtr<SOUI::IRenderFactory> pRenderFactory;

		bLoaded = pComMgr->CreateRender_Skia((IObjRef**)&pRenderFactory);
		SASSERT_FMT(bLoaded, _T("load interface [render] failed!"));
		bLoaded = pComMgr->CreateImgDecoder((IObjRef**)&pImgDecoderFactory);
		SASSERT_FMT(bLoaded, _T("load interface [%s] failed!"), _T("imgdecoder"));

		pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);
		SApplication *theApp = new SApplication(pRenderFactory, hInstance);
		//从DLL加载系统资源
		{
			HMODULE hModSysResource = LoadLibrary(SYS_NAMED_RESOURCE);
			if (hModSysResource)
			{
				CAutoRefPtr<IResProvider> sysResProvider;
				sysResProvider.Attach(souiFac.CreateResProvider(RES_PE));
				sysResProvider->Init((WPARAM)hModSysResource, 0);
				theApp->LoadSystemNamedResource(sysResProvider);
				FreeLibrary(hModSysResource);
			}
			else
			{
				SASSERT(0);
			}
		}

		CAutoRefPtr<IResProvider>   pResProvider;

#ifdef _DEBUG		
		//选择了仅在Release版本打包资源则在DEBUG下始终使用文件加载
		{
			pResProvider.Attach(souiFac.CreateResProvider(RES_FILE));
			bLoaded = pResProvider->Init((LPARAM)_T("uires"), 0);
			SASSERT(bLoaded);
		}
#else
		{
			pResProvider.Attach(souiFac.CreateResProvider(RES_PE));
			bLoaded = pResProvider->Init((WPARAM)hInstance, 0);
			SASSERT(bLoaded);
		}
#endif
		theApp->InitXmlNamedID(namedXmlID, ARRAYSIZE(namedXmlID), TRUE);
		theApp->AddResProvider(pResProvider);

		// BLOCK: Run application
		{
			CMainDlg dlgMain;
			dlgMain.Create(GetActiveWindow());
			dlgMain.SendMessage(WM_INITDIALOG);
			dlgMain.CenterWindow(dlgMain.m_hWnd);
			dlgMain.ShowWindow(SW_SHOWNORMAL);
			nRet = theApp->Run(dlgMain.m_hWnd);
		}

		delete theApp;
	}

	delete pComMgr;

	OleUninitialize();
	return nRet;
}
