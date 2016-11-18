#pragma once

#include "../iface/referencedIface.h"

//#if __cplusplus <= 199711L
    //such a check should work, yet it does not with MS
#if _MSC_VER > 1600L
  #define cpp0x
#else
  //#include <Windows.h>
#endif


#ifdef cpp0x
	#include <atomic>
#endif


class CReferenced : public virtual IReferenced {
protected:
#ifdef cpp0x
	std::atomic<ULONG> mCounter;
#else
    ULONG mCounter;
#endif
public:
	CReferenced() : mCounter(0) {};
	virtual ~CReferenced() {};

	virtual HRESULT IfaceCalling QueryInterface(/*REFIID */ void*  riid, void ** ppvObj);
	virtual ULONG IfaceCalling AddRef();
	virtual ULONG IfaceCalling Release();
};


class CNotReferenced : public CReferenced {
public:
	virtual ULONG IfaceCalling AddRef();
	virtual ULONG IfaceCalling Release();
};
