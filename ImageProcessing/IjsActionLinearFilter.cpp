#include "stdafx.h"
#include "IjsActionLinearFilter.h"

// ====================================================================================================================
CIjsActionLinearFilter::CIjsActionLinearFilter() :
  m_pMask (NULL),
  m_Radius (0)
{
}

// ====================================================================================================================
CIjsActionLinearFilter::~CIjsActionLinearFilter()
{
  if (m_pMask)
    delete[] m_pMask;
}

// ====================================================================================================================
HRESULT CIjsActionLinearFilter::SetMask(ULONG radius, const double* pMask)
{
  if (!radius || !pMask)
  {
    return E_INVALIDARG;
  }

  ULONG maskSize = (2 * radius + 1) * (2 * radius + 1);
  m_pMask = new double[maskSize];
  if (!m_pMask)
  {
    return E_OUTOFMEMORY;
  }

  for (ULONG ix = 0; ix < maskSize; ix++)
  {
    m_pMask[ix] = pMask[ix];
  }
  m_Radius = radius;

  return S_OK;
}

// ====================================================================================================================
void CIjsActionLinearFilter::GetIterationLimits(ULONG& firstX, ULONG& lastX, ULONG& firstY, ULONG& lastY)
{
  firstX = m_Radius;
  lastX = m_InImage.GetWidth() - m_Radius;
  firstY = m_Radius;
  lastY = m_InImage.GetHeight() - m_Radius;
}

// ====================================================================================================================
void CIjsActionLinearFilter::DoAction(ULONG x, ULONG y)
{
  BYTE val = m_InImage.GetPixel(x, y);

  double dNewVal = 0;
  double* pCurMask = m_pMask;
  for (ULONG curY = y - m_Radius; curY <= y + m_Radius; curY++)
  {
    for (ULONG curX = x - m_Radius; curX <= x + m_Radius; curX++)
    {
      dNewVal += (*pCurMask * m_InImage.GetPixel(curX, curY));
      pCurMask++;
    }
  }

  m_OutImage.SetPixel(x, y, (BYTE)round(dNewVal));
}
