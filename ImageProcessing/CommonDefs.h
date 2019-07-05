/**
  Useful common definitions.
*/

#pragma once
#include <Windows.h>
#include <stdio.h>
#include "stdafx.h"

#define COMP_STRING2(x) #x
#define COMP_STRING(x) COMP_STRING2(x)
#define TEXT_WITH_FILE_AND_LINE(_str)  __FILE__ "(" COMP_STRING(__LINE__) "): " _str
#define REMIND(str)   message( TEXT_WITH_FILE_AND_LINE(str) )

#ifdef _DEBUG
inline void _FancyDassert(bool fOk, char* pszDesc)
{
  if (!fOk)
  {
    __try
    {
      DebugBreak();
    }
    __except ((GetExceptionCode() == EXCEPTION_BREAKPOINT) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
    {
      __debugbreak();
    }
    OutputDebugStringA(pszDesc);
  }
}

#define DASSERT(_isOk_) _FancyDassert(_isOk_, TEXT_WITH_FILE_AND_LINE("DASSERT("#_isOk_")"))
#else
#define DASSERT(_isOk_)
#endif

#define IS_FLAG_SET(_dwVal, _dwFlagMask) ( ((_dwVal)&(_dwFlagMask)) != 0 )
#define ZERO_STRUCT(_pStruct) ZeroMemory(_pStruct, sizeof(*_pStruct));
#define NUMBYTES(_Var) sizeof(_Var)
#define NUMELMS(_Ptr) (sizeof(_Ptr)/sizeof(*_Ptr))
#define NUMCHARS NUMELMS
#define T_NULL _T('\0')

namespace utils
{
  // ==========================================================================
  inline void SetThreadName
  (
    LPCSTR pszThreadName,
    DWORD dwThreadID = (DWORD)-1  //(-1 == caller thread)
  )
  {
    if (pszThreadName && dwThreadID)
    {
#pragma pack(push,8)
      struct SThreadNameInfo
      {
        DWORD dwType;     // Must be 0x1000.
        LPCSTR szName;    // Pointer to name (in user address space).
        DWORD dwThreadID; // Thread ID (-1=caller thread).
        DWORD dwFlags;    // Reserved for future use, must be zero.
      };
#pragma pack(pop)

      const DWORD MS_VC_EXCEPTION = 0x406D1388;
      const DWORD MS_VC_TYPE_RESERVED = 0x1000;
      SThreadNameInfo info;
      info.dwType = MS_VC_TYPE_RESERVED;
      info.szName = pszThreadName;
      info.dwThreadID = dwThreadID;
      info.dwFlags = 0;

      __try
      {
        RaiseException(MS_VC_EXCEPTION, 0, 4, (ULONG_PTR*)&info);
      }
      __except (EXCEPTION_EXECUTE_HANDLER) {}
    }
  }

  // ====================================================================================================================
  inline HRESULT MakeEvent(
    HANDLE& hEvent,
    LPSECURITY_ATTRIBUTES pSecAttr = NULL,
    BOOL bManReset = FALSE,
    BOOL bInitState = FALSE,
    LPCTSTR pName = NULL
  )
  {
    hEvent = CreateEvent(pSecAttr, bManReset, bInitState, pName);
    if (!hEvent)
    {
      DASSERT(FALSE);
      return E_FAIL;
    }
    return S_OK;
  }

  // ====================================================================================================================
  inline void CloseEvent(HANDLE& hEvent)
  {
    if (hEvent)
    {
      CloseHandle(hEvent);
      hEvent = NULL;
    }
  }

  // ====================================================================================================================
  template <typename T>
  inline static bool IsInArry(const T array[], const T& val) {
    int arraySize = NUMELMS(array);
    for (int i = 0; i < arraySize; ++i) {
      if (array[i] == val) {
        return true;
      }
    }
    return false;
  }

}
