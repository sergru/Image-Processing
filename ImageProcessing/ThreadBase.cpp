#include "stdafx.h"
#include "CommonDefs.h"
#include "ThreadBase.h"


// Timing values for thread stopping in Destroy function.
const int SO_THRD_STOP_WAIT_TIME = 100;  // 100ms
const int SO_THRD_STOP_WAIT_MAX_TIME = 10000; // 1sec

// ====================================================================================================================
CThreadBase::CThreadBase()
  :
  m_hThread(NULL),
  m_pThreadParams(NULL),
  m_hEventThreadQuit(NULL)
{
}

// ====================================================================================================================
CThreadBase::~CThreadBase()
{
  SUCCEEDED(ThreadBase_Destroy(true));
  if (m_hEventThreadQuit)
  {
    CloseHandle(m_hEventThreadQuit);
  }
}

// ====================================================================================================================
HRESULT CThreadBase::ThreadBase_Create(void* pParams)
{
  HRESULT  hr;

  if (m_hThread)
  {
    // Thread already created.
    DASSERT(FALSE);
    return  E_FAIL;
  }

  if (FAILED(utils::MakeEvent(m_hEventThreadQuit)))
  {
    return E_FAIL;
  }

  m_pThreadParams = pParams;
  m_hThread = CreateThread(NULL, 0, ThreadBase_ThreadStarter, this, 0, NULL);
  if (!m_hThread)
  {
    // Failed creating new thread.
    DASSERT(FALSE);
    hr = E_FAIL;
    if (FAILED(ThreadBase_Destroy(false)))
    {
      // Why we are here? Something not right.
      DASSERT(FALSE);
    }
  }
  else
  {
    hr = S_OK;
  }

  return hr;
}

// ====================================================================================================================
HRESULT CThreadBase::ThreadBase_Destroy(bool fWiatAndTerminate)
{
  HRESULT hr = S_OK;

  if (m_hThread)
  {
    if (ThreadBase_GetExitCode() == STILL_ACTIVE)
    {
      if (!fWiatAndTerminate)
      {
        // In this case we want to destroy a thread that is already finished.
        // But thread is active.
        hr = E_FAIL;
      }
      else
      {
        ThreadBase_FireThreadQuit();

        // Wait certain time then terminate thread due to no response for quit event.
        bool fIsStopped = false;
        int iCount = 0;
        while ((iCount * SO_THRD_STOP_WAIT_TIME) < SO_THRD_STOP_WAIT_MAX_TIME)
        {
          if (ThreadBase_GetExitCode() != STILL_ACTIVE)
          {
            fIsStopped = true;
            break;
          }
          iCount++;
          Sleep(SO_THRD_STOP_WAIT_TIME);
        }

        if (!fIsStopped)
        {
          // Thread is not stopped. We have to terminate it.
          // It is very bad thing to do because we don't know what the thread
          // done and how it must be cloased safely.
          DASSERT(FALSE);
          TerminateThread(m_hThread, EXIT_THREAD_S_TERMINATE);
          hr = S_FALSE;
        }
      }
    }

    // At this point thread must be stopped in any case.
    CloseHandle(m_hThread);
    m_hThread = NULL;
  }

  // Reset evrything else if thread is stopped.
  if (SUCCEEDED(hr))
  {
    m_pThreadParams = NULL;
    CloseHandle(m_hEventThreadQuit);
    m_hEventThreadQuit = NULL;
  }

  return hr;
}

// ====================================================================================================================
DWORD CThreadBase::ThreadBase_GetExitCode()
{
  DWORD dwExitCode = EXIT_THREAD_E_NOTEXIST;
  if (m_hThread)
  {
    if (!GetExitCodeThread(m_hThread, &dwExitCode))
    {
      // Be very careful in this case.
      DASSERT(FALSE);
      dwExitCode = EXIT_THREAD_E_ERROR;
    }
  }

  return dwExitCode;
}

// ====================================================================================================================
void CThreadBase::ThreadBase_FireThreadQuit()
{
  // Set event only if thread is active.
  if (m_hEventThreadQuit && m_hThread && (ThreadBase_GetExitCode() == STILL_ACTIVE))
  {
    SetEvent(m_hEventThreadQuit);
  }
}

// ====================================================================================================================
const HANDLE CThreadBase::ThreadBase_GetThreadHandle()
{
  return m_hThread;
};

// ====================================================================================================================
bool CThreadBase::ThreadBase_IsCreated()
{
  return (m_hThread != NULL);
};

// ====================================================================================================================
bool CThreadBase::ThreadBase_IsActive()
{
  return (ThreadBase_IsCreated() && (STILL_ACTIVE == ThreadBase_GetExitCode()));
};

// ====================================================================================================================
DWORD WINAPI CThreadBase::ThreadBase_ThreadStarter(LPVOID  pParams)
{
  if (!pParams)
  {
    DASSERT(FALSE);
    return EXIT_THREAD_E_INVALIDARG;
  }
  CThreadBase* pThread = static_cast<CThreadBase*>(pParams);

  DWORD dwRet = pThread->ThreadBase_ThreadFunc(pThread->m_pThreadParams);
  return dwRet;
}
