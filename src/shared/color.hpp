/***************************************************************************
 *   Copyright (C) 2005-2008, 2013 by Daniel Gulotta                       *
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

#include <QColor>
#include <QImage>
#include <cstdint>
#include <tuple>
#include <functional>
#include "randgen.hpp"


using std::uint8_t;

struct color_t {
  color_t()
    : red( 0 )
    , green( 0 )
    , blue( 0 )
  {}
  color_t( uint8_t r, uint8_t g, uint8_t b )
    : red( r )
    , green( g )
    , blue( b )
  {}
  color_t( int r, int g, int b )
    : red( static_cast<uint8_t>( r ) )
    , green( static_cast<uint8_t>( g ) )
    , blue( static_cast<uint8_t>( b ) )
  {}
  inline int red_int() const { return red; }
  inline int green_int() const { return green; }
  inline int blue_int() const { return blue; }
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

inline uint8_t& red_part( color_t& c )
{
  return c.red;
}
inline uint8_t& green_part( color_t& c )
{
  return c.green;
}
inline uint8_t& blue_part( color_t& c )
{
  return c.blue;
}

static const color_t black( 0, 0, 0 );
static const color_t white( 255, 255, 255 );

template<typename T>
uint8_t colorchop( T x )
{
  if ( x < 0 ) {
    return 0;
  } else if ( x > 255 ) {
    return 255;
  } else {
    return static_cast< uint8_t >( x ) ;
  }
}

static color_t randomColor()
{
  std::function<int()> func = []() { return random_range_inclusive( 1, 255 ); };
  return color_t{func(), func(), func()};
}

static color_t generateColor( const QImage& sourceImage, bool useHue, bool useSaturation, bool useLightness,
                              double saturationBoost  )
{
  if ( sourceImage.isNull() ) {
    return randomColor();
  }

  QColor color = sourceImage.pixel( random_int( sourceImage.width() ), random_int( sourceImage.height() ) );
  int h = useHue ? color.hslHue() : random_int( 360 );
  int s = useSaturation ? static_cast<int>( color.hslSaturation() * ( 1 + saturationBoost ) ) : 255;

  if ( s > 255 ) { s = 255; }

  int l = useLightness ? color.lightness() : 128;
  color = QColor::fromHsl( h, s, l );
  return color_t( color.red(), color.green(), color.blue() );
}
