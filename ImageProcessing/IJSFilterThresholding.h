#pragma once
#include "IjsActionBase.h"

class CIJSFilterThresholding :
  public CIjsActionBase
{
public:
  CIJSFilterThresholding();
  virtual ~CIJSFilterThresholding();

  void GetIterationLimits(ULONG& firstX, ULONG& lastX, ULONG& firstY, ULONG& lastY);

  void SetThresshold(BYTE bVal);

  void DoAction(ULONG x, ULONG y);

protected:
  BYTE m_bThreshold;

};
