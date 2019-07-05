/**
  Image Processing Interface to accespt UI module requests.
*/

#pragma once
#include "stdafx.h"
#include "CommonDefs.h"
#include "SmartPtr.h"

const enum SUPPORTED_ACTIONS_IDS
{
  ACTIONS_ID_BLUR = 0,
  ACTIONS_ID_SHARPEN,
  ACTIONS_ID_THRESHOLD,
  ACTIONS_ID_SALT_AND_PEPPER,
  ACTIONS_ID_ADDITIVE_NOISE,
};

struct ACTION_PAIR
{
  TCHAR* name;
  int id;
};
static const ACTION_PAIR SUPPORTED_ACTIONS[] =
{
  {_T("Blur"), ACTIONS_ID_BLUR },
  {_T("Sharpen"), ACTIONS_ID_SHARPEN },
  {_T("Threshold"), ACTIONS_ID_THRESHOLD },
  {_T("Salt & Pepper"), ACTIONS_ID_SALT_AND_PEPPER },
  {_T("Additive Noise"), ACTIONS_ID_ADDITIVE_NOISE },
};

class IIPInterface abstract
{
public:

  // IPInterface_Action param data structure
  struct SAction
  {
  };
  typedef CSmartPtr<SAction> CSActionPtr;

  virtual void IPInterface_SetCalibrationFile(LPCTSTR pszFile) = 0;
  virtual void IPInterface_SetInputFile(LPCTSTR pszFile) = 0;
  virtual void IPInterface_SetOutputFile(LPCTSTR pszFile) = 0;
  virtual void IPInterface_SetAction(LPCTSTR pszAction) = 0;
  virtual void IPInterface_ActionRun() = 0;

  CSmartPtr<bool> m_psbActionRunning;
};
