#pragma once

// CameraPhotoPropPage.h : CCameraPhotoPropPage ����ҳ���������


// CCameraPhotoPropPage : �й�ʵ�ֵ���Ϣ������� CameraPhotoPropPage.cpp��

class CCameraPhotoPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CCameraPhotoPropPage)
	DECLARE_OLECREATE_EX(CCameraPhotoPropPage)

// ���캯��
public:
	CCameraPhotoPropPage();

// �Ի�������
	enum { IDD = IDD_PROPPAGE_CAMERAPHOTO };

// ʵ��
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ��Ϣӳ��
protected:
	DECLARE_MESSAGE_MAP()
};

