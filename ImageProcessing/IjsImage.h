#pragma once
#include <string>

#include "FreeImagePlus.h"

/**
  Class that responsible for the access to all image file data.
*/
class CIjsImage
{
public:
  static bool IsSupportedType(LPCTSTR strExt);

public:
  CIjsImage();
 virtual ~CIjsImage();

 HRESULT Create(ULONG width, ULONG height, ULONG bpp);
 HRESULT Save(FREE_IMAGE_FORMAT, LPCTSTR pszFilePath);
 void Free();

 // Validate input file and parse it's data.
 HRESULT ParseFile(LPCTSTR pszFilePath);

 ULONG GetWidth();
 ULONG GetHeight();
 ULONG GetBpp();

 BYTE GetPixel(ULONG x, ULONG y);
 void SetPixel(ULONG x, ULONG y, BYTE val);

protected: // methods
  bool IsBitsSupported(const BITMAPINFOHEADER& sBmpheader);
  bool IsPlanesSupported(const BITMAPINFOHEADER& sBmpheader);
  bool IsSupported(const BITMAPINFO* pBmpInfo);

protected:
  CString m_strFilePath;
  fipImage m_fipImage;
  BYTE* m_bBitmap;
};
