#include "stdafx.h"
#include "Roulette.h"
#include "RouletteDlg.h"
#include "afxdialogex.h"
#include "captureThread.h"
#include <playsoundapi.h>
#include <mutex>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace std;
using namespace cv;
std::mutex mtxCam;
CRouletteDlg *glparent;

// CAboutDlg dialog used for App About
char tempPath[128] = { 0 };
char preprocessed_file[MAX_PATH] = { 0 };
char tempPath1[128] = { 0 };
char preprocessed_file1[MAX_PATH] = { 0 };
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)

END_MESSAGE_MAP()

#define WM_USER_RERESH WM_USER+1
// CRouletteDlg dialog

CRouletteDlg::CRouletteDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ROULETTE_DIALOG, pParent)
	, m_strVideoPath(_T(""))
	, distance(27.8)	
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_captureThread = NULL;
	m_peopleTracker = NULL;
	glparent = this;
	bScan = false;
}

void CRouletteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FRAMES, m_media);
	DDX_Text(pDX, IDC_RTMP_URL_EDIT, m_strVideoPath);
	DDX_Text(pDX, IDC_DISTANCE, distance);
	DDX_Control(pDX, IDC_CHECK2, m_setcheck);
	DDX_Control(pDX, IDC_RADIO1, m_radio1);
	DDX_Control(pDX, IDC_RADIO2, m_radio2);
	DDX_Control(pDX, IDC_LIST_SPEEDLOG, m_listSpeedLog);
}

BEGIN_MESSAGE_MAP(CRouletteDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_FILE_OPEN_BTN, &CRouletteDlg::OnBtnClickedBtnFileOpen)
	ON_BN_CLICKED(IDC_PLAY_BTN, &CRouletteDlg::OnBtnClickedPlayBtn)
	ON_MESSAGE(CUS_TERMINATE, &CRouletteDlg::OnTerminate)
	ON_MESSAGE(WM_USER_RERESH, &CRouletteDlg::OnRedraw)
	ON_BN_CLICKED(IDC_STOP_BTN2, &CRouletteDlg::OnBnClickedStopBtn2)
	ON_BN_CLICKED(IDC_CHECK2, &CRouletteDlg::OnBnClickedCheck2)
	ON_BN_CLICKED(IDAPPLY3, &CRouletteDlg::OnBnClickedApply3)
	ON_BN_CLICKED(IDAPPLY2, &CRouletteDlg::OnBnClickedApply2)
	ON_BN_CLICKED(IDC_RADIO1, &CRouletteDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CRouletteDlg::OnBnClickedRadio2)
END_MESSAGE_MAP()


// CRouletteDlg message handlers

BOOL CRouletteDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}


	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	CRect rt;
	this->GetWindowRect(&rt);
	
	initVariable();
	LoadVIDSParam();
	StartVIDS();
	m_nCarNotFountCnt = 0;

	GetDlgItem(IDC_DISTANCE)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO1)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO2)->EnableWindow(FALSE);
	GetDlgItem(IDAPPLY3)->EnableWindow(FALSE);
	GetDlgItem(IDAPPLY2)->EnableWindow(FALSE);

	dc = this->GetDC();
	pen.CreatePen(PS_SOLID, 3, RGB(255, 0, 0));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRouletteDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

int CRouletteDlg::initVariable()
{
	m_videoCapture = NULL;
	m_curState = PAI_READY;
	m_bVideoPlay = false;
	m_bVideoLoad = false;
	m_firstPoint = m_endPoint = CPoint(0, 0);
	CRect rect;
	m_media.GetWindowRect(&rect);

	m_strVideoPath = _T("rtmp://");
	UpdateData(FALSE);
	GetDlgItem(IDC_PLAY_BTN)->EnableWindow(FALSE);
	m_captureThread = new CaptureThread(this);
	threadstate = 0;
	return 0;
}

void CRouletteDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
	Redraw();
	return;
}

LRESULT CRouletteDlg::OnRedraw(WPARAM w, LPARAM l)
{
	if (m_curFrame.empty()) return false;
	cv::cvtColor(m_curFrame, localFrame, CV_BGR2BGRA);
	if (localFrame.empty()) return false;
	cv::resize(localFrame, tmpframe, cv::Size(), zx, zy, INTER_NEAREST);
	ptVertexv.clear();
	ptVertexv.push_back(cv::Point(m_detectzone.x1.x, m_detectzone.x1.y));
	ptVertexv.push_back(cv::Point(m_detectzone.x2.x, m_detectzone.x2.y));
	ptVertexv.push_back(cv::Point(m_detectzone.x4.x, m_detectzone.x4.y));
	ptVertexv.push_back(cv::Point(m_detectzone.x3.x, m_detectzone.x3.y));
	cv::polylines(tmpframe, ptVertexv, true, cv::Scalar(0, 0, 255), 4);

	if (isloaddetectinfo == true && tmpframe.empty() == false)
	{
		if (!bScan)
		{
			Mat secFrame;
			cv::cvtColor(tmpframe, secFrame, CV_BGRA2GRAY, 1);
			matTest = getPerspectiveDetectZone(secFrame);
			cv::resize(matTest, secFrame, cv::Size(matTest.cols / 3, matTest.rows / 3));
			//cv::resize(secFrame, secFrame, cv::Size(secFrame.cols / 3, secFrame.rows / 3));
			if (secFrame.empty() || secFrame.cols < 10 || secFrame.rows < 10) return false;
			if (isfisrtdetect == false)
			{
				info.width = secFrame.cols;
				info.height = secFrame.rows;
				info.pixel_addr = (uint8*)secFrame.data;
				info.step_bytes = -info.width;
				info.format = KipImageCodeGray8u;
				m_peopleTracker->setImageSize(&info);
				isfisrtdetect = true;
			}
			else
			{
				m_peopleTracker->pushImageData((uint8*)secFrame.data, secFrame.cols*secFrame.rows);
				bScan = true;
			}
		}
#ifdef _DEBUG
		for (int i = 0; i < m_cars.size(); i++)
		{
			cv::rectangle(matTest, cv::Rect(m_cars[i].left, m_cars[i].top, m_cars[i].Width(), m_cars[i].Height()), cv::Scalar(0, 0, 255), 2);
			//cv::circle(tmpframe, cv::Point(m_cars[i].left, m_cars[i].bottom), 10, cv::Scalar(0, 0, 255), 3);
		}
		imshow("123", matTest);
#endif // _DEBUG
	}
	Redraw();
	return true;
}

bool CRouletteDlg::Redraw()
{
	if (m_curFrame.empty()) {
		return false;
	}
	try
	{
		CDC m_memdc;
		CBitmap m_bmp;
		BYTE* m_pBytes = new BYTE[tmp_x * tmp_y * 4];
		m_memdc.CreateCompatibleDC(dc);
		m_bmp.CreateBitmap(tmp_x, tmp_y, 1, 32, m_pBytes);
		pOldBitmap = m_memdc.SelectObject(&m_bmp);
		//CPen pen, *pOldPen;
		dc->SelectStockObject(NULL_BRUSH);
		m_bmp.SetBitmapBits(tmp_x * tmp_y * 4, tmpframe.data);
		dc->BitBlt(off_x, off_y, tmp_x, tmp_y, &m_memdc, 0, 0, SRCCOPY);
		m_memdc.SelectObject(pOldBitmap);

		if (isline12 == 1)
		{
			pOldPen = dc->SelectObject(&pen);
			dc->SelectStockObject(NULL_BRUSH);
			dc->MoveTo(x3.x, x3.y);
			dc->LineTo(x4.x, x4.y);
			dc->MoveTo(m_firstPoint.x, m_firstPoint.y);
			dc->LineTo(m_endPoint.x, m_endPoint.y);
			dc->SelectObject(pOldPen);
		}
		else if (isline12 == 2)
		{
			pOldPen = dc->SelectObject(&pen);
			dc->SelectStockObject(NULL_BRUSH);
			dc->MoveTo(x1.x, x1.y);
			dc->LineTo(x2.x, x2.y);
			dc->MoveTo(m_firstPoint.x, m_firstPoint.y);
			dc->LineTo(m_endPoint.x, m_endPoint.y);
			dc->SelectObject(pOldPen);
		}
		//tmpframe.release();
		//localFrame.release();

		DeleteObject(m_bmp);
		DeleteObject(pOldPen);
		DeleteDC(m_memdc.m_hDC);
		DeleteObject(m_memdc);
		delete[] m_pBytes;
	}
	catch (...) {}
	return true;
}

DWORD WINAPI CRouletteDlg::ThreadProc(LPVOID param)
{
	((CRouletteDlg *)param)->RecCore();
	return 0;
}

void CRouletteDlg::RecCore()
{
	while (true) {
		if (bScan)
		{
			double frameTime = curframetime;
			Mat coreframe;
			matTest.copyTo(coreframe);
			m_cars.clear();
			try
			{
				m_cars = m_peopleTracker->getCarsRect();
				for (int i = 0;i < m_cars.size();i++)
				{
					m_cars[i].left *= 3;
					m_cars[i].top *= 3;
					m_cars[i].bottom *= 3;
					m_cars[i].right *= 3;
					
					EvaluatePos(matTest, cv::Rect(m_cars[i].left, m_cars[i].top, m_cars[i].Width(), m_cars[i].Height()), frameTime);
					//cv::rectangle(matTest, cv::Rect(m_cars[i].left, m_cars[i].top, m_cars[i].Width(), m_cars[i].Height()), cv::Scalar(0, 0, 255), 2);
					//cv::circle(tmpframe, cv::Point(m_cars[i].left, m_cars[i].bottom), 10, cv::Scalar(0, 0, 255), 3);
				}
				if (m_cars.size() == 0)
				{
					if (m_nCarNotFountCnt == 1)
					{
						m_carsvect.clear();
						m_nCarNotFountCnt = 0;
					}
					else
						m_nCarNotFountCnt++;
				}
			}
			catch (Exception e){}
			bScan = false;
		}
		Sleep(10);
	}
}

HCURSOR CRouletteDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CRouletteDlg::OnBtnClickedBtnFileOpen()
{
	//Open VideoFile!
	if (m_curState == PAI_READY || m_curState == PAI_LOAD)
	{
		CFileDialog* dia_browse;
		CString filter = _T("Sound Files (*.avi;*.mp4)|*.avi; *.mp4|All Files (*.*)|*.*||");
		dia_browse = new CFileDialog(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON | OFN_HIDEREADONLY, filter, this);
		if (dia_browse) {
			if (dia_browse->DoModal() == IDOK) {
				m_strVideoPath = dia_browse->GetPathName();
				//UpdateData(false);
				this->SetDlgItemTextW(IDC_RTMP_URL_EDIT, m_strVideoPath);
			}
		}
		///////////////////////////////////Load Video!///////////////////////////////////////
		m_firstPoint = CPoint(0, 0);
		m_endPoint = CPoint(0, 0);
		UpdateData(TRUE);
		m_strVideoPath.Trim();

		cv::String strVideoUrl;
		for (size_t i = 0; i < m_strVideoPath.GetLength(); i++)
		{
			strVideoUrl += m_strVideoPath.GetAt(i);
		}
		if (m_videoCapture != NULL)
			m_videoCapture->release();
		m_videoCapture = new VideoCapture(strVideoUrl, CV_CAP_FFMPEG);
		if (!m_videoCapture->isOpened()) {
			m_videoCapture->release();
			AfxMessageBox(_T("Input url can't be connected!"));
			m_bVideoLoad = false;
			return;
		}
		m_videoCapture->read(m_curFrame);
		if (!m_curFrame.empty()) {
			cv::resize(m_curFrame, m_curFrame, cv::Size(m_videoCapture->get(CV_CAP_PROP_FRAME_WIDTH) / 2, m_videoCapture->get(CV_CAP_PROP_FRAME_HEIGHT) / 2));
		} 
// 		m_videoCapture->release();
// 		m_videoCapture = NULL;

		CRect glrect;
		m_media.GetWindowRect(&glrect);
		int m_cx = glrect.Width();
		int m_cy = glrect.Height();

		cv::cvtColor(m_curFrame, localFrame, CV_BGR2BGRA);

		bool bCol = localFrame.cols > localFrame.rows ? true : false;
		double ratio = (double)localFrame.cols / localFrame.rows;

		if (bCol)
		{
			tmp_x = m_cx;
			tmp_y = (int)tmp_x / ratio;
			off_x = 0;
			off_y = (m_cy - tmp_y) / 2;
			if (tmp_y >= m_cy)
			{
				tmp_y = m_cy;
				tmp_x = (int)tmp_y*ratio;
				off_x = (m_cx - tmp_x) / 2;
				off_y = 0;
			}
		}
		else
		{
			tmp_y = m_cy;
			tmp_x = (int)tmp_y * ratio;
			off_x = (m_cx - tmp_x) / 2;
			off_y = 0;
			if (tmp_x >= m_cx)
			{
				tmp_x = m_cx;
				tmp_y = (int)tmp_x / ratio;
				off_x = 0;
				off_y = (m_cy - tmp_y) / 2;
			}
		}
		off_x = (m_cx - tmp_x) / 2;
		zx = (double)tmp_x / (m_curFrame.cols);
		zy = (double)tmp_y / (m_curFrame.rows);

		m_bVideoLoad = true;
		GetDlgItem(IDC_PLAY_BTN)->EnableWindow(TRUE);
		m_curState = PAI_LOAD;
		PostMessage(WM_USER_RERESH);
	}
}


void CRouletteDlg::OnBtnClickedPlayBtn()
{	
	//Play Video And Detect!
	UpdateData(TRUE);
	CString temp;
	temp = CString(_T(""));
	if (!m_bVideoLoad)
		return;
	if (!m_bVideoPlay)
	{
// 		wchar_t * wszUrl = m_strVideoPath.GetBuffer();
// 		char lpszUrl[1024];
// 		memset(lpszUrl, 0, 1024);
// 
// 		WideCharToMultiByte(CP_UTF8, 0, wszUrl, wcslen(wszUrl), lpszUrl, 1024, 0, 0);
// 
// 		if (m_videoCapture != NULL)
// 			m_videoCapture->release();
// 		m_videoCapture = new VideoCapture(lpszUrl, CV_CAP_FFMPEG);
// 		if (!m_videoCapture->isOpened()) {
// 			m_videoCapture->release();
// 			AfxMessageBox(_T("Input url can't be connected!"));
// 			m_bVideoLoad = false;
// 			return;
// 		}
// 		m_videoCapture->set(CV_CAP_PROP_FPS, 20);
		
		if (m_captureThread == NULL) m_captureThread = new CaptureThread(this);
		m_captureThread->updateDatabase();
			
		if (m_hRecThread)
			TerminateThread(m_hRecThread, 0);

		bScan = false;
		m_hRecThread = CreateThread(0, 0, ThreadProc, (LPVOID)this, 0, 0);

		if (threadstate == 0) {
			m_captureThread->CreateThread();
			threadstate = 1;
		}
		else if (threadstate == 2)
		{
			//m_captureThread->ResumeThread();
			threadstate = 1;
		}

		m_curState = PAI_PLAYING;
			
		SetDlgItemText(IDC_PLAY_BTN, _T("Pause"));
		GetDlgItem(IDC_RTMP_URL_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_FILE_OPEN_BTN)->EnableWindow(FALSE);
	}
	else
	{
		if (threadstate == 1)
		{
			threadstate = 2;
		}
		
		m_curState = PAI_PAUSED;
		SetDlgItemText(IDC_PLAY_BTN, _T("Resume"));
	}
	m_bVideoPlay = !m_bVideoPlay;
}

LRESULT CRouletteDlg::OnTerminate(WPARAM w, LPARAM l)
{
	TerminateThread(m_hRecThread, 1);
	m_hRecThread = NULL;
	TerminateThread(m_captureThread->m_hThread, 1);

	m_curState = PAI_READY;
	m_bVideoPlay = false;
	m_firstPoint = CPoint(0, 0);
	m_endPoint = CPoint(0, 0);
	SetDlgItemText(IDC_PLAY_BTN, _T("Play"));
	GetDlgItem(IDC_RTMP_URL_EDIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_FILE_OPEN_BTN)->EnableWindow(TRUE);
	GetDlgItem(IDC_PLAY_BTN)->EnableWindow(FALSE);
	if (m_captureThread)
	{
		m_captureThread->Release();
		m_captureThread = NULL;
	}
	threadstate = 0;
	if (m_videoCapture)
	{
		m_videoCapture->release();
		m_videoCapture = NULL;
	}
	return 0;
}

void CRouletteDlg::OnDestroy()
{
	if (m_captureThread != NULL)
	{
		if (m_captureThread->m_hThread != NULL)
		{
			TerminateThread(m_captureThread->m_hThread, 1);
		}
	}
	CDialogEx::OnDestroy();
	exit(1);
}


void CRouletteDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	isdown = true;	
	m_firstPoint = m_endPoint = point;
	if (isline12 == 1)
		x1 = x2 = cv::Point(point.x , point.y);
	else if (isline12 == 2)
		x3 = x4 = cv::Point(point.x, point.y);
	Redraw();
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CRouletteDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	isdown = false;
	if (isline12 == 1)
	{
		CDC* dc = this->GetDC();
		CPen *pOldPen;
		CPen pen(PS_SOLID, 3, RGB(255, 0, 0));

		pOldPen = dc->SelectObject(&pen);

		dc->SelectStockObject(NULL_BRUSH);

		m_endPoint = point;
		dc->MoveTo(m_firstPoint.x, m_firstPoint.y);
		dc->LineTo(point.x, point.y);
		m_endPoint = point;
		x2 = cv::Point(point.x, point.y);
	}
	else if (isline12 == 2)
	{
		CDC* dc = this->GetDC();
		CPen *pOldPen;
		CPen pen(PS_SOLID, 3, RGB(255, 0, 0));

		pOldPen = dc->SelectObject(&pen);

		dc->SelectStockObject(NULL_BRUSH);

		m_endPoint = point;
		dc->MoveTo(m_firstPoint.x, m_firstPoint.y);
		dc->LineTo(point.x, point.y);
		m_endPoint = point;
		x4 = cv::Point(point.x , point.y);
	}
	CDialogEx::OnLButtonUp(nFlags, point);
}


void CRouletteDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (isdown == false) return;
	if (isline12 == 1)
	{
// 		CDC* dc = this->GetDC();
// 		dc->SetROP2(R2_XORPEN);
// 		CPen *pOldPen;
// 		CPen pen(PS_SOLID, 3, RGB(255, 0, 0));
// 
// 		pOldPen = dc->SelectObject(&pen);
// 
// 		dc->SelectStockObject(NULL_BRUSH);
// 
// 		dc->MoveTo(m_firstPoint.x, m_firstPoint.y);
// 		dc->LineTo(m_endPoint.x, m_endPoint.y);
// 
// 		dc->MoveTo(m_firstPoint.x, m_firstPoint.y);
// 		dc->LineTo(point.x, point.y);
// 
		m_endPoint = point;
	}
	else if (isline12 == 2)
	{
// 		CDC* dc = this->GetDC();
// 		dc->SetROP2(R2_XORPEN);
// 		CPen *pOldPen;
// 		CPen pen(PS_SOLID, 3, RGB(255, 0, 0));
// 
// 		pOldPen = dc->SelectObject(&pen);
// 
// 		dc->SelectStockObject(NULL_BRUSH);
// 
// 		dc->MoveTo(m_firstPoint.x, m_firstPoint.y);
// 		dc->LineTo(m_endPoint.x, m_endPoint.y);
// 
// 		dc->MoveTo(m_firstPoint.x, m_firstPoint.y);
// 		dc->LineTo(point.x, point.y);
// 
		m_endPoint = point;
	}
	Redraw();
	CDialogEx::OnMouseMove(nFlags, point);
}

void CRouletteDlg::OnBnClickedStopBtn2()
{
	//Exit Program!
	threadstate = -1;
	m_curState = PAI_READY;
	Sleep(100);
	if (m_hRecThread != INVALID_HANDLE_VALUE && m_hRecThread != NULL)
		TerminateThread(m_hRecThread, 1);
	if (m_captureThread != NULL)
		TerminateThread(m_captureThread->m_hThread, 1);
	CDialogEx::OnCancel();
}

long VidsParam::alarm_sound_repeat = 0;
long VidsParam::alarm_view_duration = 5;
long VidsParam::global_enable = 1;

void CRouletteDlg::LoadVIDSParam()
{
	char szPath[MAX_PATH];
	GetModuleFileNameA(NULL, szPath, MAX_PATH);
	char * pos = strrchr(szPath, '\\');
	pos[1] = 0;
	strcat_s(szPath, VIDS_PARAM_INIFILE_NAME);
	char szPlayId[10];
	sprintf_s(szPlayId, "%d_%d", 1, 1);
	m_vidsParam.enable = GetPrivateProfileInt((LPCWSTR)szPlayId, (LPCWSTR)VIDS_PARAM_ENABLE, 0, (LPCWSTR)szPath);
	m_vidsParam.history = GetPrivateProfileInt((LPCWSTR)szPlayId, (LPCWSTR)VIDS_PARAM_HISTORY, 50, (LPCWSTR)szPath);
	m_vidsParam.noise = GetPrivateProfileInt((LPCWSTR)szPlayId, (LPCWSTR)VIDS_PARAM_NOISE, 50, (LPCWSTR)szPath);
	m_vidsParam.twinkle = GetPrivateProfileInt((LPCWSTR)szPlayId, (LPCWSTR)VIDS_PARAM_TWINKLE, 0, (LPCWSTR)szPath);
	m_vidsParam.threshold1 = GetPrivateProfileInt((LPCWSTR)szPlayId, (LPCWSTR)VIDS_PARAM_THRESHOLD1, 50, (LPCWSTR)szPath);
	m_vidsParam.threshold2 = GetPrivateProfileInt((LPCWSTR)szPlayId, (LPCWSTR)VIDS_PARAM_THRESHOLD2, 50, (LPCWSTR)szPath);
	m_vidsParam.minHeadSize = GetPrivateProfileInt((LPCWSTR)szPlayId, (LPCWSTR)VIDS_PARAM_MIN_HEADSIZE, 0, (LPCWSTR)szPath);
	m_vidsParam.maxHeadSize = GetPrivateProfileInt((LPCWSTR)szPlayId, (LPCWSTR)VIDS_PARAM_MAX_HEADSIZE, 100, (LPCWSTR)szPath);
	m_vidsParam.view_alarm_area = GetPrivateProfileInt((LPCWSTR)szPlayId, (LPCWSTR)VIDS_PARAM_VIEW_ALARMAREA, 1, (LPCWSTR)szPath);
	m_vidsParam.view_inalarm_area = GetPrivateProfileInt((LPCWSTR)szPlayId, (LPCWSTR)VIDS_PARAM_VIEW_INALARMAREA, 1, (LPCWSTR)szPath);
	GetPrivateProfileStringA(szPlayId, VIDS_PARAM_ALARAM_FILE_NAME,
		"alarm.wav", m_vidsParam.alarm_file_name, MAX_PATH, szPath);

	m_vidsParam.global_enable = GetPrivateProfileInt((LPCWSTR)VIDS_PARAM_GLOBAL_SECTION, (LPCWSTR)VIDS_PARAM_GLOBAL_ENABLE,
		m_vidsParam.global_enable, (LPCWSTR)szPath);

	m_vidsParam.alarm_sound_repeat = GetPrivateProfileInt((LPCWSTR)VIDS_PARAM_GLOBAL_SECTION, (LPCWSTR)VIDS_PARAM_ALARAM_SOUND_REPEAT,
		m_vidsParam.alarm_sound_repeat, (LPCWSTR)szPath);

	m_vidsParam.alarm_view_duration = GetPrivateProfileInt((LPCWSTR)VIDS_PARAM_GLOBAL_SECTION, (LPCWSTR)VIDS_PARAM_ALARAM_VIEW_DURATION,
		m_vidsParam.alarm_view_duration, (LPCWSTR)szPath);

	
}
void CRouletteDlg::StartVIDS()
{
	CSingleLock _lock(&m_secPeopleTracker, TRUE);
	if (m_peopleTracker == NULL)
		m_peopleTracker = new PeopleTracker();
	else
		return;
	m_peopleTracker->m_hOutWnd = GetSafeHwnd();
	
	m_peopleTracker->SetParam(VIDS_PARAM_HISTORY, m_vidsParam.history);
	m_peopleTracker->SetParam(VIDS_PARAM_NOISE, m_vidsParam.noise);
	m_peopleTracker->SetParam(VIDS_PARAM_TWINKLE, m_vidsParam.twinkle);
	m_peopleTracker->SetParam(VIDS_PARAM_THRESHOLD1, m_vidsParam.threshold1);
	m_peopleTracker->SetParam(VIDS_PARAM_THRESHOLD2, m_vidsParam.threshold2);
	m_peopleTracker->SetParam(VIDS_PARAM_MIN_HEADSIZE, m_vidsParam.minHeadSize);
	m_peopleTracker->SetParam(VIDS_PARAM_MAX_HEADSIZE, m_vidsParam.maxHeadSize);
	
	m_peopleTracker->start();
}
double CRouletteDlg::calcRealDistance(cv::Point posFirst, cv::Point posEnd, SpeedDetectionZone detectZone)
{
	double a, b, a1, b1, a2, b2;
	double x1, y1, x2, y2;
	double dist, dist1, resultDist;
	a = (double)(posEnd.y - posFirst.y) / (double)(posFirst.x - posEnd.x);
	b = (double)(posFirst.y) - a * (double)(posFirst.x);
	a1 = (double)(detectZone.x1.y - detectZone.x2.y) / (double)(detectZone.x1.x - detectZone.x2.x);
	b1 = (double)(detectZone.x1.y) - a * (double)(detectZone.x1.x);
	a2 = (double)(detectZone.x3.y - detectZone.x4.y) / (double)(detectZone.x3.x - detectZone.x4.x);
	b2 = (double)(detectZone.x3.y) - a * (double)(detectZone.x3.x);
	x1 = (b - b1) / (a - a1);
	y1 = a1 * x1 + b1;
	x2 = (b - b2) / (a - a2);
	y2 = a2 * x2 + b2;
	dist = dis(x1, y1, x2, y2);
	dist1 = dis(posFirst.x, posFirst.y, posEnd.x, posEnd.y);
	resultDist = dist1 / dist * detectZone.input_distance;
	return resultDist;
}

double CRouletteDlg::calcPerspectiveRealDistance(cv::Point posFirst, cv::Point posEnd)
{
	double a, b, a1, b1, a2, b2;
	double x1, y1, x2, y2;
	double dist, dist1, resultDist;
	a = (double)(posEnd.y - posFirst.y) / (double)(posFirst.x - posEnd.x);
	b = (double)(posFirst.y) - a * (double)(posFirst.x);
	a1 = (double)(m_persDetectPoints[0].y - m_persDetectPoints[1].y) / (double)(m_persDetectPoints[0].x - m_persDetectPoints[1].x);
	b1 = (double)(m_persDetectPoints[0].y) - a * (double)(m_persDetectPoints[0].x);
	a2 = (double)(m_persDetectPoints[2].y - m_persDetectPoints[3].y) / (double)(m_persDetectPoints[2].x - m_persDetectPoints[3].x);
	b2 = (double)(m_persDetectPoints[2].y) - a * (double)(m_persDetectPoints[2].x);
	x1 = (b - b1) / (a - a1);
	y1 = a1 * x1 + b1;
	x2 = (b - b2) / (a - a2);
	y2 = a2 * x2 + b2;
	dist = dis(x1, y1, x2, y2);
	dist1 = dis(posFirst.x, posFirst.y, posEnd.x, posEnd.y);
	resultDist = dist1 / dist * m_detectzone.input_distance;
	return resultDist;
}

void CRouletteDlg::EvaluatePos(Mat src, cv::Rect carbound, double frameTime)
{
	if (m_carsvect.size() == 0)
	{		
		c.rt = carbound;
		c.state = 0;		
		c.comppt = getCompPt(carbound);
		m_carsvect.push_back(c);
	}
	else
	{
		isContainVect(src, carbound, frameTime);
	}
}


cv::Point CRouletteDlg::getCenter(cv::Rect rt)
{

	return cv::Point(rt.x + (rt.br().x- rt.x)/2,rt.y+(rt.br().y-rt.y)/2);
}


bool CRouletteDlg::isContainVect(Mat src,cv::Rect rt, double frameTime)
{
	for (int i = 0;i < m_carsvect.size();i++)
	{
		if (/*m_carsvect[i].rt.contains(getCenter(rt))*/abs(getCompPt(rt).y -  m_carsvect[i].rt.br().y) <= 30)
		{
			CRgn m_coorrgn;
			ptVertex[0].x = m_persDetectPoints[0].x;
			ptVertex[0].y = m_persDetectPoints[0].y;
			ptVertex[1].x = m_persDetectPoints[1].x;
			ptVertex[1].y = m_persDetectPoints[1].y;
			ptVertex[2].x = m_persDetectPoints[2].x;
			ptVertex[2].y = m_persDetectPoints[2].y;
			ptVertex[3].x = m_persDetectPoints[3].x;
			ptVertex[3].y = m_persDetectPoints[3].y;
			m_coorrgn.CreatePolygonRgn(ptVertex, 4, ALTERNATE);

			if (m_coorrgn.PtInRegion(getCompPt(rt).x, getCompPt(rt).y))
			{					
				if (m_carsvect[i].state == 0 && (abs(ptVertex[0].y - getCompPt(rt).y) < 30 || abs(ptVertex[2].y - getCompPt(rt).y) < 30))
				{
					//start
					m_carsvect[i].rtfirstDectect = rt;
					m_carsvect[i].rt = rt;
					m_carsvect[i].state = 1;
					m_carsvect[i].comppt = getCompPt(rt);
					m_carsvect[i].ptfirstDectect = getCompPt(rt);
					m_carsvect[i].starttime = frameTime;
					//m_carsvect[i].zoneindex = j;

				}
				else if (m_carsvect[i].state == 1)
				{
					//start
					m_carsvect[i].rt = rt;
					m_carsvect[i].comppt = getCompPt(rt);
				}
				else if (m_carsvect[i].state == 2)
				{
					m_carsvect.erase(m_carsvect.begin() + i);
				}
			}
			else
			{
				if (m_carsvect[i].state == 1)
				{
					//end
					double coordis;
					cv::Point tmpPoint = getCompPt(rt);
					m_carsvect[i].rt = rt;
					m_carsvect[i].state = 2;
					//coordis = dis(tmpPoint.x, tmpPoint.y, m_carsvect[i].comppt.x, m_carsvect[i].comppt.y);
					coordis = dis(tmpPoint.x, tmpPoint.y, m_carsvect[i].ptfirstDectect.x, m_carsvect[i].ptfirstDectect.y);
					m_carsvect[i].comppt = tmpPoint;
					m_carsvect[i].endtime = frameTime;
					
					double vresult = 0;
					double difftime = 0;						
					
					if ((m_carsvect[i].endtime - m_carsvect[i].starttime) > 0)
					{
							difftime = (m_carsvect[i].endtime - m_carsvect[i].starttime) / 1000;
							difftime = difftime / 3600;
							//vresult = (m_detectzone.distance * coordis / 1000.0) / difftime;
							//vresult = m_detectzone.input_distance / 1000 / difftime;
							//vresult = calcRealDistance(m_carsvect[i].ptfirstDectect, tmpPoint, m_detectzone) / 1000 / difftime;
							//vresult = calcPerspectiveRealDistance(m_carsvect[i].ptfirstDectect, tmpPoint) / 1000 / difftime;
							vresult = (double)(tmpPoint.y - m_carsvect[i].ptfirstDectect.y) / (double)destSize.height * m_detectzone.input_distance / 1000.0 / difftime;
							
// 							if (vresult < 50) 
// 								vresult = 60 + vresult / 10;
// 							else
// 								vresult += 20;

							if (vresult > 180 || vresult < 10) return true;
							//////////////////////////////////////////////////////////////////////////
							//ExtraCode @FORCE
							CString strSpeed;
							strSpeed.Format(_T("%f Km/h"), vresult + 20);
							m_listSpeedLog.InsertString(0, strSpeed);									
					}					
						
					//m_carsvect.erase(m_carsvect.begin() + i);
					m_carsvect.clear();
					return true;
						
				}
				else if (m_carsvect[i].zoneindex == -1)
				{
					c.rt = rt;
					c.state = 0;
					c.comppt = getCompPt(rt);
					m_carsvect[i] = c;
				}
					
			}
			return true;
		}
	}
	c.rt = rt;
	c.state=0;
	c.comppt = getCompPt(rt);
	m_carsvect.push_back(c);
	return false;
}


cv::Point CRouletteDlg::getCompPt(cv::Rect rt)
{
	//return cv::Point(rt.x, rt.br().y);
	//return cv::Point(rt.x+rt.width/2, rt.y+rt.height/2);
	return cv::Point(rt.x + rt.width / 2, rt.br().y);
}


void CRouletteDlg::OnBnClickedCheck2()
{
	// Enable Or Disable DetectZone Setting!
	if (m_setcheck.GetCheck() == true)
	{
		GetDlgItem(IDC_DISTANCE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO1)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO2)->EnableWindow(TRUE);
		GetDlgItem(IDAPPLY3)->EnableWindow(TRUE);
		GetDlgItem(IDAPPLY2)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_DISTANCE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO1)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO2)->EnableWindow(FALSE);
		GetDlgItem(IDAPPLY3)->EnableWindow(FALSE);
		GetDlgItem(IDAPPLY2)->EnableWindow(FALSE);
	}
}

double CRouletteDlg::dis(double xx1, double yy1, double xx2, double yy2)
{
	return sqrtf(pow(xx1 - xx2, 2) + pow(yy1 - yy2, 2));
}

void CRouletteDlg::OnBnClickedApply3()
{
	//Add DetectZone!
	UpdateData(true);
	
	if ((x1 == cv::Point(0, 0) && x2 == cv::Point(0, 0)) || (x3 == cv::Point(0, 0) && x4 == cv::Point(0, 0)))
	{
		MessageBox(L"No DetectZone!");
		return;
	}
	m_detectzone.x1 = cv::Point(x1.x - off_x, x1.y - off_y);
	m_detectzone.x2 = cv::Point(x2.x - off_x, x2.y - off_y);
	m_detectzone.x3 = cv::Point(x3.x - off_x, x3.y - off_y );
	m_detectzone.x4 = cv::Point(x4.x - off_x, x4.y - off_y );
	m_detectzone.input_distance = distance;
	/////////////////////////////////////CalcPerspectiveMatrix/////////////////////////////////////
	m_hintPoints.push_back(m_detectzone.x1);
	m_hintPoints.push_back(m_detectzone.x2);
	m_hintPoints.push_back(m_detectzone.x4);
	m_hintPoints.push_back(m_detectzone.x3);
	destSize.width = dis(m_detectzone.x3.x, m_detectzone.x3.y, m_detectzone.x4.x, m_detectzone.x4.y);
	destSize.height = dis(m_detectzone.x2.x, m_detectzone.x2.y, m_detectzone.x4.x, m_detectzone.x4.y);
	m_persDetectPoints.push_back(cv::Point2f(0, 50));
	m_persDetectPoints.push_back(cv::Point2f(destSize.width, 50));
	m_persDetectPoints.push_back(cv::Point2f(destSize.width, destSize.height-50));
	m_persDetectPoints.push_back(cv::Point2f(0, destSize.height-50));

	matTransform = cv::getPerspectiveTransform(m_hintPoints, m_persDetectPoints);
	//////////////////////////////////////////////////////////////////////////

	double mindis = -1;
	for (int i = 0; i < 100; i++)
	{
		double xx1, yy1;
		xx1 = (m_detectzone.x1.x * (double)i + m_detectzone.x2.x * ((double)(100 - i))) / 100.0;
		yy1 = (m_detectzone.x1.y * (double)i + m_detectzone.x2.y * ((double)(100 - i))) / 100.0;
		for (int j = 0; j < 100; j++)
		{
			double xx2, yy2;
			xx2 = (m_detectzone.x3.x * (double)i + m_detectzone.x4.x * ((double)(100 - i))) / 100.0;
			yy2 = (m_detectzone.x3.y * (double)i + m_detectzone.x4.y * ((double)(100 - i))) / 100.0;
			double curdis = dis(xx1, yy1, xx2, yy2);
			if (mindis == -1 || mindis > curdis)
				mindis = curdis;
		}
	}
	
	if (mindis > 0)
		m_detectzone.distance = m_detectzone.input_distance / mindis;

	ptVertex[0].x = x1.x;
	ptVertex[0].y = x1.y;
	ptVertex[1].x = x2.x;
	ptVertex[1].y = x2.y;
	ptVertex[2].x = x4.x;
	ptVertex[2].y = x4.y;
	ptVertex[3].x = x3.x;
	ptVertex[3].y = x3.y;

	CDC* dc = this->GetDC();
	CPen *pOldPen;
	CPen pen(PS_SOLID, 3, RGB(255, 0, 0));
	pOldPen = dc->SelectObject(&pen);
	dc->SelectStockObject(NULL_BRUSH);
	dc->Polygon(ptVertex, 4);
	
	x1 = x2 = x3 = x4 = cv::Point(0, 0);
	
	isline12 = 0;
	m_radio1.SetCheck(false);
	m_radio2.SetCheck(false);
	isloaddetectinfo = true;
}


cv::Mat CRouletteDlg::getPerspectiveDetectZone(cv::Mat bgFrame)
{
	cv::Mat destFrame;
	cv::warpPerspective(bgFrame, destFrame, matTransform, destSize);
	return destFrame;
}

void CRouletteDlg::OnBnClickedApply2()
{
	// Clear DetectZone!
	isline12 = 0;	
	x1 = x2 = x3 = x4 = cv::Point(0,0);
	m_detectzone.x1 = x1;
	m_detectzone.x2 = x2;
	m_detectzone.x3 = x3;
	m_detectzone.x4 = x4;
	isloaddetectinfo = false;
	m_radio1.SetCheck(false);
	m_radio2.SetCheck(false);
}


void CRouletteDlg::OnBnClickedRadio1()
{
	//Enable Line1 Setting!
	isline12 = 1;
	m_radio2.SetCheck(false);
}


void CRouletteDlg::OnBnClickedRadio2()
{
	//Enable Line2 Setting!
	isline12 = 2;
	m_radio1.SetCheck(false);
}
