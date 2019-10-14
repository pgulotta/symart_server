/***************************************************************************
 *   Copyright (C) 2005-2007, 2013, 2016 by Daniel Gulotta                 *
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

#ifndef _PAINTCLOUDS_H
#define _PAINTCLOUDS_H

#include "randgen.hpp"
#include "color.hpp"
#include "hyperbolic_group.hpp"
#include "symmetric_canvas.hpp"

class ImageData;

using clouds_randfunc = double ( * )( double );

void paint_clouds( ImageData& imageData, int size, int symmmetryGroup, int col1Red, int col1Green,
                   int col1Blue, int col2Red, int col2Green, int col2Blue, int col3Red,
                   int col3Green, int col3Blue, int randfuncsIndex );

void paint_hyperbolic_clouds( ImageData& imageData, int size, int fdfIndex, int rotation0, int rotation1,
                              int rotation2, int rotation3, int projType, int col1Red, int col1Green,
                              int col1Blue, int col2Red, int col2Green, int col2Blue,
                              int col3Red, int col3Green, int col3Blue, int randfuncsIndex );












#endif
