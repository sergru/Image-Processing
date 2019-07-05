#pragma once
#include "IjsActionBase.h"

/**
  
*/
class CIjsActionLinearFilter abstract :
  public CIjsActionBase
{
public:
  CIjsActionLinearFilter();
  virtual ~CIjsActionLinearFilter();

  HRESULT SetMask(ULONG radius, const double* pMask);

  void GetIterationLimits(ULONG& firstX, ULONG& lastX, ULONG& firstY, ULONG& lastY);
  void DoAction(ULONG x, ULONG y);

protected:
  ULONG m_Radius;
  double* m_pMask;
};
