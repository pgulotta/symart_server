/***************************************************************************
 *   Copyright (C) 2005, 2013-2014, 2016 by Daniel Gulotta                 *
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

#ifdef FFTWPROGS
#include <vector>
#include <memory>
#include <algorithm>
#include "randgen.hpp"
#include "stripes_common.hpp"
#include "generators.hpp"
#include "layer.hpp"
#include "imagedata.hpp"


using std::fill;
using std::function;
using std::vector;


void generate( stripes_grid& grid, const std::function<complex<double>()>& gen,
               double exponent )
{
  grid.clear();
  int i, j, size = grid.get_size();
  stripes_grid_norm norm( size, grid.get_group() );

  for ( j = 0; j < size; j++ )
    for ( i = 0; i < size; i++ ) {
      grid( i, j ) = gen();
    }

  grid.transform();

  for ( j = 0; j < size; j++ )
    for ( i = 0; i < size; i++ ) {
      grid( i, j ) *= pow( norm( i, j ), exponent / 2. );
    }

  grid.transform();
}

void fill( const stripes_grid& grid, canvas<uint8_t>& pix, const function<double( const complex<double> & )>& proj,
           double thickness, double sharpness )
{
  double norm = grid.intensity( proj );
  norm = sqrt( norm ) / ( grid.get_size() * 64 );
  double height;
  int i, j;

  for ( j = 0; j < grid.get_size(); j++ )
    for ( i = 0; i < grid.get_size(); i++ ) {
      height = proj( grid.get_symmetric( i, j ) ) / norm;
      pix( i, j ) = 255.99 / ( pow( std::abs( height ) / ( 10.*thickness ), sharpness ) + 1. );
    }
}

void fill( symmetric_canvas<uint8_t>* grid1, symmetric_canvas<uint8_t>* grid2,
           double alpha, double exponent, double thickness, double sharpness, stripes_grid& sgr )
{
  auto gen1 = std::bind( random_levy_1d, alpha, 1. );
  function<complex<double>()> gen;

  if ( grid2 )
    gen = [&gen1] () { return complex<double>( gen1(), gen1() ); };
  else
    gen = [&gen1] () { return complex<double>( gen1(), 0 ); };

  generate( sgr, gen, exponent );

  fill( sgr, grid1->unsafe_get_canvas(), ( stripes_grid::proj_t )std::real, thickness, sharpness );

  if ( grid2 ) {
    fill( sgr, grid2->unsafe_get_canvas(), ( stripes_grid::proj_t )std::imag, thickness, sharpness );
  }
}


void  update_squiggles( ImageData& imageData, int size, int symGroup )
{
  symgroup sg{static_cast<symgroup>( symGroup )};
  symmetric_canvas<color_t> canvas( size, sg );

  for ( size_t i = 0; i < imageData.layers->size(); i++ ) {
    ( *imageData.layers )[i].color = randomColor();
  }

  merge( canvas.unsafe_get_canvas(), *imageData.layers );
}

void paint_squiggles( ImageData& imageData, int ncolors, int size, int symGroup, double alpha,
                      double exponent, double thickness, double sharpness )
{
  paint_squiggles( imageData,  0.0, 0, 0, 0,  ncolors, size, symGroup, alpha, exponent, thickness, sharpness, QImage{} );

}
void  paint_squiggles( ImageData& imageData, double saturationBoost, bool useHue,
                       bool useSaturation, bool useLightness, int ncolors, int size, int symGroup, double alpha,
                       double exponent, double thickness, double sharpness, const QImage& colorImage )
{
  vector<symmetric_canvas<uint8_t>> grids( ncolors  );
  symgroup sg{static_cast<symgroup>( symGroup )};
  vector<stripes_grid> stripes_grids;

  for ( size_t i = 0; i < ncolors; i += 2 ) {
    stripes_grids.emplace_back( size, sg );
  }

  # pragma omp parallel for

  for ( size_t i = 0; i < stripes_grids.size(); i++ ) {
    symmetric_canvas<uint8_t>* grid1 = &( grids[2 * i] ), *grid2;
    ( *grid1 ) = symmetric_canvas<uint8_t>( size, sg );

    if ( 2 * i + 1 < ncolors ) {
      grid2 = &( grids[2 * i + 1] );
      ( *grid2 ) = symmetric_canvas<uint8_t>( size, sg );
    } else { grid2 = nullptr; }

    fill( grid1, grid2, alpha, exponent, thickness, sharpness, stripes_grids[i] );
  }

  imageData.layers = std::make_shared<vector<layer>>( grids.size() );

  for ( size_t i = 0; i < grids.size(); i++ ) {
    auto p_grid = std::make_shared<symmetric_canvas<uint8_t>>( std::move( grids[i] ) );
    ( *imageData.layers )[i].pixels = std::shared_ptr<const canvas<uint8_t>>( p_grid, &( p_grid->as_canvas() ) );
    ( *imageData.layers )[i].pastel = false;
  }

  symmetric_canvas<color_t> canvas( size, sg );

  for ( size_t i = 0; i < imageData.layers->size(); i++ ) {
    ( *imageData.layers )[i].color = generateColor( colorImage, useHue, useSaturation, useLightness, saturationBoost );
  }

  merge( canvas.unsafe_get_canvas(), *imageData.layers );
  imageData.img =  std::make_shared<symmetric_canvas<color_t>>( canvas );

}



#endif
