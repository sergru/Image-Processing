#include "stdafx.h"
#include "IjsFilterSharpen.h"

const ULONG RADIUS = 1;
const double P_MASK[(2 * RADIUS + 1)][(2 * RADIUS + 1)] = {
  -0.125, -0.125, -0.125,
  -0.125, 2.0, -0.125,
  -0.125, -0.125, -0.125,
};


// ====================================================================================================================
CIjsFilterSharpen::CIjsFilterSharpen()
{
  SetMask(RADIUS, &P_MASK[0][0]);
}

// ====================================================================================================================
CIjsFilterSharpen::~CIjsFilterSharpen()
{
}
