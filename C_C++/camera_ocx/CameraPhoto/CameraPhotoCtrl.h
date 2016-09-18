#pragma once

// CameraPhotoCtrl.h : CCameraPhotoCtrl ActiveX �ؼ����������
#include "MainDlg.h"

// CCameraPhotoCtrl : �й�ʵ�ֵ���Ϣ������� CameraPhotoCtrl.cpp��

class CCameraPhotoCtrl : public COleControl
{
	DECLARE_DYNCREATE(CCameraPhotoCtrl)

// ���캯��
public:
	CCameraPhotoCtrl();

// ��д
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual DWORD GetControlFlags();

	CMainDlg m_MainDialog;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();

// ʵ��
protected:
	~CCameraPhotoCtrl();

	DECLARE_OLECREATE_EX(CCameraPhotoCtrl)    // �๤���� guid
	DECLARE_OLETYPELIB(CCameraPhotoCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CCameraPhotoCtrl)     // ����ҳ ID
	DECLARE_OLECTLTYPE(CCameraPhotoCtrl)		// �������ƺ�����״̬

	BOOL IsSubclassedControl();
	LRESULT OnOcmCommand(WPARAM wParam, LPARAM lParam);

// ��Ϣӳ��
	DECLARE_MESSAGE_MAP()

// ����ӳ��
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// �¼�ӳ��
	DECLARE_EVENT_MAP()

// ���Ⱥ��¼� ID
public:
	enum {
		dispidGetVersion = 4L,
		dispidGetPhoto = 3L,
		dispidReset = 2L,
		dispidSetID = 1L
	};
protected:
	void SetID(LPCTSTR id);
	void Reset(void);
	BSTR GetPhoto(void);
	BSTR GetVersion(void);
};

