#include "requestdispatcher.hpp"
#include "generators.hpp"
#include "converters.hpp"
#include "randomize.hpp"
#include "paintlinesadapter.hpp"
#include "paintclouds.hpp"
#include "ca_adapter.hpp"
#include "randomize.hpp"
#include "symmetric_canvas.hpp"
#include <QImage>
#include <QBuffer>



QByteArray RequestDispatcher::toByteArray( const QImage& image )
{
  QByteArray ba  ;

  if ( image.sizeInBytes() > 0 ) {
    QBuffer buffer( &ba );
    buffer.open( QIODevice::WriteOnly );
    image.save( &buffer, "PNG" );
  }

  return ba ;
}

QImage RequestDispatcher::fromByteArray( const QByteArray& ba  )
{
  QImage image;
  image.loadFromData( ba, "PNG" );
  return image;
}

QByteArray RequestDispatcher::lastGeneratedImage( const QString& id )
{
  return toByteArray( QImage{makeImage( getImageData( id ).img )} );
}

QByteArray RequestDispatcher::hexagonalStretch( const QString& id )
{
  auto img = hexagonal_stretch( *( getImageData( id ).img.sym_view ) );
  auto newImageData{ImageData( std::move( img ), nullptr, false )};
  return toByteArray( QImage{makeImage( newImageData.img )} );
}

QByteArray RequestDispatcher::randomizeTiles( const QString& id, int xtiles, int ytiles )
{
  auto img = randomize( xtiles, ytiles, *( getImageData( id ).img.sym_view ) );
  auto newImageData  = ImageData( std::move( img ), nullptr, false );
  return toByteArray( QImage{makeImage( newImageData.img )} );
}

QByteArray RequestDispatcher::makeHyperbolic( const QString& id, int size,  int projType )
{
  auto newImageData =   getImageData( id );
  make_hyperbolic( newImageData, projType, size );
  return toByteArray( QImage{makeImage( newImageData.img )} );
}

QByteArray RequestDispatcher::loadImage( const QString& id, const QByteArray& byteArray, int symGroup )
{
  load_image( fromByteArray( byteArray ),  symGroup, getImageData( id ) );
  return toByteArray( QImage{makeImage( getImageData( id ).img )} );
}

bool RequestDispatcher::canTileImage( const QString& id ) {return  getImageData( id ).img.wrap_view  != nullptr; }


bool RequestDispatcher::isSymmetricView( const QString& id ) { return getImageData( id ).img.sym_view  != nullptr;}

QByteArray RequestDispatcher::paintSquiggles( const QString& id, int ncolors, int size, int symGroup, double alpha,
                                              double exponent, double thickness,
                                              double sharpness )
{
  paint_squiggles( getImageData( id ),  ncolors,  size,  symGroup,  alpha,  exponent,  thickness,  sharpness );
  return toByteArray( QImage{makeImage( getImageData( id ).img )} );
}

QByteArray RequestDispatcher::paintSquiggles( const QString& id, const QString& colorImagePath,
                                              double saturationBoost, bool useHue, bool useSaturation,
                                              bool useLightness, int ncolors, int size, int symGroup, double alpha, double exponent,
                                              double thickness, double sharpness )
{
  paint_squiggles( getImageData( id ), colorImagePath, saturationBoost, useHue, useSaturation, useLightness, ncolors,
                   size,
                   symGroup,  alpha,  exponent,  thickness,  sharpness );
  return toByteArray( QImage{makeImage( getImageData( id ).img )} );
}

QByteArray RequestDispatcher::updateSquiggles( const QString& id, int size, int symGroup )
{
  update_squiggles( getImageData( id ), size,  symGroup );
  return toByteArray( QImage{makeImage( getImageData( id ).img )} );
}

QByteArray RequestDispatcher::paintClouds( const QString& id, int size, int symmmetryGroup, int col1Red, int col1Green,
                                           int col1Blue, int col2Red,
                                           int col2Green, int col2Blue, int col3Red, int col3Green, int col3Blue, int distributionIndex )
{
  paint_clouds( getImageData( id ), size, symmmetryGroup, col1Red, col1Green, col1Blue,  col2Red,
                col2Green, col2Blue, col3Red,  col3Green, col3Blue, distributionIndex );
  return toByteArray( QImage{makeImage( getImageData( id ).img )} );
}

QByteArray RequestDispatcher::paintHyperbolicClouds( const QString& id, int size, int fdfIndex, int rotation0,
                                                     int rotation1, int rotation2,
                                                     int rotation3, int projType, int col1Red, int col1Green, int col1Blue, int col2Red, int col2Green, int col2Blue,
                                                     int col3Red, int col3Green, int col3Blue, int distributionIndex )
{
  paint_hyperbolic_clouds( getImageData( id ),  size,  fdfIndex,  rotation0,  rotation1,  rotation2,
                           rotation3,  projType,  col1Red,  col1Green,  col1Blue,  col2Red,  col2Green, col2Blue,
                           col3Red,  col3Green,  col3Blue,  distributionIndex );
  return toByteArray( QImage{makeImage( getImageData( id ).img )} );
}

QByteArray RequestDispatcher::paintLines( const QString& id, int size, int symmmetryGroup, int ncolors,
                                          const QString& ruleName1, int weight1, bool isPastel1,
                                          const QString& ruleTName, int weight2, bool isPastel2,
                                          const QString& ruleName3, int weight3, bool isPastel3 )
{
  paint_lines( getImageData( id ),  size, symmmetryGroup, ncolors, ruleName1, weight1, isPastel1,
               ruleTName, weight2, isPastel2, ruleName3, weight3, isPastel3 );
  return toByteArray( QImage{makeImage( getImageData( id ).img )} );
}


QByteArray RequestDispatcher::paintLines( const QString& id, const QString& colorImagePath,  double saturationBoost,
                                          bool useHue, bool useSaturation, bool useLightness,
                                          int size, int symmmetryGroup, int ncolors,
                                          const QString& ruleName1, int weight1, bool isPastel1,
                                          const QString& ruleTName, int weight2, bool isPastel2,
                                          const QString& ruleName3, int weight3, bool isPastel3 )
{
  paint_lines( getImageData( id ), colorImagePath,   saturationBoost, useHue,  useSaturation,  useLightness,
               size,  symmmetryGroup,  ncolors, ruleName1, weight1, isPastel1,
               ruleTName, weight2, isPastel2, ruleName3, weight3, isPastel3 );
  return toByteArray( QImage{makeImage( getImageData( id ).img )} );
}

QByteArray RequestDispatcher::paintStripes( const QString& id,  int sz, int symGroup, double alpha )
{
  paint_stripes( getImageData( id ),  sz, symGroup, alpha );
  return toByteArray( QImage{makeImage( getImageData( id ).img )} );
}


QByteArray RequestDispatcher::paintClusters( const QString& id, int sz, int symGroup, double alpha )
{
  paint_clusters( getImageData( id ),  sz, symGroup, alpha );
  return toByteArray( QImage{makeImage( getImageData( id ).img )} );
}

QByteArray RequestDispatcher::drawWalk( const QString& id, int width, int height, bool balanced, int walkFill )
{
  draw_walk( getImageData( id ), width, height, balanced, walkFill );
  return toByteArray( QImage{makeImage( getImageData( id ).img )} );
}

QByteArray RequestDispatcher::drawWalk2( const QString& id, int width, int height, bool balanced, int walkFill )
{
  draw_walk2( getImageData( id ), width, height, balanced, walkFill );
  return toByteArray( QImage{makeImage( getImageData( id ).img )} );
}

QByteArray RequestDispatcher::paintHyperbolicLines( const QString& id, int size, int fdfIndex, int rotation0,
                                                    int rotation1,
                                                    int rotation2,
                                                    int rotation3, int projType, int flipType, float thickness, float sharpness, int ncolors )
{
  paint_hyperbolic_lines( getImageData( id ), size,  fdfIndex,  rotation0,  rotation1,  rotation2,
                          rotation3,  projType,  flipType,  thickness,  sharpness,  ncolors );
  return toByteArray( QImage{makeImage( getImageData( id ).img )} );
}

QByteArray RequestDispatcher::drawTrap( const QString& id, int sz, int symGroup )
{
  drawtrap( getImageData( id ), sz,  symGroup );
  return toByteArray( QImage{makeImage( getImageData( id ).img )} );
}

QByteArray RequestDispatcher::drawQuasiTrap( const QString& id, int width, int height, int symmetry,
                                             double quasiperiod )
{
  drawquasitrap( getImageData( id ), width,  height,  symmetry,  quasiperiod );
  return toByteArray( QImage{makeImage( getImageData( id ).img )} );
}

QByteArray RequestDispatcher::drawQuasiTrapPoly( const QString& id, int width, int height, int symmetry,
                                                 double quasiperiod )
{
  drawquasitrap_poly( getImageData( id ), width,  height,  symmetry,  quasiperiod );
  return toByteArray( QImage{makeImage( getImageData( id ).img )} );
}

QByteArray RequestDispatcher::paintQuasiperiodicStripes( const QString& id, int size, int fftsize, double alpha )
{
  paint_quasiperiodic_stripes( getImageData( id ),  size,  fftsize,  alpha );
  return toByteArray( QImage{makeImage( getImageData( id ).img )} );
}

QByteArray RequestDispatcher::caGenerateImage( const QString& id, int width, int height, int ruleIndex, int left,
                                               int top, int right,
                                               int bottom,
                                               int nturns, double intensity )
{
  ca_generate_image( getImageData( id ), width,  height,  ruleIndex,  left,  top,  right,  bottom, nturns,  intensity );
  return toByteArray( QImage{makeImage( getImageData( id ).img )} );
}

QByteArray RequestDispatcher::caContinue( const QString& id, int nturns, double intensity )
{
  ca_continue( getImageData( id ), nturns,  intensity );
  return toByteArray( QImage{makeImage( getImageData( id ).img )} );
}

ImageData& RequestDispatcher::getImageData( const QString& id )
{
  ImageMetaData& data = mImageDataById[id];
  data.lastTouched = std::chrono::system_clock::now();
  return data.imageData;
}

void RequestDispatcher::setImageData( const QString& id, ImageData& imageData )
{
  ImageMetaData& data = mImageDataById[id];
  data.imageData = imageData;
  data.lastTouched = std::chrono::system_clock::now();
}

