#pragma once

#ifdef _WIN32
  #include <WTypes.h>
#else
  typedef int HRESULT;
  typedef ulong ULONG;
  const HRESULT S_OK = 0;
  const HRESULT S_FALSE = -1;
  const HRESULT E_INVALIDARG = 0x80070057;
  const HRESULT E_NOTIMPL = 0x80000001L;

  #define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
  #define FAILED(hr) (((HRESULT)(hr)) < 0)
#endif