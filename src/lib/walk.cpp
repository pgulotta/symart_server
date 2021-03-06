/***************************************************************************
 *   Copyright (C) 2017 by Daniel Gulotta                                  *
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

#include <algorithm>
#include <numeric>
#include <tuple>
#include <vector>
#include "randgen.hpp"
#include "generators.hpp"
#include "imagedata.hpp"

using namespace std;

int sgn( long x )
{
  if ( x > 0 ) {
    return 1;
  } else if ( x < 0 ) {
    return -1;
  } else {
    return 0;
  }
}

struct level_struct_1 {
  using out_type = wrap_canvas<uint8_t>;
  level_struct_1( const wrap_canvas<uint8_t>& cc )
    : c( cc ), mark( 2 * cc.width(), 2 * cc.height(), -1 ), dirs( 2 * cc.width(), 2 * cc.height() ) {}
  bool out_of_bounds( int, int ) { return false; }
  long area( long a, int i, int j, int x, int y ) { return ( x == i && y == j ) ? a : 0; }
  const wrap_canvas<uint8_t>& c;
  wrap_canvas<int> mark;
  wrap_canvas<tuple<int8_t, int8_t>> dirs;
  std::vector<long> areas;
  std::vector<int> lengths;
};

struct level_struct_2 {
  using out_type = canvas<uint8_t>;
  level_struct_2( const canvas<uint8_t>& cc )
    : c( cc ), mark( 2 * cc.width() + 1, 2 * cc.height() + 1, -1 ), dirs( 2 * cc.width() + 1, 2 * cc.height() + 1 ) {}
  bool out_of_bounds( int x, int y ) { return x < 0 || x >= mark.width() || y < 0 || y >= mark.height(); }
  long area( long a, int i, int j, int x, int y ) { return a + ( long )( j + y ) * ( i - x ); }
  const canvas<uint8_t>& c;
  canvas<int> mark;
  canvas<tuple<int8_t, int8_t>> dirs;
  std::vector<long> areas;
  std::vector<int> lengths;
};

template<typename LS>
void step( LS& l, int i, int j, int dx, int dy )
{
  if ( l.mark( i, j ) >= 0 ) { return; }

  int x = i, y = j, n = l.areas.size();
  long area = 0;
  int length = 0;

  while ( l.mark( x, y ) < 0 ) {
    l.mark( x, y ) = n;
    l.dirs( x, y ) = make_tuple( ( int8_t )dx, ( int8_t )dy );

    if ( l.out_of_bounds( x + dx, y + dy ) ) {
      break;
    }

    int cc = l.c( divide( x + dx, 2 ), divide( y + dy, 2 ) );
    int newdx, newdy;

    switch ( cc ) {
    case 0:
      newdx = dy;
      newdy = dx;
      break;

    case 1:
      newdx = -dy;
      newdy = -dx;
      break;

    default:
      newdx = dx;
      newdy = dy;
    }

    area += ( 2 * y + dy + newdy ) * ( dx + newdx );
    length += 1;
    x += dx + newdx;
    y += dy + newdy;
    dx = newdx;
    dy = newdy;
  }

  l.areas.push_back( l.area( area, i, j, x, y ) );
  l.lengths.push_back( length );
}

template<typename LS>
typename LS::out_type fill( LS& l )
{
  int w = l.mark.width() - l.c.width(), h = l.mark.height() - l.c.height();
  int maxval = 0, minval = 0;

  for ( int i = 0; i < w; i++ ) {
    int newm;

    if ( i == 0 ) {
      newm = 0;
    } else {
      int s = sgn( l.areas[l.mark( 2 * i - 1, 0 )] * std::get<1>( l.dirs( 2 * i - 1, 0 ) ) );
      newm = l.mark( 2 * i - 2, 0 ) - s;
    }

    if ( newm > maxval ) {
      maxval = newm;
    }

    if ( newm < minval ) {
      minval = newm;
    }

    l.mark( 2 * i, 0 ) = newm;

    for ( int j = 1; j < h; j++ ) {
      int s =	sgn( l.areas[l.mark( 2 * i, 2 * j - 1 )] * std::get<0>( l.dirs( 2 * i, 2 * j - 1 ) ) );
      int newm = l.mark( 2 * i, 2 * j - 2 ) + s;

      if ( newm > maxval ) {
        maxval = newm;
      }

      if ( newm < minval ) {
        minval = newm;
      }

      l.mark( 2 * i, 2 * j ) = newm;
    }
  }

  typename LS::out_type sc( w, h );

  for ( int i = 0; i < w; i++ )
    for ( int j = 0; j < h; j++ ) {
      double ll = 255.99 * ( l.mark( 2 * i, 2 * j ) - minval ) / ( maxval - minval );
      uint8_t col = colorchop( ll );
      sc( i, j ) = col;
    }

  return sc;
}

struct fill2mag {
  fill2mag( const vector<int>& l ) : lengths( l ), sorted( l ), total( l.size() )
  {
    sort( sorted.begin(), sorted.end() );
    partial_sum( sorted.begin(), sorted.end(), total.begin() );
  }
  double operator () ( int n )
  {
    auto r = equal_range( sorted.begin(), sorted.end(), lengths[n] );
    int i1 = r.first - sorted.begin() - 1;
    int i2 = r.second - sorted.begin() - 1;
    int p1 = i1 < 0 ? 0 : total[i1];
    int p2 = total[i2];
    return ( p1 + p2 ) / ( 2.*total.back() );
  }
  const vector<int>& lengths;
  vector<int> sorted;
  vector<int> total;
};

template<typename LS>
typename LS::out_type fill2( LS& l )
{
  int w = l.mark.width() - l.c.width(), h = l.mark.height() - l.c.height();
  fill2mag mag( l.lengths );
  typename LS::out_type sc( w, h );

  for ( int i = 0; i < w; i++ )
    for ( int j = 0; j < h; j++ ) {
      double v = 0.;

      if ( !l.out_of_bounds( 2 * i - 1, 2 * j ) ) {
        v += mag( l.mark( 2 * i - 1, 2 * j ) );
      }

      if ( !l.out_of_bounds( 2 * i, 2 * j - 1 ) ) {
        v += mag( l.mark( 2 * i, 2 * j - 1 ) );
      }

      if ( !l.out_of_bounds( 2 * i + 1, 2 * j ) ) {
        v += mag( l.mark( 2 * i + 1, 2 * j ) );
      }

      if ( !l.out_of_bounds( 2 * i, 2 * j + 1 ) ) {
        v += mag( l.mark( 2 * i, 2 * j + 1 ) );
      }

      uint8_t col = colorchop( 63.999 * v );
      sc( i, j ) = col;
    }

  return sc;
}

wrap_canvas<uint8_t> levels( const wrap_canvas<uint8_t>& c, walk_fill wf )
{
  int w = c.width(), h = c.height();
  level_struct_1 ls( c );

  for ( int i = 0; i < 2 * w; i++ )
    for ( int j = ( i & 1 ) ^ 1; j < 2 * h; j += 2 ) {
      step( ls, i, j, j & 1, i & 1 );
    }

  return wf == walk_fill::AREA ? fill( ls ) : fill2( ls );
}

canvas<uint8_t> levels2( const canvas<uint8_t>& c, walk_fill wf )
{
  int w = c.width(), h = c.height();
  level_struct_2 ls( c );

  for ( int i = 1; i < 2 * w; i += 2 ) {
    step( ls, i, 0, 0, 1 );
    step( ls, i, 2 * h, 0, -1 );
  }

  for ( int j = 1; j < 2 * h; j += 2 ) {
    step( ls, 0, j, 1, 0 );
    step( ls, 2 * w, j, -1, 0 );
  }

  for ( int i = 1; i < 2 * w; i++ )
    for ( int j = ( i & 1 ) + 1; j < 2 * h; j += 2 ) {
      step( ls, i, j, j & 1, i & 1 );
    }

  return wf == walk_fill::AREA ? fill( ls ) : fill2( ls );
}

template<typename C>
void generate_paths( C& cr, C& cg, C& cb, bool balanced )
{
  std::vector<double> probs( 6 );

  if ( balanced ) {
    std::vector<double> tprobs( 6 );

    do {
      for ( int i = 0; i < 6; i++ ) {
        tprobs[i] = random_normal( 1. );
      }

      double p1 = ( tprobs[0] + tprobs[1] - tprobs[2] - tprobs[3] ) / 4;
      double p2 = ( tprobs[0] - tprobs[1] - tprobs[2]
                    + tprobs[3] - 2 * tprobs[4] + 2 * tprobs[5] ) / 12;
      probs[0] = tprobs[0] - p1 - p2;
      probs[1] = tprobs[1] - p1 + p2;
      probs[2] = tprobs[2] + p1 + p2;
      probs[3] = tprobs[3] + p1 - p2;
      probs[4] = tprobs[4] + 2 * p2;
      probs[5] = tprobs[5] - 2 * p2;
    } while ( any_of( probs.begin(), probs.end(), [] ( double x ) { return x < 0; } ) );
  } else {
    for ( int i = 0; i < 6; i++ ) {
      probs[i] = fabs( random_normal( 1. ) );
    }
  }

  for ( int i = 0; i < cr.width(); i++ )
    for ( int j = 0; j < cr.height(); j++ ) {
      switch ( random_weighted( probs ) ) {
      case 0:
        cr( i, j ) = 0;
        cg( i, j ) = 1;
        cb( i, j ) = 2;
        break;

      case 1:
        cr( i, j ) = 0;
        cg( i, j ) = 2;
        cb( i, j ) = 1;
        break;

      case 2:
        cr( i, j ) = 1;
        cg( i, j ) = 0;
        cb( i, j ) = 2;
        break;

      case 3:
        cr( i, j ) = 1;
        cg( i, j ) = 2;
        cb( i, j ) = 0;
        break;

      case 4:
        cr( i, j ) = 2;
        cg( i, j ) = 0;
        cb( i, j ) = 1;
        break;

      case 5:
        cr( i, j ) = 2;
        cg( i, j ) = 1;
        cb( i, j ) = 0;
        break;
      }
    }
}

void draw_walk( ImageData& imageData, int width, int height, bool balanced, int walkFill )
{
//  print( Q_FUNC_INFO, width, height, balanced, walkFill );
  walk_fill wf{static_cast<walk_fill > ( walkFill )};
  wrap_canvas<uint8_t> cr( width, height );
  wrap_canvas<uint8_t> cg( width, height );
  wrap_canvas<uint8_t> cb( width, height );
  generate_paths( cr, cg, cb, balanced );
  auto sr = levels( cr, wf );
  auto sg = levels( cg, wf );
  auto sb = levels( cb, wf );
  wrap_canvas<color_t> sc( width, height );

  for ( int i = 0; i < width; i++ )
    for ( int j = 0; j < height; j++ ) {
      sc( i, j ) = color_t( sr( i, j ), sg( i, j ), sb( i, j ) );
    }

  imageData.layers.reset();
  imageData.img =  std::make_shared<wrap_canvas<color_t>>( sc );
}

void draw_walk2( ImageData& imageData, int width, int height, bool balanced, int walkFill )
{
//  print( Q_FUNC_INFO, width, height, balanced, walkFill );
  walk_fill wf{static_cast<walk_fill> ( walkFill )};
  canvas<uint8_t> cr( width - 1, height - 1 );
  canvas<uint8_t> cg( width - 1, height - 1 );
  canvas<uint8_t> cb( width - 1, height - 1 );
  generate_paths( cr, cg, cb, balanced );
  auto sr = levels2( cr, wf );
  auto sg = levels2( cg, wf );
  auto sb = levels2( cb, wf );
  canvas<color_t> sc( width, height );

  for ( int i = 0; i < width; i++ )
    for ( int j = 0; j < height; j++ ) {
      sc( i, j ) = color_t( sr( i, j ), sg( i, j ), sb( i, j ) );
    }

  imageData.layers.reset();
  imageData.img =  std::make_shared<canvas<color_t>>( sc );
}
