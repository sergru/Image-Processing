/**
  Smart data pointer to manage the data indifferent threads and code modules.
*/

#pragma once
#include <windows.h>

// ====================================================================================================================
template <class T>
class CSmartPtr
{
protected:
  /**
    Smart data storage class.
  */
  // ==========================================================================
  class CSmartData
  {
    friend class CSmartPtr;

  public:
    CSmartData();
    ~CSmartData();

    T*   DataGet();
    void DataSet(T* ptData);

    void DataLock();
    void DataUnlock();

  protected:
    bool IsMoreReferences();
    void RefIncrement();
    void RefDecrement();

  private:
    ULONG             m_ulRefCount;
    T*                m_ptData;
    CRITICAL_SECTION  m_csData;
    CRITICAL_SECTION  m_csRefCount;
  };
  // ==========================================================================
  CSmartData*	m_pSmartData;

protected:
  void InitSmartData();
  void Dispose();

public:
  // Default constructor.
  CSmartPtr();
  // Initialization constructor.
  CSmartPtr(T* tData);
  // Copy constructor.
  CSmartPtr(const CSmartPtr& SmartPtr);

  ~CSmartPtr();

  bool IsValid() const;
  void Lock();
  void Unlock();

  // Operators:
  T& operator*() const;
  T* operator->() const;
  T* operator=(T* pData);
  T* operator=(T Data);
  const CSmartPtr& operator=(const CSmartPtr& SmartPtr);
  bool operator!() const;
  bool operator==(const CSmartPtr& SmartPtr) const;
  bool operator==(const T* ptData) const;
  bool operator!=(const CSmartPtr& SmartPtr) const;
  bool operator!=(const T* ptData) const;

  template <class U>
  bool operator==(const CSmartPtr<U>& SmartPtr) const
  {
    return (m_pSmartData->DataGet() == SmartPtr.m_pSmartData->DataGet());
  };

  template <class U>
  bool operator!=(const CSmartPtr<U>& SmartPtr) const
  {
    return (m_pSmartData->DataGet() != SmartPtr.m_pSmartData->DataGet());
  };
  
  template <class U>
  bool operator==(const U* puData) const
  {
    return (m_pSmartData->DataGet() == puData);
  };
  
  template <class U>
  bool operator!=(const U* puData) const
  {
    return (m_pSmartData->DataGet() != puData);
  };
  
  friend bool operator==(const T* ptData, const CSmartPtr& SmartPtr)
  {
    return (ptData == SmartPtr.m_pSmartData->DataGet());
  };
  
  friend bool operator!=(const T* ptData, const CSmartPtr& SmartPtr)
  {
    return !(ptData == SmartPtr);
  };
  
  template <class U>
  friend bool operator==(const U* ptData, const CSmartPtr& SmartPtr)
  {
    return (ptData == SmartPtr.m_pSmartData->DataGet());
  
  };
  template <class U>
  friend bool operator!=(const U* ptData, const CSmartPtr& SmartPtr)
  {
    return (ptData == SmartPtr.m_pSmartData->DataGet());
  };
};


// ====================================================================================================================
template <class T>
CSmartPtr<T>::CSmartData::CSmartData()
{
  m_ulRefCount = 0;
  m_ptData = NULL;
  InitializeCriticalSection(&m_csData);
  InitializeCriticalSection(&m_csRefCount);
};

// ====================================================================================================================
template <class T>
CSmartPtr<T>::CSmartData::~CSmartData()
{
  RefDecrement();
  DeleteCriticalSection(&m_csData);
  DeleteCriticalSection(&m_csRefCount);
};

// ====================================================================================================================
template <class T>
void CSmartPtr<T>::CSmartData::DataLock()
{
  EnterCriticalSection(&m_csData);
};

// ====================================================================================================================
template <class T>
void CSmartPtr<T>::CSmartData::DataUnlock()
{
  LeaveCriticalSection(&m_csData);
};

// ====================================================================================================================
template <class T>
T* CSmartPtr<T>::CSmartData::DataGet()
{
  DataLock();
  T* ptData = m_ptData;
  DataUnlock();
  return ptData;
};

// ====================================================================================================================
template <class T>
void CSmartPtr<T>::CSmartData::DataSet(T* ptData)
{
  DataLock();
  if (m_ptData)
  {
    delete m_ptData;
  }
  m_ptData = ptData;
  DataUnlock();
};

// ====================================================================================================================
template <class T>
bool CSmartPtr<T>::CSmartData::IsMoreReferences()
{
  EnterCriticalSection(&m_csRefCount);
  bool isMore = (m_ulRefCount != 0);
  LeaveCriticalSection(&m_csRefCount);
  return isMore;
};

// ====================================================================================================================
template <class T>
void CSmartPtr<T>::CSmartData::RefIncrement()
{
  EnterCriticalSection(&m_csRefCount);
  m_ulRefCount++;
  LeaveCriticalSection(&m_csRefCount);
};

// ====================================================================================================================
template <class T>
void CSmartPtr<T>::CSmartData::RefDecrement()
{
  EnterCriticalSection(&m_csRefCount);
  if (m_ulRefCount != 0)
  {
    m_ulRefCount--;
    if (m_ulRefCount == 0)
    {
      if (m_ptData)
      {
        delete m_ptData;
        m_ptData = NULL;
      }
    }
  }
  LeaveCriticalSection(&m_csRefCount);
};

// ====================================================================================================================
template <class T>
void CSmartPtr<T>::InitSmartData()
{
  m_pSmartData = new CSmartData;
};

// ====================================================================================================================
template <class T>
void CSmartPtr<T>::Dispose()
{
  if (m_pSmartData)
  {
    m_pSmartData->RefDecrement();
    if (!m_pSmartData->IsMoreReferences())
    {
      // If nobody uses smart data object, it should be deleted.
      delete m_pSmartData;
    }

    m_pSmartData = NULL;
  }
};

// ====================================================================================================================
template <class T>
CSmartPtr<T>::CSmartPtr()
{
  InitSmartData();
};

// ====================================================================================================================
template <class T>
CSmartPtr<T>::CSmartPtr(T* ptData)
{
  InitSmartData();
  if (m_pSmartData)
  {
    m_pSmartData->DataSet(ptData);
    m_pSmartData->RefIncrement();
  }
};

// ====================================================================================================================
template <class T>
CSmartPtr<T>::CSmartPtr(const CSmartPtr<T>& SmartPtr)
{
  m_pSmartData = SmartPtr.m_pSmartData;
  if (m_pSmartData)
  {
    m_pSmartData->RefIncrement();
  }
};

// ====================================================================================================================
template <class T>
CSmartPtr<T>::~CSmartPtr()
{
  Dispose();
};

// ====================================================================================================================
template <class T>
bool CSmartPtr<T>::IsValid() const
{
  return(m_pSmartData && m_pSmartData->DataGet());
};

// ====================================================================================================================
template <class T>
void CSmartPtr<T>::Lock()
{
  if (m_pSmartData)
  {
    m_pSmartData->DataLock();
  }
};

// ====================================================================================================================
template <class T>
void CSmartPtr<T>::Unlock()
{
  if (m_pSmartData)
  {
    m_pSmartData->DataUnlock();
  }
};

// ====================================================================================================================
template <class T>
T& CSmartPtr<T>::operator*() const
{
  // If m_pSmartData is not exist, it will be an error.
  // But we cannot return anything in this case.
  return *(m_pSmartData->DataGet());
};

// ====================================================================================================================
template <class T>
T* CSmartPtr<T>::operator->() const
{
  if (m_pSmartData)
  {
    return m_pSmartData->DataGet();
  }
  else
  {
    return NULL;
  }
};

// ====================================================================================================================
template <class T>
T* CSmartPtr<T>::operator=(T* ptData)
{
  Dispose();
  InitSmartData();
  if (!m_pSmartData)
  {
    return NULL;
  }

  m_pSmartData->DataSet(ptData);
  m_pSmartData->RefIncrement();
  return m_pSmartData->DataGet();
};

// ====================================================================================================================
template <class T>
T* CSmartPtr<T>::operator=(T tData)
{
  T* ptData = new T;
  *ptData = tData;
  m_pSmartData->DataSet(ptData);
  return m_pSmartData->DataGet();
}

// ====================================================================================================================
template <class T>
const CSmartPtr<T>& CSmartPtr<T>::operator=(const CSmartPtr<T>& SmartPtr)
{
  if (!m_pSmartData || (m_pSmartData != SmartPtr.m_pSmartData))
  {
    Dispose();
    m_pSmartData = SmartPtr.m_pSmartData;
    if (m_pSmartData)
    {
      m_pSmartData->RefIncrement();
    }
  }
  return *this;
};

// ====================================================================================================================
template <class T>
bool CSmartPtr<T>::operator!() const
{
  return(!m_pSmartData || m_pSmartData->DataGet());
};

// ====================================================================================================================
template <class T>
bool CSmartPtr<T>::operator==(const CSmartPtr<T>& SmartPtr) const
{
  return(m_pSmartData == SmartPtr.m_pSmartData);
};

// ====================================================================================================================
template <class T>
bool CSmartPtr<T>::operator==(const T* ptData) const
{
  return(m_pSmartData->DataGet() == ptData);
};

// ====================================================================================================================
template <class T>
bool CSmartPtr<T>::operator!=(const CSmartPtr<T>& SmartPtr) const
{
  return(m_pSmartData != SmartPtr.m_pSmartData);
};

// ====================================================================================================================
template <class T>
bool CSmartPtr<T>::operator!=(const T* ptData) const
{
  return(m_pSmartData->DataGet() != ptData);
};
