#pragma once


// CDlgSet �Ի���

class CDlgSet : public CDialog
{
	DECLARE_DYNAMIC(CDlgSet)

public:
	CDlgSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSet();

// �Ի�������
	enum { IDD = IDD_DLG_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_nScale;
	int m_nWidth;
	int m_nHeight;
	bool GetParm(void);
	bool SetParm(void);
	afx_msg void OnBnClickedOk();
};
