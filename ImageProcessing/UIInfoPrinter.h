#pragma once
#include "stdafx.h"

// ====================================================================================================================

class CUIInfoPrinter abstract
{
public:
  CUIInfoPrinter(void);
  ~CUIInfoPrinter(void);
  bool  PrintInfoInit(CEdit* pEditCtrl);
  void  PrintInfoFreeAll();

  bool  InfoPrinter_Print(LPCTSTR strText, bool fNewLine = false);
  bool  InfoPrinter_Print(const char* strText, bool fNewLine = false);
  bool  InfoPrinter_PrintLn(LPCTSTR strText);
  bool  InfoPrinter_PrintLn(const char* strText);

protected:
  DWORD PrintInfoThreadFunc();
  static DWORD WINAPI PrintInfoThreadStarter(LPVOID pParams);

  // This function uses dialog control (m_pEditCtrl) to show the message
  void  PrintInfoShowString(LPCTSTR strText, bool fNewLine);

  void  FirePrintingQuit();
  bool IsPrintingDone();
  virtual void OnPrintThreadFinished() = 0;

protected:
  CEdit*  m_pEditCtrl;
private:
  int     m_SymbolsInEditCtrl;

  CRITICAL_SECTION  m_csPrn;
  HANDLE            m_hPrintThread;
  HANDLE            m_hEventPrint;
  HANDLE            m_hEventQuit;
  CStringArray      m_PrintArray;
  bool              m_fPrintInit;
};
