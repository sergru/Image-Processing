#pragma once
#include "ThreadBase.h"
#include "UIInfoPrinter.h"
#include "UIInterface.h"

class CUIManager abstract:
  public CThreadBase,
  public IUIInterface
{
  typedef CThreadBase super;

public:
  CUIManager();
  ~CUIManager();

  virtual void UIManager_ClearCalibrationFile() = 0;
  virtual void UIManager_ClearInputFile() = 0;
  virtual void UIManager_ClearOutputFile() = 0;
  virtual void UIManager_ClearAction() = 0;
  virtual void UIManager_ActionDone() = 0;

  // IUIInterface members
  void UIInterface_InfoOut(CSInfoOutPtr& pInfoOut);
  void UIInterface_CalibrationFileError();
  void UIInterface_InputFileError();
  void UIInterface_OutputFileError();
  void UIInterface_ActionError();
  void UIInterface_ActionDone();

  // CThreadBase members
  HRESULT ThreadBase_Create(void* pParams);
  HRESULT ThreadBase_Destroy(bool fWiatAndTerminate);

protected:
  // CThreadBase members
  virtual DWORD ThreadBase_ThreadFunc(void* pParams);

protected:
  CArray<CSInfoOutPtr> m_rangeSInfoOut;
  CRITICAL_SECTION m_csSInfoOut;
  HANDLE m_hEventInfoOut;
  HANDLE m_hEventCalibrationFileError;
  HANDLE m_hEventInputFileError;
  HANDLE m_hEventOutputFileError;
  HANDLE m_hEventActionError;
  HANDLE m_hEventActionDone;

  CUIInfoPrinter* m_pInfoPrinter;
};
