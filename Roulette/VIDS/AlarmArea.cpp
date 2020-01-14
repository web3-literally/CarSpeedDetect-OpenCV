#include "StdAfx.h"
#include "AlarmArea.h"
#include <sstream>

CAlarmArea::CAlarmArea(void)
{
	m_bEnable = FALSE;
	m_bMoving = TRUE;
}

CAlarmArea::~CAlarmArea(void)
{
}

void CAlarmArea::AddPoint(const PointF & pt)
{
	m_points.push_back(pt);
	SetEnable(TRUE);
	m_bMoving = FALSE;
}

vector<PointF> & CAlarmArea::Points()
{
	return m_points;
}

PointF CAlarmArea::GetPointF(int pt_id)
{
	return m_points[pt_id];
}

CString CAlarmArea::GetName() const
{
	return m_strName;
}

void CAlarmArea::SetName(const CString & str)
{
	m_strName = str;
}

BOOL CAlarmArea::IsEmpty() const
{
	int pts = (int)m_points.size();
	if(m_bMoving)
		pts --;
	if(pts < 2)
		return TRUE;
	return FALSE;
}

void CAlarmArea::SetEnable(BOOL bEnable)
{
	m_bEnable = bEnable;
}

BOOL CAlarmArea::IsEnable() const
{
	return m_bEnable;
}

void CAlarmArea::Reset()
{
	m_points.clear();
	m_bEnable = FALSE;
}

void CAlarmArea::MovePoint(const PointF & pt)
{
	if(m_points.empty())
		m_points.push_back(pt);
	else
		m_points.back() = pt;
	m_bMoving = TRUE;
}

void CAlarmArea::CancelPoint()
{
	if(m_bMoving && m_points.empty() == false)
		m_points.pop_back();
	m_bMoving = FALSE;
	//m_bEnable = (m_points.size() >= 2);
}

void CAlarmArea::ValidEnable()
{
	m_bEnable = (m_points.size() >= 2);
}

CString CAlarmArea::ExportToString()
{
	CancelPoint();
	CString str = m_strName + _T("| ");
	CString s;
	wostringstream outstr;
	outstr << m_bEnable << _T(' ') ;
	for (vector<PointF>::iterator itr = m_points.begin(); itr != m_points.end(); itr ++)
	{
		outstr << itr->X << _T(' ') << itr->Y << _T(' ') ;
	}

	str += outstr.str().c_str();
	return str;
}

/*BOOL CAlarmArea::ImportFromString(const CString &str)
{
	m_bMoving = FALSE;
	CString s = str;
	int pos = s.Find(_T('|'));
	if(pos < 0)
		return FALSE;
	m_strName = s.Left(pos);
	s = s.Mid(pos + 1);

	string 
	= s;
	istringstream instr(wstr);
	instr >> (int)m_bEnable;
	while(!instr.eof())
	{
		istringstream::pos_type pos = instr.tellg();
		PointF pt;
		pt.X = -1;
		pt.Y = -1;
		instr >> pt.X;
		instr >> pt.Y;
		if(instr.tellg() == pos)
			break;
		if(pt.X >= 0 && pt.Y >= 0)
			m_points.push_back(pt);
	}

	return !IsEmpty();
}
*/