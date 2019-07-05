/**
  Abstract base class for thread control.
*/

#pragma once
#include "ThreadBaseDefs.h"

class CThreadBase abstract
{
protected: // virtual methods

  // Thread work function. Should be defiend in derived class.
  virtual DWORD ThreadBase_ThreadFunc(void* pParams) = 0;

public: // methods
  CThreadBase();
  virtual ~CThreadBase();

  HRESULT ThreadBase_Create(void* pParams = NULL);
  HRESULT ThreadBase_Destroy(bool fWiatAndTerminate);
  DWORD   ThreadBase_GetExitCode();
  void    ThreadBase_FireThreadQuit();

  const HANDLE ThreadBase_GetThreadHandle();
  bool ThreadBase_IsCreated();
  bool ThreadBase_IsActive();

protected: // static methods
  static DWORD WINAPI ThreadBase_ThreadStarter(LPVOID  pParams);

protected:// members
  HANDLE  m_hThread;
  void*   m_pThreadParams;
  HANDLE  m_hEventThreadQuit;
};
