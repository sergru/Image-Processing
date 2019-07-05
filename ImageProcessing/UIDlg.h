#pragma once
#include "UIInfoPrinter.h"
#include "UIManager.h"
#include "IPInterface.h"

// CUIDlg dialog
class CUIDlg :
  public CDialogEx,
  public CUIInfoPrinter,
  public CUIManager
{
// Construction
public:
	CUIDlg(CWnd* pParent = nullptr);	// standard constructor

public: // methods
  void AddIPInterface(IIPInterface* pIPInterface);


// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IMAGEPROCESSING_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

protected: // methods
  void UpdateState();

  // CUIManager methods
  void UIManager_ClearCalibrationFile();
  void UIManager_ClearInputFile();
  void UIManager_ClearOutputFile();
  void UIManager_ClearAction();
  void UIInterface_ActionStart();
  void UIManager_ActionDone();

  CString ShowFileDialog(BOOL isOpenFile, DWORD dwInfoDlgItemId);
  void OnPrintThreadFinished();

protected: // members
  CSize m_szInfoToBorder;

  CEdit* m_pInFileCtrl;
  LONG m_lInFileBorderWidth;

  CEdit* m_pOutFileCtrl;
  LONG m_lOutFileBorderWidth;

  CEdit* m_pCalibFileCtrl;
  LONG m_lCalibFileBorderWidth;

  CButton* m_pBtnCalibrationFile;
  bool m_fEnableCalibrationFile;

  CComboBox* m_pActionCtrl;

  CButton* m_pActionRunCtrl;

  IIPInterface* m_pIPInterface;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnClose();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnBnClickedBtnInputFile();
  afx_msg void OnBnClickedBtnOutputImage();
  afx_msg void OnCbnSelchangeComboActions();
  afx_msg void OnBnClickedBtnActionRun();
  afx_msg void OnBnClickedBtnCalibrationImage();
};
