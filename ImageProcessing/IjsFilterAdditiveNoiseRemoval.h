#pragma once
#include "IjsActionBase.h"


const BYTE BASE_NOISE = 128;

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

#ifdef UNIT_TESTS
public:
  BYTE UT_GetBaseNoise()
  {
    return m_bNoiseBase;
  };
  CIjsImage& UT_GetCalibrationImage()
  {
    return m_CalibrationImage;
  };
  ULONG UT_GetInputImageHeight()
  {
    return m_InImage.GetHeight();
  }
  ULONG UT_GetInputImageWidth()
  {
    return m_InImage.GetWidth();
  }
  BYTE UT_GetCalibrationValue(ULONG x, ULONG y)
  {
    return GetCalibrationValue(x, y);
  }

#endif

};

