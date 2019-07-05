#include "stdafx.h"
#include "UIManager.h"
#include "CommonDefs.h"
#include "UIDlg.h"

// ====================================================================================================================
CUIManager::CUIManager() :
  m_hEventInfoOut (NULL),
  m_hEventInputFileError (NULL),
  m_hEventOutputFileError (NULL),
  m_hEventActionError (NULL),
  m_hEventActionDone (NULL)
{
  m_pInfoPrinter = static_cast<CUIDlg*>(this);
  InitializeCriticalSection(&m_csSInfoOut);
}

// ====================================================================================================================
CUIManager::~CUIManager()
{
  DeleteCriticalSection(&m_csSInfoOut);
}

// ====================================================================================================================
void CUIManager::UIInterface_InfoOut(CSInfoOutPtr& spInfoOut)
{
  EnterCriticalSection(&m_csSInfoOut);
  m_rangeSInfoOut.Add(spInfoOut);
  LeaveCriticalSection(&m_csSInfoOut);
  SetEvent(m_hEventInfoOut);
}

// ====================================================================================================================
void CUIManager::UIInterface_InputFileError()
{
  SetEvent(m_hEventInputFileError);
}

// ====================================================================================================================
void CUIManager::UIInterface_OutputFileError()
{
  SetEvent(m_hEventOutputFileError);
}


// ====================================================================================================================
void CUIManager::UIInterface_ActionError()
{
  SetEvent(m_hEventActionError);
}

// ====================================================================================================================
void CUIManager::UIInterface_ActionDone()
{
  SetEvent(m_hEventActionDone);
}

// ====================================================================================================================
HRESULT CUIManager::ThreadBase_Create(void* pParams)
{
  HRESULT hr;

  hr = utils::MakeEvent(m_hEventInfoOut);

  if (SUCCEEDED(hr))
  {
    hr = utils::MakeEvent(m_hEventInputFileError);
  }

  if (SUCCEEDED(hr))
  {
    hr = utils::MakeEvent(m_hEventOutputFileError);
  }

  if (SUCCEEDED(hr))
  {
    hr = utils::MakeEvent(m_hEventActionError);
  }

  if (SUCCEEDED(hr))
  {
    hr = utils::MakeEvent(m_hEventActionDone);
  }

  if (FAILED(hr))
  {
    // Error. Cannot continue.
    return hr;
  }

  return super::ThreadBase_Create(pParams);
}

// ====================================================================================================================
HRESULT CUIManager::ThreadBase_Destroy(bool fWiatAndTerminate)
{
  HRESULT hr = super::ThreadBase_Destroy(fWiatAndTerminate);

  utils::CloseEvent(m_hEventInfoOut);
  utils::CloseEvent(m_hEventInputFileError);
  return hr;
}

// ====================================================================================================================
DWORD CUIManager::ThreadBase_ThreadFunc(void* pParams)
{
  utils::SetThreadName("CUIManager");

  HANDLE hWaitObjects[] =
  {
      m_hEventThreadQuit,
      m_hEventInfoOut,
      m_hEventInputFileError,
      m_hEventOutputFileError,
      m_hEventActionError,
      m_hEventActionDone,
  };

  DWORD dwThreadExitCode = EXIT_THREAD_S_OK;
  bool isContinue = true;
  while (isContinue)
  {
    DWORD dwWaitID = WaitForMultipleObjects(NUMELMS(hWaitObjects), hWaitObjects, FALSE, INFINITE);
    switch (dwWaitID)
    {
    case WAIT_OBJECT_0: //m_hEventQuit
      dwThreadExitCode = EXIT_THREAD_S_QUIT;
      isContinue = false;
      break;

    case WAIT_OBJECT_0 + 1: //m_hEventInfoOut
      while (1)
      {
        CSInfoOutPtr spInfoOut;
        EnterCriticalSection(&m_csSInfoOut);
        if (m_rangeSInfoOut.GetSize())
        {
          spInfoOut = m_rangeSInfoOut.GetAt(0);
          m_rangeSInfoOut.RemoveAt(0);
        }
        LeaveCriticalSection(&m_csSInfoOut);

        if (spInfoOut != NULL)
        {
          m_pInfoPrinter->InfoPrinter_Print(spInfoOut->pszOutText, spInfoOut->fAddLinebreak);
        }
        else
        {
          break;
        }
      }
      break;

    case WAIT_OBJECT_0 + 2: //m_hEventInputFileError
      UIManager_ClearInputFile();
      break;

    case WAIT_OBJECT_0 + 3: //m_hEventOutputFileError
      UIManager_ClearOutputFile();
      break;

    case WAIT_OBJECT_0 + 4: //m_hEventActionError
      UIManager_ClearAction();
      break;

    case WAIT_OBJECT_0 + 5: //m_hEventActionDone
      UIManager_ActionDone();
      break;

    default:
      // Unexpected wait object state.
      DASSERT(FALSE);
      dwThreadExitCode = EXIT_THREAD_E_ERROR;
      isContinue = false;
    }
  }

  return dwThreadExitCode;
}
