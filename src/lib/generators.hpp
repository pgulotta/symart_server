/***************************************************************************
 *   Copyright (C) 2005, 2013-2018 by Daniel Gulotta                       *
 *   dgulotta@alum.mit.edu                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA         *
 *   02110-1301  USA                                                       *
 ***************************************************************************/

#pragma once

#include "canvas.hpp"
#include "color.hpp"
#include "hyperbolic_group.hpp"
#include "symmetric_canvas.hpp"

struct ImageData;
class QByteArray;

struct hyperbolic_lines_param {
  int size;
  float radius;
  float brightness;
  float sharpness;
  projtype proj;
};

void paint_hyperbolic_lines( ImageData& imageData, int size, int fdfIndex, int rotation0, int rotation1, int rotation2,
                             int rotation3, int projType, int flipType, float thickness, float sharpness, int ncolors );

void paint_stripes( ImageData& imageData, int sz, int symGroup, double alpha );

void paint_clusters( ImageData& imageData, int sz, int symGroup, double alpha );

void drawtrap( ImageData& imageData, int colRed, int colGreen, int colBlue, int sz, int symGroup );


void drawquasitrap( ImageData& imageData, int colRed, int colGreen, int colBlue, int width, int height,
                    int symmetryValueIndex, double quasiperiod );

void drawquasitrap_poly( ImageData& imageData, int col1Red, int col1Green, int col1Blue, int width, int height,
                         int symmetryValueIndex, double quasiperiod );

enum class walk_fill { AREA, LENGTH };

void draw_walk( ImageData& imageData, int width, int height, bool balanced, int walkFill );
void draw_walk2( ImageData& imageData, int width, int height, bool balanced, int walkFill );

#ifdef FFTWPROGS
#include <vector>

void paint_quasiperiodic_stripes( ImageData& imageData, int size, int fftsize, double alpha );

void  paint_squiggles( ImageData& imageData, double saturationBoost, bool useHue,
                       bool useSaturation, bool useLightness, int ncolors, int size, int symGroup,
                       double alpha, double exponent, double thickness, double sharpness, const QImage& colorImage );

void paint_squiggles( ImageData& imageData, int ncolors, int size, int symGroup, double alpha,
                      double exponent, double thickness, double sharpness );

void  update_squiggles( ImageData& imageData,  int size, int symGroup );

#endif

