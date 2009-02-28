// AnalogServerChannel.cpp: Implementation for Vrpn.AnalogServerChannel
//
// Copyright (c) 2008-2009 Chris VanderKnyff
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "stdafx.h"
#include "AnalogServerChannel.h"

using namespace System;
using namespace Vrpn;

AnalogServerChannel::AnalogServerChannel()
{
	m_value = 0;
	ClipValues = false;
	Dirty = true;
}

double AnalogServerChannel::Value::get()
{
	return m_value;
}

void AnalogServerChannel::Value::set(double value)
{
	if (ClipValues)
	{
		if (value <= Minimum)
			m_value = -1;
		else if (value < ZeroMin)
			m_value = (value - ZeroMin) / (ZeroMin - Minimum);
		else if (value <= ZeroMax)
			m_value = 0;
		else
			m_value = (value - ZeroMax) / (Maximum - ZeroMax);
	}
	else
	{
		m_value = value;
	}

	Dirty = true;
}