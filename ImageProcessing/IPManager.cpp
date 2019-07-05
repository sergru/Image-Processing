#include "stdafx.h"
#include "IPManager.h"
#include "CommonDefs.h"
#include "IjsImage.h"
#include "IjsActionBase.h"
#include "IjsFilterBlur.h"
#include "IjsFilterSharpen.h"
#include "IjsFilterThresholding.h"
#include "IjsFilterSaltAndPepperNoiseRemoval.h"
#include "IjsFilterAdditiveNoiseRemoval.h"


// ====================================================================================================================
CIPManager::CIPManager() :
  m_pUIInterface (NULL)
{
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  m_iAvailabeCores = sysinfo.dwNumberOfProcessors;

  m_psbActionRunning = false;
  
  // We are not going to handle exceptions here. App just starting.
  // Just crash it to see what is wrong.
  if (FAILED(utils::MakeEvent(m_hEventUICalibrationFile)))
  {
    throw "Event creation error";
  }

  // We are not going to handle exceptions here. App just starting.
  // Just crash it to see what is wrong.
  if (FAILED(utils::MakeEvent(m_hEventUIInputFile)))
  {
    throw "Event creation error";
  }

  // We are not going to handle exceptions here. App just starting.
  // Just crash it to see what is wrong.
  if (FAILED(utils::MakeEvent(m_hEventUIOutputFile)))
  {
    throw "Event creation error";
  }

  if (FAILED(utils::MakeEvent(m_hEventUISetAction)))
  {
    throw "Event creation error";
  }

  if (FAILED(utils::MakeEvent(m_hEventUIActionRun)))
  {
    throw "Event creation error";
  }

  ThreadBase_Create(NULL);

//  m_ipWorker.Create(NULL);
}

// ====================================================================================================================
CIPManager::~CIPManager()
{
  ThreadBase_Destroy(true);

  utils::CloseEvent(m_hEventUICalibrationFile);
  utils::CloseEvent(m_hEventUIInputFile);
  utils::CloseEvent(m_hEventUIOutputFile);
  utils::CloseEvent(m_hEventUISetAction);
  utils::CloseEvent(m_hEventUIActionRun);
  
}

// ====================================================================================================================
void CIPManager::AddUIInterface(IUIInterface* pUIInterface)
{
  m_pUIInterface = pUIInterface;
}

// ====================================================================================================================
void CIPManager::IPInterface_SetCalibrationFile(LPCTSTR pszFile)
{
  m_strCalibrationFile = pszFile;
  SetEvent(m_hEventUICalibrationFile);
}

// ====================================================================================================================
void CIPManager::IPInterface_SetInputFile(LPCTSTR pszFile)
{
  m_strInputFile = pszFile;
  SetEvent(m_hEventUIInputFile);
}

// ====================================================================================================================
void CIPManager::IPInterface_SetOutputFile(LPCTSTR pszFile)
{
  m_strOutputFile = pszFile;
  SetEvent(m_hEventUIOutputFile);
}

// ====================================================================================================================
void CIPManager::IPInterface_SetAction(LPCTSTR pszAction)
{
  m_strAction = pszAction;
  SetEvent(m_hEventUISetAction);
}

// ====================================================================================================================
void CIPManager::IPInterface_ActionRun()
{
  SetEvent(m_hEventUIActionRun);
}

// ====================================================================================================================
DWORD CIPManager::ThreadBase_ThreadFunc(void* pParams)
{
  utils::SetThreadName("CIPManager");

  HANDLE hWaitObjects[] =
  {
      m_hEventThreadQuit,
      m_hEventUIInputFile,
      m_hEventUIOutputFile,
      m_hEventUISetAction,
      m_hEventUIActionRun,
      m_hEventUICalibrationFile,
  };

  DWORD dwThreadExitCode = EXIT_THREAD_S_OK;
  bool isContinue = true;
  while (isContinue)
  {
    DWORD dwWaitID = WaitForMultipleObjects(NUMELMS(hWaitObjects), hWaitObjects, FALSE, INFINITE);
    switch (dwWaitID)
    {
    case WAIT_OBJECT_0: // EventTheradQuit
      dwThreadExitCode = EXIT_THREAD_S_QUIT;
      isContinue = false;
      break;

    case WAIT_OBJECT_0 + 1: //m_hEventUIInputFile
      SetInputFile();
      break;

    case WAIT_OBJECT_0 + 2: //m_hEventUIOutputFile
      SetOutputFile();
      break;

    case WAIT_OBJECT_0 + 3: //m_hEventUISetAction
      SetActionName();
      break;

    case WAIT_OBJECT_0 + 4: //m_hEventUIActionRun
      RunAction();
      break;

    case WAIT_OBJECT_0 + 5: //m_hEventUICalibrationFile
      SetCalibrationFile();
      break;

    default:
      // Unexpected wait object state.
      DASSERT(FALSE);
      dwThreadExitCode = EXIT_THREAD_E_ERROR;
      isContinue = false;
    }

    if (!isContinue)
    {
      // Exit "isContinue" loop if other than EventWorkRun event has been raised.
      break;
    }
  }

  // TODO: Cleanup  the mess before exit and wait for Workers !!!

  return dwThreadExitCode;
}

// ====================================================================================================================
void CIPManager::SendMessageToUI(CString& strText)
{
  IUIInterface::SInfoOut* pSInfoOut = new IUIInterface::SInfoOut;
  pSInfoOut->pszOutText = new TCHAR[strText.GetLength() + 1];
  wcscpy_s(pSInfoOut->pszOutText, strText.GetLength() + 1, strText);
  pSInfoOut->fAddLinebreak = true;
  m_pUIInterface->UIInterface_InfoOut(IUIInterface::CSInfoOutPtr(pSInfoOut));
}

// ====================================================================================================================
void CIPManager::SendMessageToUI(LPCTSTR pszText)
{
  CString strText = pszText;
  SendMessageToUI(strText);
}

// ====================================================================================================================
void CIPManager::SetCalibrationFile()
{
  CString strText;

  CIjsImage image;
  HRESULT hr = image.ParseFile(m_strCalibrationFile);
  switch (hr)
  {
  case S_OK:
    break;

  case E_INVALIDARG:
    strText = _T("ERROR: Invalid calibration file name");
    break;

  case E_NOTIMPL:
    strText = _T("ERROR: Calibration file format is not supported");
    break;

  case E_ACCESSDENIED:
    strText = _T("ERROR: Cannot load calibration file");
    break;

  default:
    strText = _T("ERROR: Unhandled error");
    break;
  }

  if (strText.GetLength() > 0)
  {
    // Error. Release the file name and inform UI.
    m_strCalibrationFile.Empty();

    SendMessageToUI(strText);
    m_pUIInterface->UIInterface_CalibrationFileError();
    return;
  }

  SendMessageToUI(_T("Calibration file accepted"));
}

// ====================================================================================================================
void CIPManager::SetInputFile()
{
  CString strText;

  CIjsImage image;
  HRESULT hr = image.ParseFile(m_strInputFile);
  switch (hr)
  {
  case S_OK:
    if (!m_strOutputFile.IsEmpty())
    {
      if (m_strOutputFile.CompareNoCase(m_strInputFile) == 0)
      {
        strText = _T("ERROR: Input file name and output file name identical");
      }
    }
    break;

  case E_INVALIDARG:
    strText = _T("ERROR: Invalid input file name");
    break;

  case E_NOTIMPL:
    strText = _T("ERROR: Input file format is not supported");
    break;

  case E_ACCESSDENIED:
    strText = _T("ERROR: Cannot load input file");
    break;

  default:
    strText = _T("ERROR: Unhandled error");
    break;
  }

  if (strText.GetLength() > 0)
  {
    // Error. Release the file name and inform UI.
    m_strInputFile.Empty();

    SendMessageToUI(strText);
    m_pUIInterface->UIInterface_InputFileError();
    return;
  }

  SendMessageToUI(_T("Input file accepted"));
}

// ====================================================================================================================
void CIPManager::SetOutputFile()
{
  CString strText;

  HRESULT hr = S_OK;

  if (m_strOutputFile.IsEmpty())
  {
    strText = _T("ERROR: Empty output file name");
    hr = E_INVALIDARG;
  }

  if(SUCCEEDED(hr))
  {
    int ix = m_strOutputFile.ReverseFind(_T('.'));
    if (ix < 0)
    {
      strText = _T("ERROR: Output file does not have extension");
      hr = E_INVALIDARG;
    }
    else
    {
      CString strExt = m_strOutputFile.Right(m_strOutputFile.GetLength() - ix - 1);
      if (!CIjsImage::IsSupportedType(strExt))
      {
        strText = _T("ERROR: Unsupported output file format");
        hr = E_NOTIMPL;
      }
    }
  }

  if (SUCCEEDED(hr))
  {
    if (!m_strInputFile.IsEmpty())
    {
      if (m_strOutputFile.CompareNoCase(m_strInputFile) == 0)
      {
        strText = _T("ERROR: Output file name and input file name identical");
        hr = E_INVALIDARG;
      }
    }
  }

  if (FAILED(hr))
  {
    // Error. Release the file name and inform UI.
    m_strOutputFile.Empty();

    SendMessageToUI(strText);
    m_pUIInterface->UIInterface_OutputFileError();
    return;
  }

  SendMessageToUI(_T("Output file accepted"));
}

// ====================================================================================================================
void CIPManager::SetActionName()
{
  CString strText;

  HRESULT hr = S_OK;

  if (m_strAction.IsEmpty())
  {
    strText = _T("ERROR: Empty action name");
    hr = E_INVALIDARG;
  }

  if (SUCCEEDED(hr))
  {
    hr = E_NOTIMPL;
    strText = _T("ERROR: Unknown action name");

    for (int ix = 0; ix < NUMELMS(SUPPORTED_ACTIONS); ix++)
    {
      if (_tcsicmp(m_strAction, SUPPORTED_ACTIONS[ix].name) == 0)
      {
        hr = S_OK;
        break;
      }
    }
  }

  if (FAILED(hr))
  {
    // Error. Release the action name and inform UI.
    m_strAction.Empty();

    SendMessageToUI(strText);
    m_pUIInterface->UIInterface_ActionError();
    return;
  }

  SendMessageToUI(_T("Action accepted"));
}

// ====================================================================================================================
void CIPManager::RunAction()
{
  m_psbActionRunning = true;
  m_pUIInterface->UIInterface_ActionStart();
  SendMessageToUI(_T("Action running..."));

  int foundIx = -1;
  for (int ix = 0; ix < NUMELMS(SUPPORTED_ACTIONS); ix++)
  {
    if (_tcsicmp(m_strAction, SUPPORTED_ACTIONS[ix].name) == 0)
    {
      foundIx = ix;
      break;
    }
  }
  if (foundIx < 0)
  {
    DASSERT(FALSE);
    SendMessageToUI(_T("ERROR: Cannot detect action"));
    return;
  }

  CIjsFilterBlur          ijsFilterBlur;
  CIjsFilterSharpen       ijsFilterSharpen;
  CIJSFilterThresholding  ijsFilterThresholding;
  CIjsFilterSaltAndPepperNoiseRemoval ijsSaltAndPepper;
  CIjsFilterAdditiveNoiseRemoval ijsAdditiveNoise;

  HRESULT hr = S_OK;
  CIjsActionBase* pBaseAction;
  switch (SUPPORTED_ACTIONS[foundIx].id)
  {
  case ACTIONS_ID_BLUR:
    ijsFilterBlur.SetInputFile(m_strInputFile);
    ijsFilterBlur.SetOutputFile(m_strOutputFile);
    pBaseAction = &ijsFilterBlur;
    break;

  case ACTIONS_ID_SHARPEN:
    ijsFilterSharpen.SetInputFile(m_strInputFile);
    ijsFilterSharpen.SetOutputFile(m_strOutputFile);
    pBaseAction = &ijsFilterSharpen;
    break;

  case ACTIONS_ID_THRESHOLD:
    ijsFilterThresholding.SetInputFile(m_strInputFile);
    ijsFilterThresholding.SetOutputFile(m_strOutputFile);
    pBaseAction = &ijsFilterThresholding;
    break;

  case ACTIONS_ID_SALT_AND_PEPPER:
    ijsSaltAndPepper.SetInputFile(m_strInputFile);
    ijsSaltAndPepper.SetOutputFile(m_strOutputFile);
    pBaseAction = &ijsSaltAndPepper;
    break;

  case ACTIONS_ID_ADDITIVE_NOISE:
    ijsAdditiveNoise.SetCalibrationFile(m_strCalibrationFile);
    ijsAdditiveNoise.SetInputFile(m_strInputFile);
    ijsAdditiveNoise.SetOutputFile(m_strOutputFile);
    pBaseAction = &ijsAdditiveNoise;
    break;

  default:
    SendMessageToUI(_T("ERROR: Unknown filter selected"));
    hr = E_NOTIMPL;
  }

  if (SUCCEEDED(hr))
  {
    hr = pBaseAction->ExecuteAction();

    if (SUCCEEDED(hr))
    {
      SendMessageToUI(_T("Action done"));
    }
    else
    {
      SendMessageToUI(_T("ERROR: Action failed"));
    }
  }

  if (m_psbActionRunning != NULL)
  {
    m_psbActionRunning = false;
  }
  m_pUIInterface->UIInterface_ActionDone();
}
