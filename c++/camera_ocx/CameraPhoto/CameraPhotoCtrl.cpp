// CameraPhotoCtrl.cpp : CCameraPhotoCtrl ActiveX �ؼ����ʵ�֡�

#include "stdafx.h"
#include "CameraPhoto.h"
#include "CameraPhotoCtrl.h"
#include "CameraPhotoPropPage.h"

#include "Base64.h"
#define Picture_TempFileName _T("\\LX_CameraPHoto_Temp.jpg")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CCameraPhotoCtrl, COleControl)

// ��Ϣӳ��
BEGIN_MESSAGE_MAP(CCameraPhotoCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_EDIT, OnEdit)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_MESSAGE(OCM_COMMAND, &CCameraPhotoCtrl::OnOcmCommand)
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// ����ӳ��
BEGIN_DISPATCH_MAP(CCameraPhotoCtrl, COleControl)
	DISP_FUNCTION_ID(CCameraPhotoCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CCameraPhotoCtrl, "SetID", dispidSetID, SetID, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION_ID(CCameraPhotoCtrl, "Reset", dispidReset, Reset, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CCameraPhotoCtrl, "GetPhoto", dispidGetPhoto, GetPhoto, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CCameraPhotoCtrl, "GetVersion", dispidGetVersion, GetVersion, VT_BSTR, VTS_NONE)
END_DISPATCH_MAP()



// �¼�ӳ��

BEGIN_EVENT_MAP(CCameraPhotoCtrl, COleControl)
END_EVENT_MAP()



// ����ҳ

// TODO: ����Ҫ��Ӹ�������ҳ�����ס���Ӽ���!
BEGIN_PROPPAGEIDS(CCameraPhotoCtrl, 1)
	PROPPAGEID(CCameraPhotoPropPage::guid)
END_PROPPAGEIDS(CCameraPhotoCtrl)



// ��ʼ���๤���� guid

IMPLEMENT_OLECREATE_EX(CCameraPhotoCtrl, "CAMERAPHOTO.CameraPhotoCtrl.1",
	0x22fbae15, 0x359b, 0x45b1, 0x81, 0xa1, 0x76, 0xec, 0xa1, 0x96, 0x9a, 0x71)



// ����� ID �Ͱ汾

IMPLEMENT_OLETYPELIB(CCameraPhotoCtrl, _tlid, _wVerMajor, _wVerMinor)



// �ӿ� ID

const IID BASED_CODE IID_DCameraPhoto =
		{ 0x4BF027D0, 0x97A5, 0x4A8B, { 0x97, 0x9C, 0x93, 0x59, 0x42, 0x66, 0xC0, 0xFE } };
const IID BASED_CODE IID_DCameraPhotoEvents =
		{ 0x96DD868A, 0x6A81, 0x4D73, { 0x90, 0xB, 0x85, 0x8E, 0x16, 0x82, 0x13, 0xBC } };



// �ؼ�������Ϣ

static const DWORD BASED_CODE _dwCameraPhotoOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CCameraPhotoCtrl, IDS_CAMERAPHOTO, _dwCameraPhotoOleMisc)



// CCameraPhotoCtrl::CCameraPhotoCtrlFactory::UpdateRegistry -
// ��ӻ��Ƴ� CCameraPhotoCtrl ��ϵͳע�����

BOOL CCameraPhotoCtrl::CCameraPhotoCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: ��֤���Ŀؼ��Ƿ���ϵ�Ԫģ���̴߳������
	// �йظ�����Ϣ����ο� MFC ����˵�� 64��
	// ������Ŀؼ������ϵ�Ԫģ�͹�����
	// �����޸����´��룬��������������
	// afxRegInsertable | afxRegApartmentThreading ��Ϊ afxRegInsertable��

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_CAMERAPHOTO,
			IDB_CAMERAPHOTO,
			afxRegInsertable | afxRegApartmentThreading,
			_dwCameraPhotoOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// CCameraPhotoCtrl::CCameraPhotoCtrl - ���캯��

CCameraPhotoCtrl::CCameraPhotoCtrl()
{
	InitializeIIDs(&IID_DCameraPhoto, &IID_DCameraPhotoEvents);
	// TODO: �ڴ˳�ʼ���ؼ���ʵ�����ݡ�
}



// CCameraPhotoCtrl::~CCameraPhotoCtrl - ��������

CCameraPhotoCtrl::~CCameraPhotoCtrl()
{
	// TODO: �ڴ�����ؼ���ʵ�����ݡ�
}



// CCameraPhotoCtrl::OnDraw - ��ͼ����

void CCameraPhotoCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (!pdc)
		return;

	// TODO: �����Լ��Ļ�ͼ�����滻����Ĵ��롣
	//pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
	//pdc->Ellipse(rcBounds);
	DoSuperclassPaint(pdc, rcBounds);
}



// CCameraPhotoCtrl::DoPropExchange - �־���֧��

void CCameraPhotoCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Ϊÿ���־õ��Զ������Ե��� PX_ ������
}



// CCameraPhotoCtrl::OnResetState - ���ؼ�����ΪĬ��״̬

void CCameraPhotoCtrl::OnResetState()
{
	COleControl::OnResetState();  // ���� DoPropExchange ���ҵ���Ĭ��ֵ

	// TODO: �ڴ��������������ؼ�״̬��
}



// CCameraPhotoCtrl::AboutBox - ���û���ʾ�����ڡ���

void CCameraPhotoCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_CAMERAPHOTO);
	dlgAbout.DoModal();
}

int CCameraPhotoCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������

	// �����ؼ����Ի���
	m_MainDialog.Create(IDD_LX_CPMAINDLG_DIALOG, this);

	return 0;
}

void CCameraPhotoCtrl::OnDestroy()
{
	COleControl::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	m_MainDialog.EndDialog(TRUE);
}

DWORD CCameraPhotoCtrl::GetControlFlags()
{
	DWORD dwFlags = COleControl::GetControlFlags();

	// �ڻ�Ͳ��״̬֮�����ת��ʱ��
	// �������»��ƿؼ���
	dwFlags |= noFlickerActivate;
	return dwFlags;
}

BOOL CCameraPhotoCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.lpszClass = _T("STATIC");
	return COleControl::PreCreateWindow(cs);
}

// CCameraPhotoCtrl ��Ϣ�������

void CCameraPhotoCtrl::SetID(LPCTSTR id)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	this->m_MainDialog.m_id = id;
	this->m_MainDialog.UpdateData(FALSE);
}

void CCameraPhotoCtrl::Reset(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	this->m_MainDialog.ReSet();
}

BSTR CCameraPhotoCtrl::GetPhoto(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CStringA strResult;

	// TODO: �ڴ���ӵ��ȴ���������
	TCHAR path[MAX_PATH];
	CString myFilename;
	
	CFile NativeFile;

	char * pszInput = NULL;
	char * pszOutput = NULL;


	// TODO: �ڴ�������Դ���������
	try
	{
		GetTempPath(MAX_PATH,path);
		//GetModuleFileName(NULL, path, MAX_PATH);	
		//PathRemoveFileSpec(path);
		myFilename.Format(_T("%s%s"),path,Picture_TempFileName);

		DeleteFile(myFilename.GetBuffer());
		this->m_MainDialog.m_PhotoBitmapEx.SaveJPGFile(myFilename.GetBuffer());

		if (FALSE != NativeFile.Open(myFilename, CFile::modeRead))
		{
			pszInput = new char[CDC_ONCE_LEN+1];
			pszOutput = new char[DCD_ONCE_LEN+1];

			memset(pszInput, 0x00, CDC_ONCE_LEN+1);
			memset(pszOutput, 0x00, DCD_ONCE_LEN+1);

			while (1)
			{
				int iLen = 0;

				iLen = NativeFile.Read(pszInput, CDC_ONCE_LEN);

				int iDstLen = EncodeBase64((BYTE*)pszInput, iLen, (BYTE*)pszOutput);

				strResult.Format("%s",pszOutput);

				if (iLen < CDC_ONCE_LEN)
				{
					break;
				}
			}

			NativeFile.Close();

			delete [] pszInput;
			delete [] pszOutput;
		}
	}
	catch(...)
	{
		NativeFile.Close();
		if (pszInput != NULL)
			delete [] pszInput;
		if (pszOutput != NULL)
			delete [] pszOutput;
	}

	return strResult.AllocSysString();
}

BSTR CCameraPhotoCtrl::GetVersion(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;

	// TODO: �ڴ���ӵ��ȴ���������
	strResult = _T("1.1.0.1");

	return strResult.AllocSysString();
}

BOOL CCameraPhotoCtrl::IsSubclassedControl()
{
	return TRUE;
}

LRESULT CCameraPhotoCtrl::OnOcmCommand(WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32
	WORD wNotifyCode = HIWORD(wParam);
#else
	WORD wNotifyCode = HIWORD(lParam);
#endif

	// TODO: �ڴ˽�ͨ wNotifyCode��

	return 0;
}