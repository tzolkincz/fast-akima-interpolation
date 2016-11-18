#include "LogicalClockImpl.h"

CLogical_Clock::CLogical_Clock() : mClock{0} {
}

CLogical_Clock::~CLogical_Clock() {

}

HRESULT IfaceCalling CLogical_Clock::Signal_Clock() {
	mClock++;
	return S_OK;
}

ULONG IfaceCalling CLogical_Clock::Clock() {
	return mClock;
}