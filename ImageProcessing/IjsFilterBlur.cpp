#include "stdafx.h"
#include "IjsFilterBlur.h"


const ULONG RADIUS = 1;
const double P_MASK [(2 * RADIUS + 1)][(2 * RADIUS + 1)] = {
  1.0 / 9, 1.0 / 9, 1.0 / 9,
  1.0 / 9, 1.0 / 9, 1.0 / 9,
  1.0 / 9, 1.0 / 9, 1.0 / 9,
};


// ====================================================================================================================
CIjsFilterBlur::CIjsFilterBlur()
{
  SetMask(RADIUS, &P_MASK[0][0]);
}

// ====================================================================================================================
CIjsFilterBlur::~CIjsFilterBlur()
{
}
