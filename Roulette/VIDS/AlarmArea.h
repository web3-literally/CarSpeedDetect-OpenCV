#pragma once
#include <vector>
using namespace std;

class CAlarmArea
{
public:
	CAlarmArea(void);
	~CAlarmArea(void);
	void AddPoint(const PointF & pt);
	void MovePoint(const PointF & pt);
	void CancelPoint();
	vector<PointF> & Points();
	PointF GetPointF(int pt_id);
	void SetName(const CString & str);
	CString GetName() const;
	BOOL IsEmpty() const;
	void SetEnable(BOOL bEnable = TRUE);
	BOOL IsEnable() const;
	void Reset();
	CString ExportToString();
	BOOL ImportFromString(const CString &str);
	void ValidEnable();
public:
	vector<PointF>	m_points;
	CString			m_strName;
	BOOL			m_bEnable;
	BOOL			m_bMoving;
};
