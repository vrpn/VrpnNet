#include "stdafx.h"

#include "BaseTypes.h"

using namespace System;

using namespace Vrpn;

Quaternion::Quaternion(Double x, Double y, Double z, Double w)
{
	X = x;
	Y = y;
	Z = z;
	W = w;
}