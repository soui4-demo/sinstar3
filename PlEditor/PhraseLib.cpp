//	PhraseLib.cpp: implementation of the CPLEditor class.
//	�ʿ��Ϊ2���֣�	1������������Ĵʿ�;
//					2��ÿһ�����ڿ��е�ƫ������
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PhraseLib.h"
#include "TxtReader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPLEditor::CPLEditor()
{
	m_bModify=FALSE;
	m_funProgCB=NULL;
}

CPLEditor::~CPLEditor()
{
	Free();
}

void CPLEditor::Free()
{
	CGroupManager::Free();
	m_mapPhrase.RemoveAll();
	m_bModify=FALSE;
}

int Phrase2Cmp(void *ctx,const void *p1,const void *p2)
{
	const PHRASE2 *pSour=(const PHRASE2*)p1;
	const PHRASE2 *pDest = (const PHRASE2 *)p2;
	return wcscmp(pSour->szWord,pDest->szWord);
}

//-------------------------------------------------
//	���ʿ�������һ���´ʻ�,-1:dest phrase has existed and failed
//	BOOL bCheckNeed:����Ƿ���ڵĿ���,����������ʱ����ΪFALSE
//	BYTE byGroup:�����Ĵʿ���
//---------------------------------------------------
BOOL CPLEditor::AddWord(LPCWSTR pszWord,char cLen/*=-1*/,BYTE byRate/*=0*/,BOOL bCheckNeed/*=TRUE*/,BYTE byGroup/*=0*/)
{
	if(byGroup>=m_arrGroup.size()) return FALSE;
	if(cLen==-1) cLen=wcslen(pszWord);
	if(cLen>MAX_PHRASE) return FALSE;

	PHRASE2 p={byGroup,byRate,cLen};
	wcsncpy(p.szWord,pszWord,cLen);
	p.szWord[cLen]=0;

	SStringW key(p.szWord,cLen);
	if(bCheckNeed)
	{
		SMap<SStringW,PHRASE2>::CPair *p = m_mapPhrase.Lookup(key);
		if(p)
		{
			if(byGroup!=p->m_value.byGroup && byGroup<m_arrGroup.size())
			{//update group info.
				m_arrGroup[p->m_value.byGroup].dwCount--;
				m_arrGroup[byGroup].dwCount++;
				p->m_value.byGroup = byGroup;
				if(byRate != 0) 
					p->m_value.byRate = byRate;
			}
			return FALSE;
		}
	}
	m_mapPhrase[key]=p;
	m_arrGroup[byGroup].dwCount++;
	m_bModify=TRUE;
	return TRUE;
}


void CPLEditor::LoadData(FILE *f)
{
	CGroupManager::Read(f);
	DWORD dwCount=CGroupManager::GetCount();
	BYTE byRate;
	char cLen;
	WCHAR szWord[MAX_WORDLEN];
	fseek(f,sizeof(DWORD),SEEK_CUR);//���dwDataSize
	BYTE byGroup=0;
	if(m_funProgCB) m_funProgCB->OnStart(100);
	int nSegLen = dwCount/100;

	for(int j=0;j<m_arrGroup.size();j++)
	{
		m_arrGroup[j].dwCount=0;
	}
	for(DWORD i=0;i<dwCount;i++)
	{
		fread(&byGroup,1,1,f);
		fread(&byRate,1,1,f);
		fread(&cLen,1,1,f);
		fread(szWord,sizeof(WCHAR),cLen,f);
		AddWord(szWord,cLen,byRate,FALSE,byGroup);
		if(m_funProgCB) 
		{
			if(i%nSegLen==0)
				m_funProgCB->OnProg(i/nSegLen,100);
		}
	}
	if(m_funProgCB) m_funProgCB->OnEnd(true);
	m_bModify=FALSE;
}


//---------------------------------------------------------
//	format:head+indexPos+data+index
//---------------------------------------------------------
BOOL CPLEditor::Load(LPCTSTR pszFileName)
{
	Free();
	FILE *f=_tfopen(pszFileName,_T("rb"));
	if(!f) return FALSE;
	if(!ISDOC_ReadHeader(f,SSID_CL,SVCL_MAJOR1,SVCL_MINOR3))
	{
		fclose(f);
		return FALSE;
	}
	
	LoadData(f);

	fclose(f);
	return TRUE;
}

void CPLEditor::WriteData(FILE *f)
{
	CGroupManager::Write(f);//д��ʿ�����Ϣ
	DWORD dwDataSize=0;	//���ݳ���
	fwrite(&dwDataSize,sizeof(DWORD),1,f);
	//д�뾭��������Ĵʿ�����
	UINT nCount = m_mapPhrase.GetCount();
	int nSegLen = nCount/100;
	if(m_funProgCB) m_funProgCB->OnStart(100);

	PHRASE2 *pArr = (PHRASE2*) malloc(sizeof(PHRASE2)*nCount);
	SPOSITION pos = m_mapPhrase.GetStartPosition();
	int i=0;
	while(pos)
	{
		pArr[i++]=m_mapPhrase.GetNextValue(pos);
	}
	qsort_s(pArr,m_mapPhrase.GetCount(),sizeof(PHRASE2),Phrase2Cmp,NULL);

	DWORD *pdwOffset=(DWORD*)malloc(sizeof(DWORD)*nCount);
	LONG lBegin=ftell(f);
	for(UINT i=0;i<nCount;i++)
	{
		pdwOffset[i]=ftell(f)-lBegin;
		const PHRASE2 &p=pArr[i];
		fwrite(&p.byGroup,1,1,f);
		fwrite(&p.byRate,1,1,f);
		fwrite(&p.cLen,1,1,f);
		fwrite(p.szWord,sizeof(WCHAR),p.cLen,f);
	 	if(m_funProgCB) 
		{
			if(i%nSegLen==0)
				m_funProgCB->OnProg(i/nSegLen,100);
		}
	}
	//�������ݳ���
	dwDataSize=ftell(f)-lBegin;
	if(m_funProgCB) m_funProgCB->OnEnd(false);
	//д��������
	fwrite(pdwOffset,sizeof(DWORD),nCount,f);
	free(pdwOffset);
	//��д���ݳ���
	fseek(f,lBegin-sizeof(DWORD),SEEK_SET);
	fwrite(&dwDataSize,sizeof(DWORD),1,f);
	fseek(f,0,SEEK_END);
	
	free(pArr);
}

BOOL CPLEditor::Save(LPCTSTR pszFileName)
{
	FILE *f=_tfopen(pszFileName,_T("wb"));
	if(!f) return FALSE;
	DWORD dwID=GetTickCount();
	ISDOC_WriteHeader(f,SSID_CL,SVCL_MAJOR1,SVCL_MINOR3,dwID);
	WriteData(f);
	fclose(f);
	return TRUE;
}

void CPLEditor::SetProgCallBack(IProgListener *funCb)
{
	m_funProgCB=funCb;
}

//�����ı���
//LPCWSTR pszLine:�����ı���
//int &nBegin:��Ƶ��ʼλ��
//int &nEnd:�ʽ���λ��
BOOL CPLEditor::ParseLine(LPCWSTR pszLine,int &nBegin,int &nEnd)
{
	int i=0;
	while(pszLine[i])
	{
		if(pszLine[i]=='\t' || pszLine[i]==0x20)
		{
			nEnd=i;
			break;
		}else
		{
			i++;
		}
	}
	if(pszLine[i])
	{
		while(pszLine[i] && (pszLine[i]=='\t' || pszLine[i]==0x20)) i++;
		nBegin=i;
		return TRUE;
	}
	return FALSE;
}

int CPLEditor::Import2Group(LPCTSTR pszFile,BYTE byMin, BYTE byMax,BYTE iGroup/*=-1*/)
{
	CTxtReader f('#');
	int nRet = 0;
	if(f.Open(pszFile))
	{
		if(iGroup == 0xFF)
		{
			if(m_arrGroup.size()>=250)
				return 0;
			GROUPINFO groupInfo={0};
			wcscpy(groupInfo.szName,L"none");
			m_arrGroup.push_back(groupInfo);
			iGroup = m_arrGroup.size()-1;
		}else if (iGroup>250)
		{
			return 0;
		}
		
		if(m_funProgCB)
			m_funProgCB->OnStart(100);
		int nSegLen=f.getLength()/100;

		WCHAR szLine[100];
		BOOL bRead=f.getline(szLine,100);
		int nEnd,nBegin;
		BYTE byGroup=(BYTE)iGroup;
		while(bRead)
		{
			if(szLine[0]!='@')
			{
				BYTE byRate=0;
				if(ParseLine(szLine,nBegin,nEnd))
				{
					byRate=_wtoi(szLine+nBegin);
					if(byRate>MAX_RATE) byRate=MAX_RATE;
				}else
				{
					nEnd=wcslen(szLine);
				}
				if(byMin<=byRate && byRate<=byMax ) 
					nRet+=AddWord(szLine,nEnd,byRate,TRUE,byGroup)?1:0;
			}
			if(m_funProgCB)
			{
				if(f.getReadPos()%nSegLen == 0)
				m_funProgCB->OnProg(f.getReadPos()/nSegLen,100);
			}
			bRead = f.getline(szLine,100);
		}
		if(m_funProgCB)
			m_funProgCB->OnEnd(true);
	}
	return 0;
}

BOOL CPLEditor::ExportGroup(LPCTSTR pszFile,BYTE iGroup)
{
	FILE *f = _tfopen(pszFile,_T("w"));
	if(!f)
		return FALSE;

	DWORD dwCount = m_mapPhrase.GetCount();
	int   nSegLen = dwCount/100;
	if(m_funProgCB)
		m_funProgCB->OnStart(100);
	DWORD i=0;
	SPOSITION pos = m_mapPhrase.GetStartPosition();
	while(pos)
	{
		i++;
		if(m_funProgCB)
		{
			if(i%nSegLen == 0)
			{
				m_funProgCB->OnProg(i/nSegLen,100);
			}
		}
		const PHRASE2 & value=m_mapPhrase.GetNextValue(pos);
		if(value.byGroup == iGroup)
		{
			fprintf(f,"%s\t%d\n",value.szWord,(int)value.byRate);
		}
	}
	if(m_funProgCB)
		m_funProgCB->OnEnd(false);
	fclose(f);
	return TRUE;
}

BOOL CPLEditor::EraseGroup(BYTE iGroup)
{
	if(iGroup>=m_arrGroup.size())
		return FALSE;

	DWORD dwCount = m_mapPhrase.GetCount();
	int   nSegLen = dwCount/100;
	if(m_funProgCB)
		m_funProgCB->OnStart(100);
	DWORD i=0;
	SPOSITION pos = m_mapPhrase.GetStartPosition();
	while(pos)
	{
		i++;
		if(m_funProgCB)
		{
			if(i%nSegLen == 0)
			{
				m_funProgCB->OnProg(i/nSegLen,100);
			}
		}
		SPOSITION posPrev = pos;
		PHRASE2 & value=m_mapPhrase.GetNextValue(pos);
		if(value.byGroup == iGroup)
		{
			m_mapPhrase.RemoveAtPos(posPrev);
		}else if(value.byGroup>iGroup)
		{
			value.byGroup--;
		}
	}
	m_arrGroup.erase(m_arrGroup.begin()+iGroup);
	if(m_funProgCB)
		m_funProgCB->OnEnd(true);
	return TRUE;
}