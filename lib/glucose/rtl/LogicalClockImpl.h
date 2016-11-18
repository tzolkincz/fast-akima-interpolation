#pragma once

#include "../iface/LogicalClockIface.h"

#include <atomic>

class CLogical_Clock : public virtual ILogical_Clock {
protected:
	std::atomic<ULONG> mClock;
public:
	CLogical_Clock();
	virtual ~CLogical_Clock();
	virtual HRESULT IfaceCalling Signal_Clock();
	virtual ULONG IfaceCalling Clock();
};