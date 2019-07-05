#pragma once
#include "IjsActionBase.h"
class CIjsFilterAdditiveNoiseRemoval :
  public CIjsActionBase
{
public:
  CIjsFilterAdditiveNoiseRemoval();
  virtual ~CIjsFilterAdditiveNoiseRemoval();

  void GetIterationLimits(ULONG& firstX, ULONG& lastX, ULONG& firstY, ULONG& lastY);

  HRESULT SetCalibrationFile(LPCTSTR pszCailbrationFile);

  void SetBaseNoise(BYTE bVal);

  void DoAction(ULONG x, ULONG y);

protected: // methods
  BYTE GetCalibrationValue(ULONG x, ULONG y);

protected: // members
  CIjsImage m_CalibrationImage;
  BYTE m_bNoiseBase;
};

