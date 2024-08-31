/*
    nt5 -- Windows XP simulator.
    Copyright (C) 2023  Sergei Baigerov

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    Contact Sergei Baigerov -- @dogotrigger in Discord
*/

double _rendererLinear(double x) {
    return 1.f - x;
}

#include <math.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif

double _rendererInSine(double x) {
    return 1 - cosl((x * PI) / 2);
}
double _rendererOutSine(double x) {
    return sinl((x * PI) / 2);
}
double _rendererInOutSine(double x) {
    return -(cosl(PI * x) - 1) / 2;
}

double _rendererInQuad(double x) {
    return x * x;
}
double _rendererOutQuad(double x) {
    return 1.f - (1.f - x) * (1.f - x);
}
double _rendererInOutQuad(double x) {
    return x < 0.5f ? 2.f * x * x : 1 - powl(-2.f * x + 2.f, 2.f) / 2.f;
}

double _rendererInCubic(double x) {
    return x * x * x;
}
double _rendererOutCubic(double x) {
    return 1.f - powl(1.f - x, 3.f);
}
double _rendererInOutCubic(double x) {
    return x < 0.5f ? 4.f * _rendererInCubic(x) : 1.f - powl(-2.f * x + 2.f, 3.f) / 2.f;
}

double _rendererInQuart(double x) {
    return x * x * x * x;
}
double _rendererOutQuart(double x) {
    return 1.f - powl(1.f - x, 4.f);
}
double _rendererInOutQuart(double x) {
    return x < 0.5f ? 8.f * _rendererInQuart(x) : 1.f - powl(-2.f * x + 2.f, 4.f) / 2.f;
}

double _rendererInQuint(double x) {
    return x * x * x * x * x;
}
double _rendererOutQuint(double x) {
    return 1.f - powl(1.f - x, 5.f);
}
double _rendererInOutQuint(double x) {
    return x < 0.5f ? 16.f * x * x * x * x * x : 1.f - powl(-2.f * x + 2.f, 5.f) / 2;
}

double _rendererInExpo(double x) {
    return x == 0.f ? 0.f : powl(2.f, 10.f * x - 10.f);
}
double _rendererOutExpo(double x) {
    return x == 1.f ? 1.f : 1.f - powl(2.f, -10.f * x);
}
double _rendererInOutExpo(double x) {
    return x == 0.f ? 0.f : x == 1.f ? 1.f : x < 0.5f ? powl(2.f, 20.f * x - 10.f) / 2.f : (2.f - powl(2.f, -20.f * x + 10.f)) / 2.f;
}

double _rendererInCirc(double x) {
    return 1.f - sqrtl(1.f - powl(x, 2.f));
}
double _rendererOutCirc(double x) {
    return sqrtl(1.f - powl(x - 1.f, 2.f));
}
double _rendererInOutCirc(double x) {
    return x < 0.5f ? (1.f - sqrtl(1.f - powl(2.f * x, 2.f))) / 2.f : (sqrtl(1.f - powl(-2.f * x + 2.f, 2.f)) + 1.f) / 2.f;
}

double _rendererInBack(double x) {
    double c1 = 1.70158f;
	double c3 = c1 + 1.f;

	return c3 * _rendererInCubic(x) - c1 * x;
}
double _rendererOutBack(double x) {
    double c1 = 1.70158f;
	double c3 = c1 + 1.f;

	return 1.f + c3 * powl(x - 1.f, 3.f) + c1 * powl(x - 1.f, 2.f);
}
double _rendererInOutBack(double x) {
    double c1 = 1.70158f;
	double c2 = c1 * 1.525f;
	double c3 = c1 + 1.f;

	return x < 0.5f ? (powl(2.f * x, 2.f) * ((c2 + 1.f) * 2.f * x - c2)) / 2.f : (powl(2.f * x - 2.f, 2.f) * ((c2 + 1.f) * (x * 2.f - 2.f) + c2) + 2.f) / 2.f;
}

double _rendererInElastic(double x) {
    double c4 = (2.f * PI) / 3.f;

	return x == 0.f ? 0.f : x == 1.f ? 1.f : -powl(2.f, 10.f * x - 10.f) * sinl((x * 10.f - 10.75f) * c4);
}
double _rendererOutElastic(double x) {
    double c4 = (2.f * PI) / 3.f;

	return x == 0.f ? 0.f : x == 1.f ? 1.f : powl(2.f, -10.f * x) * sinl((x * 10.f - 0.75f) * c4) + 1.f;
}
double _rendererInOutElastic(double x) {
    double c5 = (2.f * PI) / 4.5f;

	return x == 0.f ? 0.f : x == 1.f ? 1.f : x < 0.5f ? -(powl(2.f, 20.f * x - 10.f) * sinl((20.f * x - 11.125f) * c5)) / 2.f : (powl(2.f, -20.f * x + 10.f) * sinl((20.f * x - 11.125f) * c5)) / 2.f + 1.f;
}

double _rendererOutBounce(double x) {
	double n1 = 7.5625f;
	double d1 = 2.75f;

	if (x < 1.f / d1) {
		return n1 * x * x;
	}
	else if (x < 2.f / d1) {
		return n1 * (x - (1.5f / d1)) * x + 0.75f;
	}
	else if (x < 2.5f / d1) {
		return n1 * (x - (2.25f / d1)) * x + 0.9375f;
	}
	else {
		return n1 * (x - (2.625f / d1)) * x + 0.984375f;
	}
}
double _rendererInBounce(double x) {
    return 1.f - _rendererOutBounce(1.f - x);
}
double _rendererInOutBounce(double x) {
    return x < 0.5f ? (1.f - _rendererOutBounce(1.f - 2.f * x)) / 2.f : (1.f + _rendererInBounce(2.f * x - 1.f)) / 2.f;
}