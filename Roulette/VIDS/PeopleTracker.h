#pragma once
#include <string>
#include <list>
#include <memory>
#include <vector>
#include <afxmt.h>
#include "AlarmArea.h"
#include <list>

#include "./Include/KipPeopleDetector.h"
//#pragma comment(lib, "../KipEngine/Lib/KipPeopleDetector10.lib")

#define KIP_PD_PARAM_MIN_HEADSIZE		"min_head_size"
#define KIP_PD_PARAM_MAX_HEADSIZE		"max_head_size"
#define KIP_PD_PARAM_VIEW_ALARMAREA		"view_alarm_area"
#define KIP_PD_PARAM_VIEW_INALARMAREA		"view_inalarm_area"

#define UM_CHANGE_PEOPLE_INFO			WM_USER + 100
#define UM_DETECT_PEOPLE_IN_ALARM_AREA	WM_USER + 101

using namespace std;
struct PeopleData
{
	KipPeopleInfo	info;
	int				alram;
};

class PeopleTracker
{
public:
	PeopleTracker();
	~PeopleTracker();
	void setImageSize(const KipImageInfo * pImage);
	void pushImageData(uint8 * pData, int len);
	void clearPeopleData();
	void start();
	void stop();
	void restart();
	//void draw(Graphics & g, const Rect & rcVideo, BOOL bViewBox);
	void initialize();
	int getAlarmDetection();
	// parameter
	void SetParam(const char * name, int value);
	int GetParam(const char * name);
	void SetInAlarmAreaMask(BYTE* mask_buffer, int w, int h);
	void SetInAlarmAreaMask(const KipAlarmArea* pAlarmArea, int nAlarmAreaCount, int w, int h);

	CCriticalSection * alarmAreaCriticalSection();
	CCriticalSection * inalarmAreaCriticalSection();

	//kk_4.11
	CRect	GetChasePeopleRect(BOOL type = TRUE);
	int		GetPeopleRectCount();
	CSize	GetChaseCameraSize();

	BOOL	PeopleInAlarmArea(KipPeopleInfo* peopleinfo);

static	DWORD WINAPI ThreadProc(LPVOID param);
	void run();
protected:
	void updatePeopleData(vector<PeopleData> & data);
	void determineAlram();
	bool IsValidPeopleInfo(const PeopleData & data);
public:
	KipImageInfo	m_imageInfo;
	KipImageInfo	m_drawImageInfo;
	CCriticalSection m_secImage;
	CCriticalSection m_secDraw;
	CCriticalSection m_secData;
	CCriticalSection m_secAlarmArea;
	CCriticalSection m_secInAlarmArea;
	CEvent	m_eventUpdated;
	CEvent	m_eventDoneProcess;
	CEvent	m_eventStop;
	HANDLE	m_hThread;
	KipEngineHandle	m_hDetector;
	vector<PeopleData>	m_peopleData;
	HWND	m_hOutWnd;
	bool	m_fViewHumanFactors;
	bool	m_fViewAlarmArea;
	bool	m_fViewInAlarmArea;
	list<CAlarmArea>	m_listAlarmArea;
	list<CAlarmArea>	m_listInAlarmArea;
	CString	m_strAlramName;
	int		m_nMinHeadRatio;
	int		m_nMaxHeadRatio;
	int		m_nAlarmDetection;
	CRect	m_ptChasePople;

	//2014-06-20///////////////////////////////////
	CPoint	m_ptTargetInitPos;

	void	SetTargetInitPos(CPoint point){m_ptTargetInitPos = point;}
	void	StopTracker();
	vector<CRect> getCarsRect();
	vector<CRect> m_cars;
	///////////////////////////////////////////////
};