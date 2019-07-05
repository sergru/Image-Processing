#include "stdafx.h"
#include "IJSFilterThresholding.h"

const BYTE THRESHOLD = 128;

// ====================================================================================================================
CIJSFilterThresholding::CIJSFilterThresholding()
{
  SetThresshold(THRESHOLD);
}

// ====================================================================================================================
CIJSFilterThresholding::~CIJSFilterThresholding()
{
}

// ====================================================================================================================
void CIJSFilterThresholding::GetIterationLimits(ULONG& firstX, ULONG& lastX, ULONG& firstY, ULONG& lastY)
{
  firstX = 0;
  lastX = m_InImage.GetWidth() - 1;
  firstY = 0;
  lastY = m_InImage.GetHeight() - 1;
}

// ====================================================================================================================
void CIJSFilterThresholding::SetThresshold(BYTE bVal)
{
  m_bThreshold = bVal;
}

// ====================================================================================================================
void CIJSFilterThresholding::DoAction(ULONG x, ULONG y)
{
  BYTE bVal = m_InImage.GetPixel(x, y);
  bVal = (bVal >= m_bThreshold) ? 255 : 0;
  m_OutImage.SetPixel(x, y, bVal);
}
