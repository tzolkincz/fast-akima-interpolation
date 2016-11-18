#pragma once

#include "referencedIface.h"	//to get IfaceCalling, etc.

//A simple mechanism to detect whether a particular (possibly cached) object is out of date or not.
class ILogical_Clock {
public:
   virtual HRESULT IfaceCalling Signal_Clock() = 0;	//increments the clock
   virtual ULONG IfaceCalling Clock() = 0;		//returns the clock
};