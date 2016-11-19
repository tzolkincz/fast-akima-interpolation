#include "GlucoseLevels.h"

#include <tbb/tbb.h>
#include <tbb/parallel_reduce.h>


using namespace std;

HRESULT IfaceCalling CGlucoseLevels::GetLevels(TGlucoseLevel** levels) {
	if (mLevels.size() > 0) {

		*levels = &mLevels[0];
		return S_OK;
	}
	else
		return S_FALSE;
}

HRESULT IfaceCalling CGlucoseLevels::GetLevelsCount(size_t* count) {
	*count = mLevels.size();
	return S_OK;
}

HRESULT IfaceCalling CGlucoseLevels::SetLevelsCount(size_t count) {
	mLevels.resize(count);
	CLogical_Clock::Signal_Clock();
	return S_OK;
}

HRESULT IfaceCalling CGlucoseLevels::SetLevels(const std::vector<TGlucoseLevel> &levels) {
	mLevels = levels;
	return S_OK;
}

class CFindBounds {
	TGlucoseLevel *mLevels;
public:
	TGlucoseLevelBounds mBounds;

	CFindBounds(TGlucoseLevel* levels) : mLevels(levels) {
		mBounds.MaxLevel = mLevels[0].level;
		mBounds.MaxTime = mLevels[0].datetime;
		mBounds.MinLevel = mBounds.MaxLevel;
		mBounds.MinTime = mBounds.MaxTime;
	}

	CFindBounds(CFindBounds& x, tbb::split) : mLevels(x.mLevels), mBounds(x.mBounds) {}

	void operator()(const tbb::blocked_range<size_t>& r) {
		TGlucoseLevel *levels = mLevels;
		TGlucoseLevelBounds bounds = mBounds;
		size_t end = r.end();


		for (size_t i = r.begin(); i != end; ++i) {
			bounds.MaxLevel = max(bounds.MaxLevel, levels[i].level);
			bounds.MinLevel = min(bounds.MinLevel, levels[i].level);

			bounds.MaxTime = max(bounds.MaxTime, levels[i].datetime);
			bounds.MinTime = min(bounds.MinTime, levels[i].datetime);
		}

		mBounds = bounds;
	}



	void join(const CFindBounds& y) {
		mBounds.MaxLevel = max(mBounds.MaxLevel, y.mBounds.MaxLevel);
		mBounds.MinLevel = min(mBounds.MinLevel, y.mBounds.MinLevel);

		mBounds.MaxTime = max(mBounds.MaxTime, y.mBounds.MaxTime);
		mBounds.MinTime = min(mBounds.MinTime, y.mBounds.MinTime);
	}


};

HRESULT IfaceCalling CGlucoseLevels::GetBounds(TGlucoseLevelBounds *bounds) {

	size_t cnt = mLevels.size();
	if (cnt<1) return S_FALSE;

	CFindBounds fb(&mLevels[0]);
	tbb::parallel_reduce(tbb::blocked_range<size_t>(0, cnt), fb);

	*bounds = fb.mBounds;
	return S_OK;
}


CGlucoseLevels::~CGlucoseLevels() {}