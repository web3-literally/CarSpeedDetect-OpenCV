#include "stdafx.h"
#include "PeopleTracker.h"
#include "./Include/KipImageUtil.h"
#include "../LogOut.h"

#pragma comment (lib, "gdiplus.lib")
#pragma comment (lib, "./VIDS/Lib/KipPeopleDetector10.lib")

#define DEF_HEAD_SIZE_MAX		1000
#define DEF_HEAD_SIZE_MIN		1

PeopleTracker::PeopleTracker()
:m_eventDoneProcess(FALSE, TRUE)
,m_eventStop(FALSE, TRUE)
,m_eventUpdated(FALSE, TRUE)
{
	m_hThread = 0;
	m_hDetector = KipPD_Create();
	m_fViewHumanFactors = true;
	m_fViewAlarmArea = true;
	m_fViewInAlarmArea = true;

	memset(&m_imageInfo, 0, sizeof(m_imageInfo));
	memset(&m_drawImageInfo, 0, sizeof(m_drawImageInfo));
	m_nMaxHeadRatio = 100;
	m_nMinHeadRatio = 0;
	m_nAlarmDetection = 0;
	m_ptChasePople = CRect(0, 0, 0, 0);
	//m_imageInfo.width = 500;
	//m_imageInfo.height = 500;
}

PeopleTracker::~PeopleTracker()
{
	stop();
	clearPeopleData();
	KipImageFree(&m_imageInfo);
	KipImageFree(&m_drawImageInfo);
	CloseHandle(m_hThread);
	KipPD_Destroy(m_hDetector);
}

void PeopleTracker::StopTracker()
{
	
	/*clearPeopleData();
	
	if (m_hDetector){
		KipPD_Destroy(m_hDetector);
		m_hDetector = NULL;
	}*/
	//KipImageFree(&m_imageInfo);
	//KipImageFree(&m_drawImageInfo);
	////CloseHandle(m_hThread);
	//KipPD_Destroy(m_hDetector);
}

#define CALCU_EFF_WIDTH(effwidth, width, bpp) \
{ \
	effwidth = (int)ceil(width * bpp / 8.0); \
	if (effwidth % 4 != 0) \
	effwidth = (effwidth / 4 + 1) * 4; \
}

void PeopleTracker::setImageSize(const KipImageInfo * pImage)
{
	CSingleLock locker(&m_secImage);

	if(KipImageIsSameInfo(&m_imageInfo, pImage))
		return;
	KipImageFree(&m_imageInfo);
	m_imageInfo = KipImageCreate(pImage);
	m_drawImageInfo = KipImageCreate(pImage);
	clearPeopleData();
}

void PeopleTracker::clearPeopleData()
{
	CSingleLock _dataLock(&m_secData, TRUE);
	m_peopleData.clear();
}

void PeopleTracker::pushImageData(uint8 * pData, int len)
{
	CSingleLock _locker1(&m_eventDoneProcess, FALSE);
	if(_locker1.Lock(0) == FALSE)
		return;

	CSingleLock _locker(&m_secImage, TRUE);

	memcpy_s(m_imageInfo.pixel_addr, KipImageDataSize(&m_imageInfo), pData, min(KipImageDataSize(&m_imageInfo), len));
	m_eventUpdated.SetEvent();

}

void PeopleTracker::start()
{
	if(m_hThread == NULL)
		m_hThread = CreateThread(0, 0, ThreadProc, (LPVOID)this, 0, 0);
}

void PeopleTracker::stop()
{
	m_eventStop.SetEvent();
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	m_hThread = NULL;
}

void PeopleTracker::restart()
{
	start();
}

DWORD WINAPI PeopleTracker::ThreadProc(LPVOID param)
{
	((PeopleTracker*)param)->run();
	return 0;
}

void PeopleTracker::run()
{
	CSyncObject* pEvens[] = {&m_eventUpdated, &m_eventStop};
	CMultiLock _lock(pEvens, 2);
	DWORD dwEvent;
	m_eventDoneProcess.SetEvent();
	while (true)
	{
		dwEvent = _lock.Lock(INFINITE, FALSE);
		if(dwEvent == WAIT_OBJECT_0 + 1)
			return;

		m_eventDoneProcess.ResetEvent();
		m_eventUpdated.ResetEvent();
		CSingleLock _lock(&m_secImage, TRUE);
		int nCount;
		nCount = KipPD_Detect(m_hDetector, &m_imageInfo);
		vector<PeopleData> result;
		for (int i = 0; i < nCount; i ++)
		{
			PeopleData data = {0};
			KipPD_GetPeopleInfo(m_hDetector, i, &data.info);//********************************
			if(IsValidPeopleInfo(data))
				result.push_back(data);
		}
		{
			CSingleLock _lock1(&m_secDraw, TRUE);
			memcpy_s(m_drawImageInfo.pixel_addr, KipImageDataSize(&m_drawImageInfo), m_imageInfo.pixel_addr, KipImageDataSize(&m_imageInfo));
			updatePeopleData(result);
			PostMessage(m_hOutWnd, UM_CHANGE_PEOPLE_INFO, 0, 0);
		}

		determineAlram();
		m_eventDoneProcess.SetEvent();
	}
}


bool isSamePeople(const PeopleData & data1, const PeopleData & data2)
{
//	return false;
	RECT rc;
	if(IntersectRect(&rc, (RECT*)&data1.info.bound, (RECT*)&data2.info.bound) == FALSE)
		return false;
	return true;
}

void PeopleTracker::updatePeopleData(vector<PeopleData> & data)
{
	bool fSaveImage = false;
	CSingleLock _dataLock(&m_secData, TRUE);
	for(vector<PeopleData>::iterator itr = data.begin(); itr != data.end(); itr ++)
	{
		for(vector<PeopleData>::iterator itrr = m_peopleData.begin(); itrr != m_peopleData.end(); itrr++)
		{
			if(isSamePeople(*itr, *itrr))
			{
				itr->alram += itrr->alram;
//				itrr->alram = 0;
			}
		}
	}

	m_peopleData = data;
}

CCriticalSection * PeopleTracker::alarmAreaCriticalSection()
{
	return &m_secAlarmArea;
}

CCriticalSection * PeopleTracker::inalarmAreaCriticalSection()
{
	return &m_secInAlarmArea;
}

BOOL PeopleTracker::PeopleInAlarmArea(KipPeopleInfo* peopleinfo)
{
	CSingleLock _lock(&m_secAlarmArea, TRUE);
	for(list<CAlarmArea>::iterator itr = m_listAlarmArea.begin(); itr != m_listAlarmArea.end(); itr ++)
	{
		CAlarmArea & aa = *itr;
		if(aa.IsEnable() == FALSE)
			continue;
		vector<PointF> & pts = aa.Points();
		int nPoints = (int)pts.size();
		KipAlarmArea kaa;
		kaa.count = nPoints;
		kaa.points = new KipCoord2f[nPoints];
		for (int i = 0; i < nPoints; i ++)
		{
			kaa.points[i].x = pts[i].X;
			kaa.points[i].y = pts[i].Y;
		}

		if(KipPD_PeopleInAlarmArea(m_hDetector, peopleinfo, &kaa) == 0)
		{
			delete [] kaa.points;
			return TRUE;
		}
		
		delete [] kaa.points;
	}
	return FALSE;
}

void PeopleTracker::determineAlram()
{
	m_strAlramName = _T("");
	m_nAlarmDetection = 0;
	int nAlarms = 0;
	{
		CSingleLock _lock(&m_secAlarmArea, TRUE);
		for(list<CAlarmArea>::iterator itr = m_listAlarmArea.begin(); itr != m_listAlarmArea.end(); itr ++)
		{
			CAlarmArea & aa = *itr;
			if(aa.IsEnable() == FALSE)
				continue;
			vector<PointF> & pts = aa.Points();
			int nPoints = (int)pts.size();
			KipAlarmArea kaa;
			kaa.count = nPoints;
			kaa.points = new KipCoord2f[nPoints];
			for (int i = 0; i < nPoints; i ++)
			{
				kaa.points[i].x = pts[i].X;
				kaa.points[i].y = pts[i].Y;
			}

			for(vector<PeopleData>::iterator itrr = m_peopleData.begin(); itrr != m_peopleData.end(); itrr++)
			{
				PeopleData & peopleInfo = *itrr;
//				if(peopleInfo.alram > 0)
//					continue;
				if(KipPD_PeopleInAlarmArea(m_hDetector, &peopleInfo.info, &kaa) == 0)
				{
					if(nAlarms > 0)
						m_strAlramName += _T(";");
					m_strAlramName += aa.GetName();
					nAlarms ++;
					peopleInfo.alram ++;
					break;
				}
			}
			delete [] kaa.points;
		}
	}

	m_nAlarmDetection = nAlarms;
/*
	if(nAlarms > 0)
	{
		SendMessage(m_hOutWnd, UM_DETECT_PEOPLE_IN_ALARM_AREA, 0, 0);
	}*/
}

bool PeopleTracker::IsValidPeopleInfo(const PeopleData & data)
{
	int headSize = (int)sqrt((float)(data.info.bound.right - data.info.bound.left) * (data.info.bound.right - data.info.bound.left) +
		(data.info.bound.bottom - data.info.bound.top) * (data.info.bound.bottom - data.info.bound.top));
	headSize = (headSize * 100) / m_imageInfo.width;
	return m_nMinHeadRatio <= headSize && m_nMaxHeadRatio >= headSize;
}

void PeopleTracker::initialize()
{
	CSingleLock _dataLock(&m_secData, TRUE);
	if(m_hDetector)
		KipPD_InitBackground(m_hDetector);
	else
		m_hDetector = KipPD_Create();
	m_nAlarmDetection = 0;
}

void PeopleTracker::SetParam(const char * name, int value)
{
	if(strcmp(name, KIP_PD_PARAM_MIN_HEADSIZE) == 0)
		m_nMinHeadRatio = value;
	else if(strcmp(name, KIP_PD_PARAM_MAX_HEADSIZE) == 0)
		m_nMaxHeadRatio = value;
	else if(strcmp(name, KIP_PD_PARAM_VIEW_ALARMAREA) == 0)
		m_fViewAlarmArea = (value == 0) ? false : true;
	else if(strcmp(name, KIP_PD_PARAM_VIEW_INALARMAREA) == 0)
		m_fViewInAlarmArea = (value == 0) ? false : true;
	else
		KipPD_SetParam(m_hDetector, name, value);
}

int PeopleTracker::GetParam(const char * name)
{
	if(strcmp(name, KIP_PD_PARAM_MIN_HEADSIZE) == 0)
		 return m_nMinHeadRatio;
	else if(strcmp(name, KIP_PD_PARAM_MAX_HEADSIZE) == 0)
		return m_nMaxHeadRatio;
	else if(strcmp(name, KIP_PD_PARAM_VIEW_ALARMAREA) == 0)
		return m_fViewAlarmArea ? 1 : 0;
	else if(strcmp(name, KIP_PD_PARAM_VIEW_INALARMAREA) == 0)
		return m_fViewInAlarmArea ? 1 : 0;
	else	return KipPD_GetParam(m_hDetector, name);
}

void PeopleTracker::SetInAlarmAreaMask(BYTE* mask_buffer, int w, int h)
{
	//KipPD_SetDetectMask(m_hDetector, mask_buffer, w, h);
}

void PeopleTracker::SetInAlarmAreaMask(const KipAlarmArea* pAlarmArea, int nAlarmAreaCount, int w, int h)
{
	KipPD_SetDetectMask(m_hDetector, pAlarmArea, nAlarmAreaCount, w, h);
}

int PeopleTracker::getAlarmDetection()
{
	return m_nAlarmDetection;
}

CRect PeopleTracker::GetChasePeopleRect(BOOL type)
{
	CRect result(0, 0, 0, 0);
	int peoplecount = GetPeopleRectCount();

	if (peoplecount == 0)	return result;

	CSize camerasize = GetChaseCameraSize();
	CPoint camera_org_pt(camerasize.cx / 2, camerasize.cy / 2);
	int dis1, dis2;
	CPoint org_pt1, org_pt2;
	CRect rt;
	PeopleData people;
	int left(1200), top(1200), right(-1), bottom(-1);

	if (type){
		if (m_ptChasePople == CRect(0, 0, 0, 0)){
			for (int i = 0; i < peoplecount; i++){
				KipPD_GetPeopleInfo(m_hDetector, i, &people.info);
				if (PeopleInAlarmArea(&people.info)){
					result = CRect(people.info.bound.left, people.info.bound.top, people.info.bound.right, people.info.bound.bottom);	
					break;
				}
			}
			//result = CRect(m_ptTargetInitPos, CSize(0, 0));
		}else{
			result = m_ptChasePople;		
		}
		
		for (int i = 0; i < peoplecount; i++){
			KipPD_GetPeopleInfo(m_hDetector, i, &people.info);
			if (PeopleInAlarmArea(&people.info) == FALSE)
				continue;
			rt = CRect(people.info.bound.left, people.info.bound.top, people.info.bound.right, people.info.bound.bottom);

			org_pt1 = CPoint((result.left + result.right) / 2, (result.top + result.bottom) / 2);
			dis1 = (int)sqrt(pow((double)(camera_org_pt.x - org_pt1.x), 2) + pow((double)(camera_org_pt.y - org_pt1.y), 2));

			org_pt2 = CPoint((rt.left + rt.right) / 2, (rt.top + rt.bottom) / 2);
			dis2 = (int)sqrt(pow((double)(camera_org_pt.x - org_pt2.x), 2) + pow((double)(camera_org_pt.y - org_pt2.y), 2));

			if (dis2 < dis1)
				result = rt;
		}
		
	}else{
		bool bIsFirst(true);
		for (int i = 0; i < peoplecount; i++){
			KipPD_GetPeopleInfo(m_hDetector, i, &people.info);
			if (PeopleInAlarmArea(&people.info) == FALSE)
				continue;
			rt = CRect(people.info.bound.left, people.info.bound.top, people.info.bound.right, people.info.bound.bottom);
			if (!bIsFirst){
				org_pt1 = CPoint((result.left + result.right) / 2, (result.top + result.bottom) / 2);
				dis1 = (int)sqrt(pow((double)(camera_org_pt.x - org_pt1.x), 2) + pow((double)(camera_org_pt.y - org_pt1.y), 2));

				org_pt2 = CPoint((rt.left + rt.right) / 2, (rt.top + rt.bottom) / 2);
				dis2 = (int)sqrt(pow((double)(camera_org_pt.x - org_pt2.x), 2) + pow((double)(camera_org_pt.y - org_pt2.y), 2));

				if (dis2 < dis1)
					result = rt;
			}else{
				result = rt;
			}
			bIsFirst = false;

			/*if (people.info.bound.left < left)
				left = people.info.bound.left;
			if (people.info.bound.top < top)
				top = people.info.bound.top;
			if (people.info.bound.right > right)
				right = people.info.bound.right;
			if (people.info.bound.bottom > bottom)
				bottom = people.info.bound.bottom;*/
			//result = CRect(left, top, right, bottom);
		}
	}

	m_ptChasePople = result;
	return result;
}

int	PeopleTracker::GetPeopleRectCount()
{
	int nCount;

	nCount = KipPD_Detect(m_hDetector, &m_imageInfo);

	return nCount;
}

CSize PeopleTracker::GetChaseCameraSize()
{
	return CSize(m_imageInfo.width, m_imageInfo.height);	
}
vector<CRect> PeopleTracker::getCarsRect()
{
	CRect result(0, 0, 0, 0);
	PeopleData people;
	m_cars.clear();
	int peoplecount = GetPeopleRectCount();
	for (int i = 0; i < peoplecount; i++) {
		KipPD_GetPeopleInfo(m_hDetector, i, &people.info);
		if (people.info.bound.right - people.info.bound.left >10 && people.info.bound.bottom - people.info.bound.top >10)
		{
			result = CRect(people.info.bound.left, people.info.bound.top, people.info.bound.right, people.info.bound.bottom);
			m_cars.push_back(result);
		}
	}
	return m_cars;
}