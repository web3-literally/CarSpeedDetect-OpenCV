#pragma once

#include "afxwin.h"
#include <vector>
#include "cv_header.h"
#include "opencv2/core/mat.hpp"
#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/samples_utility.hpp>
#include "afxcmn.h"

#include "./VIDS/Include/KipPeopleDetector.h"
#include "./VIDS/PeopleTracker.h"


#define MAX_SIZE 65536
#pragma comment(lib,"libmysql")
using namespace std;


#define VIDS_PARAM_ENABLE				"enable"
#define VIDS_PARAM_HISTORY				"history"
#define VIDS_PARAM_NOISE				"noise"
#define VIDS_PARAM_TWINKLE				"twinkle"
#define VIDS_PARAM_THRESHOLD1			"threshold1"
#define VIDS_PARAM_THRESHOLD2			"threshold2"
#define VIDS_PARAM_MIN_HEADSIZE			"min_head_size"
#define VIDS_PARAM_MAX_HEADSIZE			"max_head_size"
#define VIDS_PARAM_VIEW_ALARMAREA		"view_alarm_area"
#define VIDS_PARAM_VIEW_INALARMAREA		"view_inalarm_area"

#define VIDS_PARAM_GLOBAL_ENABLE		"global_enable"
#define VIDS_PARAM_ALARAM_VIEW_DURATION	"alarm_view_duration"
#define VIDS_PARAM_ALARAM_SOUND_REPEAT	"alarm_sound_repeat"
#define VIDS_PARAM_ALARAM_FILE_NAME		"alarm_file_name"

#define VIDS_PARAM_INIFILE_NAME			"vids_param.ini"
#define VIDS_PARAM_GLOBAL_SECTION		"global"

struct VidsParam
{
	long enable;
	long history;
	long noise;
	long twinkle;
	long threshold1;
	long threshold2;
	long minHeadSize;
	long maxHeadSize;
	long view_alarm_area;
	long view_inalarm_area;
	char alarm_file_name[MAX_PATH];
	static long global_enable;
	static long alarm_view_duration;
	static long alarm_sound_repeat;
	long alarm_processed;
};


enum RECO_State
{
	PAI_READY,
	PAI_LOAD,
	PAI_PLAYING,
	PAI_PAUSED,
	PAI_STOP,
	PAI_TRAIN,
	PAI_CHAEK,
	PAI_TEXT_AREA_SELECT,
	PAI_TITLE_AREA_SELECT
};

class SpeedDetectionZone
{
public:

	SpeedDetectionZone()
	{
		x1 = cv::Point(0, 0);
		x2 = cv::Point(0, 0);
		x3 = cv::Point(0, 0);
		x4 = cv::Point(0, 0);
	}
	~SpeedDetectionZone()
	{
	}
	cv::Point x1, x2, x3, x4;
	double distance;
	double input_distance;
};

class CarsClass
{
public:
	CarsClass()
	{
		state = 0;
		zoneindex = -1;
	}
	cv::Rect rtfirstDectect, rt;
	cv::Point ptfirstDectect, comppt;
	double starttime;
	double endtime;
	int state;//1=rec start,2=rec stop
	int zoneindex;
};

class CaptureThread;
// CRouletteDlg dialog
class CRouletteDlg : public CDialogEx
{
// Construction
public:
	CRouletteDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ROULETTE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
// Implementation
protected:
	HICON m_hIcon;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg void	OnBtnClickedBtnFileOpen();
	afx_msg void	OnBtnClickedPlayBtn();
	afx_msg void	OnBnClickedStopBtn2();
	afx_msg void	OnBnClickedCheck2();
	afx_msg void	OnBnClickedApply3();
	afx_msg void	OnBnClickedApply2();
	afx_msg void	OnBnClickedRadio1();
	afx_msg void	OnBnClickedRadio2();
	afx_msg LRESULT OnTerminate(WPARAM w, LPARAM l);
	afx_msg LRESULT OnRedraw(WPARAM w, LPARAM l);
	
	void		LoadVIDSParam();
	void		StartVIDS();
	cv::Mat		getPerspectiveDetectZone(cv::Mat bgFrame);
	int			initVariable();
	bool		Redraw();
	double		calcRealDistance(cv::Point, cv::Point, SpeedDetectionZone);
	double		calcPerspectiveRealDistance(cv::Point, cv::Point);
	void		EvaluatePos(Mat src, cv::Rect carbound, double frameTime);
	cv::Point	getCenter(cv::Rect rt);
	bool		isContainVect(Mat src, cv::Rect rt, double frameTime);
	cv::Point	getCompPt(cv::Rect rt);
	void		RecCore();
	double		dis(double x1, double y1, double x2, double y2);

	static DWORD WINAPI ThreadProc(LPVOID param);
public:
	CStatic			m_media;
	CString			m_strVideoPath;
	float			distance;
	CListBox		m_listSpeedLog;
	CButton			m_radio1;
	CButton			m_radio2;
	CButton			m_setcheck;

	VideoCapture*	m_videoCapture;
	Mat				m_curFrame;
	PeopleTracker	*m_peopleTracker;	
	int				threadstate;
	double			curframetime;
	RECO_State  	m_curState;
	CCriticalSection	m_secPeopleTracker;
private:
	volatile bool bScan;
	HANDLE m_hRecThread;

	CPoint			m_firstPoint;
	CPoint			m_endPoint;
	int				m_nCarNotFountCnt;
	Mat				localFrame, tmpframe;
	int				off_x, off_y, tmp_x, tmp_y;
	vector<cv::Point> ptVertexv;
	double			zx, zy;
	bool			m_bVideoPlay;
	bool			m_bVideoLoad;
	CaptureThread*	m_captureThread;
	bool			recflag;
	bool			isloaddetectinfo = false;
	VidsParam		m_vidsParam;
	vector<CRect>	m_cars;
	bool			isfisrtdetect = false;
	KipImageInfo	info;
	vector<CarsClass> m_carsvect;
	CarsClass		c;
	SpeedDetectionZone m_detectzone;
	CPoint			ptVertex[4];
	cv::Point		x1, x2, x3, x4;
	bool			isdown = false;
	int				isline12 = 0;

	CPen*		pOldPen;
	CBitmap*	pOldBitmap;
	CDC*		dc;
	CPen		pen;
	///////////////////////////////////Perspective Variable///////////////////////////////////////
	cv::Mat matTransform;
	std::vector<cv::Point2f> m_hintPoints;
	std::vector<cv::Point2f> m_persDetectPoints;
	cv::Size destSize;
	cv::Mat matTest;
};
