// CameraPhotoPropPage.cpp : CCameraPhotoPropPage ����ҳ���ʵ�֡�

#include "stdafx.h"
#include "CameraPhoto.h"
#include "CameraPhotoPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CCameraPhotoPropPage, COlePropertyPage)



// ��Ϣӳ��

BEGIN_MESSAGE_MAP(CCameraPhotoPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// ��ʼ���๤���� guid

IMPLEMENT_OLECREATE_EX(CCameraPhotoPropPage, "CAMERAPHOTO.CameraPhotoPropPage.1",
	0x5fcc58c6, 0xd78b, 0x4966, 0x9c, 0xaf, 0x2d, 0x6c, 0x35, 0x11, 0xab, 0x1)



// CCameraPhotoPropPage::CCameraPhotoPropPageFactory::UpdateRegistry -
// ��ӻ��Ƴ� CCameraPhotoPropPage ��ϵͳע�����

BOOL CCameraPhotoPropPage::CCameraPhotoPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_CAMERAPHOTO_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CCameraPhotoPropPage::CCameraPhotoPropPage - ���캯��

CCameraPhotoPropPage::CCameraPhotoPropPage() :
	COlePropertyPage(IDD, IDS_CAMERAPHOTO_PPG_CAPTION)
{
}



// CCameraPhotoPropPage::DoDataExchange - ��ҳ�����Լ��ƶ�����

void CCameraPhotoPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CCameraPhotoPropPage ��Ϣ�������
