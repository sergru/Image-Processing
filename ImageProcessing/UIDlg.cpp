
// UIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImageProcessing.h"
#include "UIDlg.h"
#include "afxdialogex.h"
#include "CommonDefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUIDlg dialog
LPCTSTR STR_FILE_DIALOG_FILTER = _T("PNG Files(*.png)|*.png|JPEG Files(*.jpg)|*.jpg|Image Files(*.jpg, *.png)|*.jpg; *.png|All Files(*.*)|*.*||");
LPCTSTR STR_DEFAULT_EXTENSION = _T(".png");


CUIDlg::CUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IMAGEPROCESSING_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

  m_pInFileCtrl = NULL;
  m_pOutFileCtrl = NULL;

}

// ====================================================================================================================
void CUIDlg::AddIPInterface(IIPInterface* pIPInterface)
{
  m_pIPInterface = pIPInterface;
}

void CUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CUIDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
  ON_WM_SIZE()
  ON_WM_CLOSE()
  ON_BN_CLICKED(IDC_BTN_INPUT_FILE, &CUIDlg::OnBnClickedBtnInputFile)
  ON_BN_CLICKED(IDC_BTN_OUTPUT_IMAGE, &CUIDlg::OnBnClickedBtnOutputImage)
  ON_CBN_SELCHANGE(IDC_COMBO_ACTIONS, &CUIDlg::OnCbnSelchangeComboActions)
  ON_BN_CLICKED(IDC_BTN_ACTION_RUN, &CUIDlg::OnBnClickedBtnActionRun)
  ON_BN_CLICKED(IDC_BTN_CALIBRATION_IMAGE, &CUIDlg::OnBnClickedBtnCalibrationImage)
END_MESSAGE_MAP()

// ====================================================================================================================
void CUIDlg::UpdateState()
{
  bool fEnabled = 
    *m_pIPInterface->m_psbActionRunning == true ||
    !m_pInFileCtrl->GetWindowTextLength() ||
    !m_pOutFileCtrl->GetWindowTextLength() ||
    m_pActionCtrl->GetCurSel() == CB_ERR ?
    false : true;

  if (m_fEnableCalibrationFile)
  {
    fEnabled = fEnabled && (m_pCalibFileCtrl->GetWindowTextLength() > 0);
  }

  m_pActionRunCtrl->EnableWindow(fEnabled);

}


// ====================================================================================================================
void CUIDlg::UIManager_ClearCalibrationFile()
{
  GetDlgItem(IDC_EDIT_CALIBRATION_FILE)->SetWindowText(_T(""));
  UpdateState();
}

// ====================================================================================================================
void CUIDlg::UIManager_ClearInputFile()
{
  GetDlgItem(IDC_EDIT_INPUT_FILE)->SetWindowText(_T(""));
  UpdateState();
}

// ====================================================================================================================
void CUIDlg::UIManager_ClearOutputFile()
{
  GetDlgItem(IDC_EDIT_OUTPUT_FILE)->SetWindowText(_T(""));
  UpdateState();
}

// ====================================================================================================================
void CUIDlg::UIManager_ClearAction()
{

  GetDlgItem(IDC_EDIT_OUTPUT_FILE)->SetWindowText(_T(""));
  UpdateState();
}

// ====================================================================================================================
void CUIDlg::UIInterface_ActionStart()
{
  UpdateState();
}

// ====================================================================================================================
void CUIDlg::UIManager_ActionDone()
{
  UpdateState();
}

// ====================================================================================================================
CString CUIDlg::ShowFileDialog(BOOL isOpenFile, DWORD dwInfoDlgItemId)
{
  DWORD dwOFFlags = 0;
  if (isOpenFile) {
    dwOFFlags = OFN_FILEMUSTEXIST;
  }
  else
  {
    dwOFFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
  }

  CFileDialog dlg(isOpenFile, STR_DEFAULT_EXTENSION, NULL, dwOFFlags, STR_FILE_DIALOG_FILTER, this);
  if (dlg.DoModal() == IDOK)
  {
    CString sFilePath = dlg.GetPathName();
    GetDlgItem(dwInfoDlgItemId)->SetWindowText(sFilePath);
    return sFilePath;
  }
  return _T("");
}

// ====================================================================================================================
void CUIDlg::OnPrintThreadFinished()
{
  PostMessage(WM_CLOSE, 0, 0);
}


// CUIDlg message handlers

BOOL CUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

  m_pEditCtrl = (CEdit*)GetDlgItem(IDC_EDIT_INFO);
  if (!m_pEditCtrl)
  {
    MessageBoxA(NULL, "Cannot get info control", "OnInitDialog ERROR", MB_OK | MB_ICONSTOP);
    PostMessage(WM_QUIT);
  }
  else
  {
    PrintInfoInit(m_pEditCtrl);

    CRect rt;
    m_pEditCtrl->GetWindowRect(&rt);

    CRect rtd;
    GetClientRect(&rtd);

    m_szInfoToBorder.cx = rtd.Width() - rt.Width();
    m_szInfoToBorder.cy = rtd.Height() - rt.Height();

    m_pInFileCtrl = (CEdit*)GetDlgItem(IDC_EDIT_INPUT_FILE);
    m_pInFileCtrl->GetWindowRect(&rt);
    m_lInFileBorderWidth = rt.left + rtd.right - rt.right;

    m_pOutFileCtrl = (CEdit*)GetDlgItem(IDC_EDIT_OUTPUT_FILE);
    m_pOutFileCtrl->GetWindowRect(&rt);
    m_lOutFileBorderWidth = rt.left + rtd.right - rt.right;

    m_pCalibFileCtrl = (CEdit*)GetDlgItem(IDC_EDIT_CALIBRATION_FILE);
    m_pCalibFileCtrl->GetWindowRect(&rt);
    m_lCalibFileBorderWidth = rt.left + rtd.right - rt.right;

    m_fEnableCalibrationFile = false;
    m_pBtnCalibrationFile = (CButton*)GetDlgItem(IDC_BTN_CALIBRATION_IMAGE);
    m_pBtnCalibrationFile->EnableWindow(m_fEnableCalibrationFile);
  }

  m_pActionCtrl = (CComboBox*)GetDlgItem(IDC_COMBO_ACTIONS);
  for (int ix = 0; ix < NUMELMS(SUPPORTED_ACTIONS); ix++)
  {
    m_pActionCtrl->AddString(SUPPORTED_ACTIONS[ix].name);
  }

  m_pActionRunCtrl = (CButton*)GetDlgItem(IDC_BTN_ACTION_RUN);
  
  UpdateState();

  ThreadBase_Create(NULL);

  return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUIDlg::OnPaint()
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
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ====================================================================================================================
void CUIDlg::OnSize(UINT nType, int cx, int cy)
{
  CDialogEx::OnSize(nType, cx, cy);

  if (m_pEditCtrl)
  {
    SIZE szTextMain;
    szTextMain.cx = cx - m_szInfoToBorder.cx;
    szTextMain.cy = cy - m_szInfoToBorder.cy;
    m_pEditCtrl->SetWindowPos(NULL, 0, 0, szTextMain.cx, szTextMain.cy, SWP_NOMOVE | SWP_NOZORDER);

    CRect rt;
    m_pInFileCtrl->GetWindowRect(&rt);
    m_pInFileCtrl->SetWindowPos(NULL, 0, 0, cx - m_lInFileBorderWidth, rt.Height(), SWP_NOMOVE | SWP_NOZORDER);

    m_pOutFileCtrl->GetWindowRect(&rt);
    m_pOutFileCtrl->SetWindowPos(NULL, 0, 0, cx - m_lOutFileBorderWidth, rt.Height(), SWP_NOMOVE | SWP_NOZORDER);

    m_pCalibFileCtrl->GetWindowRect(&rt);
    m_pCalibFileCtrl->SetWindowPos(NULL, 0, 0, cx - m_lCalibFileBorderWidth, rt.Height(), SWP_NOMOVE | SWP_NOZORDER);
  }
}

// ====================================================================================================================
void CUIDlg::OnClose()
{
  if (IsPrintingDone() && !ThreadBase_IsActive())
  {
    __super::OnClose();
  }
  else
  {
    if (IsPrintingDone())
    {
      PrintInfoFreeAll();
    }
    else
    {
      FirePrintingQuit();
    }

    if (ThreadBase_IsActive())
    {
      ThreadBase_Destroy(true);
    }
    else
    {
      ThreadBase_FireThreadQuit();
    }
  }
}

// ====================================================================================================================
void CUIDlg::OnBnClickedBtnInputFile()
{
  CString strFilePath = ShowFileDialog(TRUE, IDC_EDIT_INPUT_FILE);

  if (strFilePath)
  {
    m_pIPInterface->IPInterface_SetInputFile(strFilePath);
  }
  UpdateState();
}

// ====================================================================================================================
void CUIDlg::OnBnClickedBtnOutputImage()
{
  CString strFilePath = ShowFileDialog(FALSE, IDC_EDIT_OUTPUT_FILE);

  if (strFilePath)
  {
    m_pIPInterface->IPInterface_SetOutputFile(strFilePath);
  }
  UpdateState();
}

// ====================================================================================================================
void CUIDlg::OnBnClickedBtnCalibrationImage()
{
  CString strFilePath = ShowFileDialog(TRUE, IDC_EDIT_CALIBRATION_FILE);

  if (strFilePath)
  {
    m_pIPInterface->IPInterface_SetCalibrationFile(strFilePath);
  }
  UpdateState();
}

// ====================================================================================================================
void CUIDlg::OnCbnSelchangeComboActions()
{
  CString strAction;
  int ix = m_pActionCtrl->GetCurSel();
  strAction = SUPPORTED_ACTIONS[ix].name;

  m_fEnableCalibrationFile = ACTIONS_ID_ADDITIVE_NOISE == SUPPORTED_ACTIONS[ix].id;
  m_pBtnCalibrationFile->EnableWindow(m_fEnableCalibrationFile);

  m_pIPInterface->IPInterface_SetAction(strAction);
  UpdateState();
}

// ====================================================================================================================
void CUIDlg::OnBnClickedBtnActionRun()
{
  m_pIPInterface->IPInterface_ActionRun();
  UpdateState();
}
