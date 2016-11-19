#pragma once

#include <vector>

#include "iface/ApproxIface.h"
#include "rtl/hresult.h"
#include "rtl/referencedImpl.h"
#include "rtl/LogicalClockImpl.h"


#pragma warning( push )
#pragma warning( disable : 4250 ) // C4250 - 'class1' : inherits 'class2::member' via dominance

class CGlucoseLevels : public IGlucoseLevels, public virtual CReferenced, public virtual CLogical_Clock {
protected:
	std::vector<TGlucoseLevel> mLevels;
public:

	CGlucoseLevels() {};

	virtual ~CGlucoseLevels();
		//dctor has to be virtual, even if it is empty, due to the inheritance by dominance

	HRESULT IfaceCalling GetLevels(TGlucoseLevel** levels);
	HRESULT IfaceCalling GetLevelsCount(size_t* count);
	HRESULT IfaceCalling SetLevelsCount(size_t count);
	HRESULT IfaceCalling GetBounds(TGlucoseLevelBounds *bounds);
	HRESULT IfaceCalling SetLevels(const std::vector<TGlucoseLevel> &levels);
};

#pragma warning( pop )