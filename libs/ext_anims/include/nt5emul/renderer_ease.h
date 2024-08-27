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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

double _rendererLinear(double x);

double _rendererInSine(double x);
double _rendererOutSine(double x);
double _rendererInOutSine(double x);

double _rendererInQuad(double x);
double _rendererOutQuad(double x);
double _rendererInOutQuad(double x);

double _rendererInCubic(double x);
double _rendererOutCubic(double x);
double _rendererInOutCubic(double x);

double _rendererInQuart(double x);
double _rendererOutQuart(double x);
double _rendererInOutQuart(double x);

double _rendererInQuint(double x);
double _rendererOutQuint(double x);
double _rendererInOutQuint(double x);

double _rendererInExpo(double x);
double _rendererOutExpo(double x);
double _rendererInOutExpo(double x);

double _rendererInCirc(double x);
double _rendererOutCirc(double x);
double _rendererInOutCirc(double x);

double _rendererInBack(double x);
double _rendererOutBack(double x);
double _rendererInOutBack(double x);

double _rendererInElastic(double x);
double _rendererOutElastic(double x);
double _rendererInOutElastic(double x);

double _rendererInBounce(double x);
double _rendererOutBounce(double x);
double _rendererInOutBounce(double x);

#ifdef __cplusplus
}
#endif