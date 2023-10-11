#pragma once
#include <TaskLoop.h>
#include <Windows.h>
#include <tchar.h>


SNSBEGIN
class LogWriter : public TASKLOOP::STaskLoop {
		enum {
			kMaxLog = 10 * 1024 * 1024,
			kMaxName = MAX_PATH,
		};
	public:
		LogWriter();
		~LogWriter();

		static LogWriter* instance();
		void setLoggerName(const TCHAR* name);
		void setLoggerPath(const TCHAR * path);
		void enableLog(bool bEnable);
	public:
		void OnWriteLog(const std::string& log, int level);
	private:
		void buildLogFile(LPTSTR pszBuf,BOOL bBackup);
		static void Sinstar3_LogCallback(const char *tag, const char *pLogStr, int level, const char *file, int line, const char *fun, void *retAddr);
		TCHAR m_szLogFile[MAX_PATH];
		TCHAR m_szLogName[kMaxName+1];
		TCHAR m_szLogPath[MAX_PATH+1];
		bool  m_bEnable;

	};
SNSEND

