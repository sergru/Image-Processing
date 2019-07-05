#pragma once
#include "IjsActionBase.h"
class CIjsFilterSaltAndPepperNoiseRemoval :
  public CIjsActionBase
{
public:
  CIjsFilterSaltAndPepperNoiseRemoval();
  virtual ~CIjsFilterSaltAndPepperNoiseRemoval();

  void GetIterationLimits(ULONG& firstX, ULONG& lastX, ULONG& firstY, ULONG& lastY);
  void DoAction(ULONG x, ULONG y);

protected:
  void ResetMedian();
  void AddToMedian(BYTE bVal);
  BYTE GetMedian(BYTE bValIfNoMedian);

protected:
  ULONG m_Radius;
  ULONG m_MaxW;

  BYTE* m_pbMedian;
  int   m_iMedianCount;
};
