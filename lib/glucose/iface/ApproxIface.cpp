#include "ApproxIface.h"

#include <algorithm>

#undef min
#undef max

const floattype mgdl2mmoll = 1.0 / 18.0182;
	//http://www.soc-bdr.org/rds/authors/unit_tables_conversions_and_genetic_dictionaries/conversion_glucose_mg_dl_to_mmol_l/index_en.html

const floattype OneHour   = 1.0 / (24.0);
const floattype InvOneHour = 24.0;
const floattype OneMinute = 1.0 / (24.0*60.0);
const floattype OneSecond = 1.0 / (24.0*60.0*60.0);

const size_t apxmAverageExponential = 1;
const size_t apxmAverageLine = 2;

const size_t apxNoDerivation = 0;
const size_t apxFirstOrderDerivation = 1;


const floattype HypoglycemiaThreshold = 4.0;
const floattype HyperglycemiaThreshold = 11.0;

const TApproximationParams dfApproximationParams = {
	apxmAverageExponential, //ApproximationMethod
	{	//avgexp
		10, //Passes
		0, // Iterations
		etFixedIterations, // EpsilonType
		0.1, //Epsilon
		OneSecond// ResamplingStepping
	}
};


TGlucoseLevelBounds IntersectGlucoseLevelBounds(const TGlucoseLevelBounds &a, const TGlucoseLevelBounds &b) {
	TGlucoseLevelBounds result;

	result.MaxLevel = std::min(a.MaxLevel, b.MaxLevel);
	result.MinLevel = std::max(a.MinLevel, b.MinLevel);

	result.MaxTime = std::min(a.MaxTime, b.MaxTime);
	result.MinTime = std::max(a.MinTime, b.MinTime);

	return result;
}