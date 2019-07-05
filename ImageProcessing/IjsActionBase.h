#pragma once
#include "IjsImage.h"

/**
  Base action class responsible for the basic image iterations.
*/
class CIjsActionBase abstract
{
public: // methods
  CIjsActionBase();
  virtual ~CIjsActionBase();

  // Set the image file to work with.
  HRESULT SetInputFile(LPCTSTR pszFilePath);

  // Set output image file.
  HRESULT SetOutputFile(LPCTSTR pszFilePath);

  // Execut selected action.
  HRESULT ExecuteAction();

protected: // methods
  virtual void GetIterationLimits(ULONG& firstX, ULONG& lastX, ULONG& firstY, ULONG& lastY) = 0;
  virtual void DoAction(ULONG x, ULONG y) = 0;

protected:  //members
  // Image file to work with.
  CString m_strInFilePath;
  CIjsImage m_InImage;

  // Uotput image.
  CString m_strOutFilePath;
  CIjsImage m_OutImage;
};
