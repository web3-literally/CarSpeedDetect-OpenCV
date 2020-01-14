#pragma once
#include <Windows.h>
#include <time.h>
#include <stdio.h>

#define SHOW_LOG_DATA

static WCHAR	g_wzLogWndClassName[] = L"LogWindow-apollo-v1.0";
static HWND		g_hLogWnd = NULL;

#define			MAX_MESSAGE_LEN		1024

#define			LOG_MESSAGE			0
#define			LOG_WARNING			1
#define			LOG_ERROR			2
#define			LOG_DATA_ID			'LOGD'

typedef struct _LOGDATA
{
	int			nType;
	DWORD		dwProcessId;
	WCHAR		wzFileName[MAX_PATH];
	int			nLine;
	time_t		timeLog;
	WCHAR		wzMessage[MAX_MESSAGE_LEN];
}	LOGDATA, FAR * LPLOGDATA;

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)

class CLogMaker
{
public:
	CLogMaker(int nType, const WCHAR * pwzFile, int nLine)
		:m_nType(nType),
		m_pwzFile(pwzFile),
		m_nLine(nLine)
	{
		time(&m_time);
	}

	void Out(LPCWSTR pFormat, ...)
	{
		LOGDATA logData;
		logData.nType = m_nType;
		logData.dwProcessId = GetCurrentProcessId();
		wcscpy_s(logData.wzFileName, m_pwzFile);
		logData.nLine = m_nLine;
		logData.timeLog = m_time;
		va_list args;
		va_start(args, pFormat);
		vswprintf_s(logData.wzMessage, pFormat, args);
		va_end(args);

		COPYDATASTRUCT cpData;
		cpData.dwData = LOG_DATA_ID;
		cpData.cbData = sizeof(LOGDATA);
		cpData.lpData = &logData;

		if(SendMessage(g_hLogWnd, WM_COPYDATA, NULL, (LPARAM)(LPVOID)&cpData) == FALSE)
		{
			g_hLogWnd = ::FindWindowW(g_wzLogWndClassName, NULL);
			if(g_hLogWnd)
				SendMessage(g_hLogWnd, WM_COPYDATA, NULL, (LPARAM)(LPVOID)&cpData);
		}
	}

	void Out(LPCSTR pFormat, ...)
	{

		LOGDATA logData;
		logData.nType = m_nType;
		logData.dwProcessId = GetCurrentProcessId();
		wcscpy_s(logData.wzFileName, m_pwzFile);
		logData.nLine = m_nLine;
		logData.timeLog = m_time;
		CHAR szMessage[MAX_MESSAGE_LEN];

		va_list args;
		va_start(args, pFormat);

		vsprintf_s(szMessage, pFormat, args);
		swprintf_s(logData.wzMessage, L"%S", szMessage);
		va_end(args);

		COPYDATASTRUCT cpData;
		cpData.dwData = LOG_DATA_ID;
		cpData.cbData = sizeof(LOGDATA);
		cpData.lpData = &logData;

		if(SendMessage(g_hLogWnd, WM_COPYDATA, NULL, (LPARAM)(LPVOID)&cpData) == FALSE)
		{
			g_hLogWnd = ::FindWindowW(g_wzLogWndClassName, NULL);
			if(g_hLogWnd)
				SendMessage(g_hLogWnd, WM_COPYDATA, NULL, (LPARAM)(LPVOID)&cpData);
		}
	}
protected:
	int				m_nType;
	const WCHAR*	m_pwzFile;
	int				m_nLine;
	time_t			m_time;
};

#ifdef SHOW_LOG_DATA
	#define		LOGMSG	CLogMaker(LOG_MESSAGE, __WFILE__, __LINE__).Out
	#define		LOGERR	CLogMaker(LOG_ERROR, __WFILE__, __LINE__).Out
	#define		LOGWAR	CLogMaker(LOG_WARNING, __WFILE__, __LINE__).Out
#else
	#define		LOGMSG	__noop
	#define		LOGERR	__noop
	#define		LOGWAR	__noop
#endif
