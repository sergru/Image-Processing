#include "stdafx.h"
#include "IjsActionBase.h"
#include "CommonDefs.h"

// ====================================================================================================================
CIjsActionBase::CIjsActionBase()
{
}

// ====================================================================================================================
CIjsActionBase::~CIjsActionBase()
{
}

// ====================================================================================================================
HRESULT CIjsActionBase::SetInputFile(LPCTSTR pszFilePath)
{
  HRESULT hr;
  hr = m_InImage.ParseFile(pszFilePath);
  return hr;
}

// ====================================================================================================================
HRESULT CIjsActionBase::SetOutputFile(LPCTSTR pszFilePath)
{
  m_strOutFilePath = pszFilePath;
  return S_OK;
}

// ====================================================================================================================
HRESULT CIjsActionBase::ExecuteAction()
{
  HRESULT hr;
  hr = m_OutImage.Create(m_InImage.GetWidth(), m_InImage.GetHeight(), m_InImage.GetBpp());
  if (FAILED(hr))
  {
    return hr;
  }

  ULONG firstX;
  ULONG lastX;
  ULONG firstY;
  ULONG lastY;
  GetIterationLimits(firstX, lastX, firstY, lastY);

  for (ULONG y = firstY; y < lastY; y++)
  {
    for (ULONG x = firstX; x < lastX; x++)
    {
      DoAction(x, y);
    }
  }

  m_OutImage.Save(FIF_PNG, m_strOutFilePath);
  m_OutImage.Free();

  return S_OK;
}
