#pragma once

#include <memory>
#include <vector>
#include <QImage>

#include "color.hpp"
#include "canvasview.hpp"
#include "paintlinesadapter.hpp"

struct layer;



QImage makeImage( const CanvasView<color_t>& v );

QByteArray toByteArray( const QImage& image ) ;

QImage fromByteArray( const QByteArray& ba );

struct ImageData {
  ImageData() : layers( nullptr ), original( true ) {}
  template<typename U>
  explicit ImageData( U&& c, const std::shared_ptr< std::vector<layer>> l = nullptr,
                      bool o = true ) : img( std::forward<U>( c ) ), layers( l ), original( o ) {}
  CanvasView<color_t> img;
  std::shared_ptr< std::vector<layer>> layers;
  bool original;
};


