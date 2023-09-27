#pragma once

class CSkinMananger 
{
public:
	CSkinMananger();
	~CSkinMananger(void);

	void ClearMap();
	int InitSkinMenu(HMENU hmenu, const SStringT & strSkinPath, int nStartId, const SStringT & strCurSkin);
	SStringT SkinPathFromID(int nSkinID) const;
	SStringT SkinPathFromCtxID(int nCtxID) const;
	int GetSkinMaxID() const{return m_nMaxMenuID;}
	static bool ExtractSkinOffset(IResProvider *pResProvider,SkinInfo & info);
	static bool ExtractSkinInfo(const SStringT & strSkinPath,SStringT & strDesc);
	static bool ExtractPreview(const SStringT & strSkinPath,IBitmapS ** ppImg);
	static bool VerifySkin(const SStringT & strSkinPath,SStringW &strErrReport);
protected:
	static bool _ExtractSkinInfo(const SStringT & strSkinPath,SStringW &strDesc,bool bVerify);

	SMap<int,SStringT> m_mapSkin;	//map of ID->skin path
	SMap<int,SStringT> m_mapCtxId2Path;//map of sub menu to path.
	int				   m_nMaxCtxID;
	int				   m_nMaxMenuID;
};
