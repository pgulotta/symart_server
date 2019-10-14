/***************************************************************************
 *   Copyright (C) 2014, 2016 by Daniel Gulotta                            *
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

#ifndef _IMAGEDATA_H
#define _IMAGEDATA_H

#include <memory>
#include <vector>
#include <QImage>

#include "color.hpp"
#include "canvasview.hpp"
#include "paintlinesadapter.hpp"

struct layer;



QImage makeImage( const CanvasView<color_t>& v );


struct ImageData {
  ImageData() : layers( nullptr ), original( true ) {}
  template<typename U>
  explicit ImageData( U&& c, const std::shared_ptr< std::vector<layer>> l = nullptr,
                      bool o = true ) : img( std::forward<U>( c ) ), layers( l ), original( o ) {}
  CanvasView<color_t> img;
  std::shared_ptr< std::vector<layer>> layers;
  bool original;
};

#endif
