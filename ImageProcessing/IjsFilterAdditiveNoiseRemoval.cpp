#include "stdafx.h"
#include "IjsFilterAdditiveNoiseRemoval.h"

const BYTE BASE_NOISE = 128;

// ====================================================================================================================
CIjsFilterAdditiveNoiseRemoval::CIjsFilterAdditiveNoiseRemoval()
{
  SetBaseNoise(BASE_NOISE);
}

// ====================================================================================================================
CIjsFilterAdditiveNoiseRemoval::~CIjsFilterAdditiveNoiseRemoval()
{
}

// ====================================================================================================================
void CIjsFilterAdditiveNoiseRemoval::GetIterationLimits(ULONG& firstX, ULONG& lastX, ULONG& firstY, ULONG& lastY)
{
  firstX = 0;
  lastX = m_InImage.GetWidth() - 1;
  firstY = 0;
  lastY = m_InImage.GetHeight() - 1;
}

// ====================================================================================================================
void CIjsFilterAdditiveNoiseRemoval::SetBaseNoise(BYTE bVal)
{
  m_bNoiseBase = bVal;
}

// ====================================================================================================================
HRESULT CIjsFilterAdditiveNoiseRemoval::SetCalibrationFile(LPCTSTR pszCailbrationFile)
{
  return m_CalibrationImage.ParseFile(pszCailbrationFile);
}

// ====================================================================================================================
void CIjsFilterAdditiveNoiseRemoval::DoAction(ULONG x, ULONG y)
{
  BYTE bVal = m_InImage.GetPixel(x, y) + GetCalibrationValue(x, y);
  m_OutImage.SetPixel(x, y, bVal);
}

// ====================================================================================================================
BYTE CIjsFilterAdditiveNoiseRemoval::GetCalibrationValue(ULONG x, ULONG y)
{
  return m_bNoiseBase - m_CalibrationImage.GetPixel(x, y);
}
