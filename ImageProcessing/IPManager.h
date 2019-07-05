/**
  Main image processing manager responsible to handlng all work and communicate with UI component.
*/

#pragma once
#include "ThreadBase.h"
#include "UIInterface.h"
#include "IPInterface.h"

class CIPManager :
  public CThreadBase,
  public IIPInterface
{
public:
  CIPManager();
  virtual ~CIPManager();

  void AddUIInterface(IUIInterface* pUIInterface);

  void IPInterface_SetCalibrationFile(LPCTSTR pszFile);
  void IPInterface_SetInputFile(LPCTSTR pszFile);
  void IPInterface_SetOutputFile(LPCTSTR pszFile);
  void IPInterface_SetAction(LPCTSTR pszAction);
  void IPInterface_ActionRun();

protected:
  DWORD ThreadBase_ThreadFunc(void* pParams);

  void SendMessageToUI(CString& strText);
  void SendMessageToUI(LPCTSTR pszText);

  void SetCalibrationFile();
  void SetInputFile();
  void SetOutputFile();
  void SetActionName();
  void RunAction();

protected:
  IUIInterface* m_pUIInterface;
  int           m_iAvailabeCores;

  HANDLE        m_hEventUICalibrationFile;
  HANDLE        m_hEventUIInputFile;
  HANDLE        m_hEventUIOutputFile;
  HANDLE        m_hEventUISetAction;
  HANDLE        m_hEventUIActionRun;

  CString       m_strCalibrationFile;
  CString       m_strInputFile;
  CString       m_strOutputFile;
  CString       m_strAction;
};
