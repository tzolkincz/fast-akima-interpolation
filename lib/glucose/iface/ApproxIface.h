#pragma once

#include "referencedIface.h"
#include "LogicalClockIface.h"

typedef double floattype;

//Units used for glucose levels
enum class GlucoseLevelsUnits
{
	MmolPerL, //mmol/l
	MgPerDl,	//mg/dl
	DgPerDl,	//dg/dl

};

//constant to convert mg/dl to mmol/l
extern const floattype mgdl2mmoll;	//we assume mmol/l, so to make a conversion possible

extern const floattype OneHour;
extern const floattype InvOneHour;	//1.0 / OneHour
extern const floattype OneMinute;
extern const floattype OneSecond;

extern const floattype HypoglycemiaThreshold;
extern const floattype HyperglycemiaThreshold;

typedef struct {
	floattype datetime;		//time of measuring
	floattype level;		//the glucose level/concetration measured
} TGlucoseLevel;

/*
  The time is encoded as the number of days since January 0, 1900 00:00 UTC, see
  http://en.wikipedia.org/wiki/January_0

  Integral part stores the number of days, fractional part stores the time.
  It could have been any fixed dates, but this one is compatible with
  FreePascal, Delphi and Microsoft Products such as Excel, Access and COM's variant in general.

  Therefore, 01 Jan 1900 00:00 would be 1.0 and 01 Jan 1900 24:00 would be 2.0

  However, the UI is supposed to use QDateTime whose epoch starts on 1.1. 1970 0:0 UTC0 aka UNIX epoch start.
  But note that leap seconds are not calculated with when using the UNIX epoch!
*/


typedef struct {
	floattype datetime;
	floattype bloodlevel;
	floattype istlevel;
} TPairedGlucoseLevels;


typedef struct {
	floattype MinTime,
			  MaxTime,
			  MinLevel,
			  MaxLevel;
} TGlucoseLevelBounds;

TGlucoseLevelBounds IntersectGlucoseLevelBounds(const TGlucoseLevelBounds &a, const TGlucoseLevelBounds &b);

extern const size_t apxmAverageExponential;		//so far, only exp has the derivation implemented
//extern const size_t apxmAverageLine;			//currently disabled as not derivation is not implemented for the line

//Epsilon Types - they cannot be declared with extern to allow using them with switch
const static size_t etFixedIterations = 1;		//fixed number of iterations
const static size_t etMaxAbsDiff = 2;			//maximum aboslute difference of all pointes
const static size_t etApproxRelative = 3;		//for each point, maximum difference < (Interpolated-Approximated)*epsilon for interpolated>=approximated
											//else maximum difference < (-Interpolated+Approximated)*epsilon for interpolated<approximated

extern const size_t apxNoDerivation;
extern const size_t apxFirstOrderDerivation;

typedef struct {
	size_t Passes;
	size_t Iterations;
	size_t EpsilonType;
	floattype Epsilon;
	floattype ResamplingStepping;
} TAvgExpApproximationParams;

typedef struct {
	size_t ApproximationMethod;	// = apxmAverageExponential
	union {
		TAvgExpApproximationParams avgexp;
	};
} TApproximationParams;


extern const TApproximationParams dfApproximationParams;

//recommended bufer sizes
const size_t rbsApproxBufSize = 10240;


class IGlucoseLevels : public virtual IReferenced, public virtual ILogical_Clock {
public:
	virtual HRESULT IfaceCalling GetLevels(TGlucoseLevel** levels) = 0;
		//provides just a pointer to SetLevelsCount number of elements
	virtual HRESULT IfaceCalling GetLevelsCount(size_t* count) = 0;
		//returns std::vector.size()
	virtual HRESULT IfaceCalling SetLevelsCount(size_t count) = 0;
		//calls std::vector.SetSize() and increments the clock
	virtual HRESULT IfaceCalling GetBounds(TGlucoseLevelBounds *bounds) = 0;
};

class IApproximatedGlucoseLevels : public virtual IReferenced, public virtual ILogical_Clock {
public:
	virtual HRESULT IfaceCalling Approximate(TApproximationParams *params) = 0;
		//calls ILogical_Clock->Signal_Clock

	virtual HRESULT IfaceCalling GetLevels(floattype desired_time, floattype stepping, size_t count,
		                                floattype *levels, size_t *filled, size_t derivationorder) = 0;
	/* time - time from which to calculate the approximation
	   stepping - distance between two times
	   count - the total number of times for which to get the approximation
	   levels - the approximated levels, must be already allocated with size of count
	   filled - the number of levels approximated
	*/


virtual HRESULT IfaceCalling GetLevels(floattype* times, size_t count,
		                                floattype *levels, size_t *filled) = 0;

	/* times has size of count
	   times holds time for which to calculate glucose level approximation
	   levels has to be allocated with size of count
	   levels will be filled with approximated glucose levels at respective indexes for respective times
	   first *filled  levels will be calculated
	 */

	   virtual HRESULT IfaceCalling GetBounds(TGlucoseLevelBounds *bounds) = 0;

};