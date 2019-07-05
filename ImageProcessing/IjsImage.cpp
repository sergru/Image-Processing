#include "stdafx.h"
#include "IjsImage.h"
#include <algorithm>
#include <iterator>
#include "CommonDefs.h"

const TCHAR* SUPPPORTED_IMAGE_TYPES [] =
{
  _T("PNG"),
  _T("JPG"),
};

// ====================================================================================================================
WORD SUPPORTED_BITS[] =
{
  8,
};

// ====================================================================================================================
WORD SUPPORTED_PLANES[] =
{
  1,
};

// ====================================================================================================================
CIjsImage::CIjsImage()
{
}

// ====================================================================================================================
CIjsImage::~CIjsImage()
{
  m_fipImage.clear();
}

// ====================================================================================================================
HRESULT CIjsImage::Create(ULONG width, ULONG height, ULONG bpp)
{
  HRESULT hr = S_OK;
  m_fipImage = fipImage(FIT_BITMAP, width, height, bpp);

  m_bBitmap = m_fipImage.accessPixels();
  return hr;
}

// ====================================================================================================================
void CIjsImage::Free()
{
  m_fipImage.clear();
}

// ====================================================================================================================
HRESULT CIjsImage::Save(FREE_IMAGE_FORMAT imageFormat, LPCTSTR pszFilePath)
{
  HRESULT hr = S_OK;
  hr = m_fipImage.saveU(imageFormat, pszFilePath) ? S_OK : E_FAIL;
  return hr;
}

// ====================================================================================================================
HRESULT CIjsImage::ParseFile(LPCTSTR pszFilePath)
{
  if (!pszFilePath || !_tcslen(pszFilePath))
  {
    DASSERT(FALSE);
    return E_INVALIDARG;
  }

  HRESULT hr = S_OK;

  fipImage image;
  if (image.loadU(pszFilePath))
  {
    if (!this->IsSupported(image.getInfo()))
    {
      hr = E_NOTIMPL;
    }
  }
  else
  {
    hr = E_ACCESSDENIED;
  }

  if (SUCCEEDED(hr))
  {
    m_strFilePath = pszFilePath;
    m_fipImage = image;
    m_bBitmap = m_fipImage.accessPixels();
  }

  return hr;
}

// ====================================================================================================================
bool CIjsImage::IsSupportedType(LPCTSTR strExt)
{
  if (!strExt || _tcslen(strExt) <= 0)
  {
    return false;
  }

  bool fSupported = false;
  for (int ix = 0; ix < NUMELMS(SUPPPORTED_IMAGE_TYPES); ix++)
  {
    if (_tcsicmp(strExt, SUPPPORTED_IMAGE_TYPES[ix]) == 0)
    {
      fSupported = true;
      break;
    }
  }
  return fSupported;
}

// ====================================================================================================================
ULONG CIjsImage::GetWidth()
{
  return m_fipImage.getWidth();
}

// ====================================================================================================================
ULONG CIjsImage::GetHeight()
{
  return m_fipImage.getHeight();
}

// ====================================================================================================================
ULONG CIjsImage::GetBpp()
{
  return m_fipImage.getBitsPerPixel();
}

// ====================================================================================================================
BYTE CIjsImage::GetPixel(ULONG x, ULONG y)
{
  return m_bBitmap[x + m_fipImage.getWidth() * y];
}

// ====================================================================================================================
void CIjsImage::SetPixel(ULONG x, ULONG y, BYTE val)
{
  m_bBitmap[x + m_fipImage.getWidth() * y] = val;
}

// ====================================================================================================================
bool CIjsImage::IsBitsSupported(const BITMAPINFOHEADER& sBmpheader)
{
  return utils::IsInArry<WORD>(SUPPORTED_BITS, sBmpheader.biBitCount);
}

// ====================================================================================================================
bool CIjsImage::IsPlanesSupported(const BITMAPINFOHEADER& sBmpheader)
{
  return utils::IsInArry<WORD>(SUPPORTED_PLANES, sBmpheader.biPlanes);
}

// ====================================================================================================================
bool CIjsImage::IsSupported(const BITMAPINFO* pBmpInfo)
{
  return IsBitsSupported(pBmpInfo->bmiHeader) && IsPlanesSupported(pBmpInfo->bmiHeader);
}
