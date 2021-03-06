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

#ifndef _CANVASVIEW_H
#define _CANVASVIEW_H

#include <memory>
#include "symmetric_canvas.hpp"

struct layer;

template<typename T>
struct CanvasView {
  CanvasView() : canvas_view( nullptr ), wrap_view( nullptr ), sym_view( nullptr ) {}

  template < typename U, typename = std::enable_if_t < !std::is_convertible<U, std::shared_ptr<void>>::value >>

  explicit CanvasView( U && c ) : CanvasView( std::make_shared<U>( std::forward<U>( c ) ) ) {}

  CanvasView( std::shared_ptr<canvas<T>> p ) : data( p ),
    canvas_view( p.get() ), wrap_view( nullptr ), sym_view( nullptr ) {}

  CanvasView( std::shared_ptr<wrap_canvas<T>> p ) : data( p ),
    canvas_view( &( p->as_canvas() ) ), wrap_view( p.get() ), sym_view( nullptr ) {}

  CanvasView( std::shared_ptr<symmetric_canvas<T>> p ) : data( p ),
    canvas_view( &( p->as_canvas() ) ), wrap_view( &( p->as_wrap_canvas() ) ),
    sym_view( p.get() ) {}

  std::shared_ptr<void> data;
  const canvas<T>* canvas_view;
  const wrap_canvas<T>* wrap_view;
  const symmetric_canvas<T>* sym_view;
};


#endif
