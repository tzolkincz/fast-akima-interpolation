#include "CommonApprox.h"

const floattype dfYOffset = 10000.0;

CCommonApprox::CCommonApprox(IGlucoseLevels *levels) : mEnumeratedLevels(levels) {
	if (mEnumeratedLevels != NULL) mEnumeratedLevels->AddRef();
}

CCommonApprox::~CCommonApprox() {
	if (mEnumeratedLevels != NULL) mEnumeratedLevels->Release();
}
