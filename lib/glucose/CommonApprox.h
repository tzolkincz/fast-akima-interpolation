#pragma once

#include "./iface/ApproxIface.h"
#include "./rtl/hresult.h"
#include "./rtl/referencedImpl.h"
#include "./rtl/LogicalClockImpl.h"


extern const floattype dfYOffset; //some interpolation requires negative values
//and it is impossible to compute real ln of a negative number
//100.0 and lower do not work always

#pragma warning( push )
#pragma warning( disable : 4250 ) // C4250 - 'class1' : inherits 'class2::member' via dominance

class CCommonApprox : public IApproximatedGlucoseLevels, public virtual CReferenced, public virtual CLogical_Clock {
protected:
	IGlucoseLevels *mEnumeratedLevels;
public:
	CCommonApprox(IGlucoseLevels *levels);
	virtual ~CCommonApprox();
	//dctor has to be virtual, even if it is empty, due to the inheritance by dominance
};

#pragma warning( pop )