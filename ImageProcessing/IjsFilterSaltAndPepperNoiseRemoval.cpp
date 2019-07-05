#include "stdafx.h"
#include "IjsFilterSaltAndPepperNoiseRemoval.h"

// ====================================================================================================================
CIjsFilterSaltAndPepperNoiseRemoval::CIjsFilterSaltAndPepperNoiseRemoval() :
  m_iMedianCount (0)
{
  m_Radius = 1;

  int medianSize = (2 * m_Radius + 1)*(2 * m_Radius + 1) - 1;
  m_pbMedian = new BYTE[medianSize];
}

// ====================================================================================================================
CIjsFilterSaltAndPepperNoiseRemoval::~CIjsFilterSaltAndPepperNoiseRemoval()
{
  if (m_pbMedian)
  {
    delete m_pbMedian;
  }
}

// ====================================================================================================================
void CIjsFilterSaltAndPepperNoiseRemoval::GetIterationLimits(ULONG& firstX, ULONG& lastX, ULONG& firstY, ULONG& lastY)
{
  firstX = m_Radius;
  lastX = m_InImage.GetWidth() - m_Radius;
  firstY = m_Radius;
  lastY = m_InImage.GetHeight() - m_Radius;
}

// ====================================================================================================================
void CIjsFilterSaltAndPepperNoiseRemoval::DoAction(ULONG x, ULONG y)
{
  BYTE bVal = m_InImage.GetPixel(x, y);

  if (bVal > 0 && bVal < 255)
  {
    // Correct value.
    m_OutImage.SetPixel(x, y, bVal);
    return;
  }

  ResetMedian();
  for (ULONG curY = y - m_Radius; curY <= y + m_Radius; curY++)
  {
    for (ULONG curX = x - m_Radius; curX <= x + m_Radius; curX++)
    {
      BYTE bCurPixel = m_InImage.GetPixel(curX, curY);

      if ((bCurPixel) == 0 || (bCurPixel == 255))
      {
        // Skip corrupted pixel
        continue;
      }

      AddToMedian(bCurPixel);
    }
  }

  m_OutImage.SetPixel(x, y, GetMedian(m_InImage.GetPixel(x, y)));
}

// ====================================================================================================================
void CIjsFilterSaltAndPepperNoiseRemoval::ResetMedian()
{
  m_iMedianCount = 0;
}

// ====================================================================================================================
void CIjsFilterSaltAndPepperNoiseRemoval::AddToMedian(BYTE bVal)
{
  int minIx = 0;
  int maxIx = m_iMedianCount;
  while(minIx < maxIx)
  {
    int curIx = (maxIx + minIx) / 2;
    if (bVal >= m_pbMedian[curIx])
    {
      minIx = curIx + 1;
    }
    else
    {
      maxIx = curIx;
    }
  }

  for (int ix = m_iMedianCount; ix > minIx; ix--)
  {
    m_pbMedian[ix] = m_pbMedian[ix - 1];
  }

  m_pbMedian[minIx] = bVal;
  m_iMedianCount++;
}

// ====================================================================================================================
BYTE CIjsFilterSaltAndPepperNoiseRemoval::GetMedian(BYTE bValIfNoMedian)
{
  return m_iMedianCount ? m_pbMedian[m_iMedianCount / 2] : bValIfNoMedian;
}
