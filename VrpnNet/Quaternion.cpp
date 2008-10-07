// Quaternion.cpp: Implementation for Vrpn.Quaternion
//
// Copyright (c) 2008 Chris VanderKnyff
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

// Portions of the quaternion code are based on Quatlib, a public domain library
// initially written by Warren Robinett and Richard Holloway.

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

String^ Quaternion::ToString()
{
	return String::Format("[{0}, {1}, {2}, {3}]", X, Y, Z, W);
}

Quaternion Quaternion::FromAxisAngle(Vector3 axis, double angle)
{
	double length = axis.Magnitude;

	if (length < Tolerance)
		return Quaternion(0, 0, 0, 1);

	axis.Normalize();

	double cosine = Math::Cos(angle / 2.0);
	double sine = Math::Sin(angle / 2.0);

	return Quaternion(sine * axis.X, sine * axis.Y, sine * axis.Z, cosine);
}

void Quaternion::ToAxisAngle(Vector3 %axis, double %angle)
{
	double length = Math::Sqrt(X*X + Y*Y + Z*Z);
	if (length < Tolerance)
	{
		axis = Vector3(0, 0, 1);
		angle = 0;
		return;
	}

	axis = Vector3(X / length, Y / length, Z / length);
	angle = 2 * Math::Acos(W);
}

void Quaternion::Normalize()
{
	double length = Math::Sqrt(X * X + Y * Y + Z * Z + W * W);

	X /= length;
	Y /= length;
	Z /= length;
	W /= length;
}

Quaternion Quaternion::Exp(Quaternion quat)
{
	double theta, scale;

	theta = Math::Sqrt(quat.X * quat.X + quat.Y * quat.Y + quat.Z * quat.Z);
	
	if (theta > Tolerance)
		scale = Math::Sin(theta) / theta;
	else
		scale = 1.0;

	return Quaternion(scale * quat.X, scale * quat.Y, scale * quat.Z, Math::Cos(theta));
}

Quaternion Quaternion::Log(Quaternion quat)
{
	double theta, scale;

	scale = Math::Sqrt(quat.X * quat.X + quat.Y * quat.Y + quat.Z * quat.Z);
	theta = Math::Atan2(scale, quat.W);
	
	if (scale > 0.0)
		scale = theta / scale;

	return Quaternion(scale * quat.X, scale * quat.Y, scale * quat.Z, 0.0);
}

Quaternion Quaternion::Invert(Quaternion quat)
{
	double length = Math::Sqrt(quat.X * quat.X + quat.Y * quat.Y + quat.Z * quat.Z + quat.W * quat.W);

	return Quaternion(-quat.X * length, -quat.Y * length, -quat.Z * length, quat.W);
}

Quaternion Quaternion::Conjugate(Quaternion quat)
{
	return Quaternion(-quat.X, -quat.Y, -quat.Z, quat.W);
}