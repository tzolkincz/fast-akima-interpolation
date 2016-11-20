
#include "../lib/glucose/iface/ApproxIface.h"
#include "../lib/glucose/CommonApprox.h"


#include <vector>

#ifndef GLUCOSE_IMPL_H
#define GLUCOSE_IMPL_H

class GlucoseImplementation : public CCommonApprox {
private:
	size_t count;
	std::vector<double> times;
	std::vector<double> levels;
	AlignedCoefficients coefficients;
	TGlucoseLevelBounds levelsBounds;
	bool isApproximed = false;

public:

	GlucoseImplementation(IGlucoseLevels *levelsContainer) : CCommonApprox(levelsContainer) {

		levelsContainer->GetLevelsCount(&count);

		TGlucoseLevel *g_levels;
		levelsContainer->GetLevels(&g_levels);

		times = std::vector<double>(count);
		levels = std::vector<double>(count);

		for (int i = 0; i < count; i++) {
			times[i] = g_levels[i].datetime;
			levels[i] = g_levels[i].level;
		}

		levelsContainer->GetBounds(&levelsBounds);
	}

	virtual ~GlucoseImplementation() {
	};
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
	HRESULT IfaceCalling GetLevels(floattype* times, size_t count,
			floattype *levels, size_t *filled);



	HRESULT IfaceCalling GetBounds(TGlucoseLevelBounds *bounds);
};



#endif /* GLUCOSE_IMPL_H */

