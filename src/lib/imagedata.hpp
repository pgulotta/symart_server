#pragma once

#include <memory>
#include <vector>
#include <QImage>
#include <string>
#include <iostream>
#include "color.hpp"
#include "canvasview.hpp"
#include "paintlinesadapter.hpp"
#include <QDateTime>
#include <string>

struct layer;

template<typename T>
class AppendSpace
{
private:
  T const& ref;                  // refer to argument passed in constructor
public:
  AppendSpace( T const& r ): ref( r )
  {
  }
  friend std::ostream& operator<< ( std::ostream& os, AppendSpace<T> s )
  {
    return os << s.ref << ' '; // output passed argument and a space
  }
};

template < typename... Args >
void print ( Args... args )
{
  ( std::cout << ... << AppendSpace( args ) ) << '\n';
}

QImage makeImage( const CanvasView<color_t>& v );

QByteArray toByteArray( const QImage& image ) ;

QImage fromByteArray( const QByteArray& ba );

struct ImageData {
  ImageData() : layers( nullptr ), original( true ) {}
  template<typename U>
  ImageData( U&& c,  std::shared_ptr< std::vector<layer>> l,
             bool o = true ) : img( std::forward<U>( c ) ), layers( l ), original( o ) {}
  CanvasView<color_t> img;
  std::shared_ptr< std::vector<layer>> layers;
  bool original;
};


