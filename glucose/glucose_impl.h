
#include "../lib/glucose/iface/ApproxIface.h"
//#include "../lib/glucose/CommonApprox.h"












typedef double floattype;

#ifdef WIN32
	#define IfaceCalling __stdcall
#else
	#define IfaceCalling //empty macro for unix systems
#endif

//typedef struct {
//	floattype MinTime,
//			  MaxTime,
//			  MinLevel,
//			  MaxLevel;
//} TGlucoseLevelBounds;


//#ifdef _WIN32
//  #include <WTypes.h>
//#else
//  typedef int HRESULT;
//  typedef ulong ULONG;
//  const HRESULT S_OK = 0;
//  const HRESULT S_FALSE = -1;
//  const HRESULT E_INVALIDARG = 0x80070057;
//  const HRESULT E_NOTIMPL = 0x80000001L;
//
//  #define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
//  #define FAILED(hr) (((HRESULT)(hr)) < 0)
//#endif

//typedef struct {
//	floattype datetime;		//time of measuring
//	floattype level;		//the glucose level/concetration measured
//} TGlucoseLevel;


//typedef struct {
//	size_t ApproximationMethod;	// = apxmAverageExponential
//	union {
//		size_t avgexp;
//	};
//} TApproximationParams;










#include <vector>

#ifndef GLUCOSE_IMPL_H
#define GLUCOSE_IMPL_H

//class GlucoseImplementation : public CCommonApprox {
class GlucoseImplementation {
private:
	size_t count;
	std::vector<double> times;
	std::vector<double> levels;
	AlignedCoefficients coefficients;
	TGlucoseLevelBounds levelsBounds;

public:

//	GlucoseImplementation(IGlucoseLevels *levelsContainer) : CCommonApprox(levelsContainer) {
//	GlucoseImplementation(IGlucoseLevels *levelsContainer) {
	GlucoseImplementation(TGlucoseLevel *levels) {

//		levelsContainer->GetLevelsCount(&count);
//
//		TGlucoseLevel *levels;
//		levelsContainer->GetLevels(&levels);

		for (int i = 0; i < count; i++) {
			this->times[i] = levels[i].datetime;
			this->levels[i] = levels[i].level;
		}



//		levelsContainer->GetBounds(&levelsBounds);

	}


//	virtual ~GlucoseImplementation();
	//dctor has to be virtual, even if it is empty, due to the inheritance by dominance


	HRESULT IfaceCalling Approximate(TApproximationParams *params);
	//calls ILogical_Clock->Signal_Clock


	/**
	 *  time - time from which to calculate the approximation
  stepping - distance between two times
  count - the total number of times for which to get the approximation
  levels - the approximated levels, must be already allocated with size of count
  filled - the number of levels approximated
	 *
	 *
	 * @param desired_time
	 * @param stepping
	 * @param count
	 * @param levels
	 * @param filled
	 * @param derivationorder
	 * @return
	 */
	HRESULT IfaceCalling GetLevels(floattype desired_time, floattype stepping, size_t count,
			floattype *levels, size_t *filled, size_t derivationorder);



	/**
	 * times has size of count
	  times holds time for which to calculate glucose level approximation
	  levels has to be allocated with size of count
	  levels will be filled with approximated glucose levels at respective indexes for respective times
	  first *filled  levels will be calculated
	 *
	 *
	 * @param times
	 * @param count
	 * @param levels
	 * @param filled
	 * @return
	 */
//	HRESULT IfaceCalling GetLevels(floattype* times, size_t count,
//			floattype *levels, size_t *filled);
//
//
//
//	HRESULT IfaceCalling GetBounds(TGlucoseLevelBounds *bounds);
};



#endif /* GLUCOSE_IMPL_H */

