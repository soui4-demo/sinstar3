#include "LogWriter.h"
#include <ShlObj.h>
#include <sstream>
#include <helper/SFunctor.hpp>
#include "slog.h"

SNSBEGIN

	LogWriter* LogWriter::instance()
	{
		static LogWriter theObj;
		return &theObj;
	}

	LogWriter::LogWriter():m_bEnable(false)
	{
		Log::setLogCallback(&LogWriter::Sinstar3_LogCallback);
		start("log_writer",Low);
		m_szLogPath[0]=0;
		m_szLogFile[0]=0;
		m_szLogName[0]=0;
	}

	LogWriter::~LogWriter()
	{
		stop();
		Log::setLogCallback(NULL);
	}

	void LogWriter::setLoggerName(const TCHAR* name)
	{
		_tcscpy_s(m_szLogName, kMaxName, name);
		buildLogFile(m_szLogFile, FALSE);
	}

	void LogWriter::setLoggerPath(const TCHAR * path)
	{
		_tcscpy_s(m_szLogPath, MAX_PATH, path);
		buildLogFile(m_szLogFile, FALSE);
	}

	void LogWriter::Sinstar3_LogCallback(const char *tag, const char *pLogStr, int level, const char *file, int line, const char *fun, void *retAddr){
		if(!LogWriter::instance()->m_bEnable)
			return;
		DWORD tid=GetCurrentThreadId();
		DWORD pid = GetCurrentProcessId();
		std::stringstream ss;
		static const char * kLevelStr[] = {
			"unknown",
			"default",
			"verbose",
			"debug",
			"info",
			"warn",
			"error",
			"fatal"
		};
		ss << "pid="<<pid << " tid=" << tid << " ";
		//time
		SYSTEMTIME wtm;
		GetLocalTime(&wtm);
		ss << wtm.wYear << "-" << wtm.wMonth << "-" << wtm.wDay << " " << wtm.wHour << ":" << wtm.wMinute << ":" << wtm.wSecond << " " << wtm.wMilliseconds << "ms ";
		if (level >= 0 && level < ARRAYSIZE(kLevelStr)) {
			ss << kLevelStr[level] << " ";
		}
		ss<< pLogStr;
#ifdef _DEBUG
		ss<<" "<<fun<<" "<<file<<":"<<line;
#endif
		STaskHelper::post((ITaskLoop*)LogWriter::instance(),LogWriter::instance(),&LogWriter::OnWriteLog, ss.str(), level,false);
	}

	void LogWriter::buildLogFile(LPTSTR pszBuf,BOOL bBackup)
	{
		_stprintf(pszBuf, _T("%s\\%s%s.log"), m_szLogPath, m_szLogName, bBackup ? _T(".bk") : _T(""));
	}

	void LogWriter::OnWriteLog(const std::string& log, int level) {
		FILE* f = _tfopen(m_szLogFile, _T("a+"));
		if (f) {
			fseek(f, 0, SEEK_END);
			long len = ftell(f);
			if (len > kMaxLog) {
				fclose(f);
				TCHAR szLogBk[MAX_PATH];
				buildLogFile(szLogBk, TRUE);
				DeleteFile(szLogBk);
				MoveFile(m_szLogFile, szLogBk);
				f = _tfopen(m_szLogFile, _T("a+"));
				if (!f) return;
			}
			fwrite(log.c_str(), 1, log.size(), f);
			fwrite("\n", 1, 1, f);
			fclose(f);
		}
#ifndef DISABLE_LOG_ECHO
		OutputDebugStringA(log.c_str());
		OutputDebugStringA("\n");
#endif
	}
SNSEND
