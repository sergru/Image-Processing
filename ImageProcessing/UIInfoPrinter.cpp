#include "StdAfx.h"
#include "CommonDefs.h"
#include "UIInfoPrinter.h"

// ====================================================================================================================

CUIInfoPrinter::CUIInfoPrinter(void) :
  m_pEditCtrl (NULL),
  m_SymbolsInEditCtrl (0),
  m_hPrintThread (NULL),
  m_hEventPrint (NULL),
  m_hEventQuit (NULL),
  m_fPrintInit (false)
{
  m_PrintArray.RemoveAll();

  InitializeCriticalSection(&m_csPrn);
}

// ====================================================================================================================

CUIInfoPrinter::~CUIInfoPrinter(void)
{
  PrintInfoFreeAll();
  DeleteCriticalSection(&m_csPrn);
}

// ====================================================================================================================

bool CUIInfoPrinter::PrintInfoInit(CEdit* pEditCtrl)
{
  if( m_fPrintInit )
  {
    return true;
  }

  if( !pEditCtrl )
  {
    DASSERT(false);
    return false;
  }

  m_pEditCtrl = pEditCtrl;

  bool fOk = true;
  try
  {
    if (FAILED(utils::MakeEvent(m_hEventPrint)))
    {
      throw "Event creation error";
    }

    if (FAILED(utils::MakeEvent(m_hEventQuit)))
    {
      throw "Event creation error";
    }

    m_hPrintThread = CreateThread(NULL, 0, PrintInfoThreadStarter, this, 0, NULL);
    if (!m_hPrintThread)
    {
      throw "Event creation error";
    }
  }
  catch (...)
  {
    DASSERT(FALSE);
    fOk = false;
  }


  if( fOk )
  {
    m_fPrintInit = true;
  }
  else
  {
    PrintInfoFreeAll();
  }

  return fOk;
}

// ====================================================================================================================

bool CUIInfoPrinter::InfoPrinter_Print(LPCTSTR strText, bool fNewLine)
{
  if( !m_fPrintInit )
  {
    DASSERT(false);
    return false;
  }

  CString strOut = strText;
  if (fNewLine)
  {
    strOut += _T("\n");
  }

  EnterCriticalSection(&m_csPrn);
  m_PrintArray.Add(strOut);
  LeaveCriticalSection(&m_csPrn);
  SetEvent(m_hEventPrint);
  
  return true;
}

// ====================================================================================================================
bool CUIInfoPrinter::InfoPrinter_Print(const char* strText, bool fNewLine)
{
  CString strOutText;
  strOutText.Format(_T("%S"), strText);
  return InfoPrinter_Print(strOutText, fNewLine);
}

// ====================================================================================================================

bool  CUIInfoPrinter::InfoPrinter_PrintLn(LPCTSTR strText)
{
  return InfoPrinter_Print(strText, true);
};

// ====================================================================================================================

bool  CUIInfoPrinter::InfoPrinter_PrintLn(const char* strText)
{
  return InfoPrinter_Print(strText, true);
};

// ====================================================================================================================

void CUIInfoPrinter::PrintInfoFreeAll()
{
  if( m_hPrintThread )
  {
    if( !m_hEventQuit )
    {
      // Event must exist.
      DASSERT(FALSE);
      TerminateThread(m_hPrintThread, 0xFFFF0000);
    }
    else
    {
      SetEvent(m_hEventQuit);

      DWORD dwCode  = 0xFFFFFFFF;
      int iCount    = 0;
      int iTimer    = 100;
      int iMaxTime  = 10000;

      while( (iCount * iTimer) < iMaxTime )
      {
        GetExitCodeThread(m_hPrintThread, &dwCode);
        if( dwCode != STILL_ACTIVE )
        {
          break;
        }
        else
        {
          Sleep(iTimer);
        }
      }

      if( dwCode == STILL_ACTIVE )
      {
        DASSERT(FALSE);
        TerminateThread(m_hPrintThread, 0xFFFF0000);
      }

      m_hPrintThread = NULL;
    }
  }

  if( m_hEventPrint )
  {
    CloseHandle(m_hEventPrint);
    m_hEventPrint = NULL;
  }

  if( m_hEventQuit )
  {
    CloseHandle(m_hEventQuit);
    m_hEventQuit = NULL;
  }

  m_PrintArray.RemoveAll();
  m_pEditCtrl = NULL;

  m_fPrintInit = false;
}

// ====================================================================================================================

DWORD WINAPI CUIInfoPrinter::PrintInfoThreadStarter(LPVOID pParams)
{
  utils::SetThreadName("CInfoPrinter_PrintThread");

  if( !pParams )
  {
    DASSERT(FALSE);
    return 0xFFFFFFFF;
  }

  return ((CUIInfoPrinter*)pParams)->PrintInfoThreadFunc();
}

// ====================================================================================================================

DWORD CUIInfoPrinter::PrintInfoThreadFunc()
{
  utils::SetThreadName("CUIInfoPrinter");

  HANDLE  grSyncs[2];

  bool fGo = true;
  while( fGo )
  {
    grSyncs[0] = m_hEventQuit;  DASSERT(m_hEventQuit != NULL);
    grSyncs[1] = m_hEventPrint; DASSERT(m_hEventPrint != NULL);
    DWORD dwSync = WaitForMultipleObjects(2, grSyncs, FALSE, INFINITE);
    DWORD dwTstSync = WaitForSingleObject(m_hEventQuit, 0);
    if( dwTstSync == WAIT_OBJECT_0)
    {
      // In case m_hEventQuit had lower order when checked in WaitForMultipleObjects()
      dwSync = WAIT_OBJECT_0;
    }

    switch( dwSync )
    {
      case WAIT_OBJECT_0:
      {
        // QUIT event.
        fGo = false;
        break;
      }

      case WAIT_OBJECT_0 + 1:
      {
        CString strOutText;
        bool fPrint = true;
        while( fPrint )
        {
          // Make the access to the string array thread safe. It is very important.
          // Only here we remove strings from array. So it is safe to use
          // an iCount after unlock the array.
          EnterCriticalSection(&m_csPrn);
          int iCount = (int)m_PrintArray.GetCount();
          if( iCount > 0 )
          {
            strOutText = m_PrintArray.GetAt(0);
            m_PrintArray.RemoveAt(0);
            iCount = (int)m_PrintArray.GetCount();
            fPrint = true;
          }
          else
          {
            fPrint = false;
          }
          LeaveCriticalSection(&m_csPrn);
          if( !fPrint ) break;  // No more for print. Will wait now.

          PrintInfoShowString(strOutText, false);

          DWORD dwTstSync = WaitForSingleObject(m_hEventQuit, 0);
          if( dwTstSync == WAIT_OBJECT_0)
          {
            // Host asked to quit right away.
            fGo = false;
            break;
          }
        }
        break;
      }

      default:
      {
        DASSERT(FALSE);
      }
    }
  }

  OnPrintThreadFinished();

  return 0;
}

// ====================================================================================================================

void CUIInfoPrinter::PrintInfoShowString(LPCTSTR strText, bool fNewLine)
{
  if( m_pEditCtrl && strText )
  {
    m_pEditCtrl->SetSel(m_SymbolsInEditCtrl, m_SymbolsInEditCtrl, TRUE);
    m_pEditCtrl->ReplaceSel(strText);
    CString strMainText;
    m_pEditCtrl->GetWindowText(strMainText);
    m_SymbolsInEditCtrl += strMainText.GetLength();
    if( fNewLine )
    {
      m_pEditCtrl->SetSel(m_SymbolsInEditCtrl, m_SymbolsInEditCtrl, TRUE);
      m_pEditCtrl->ReplaceSel(_T("\n"));
      CString strMainText;
      m_pEditCtrl->GetWindowText(strMainText);
      m_SymbolsInEditCtrl += strMainText.GetLength();
    }
  }
}

// ====================================================================================================================

void  CUIInfoPrinter::FirePrintingQuit()
{
  if (m_hEventQuit)
  {
    SetEvent(m_hEventQuit);
  }
}

// ====================================================================================================================

bool CUIInfoPrinter::IsPrintingDone()
{
  bool fDone = true;
  if( m_hPrintThread )
  {
    Sleep(1); // Get small time to handle thread finishing.
    DWORD dwExitCode;
    GetExitCodeThread(m_hPrintThread, &dwExitCode);
    if( dwExitCode == STILL_ACTIVE )
    {
      fDone = false;
    }
  }
  return fDone;
}

// ====================================================================================================================
