// dui-demo.cpp : main source file
//

#include "stdafx.h"
#include "MainDlg.h"
#include <SouiFactory.h>

//定义唯一的一个R,UIRES对象,ROBJ_IN_CPP是resource.h中定义的宏。
ROBJ_IN_CPP

//从PE文件加载，注意从文件加载路径位置
#define SYS_NAMED_RESOURCE _T("soui-sys-resource.dll")

#define DELAYIMP_INSECURE_WRITABLE_HOOKS
#include <Delayimp.h>

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


class SOUIEngine
{
private:
	SComMgr m_ComMgr;
	SApplication *m_theApp;
	bool m_bInitSucessed;
	SouiFactory m_souiFac;
public:
	//debug时方便调试设置当前目录以便从文件加载资源
	void SetDefaultDir()
	{
		TCHAR szCurrentDir[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, szCurrentDir, sizeof(szCurrentDir));

		LPTSTR lpInsertPos = _tcsrchr(szCurrentDir, _T('\\'));
#ifdef _DEBUG
		_tcscpy(lpInsertPos + 1, _T("..\\..\\PlEditor"));
#else
		_tcscpy(lpInsertPos+1,_T("..\\program"));
		m_ComMgr.SetDllPath(szCurrentDir);
#endif
		SetCurrentDirectory(szCurrentDir);
	}

	SOUIEngine(HINSTANCE hInstance):m_theApp(NULL), m_bInitSucessed(false){
		
		SetDefaultDir();
		CAutoRefPtr<SOUI::IRenderFactory> pRenderFactory;
		BOOL bLoaded = FALSE;
		//使用SKIA渲染界面
		bLoaded = m_ComMgr.CreateRender_Skia((IObjRef * *)& pRenderFactory);
		SASSERT_FMT(bLoaded, _T("load interface [render] failed!"));
		//设置图像解码引擎。默认为GDIP。基本主流图片都能解码。系统自带，无需其它库
		CAutoRefPtr<SOUI::IImgDecoderFactory> pImgDecoderFactory;
		bLoaded = m_ComMgr.CreateImgDecoder((IObjRef * *)& pImgDecoderFactory);
		SASSERT_FMT(bLoaded, _T("load interface [%s] failed!"), _T("imgdecoder"));

		pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);
		m_theApp = new SApplication(pRenderFactory, hInstance);	
		m_bInitSucessed = (TRUE==bLoaded);

		m_theApp->EnableNotifyCenter(TRUE);
	};
	operator bool()const
	{
		return m_bInitSucessed;
	}
	//加载系统资源
	bool LoadSystemRes()
	{
		BOOL bLoaded = FALSE;

		//从DLL加载系统资源
		{
			HMODULE hModSysResource = LoadLibrary(SYS_NAMED_RESOURCE);
			if (hModSysResource)
			{
				CAutoRefPtr<IResProvider> sysResProvider;
				sysResProvider.Attach(m_souiFac.CreateResProvider(RES_PE));
				sysResProvider->Init((WPARAM)hModSysResource, 0);
				m_theApp->LoadSystemNamedResource(sysResProvider);
				FreeLibrary(hModSysResource);
			}
			else
			{
				SASSERT(0);
			}
		}

		return TRUE==bLoaded;
	}
	//加载用户资源
	bool LoadUserRes()
	{
		CAutoRefPtr<IResProvider>   pResProvider;
		BOOL bLoaded = FALSE;
#ifdef _DEBUG		
		//选择了仅在Release版本打包资源则在DEBUG下始终使用文件加载
		{
			pResProvider.Attach(m_souiFac.CreateResProvider(RES_FILE));
			bLoaded = pResProvider->Init((LPARAM)_T("uires"), 0);
			SASSERT(bLoaded);
		}
#else
		{
			pResProvider.Attach(m_souiFac.CreateResProvider(RES_PE));
			bLoaded = pResProvider->Init((WPARAM)m_theApp->GetModule(), 0);
			SASSERT(bLoaded);
		}
#endif
		m_theApp->InitXmlNamedID(namedXmlID, ARRAYSIZE(namedXmlID), TRUE);
		m_theApp->AddResProvider(pResProvider);
		return TRUE==bLoaded;
	}


	~SOUIEngine()
	{
		if (m_theApp)
		{
			delete m_theApp;
			m_theApp = NULL;
		}
	}

	template<class MainWnd>
	int Run()
	{
		MainWnd dlgMain;
		dlgMain.Create(GetActiveWindow());
		dlgMain.SendMessage(WM_INITDIALOG);
		dlgMain.CenterWindow(dlgMain.m_hWnd);
		dlgMain.ShowWindow(SW_SHOWNORMAL);
		return m_theApp->Run(dlgMain.m_hWnd);
	}

	SApplication* GetApp()
	{
		return m_theApp;
	}
};

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int /*nCmdShow*/)
{
	HRESULT hRes = OleInitialize(NULL);
	SASSERT(SUCCEEDED(hRes));
	int nRet = 0;
	{
		SOUIEngine souiEngine(hInstance);
		if (souiEngine)
		{
			//加载系统资源
			souiEngine.LoadSystemRes();
			//加载用户资源
			souiEngine.LoadUserRes();
			nRet = souiEngine.Run<CMainDlg>();
		}
		else
		{
			MessageBox(NULL, _T("无法正常初使化SOUI"), _T("错误"), MB_OK | MB_ICONERROR);
		}
	}
	OleUninitialize();
	return nRet;
}
