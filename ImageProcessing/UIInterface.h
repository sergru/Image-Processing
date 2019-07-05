#pragma once
#include "stdafx.h"
#include "CommonDefs.h"
#include "SmartPtr.h"

/**
  UI Interface to accespt processing module requests.
*/

class IUIInterface
{
public:
  // UIInterface_InfoOut param data structure
  struct SInfoOut
  {
    LPTSTR pszOutText;
    bool fAddLinebreak;

    ~SInfoOut()
    {
      if (pszOutText)
      {
        delete[] pszOutText;
      }
    }
  };
  typedef CSmartPtr<SInfoOut> CSInfoOutPtr;

  virtual void UIInterface_InfoOut(CSInfoOutPtr& pInfoOut) = 0;
  virtual void UIInterface_CalibrationFileError() = 0;
  virtual void UIInterface_InputFileError() = 0;
  virtual void UIInterface_OutputFileError() = 0;
  virtual void UIInterface_ActionError() = 0;
  virtual void UIInterface_ActionStart() = 0;
  virtual void UIInterface_ActionDone() = 0;
};

