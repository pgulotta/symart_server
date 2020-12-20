#include "requestdispatcher.hpp"
#include "generators.hpp"
#include "converters.hpp"
#include "randomize.hpp"
#include "paintlinesadapter.hpp"
#include "paintclouds.hpp"
#include "ca_adapter.hpp"
#include "randomize.hpp"
#include "symmetric_canvas.hpp"
#include "wallpapergenerator.hpp"
#include <chrono>
#include <QImage>
#include <QBuffer>
#include <QDebug>
#include <QDateTime>
#include <QThreadPool>
#include <shared_mutex>
#include <thread>
#include "layer.hpp"

std::shared_mutex shared_mut;
using shrd_lck = std::shared_lock<std::shared_mutex>;
using uniq_lck = std::unique_lock<std::shared_mutex>;
ImageMetaData emptyImageMetaData;
QImage emptyImage;


RequestDispatcher::RequestDispatcher( )
{
}


ImageData& RequestDispatcher::getOldImageData( const QString& id )
{
  uniq_lck l {shared_mut, std::defer_lock};

  if ( l.try_lock() ) {
    auto it = mImageDataById.find( id );

    if ( it != mImageDataById.end() ) {
      return mImageDataById[id].imageData;
    }
  }

  return emptyImageMetaData.imageData;

}

ImageData& RequestDispatcher::getNewImageData( const QString& id )
{
  uniq_lck l {shared_mut, std::defer_lock};

  if ( l.try_lock() ) {
    auto it = mImageDataById.find( id );

    if ( it != mImageDataById.end() ) {
      it->second.imageData.clear();
      mImageDataById.erase( it );
    }

    mImageDataById.try_emplace( id, ImageMetaData{ImageData{}, QDateTime::currentMSecsSinceEpoch() } );
    return mImageDataById[id].imageData;
  } else {
    return emptyImageMetaData.imageData;
  }
}

QString RequestDispatcher::now()
{
  return QDateTime::currentDateTime().toString( Qt::DateFormat::ISODateWithMs ) ;
}

QByteArray RequestDispatcher::generateWallpaper()
{
  WallpaperGenerator generator;
  return generator.getWallpaper( getNewImageData( "wallpaper" ) );
}

QByteArray RequestDispatcher::lastGeneratedImage( const QString& id )
{
  auto& imageData { getOldImageData( id )};

  if ( imageData.img.data == nullptr )
    return {QByteArray()};

  return toByteArray( makeImage( imageData.img ) );
}

QByteArray RequestDispatcher::hexagonalStretch( const QString& id )
{
  auto& imageData { getOldImageData( id )};

  if ( imageData.img.data == nullptr )
    return {QByteArray()};

  auto img = hexagonal_stretch( *( imageData.img.sym_view ) );

  auto newImageData{ImageData( std::move( img ), nullptr, false )};

  newImageData.clear();

  return toByteArray( makeImage( newImageData.img ) );
}

QByteArray RequestDispatcher::makeHyperbolic( const QString& id, int size,  int projType )
{
  auto& imageData { getOldImageData( id )};

  if ( imageData.img.data == nullptr )
    return {QByteArray()};

  ImageData newImageData{imageData};

  make_hyperbolic( newImageData, projType, size );

  newImageData.clear();

  return toByteArray( makeImage( newImageData.img ) );
}

QByteArray RequestDispatcher::randomizeTiles( const QString& id, int xtiles, int ytiles )
{
  auto& imageData { getOldImageData( id )};

  if ( imageData.img.data == nullptr )
    return {QByteArray()};

  wrap_canvas<color_t> img = randomize( xtiles, ytiles, *( imageData.img.sym_view ) );

  const auto canvasView = std::make_shared<wrap_canvas<color_t>>( img );

  return toByteArray( makeImage( canvasView )  );
}

bool RequestDispatcher::canTileImage( const QString& id )
{
  auto& imageData { getOldImageData( id )};

  if ( imageData.img.data == nullptr  ) {
    return false;
  }

  return  imageData.img.wrap_view  != nullptr;
}


bool RequestDispatcher::isSymmetricView( const QString& id )
{
  auto& imageData { getOldImageData( id )};

  if ( imageData.img.data == nullptr  ) {
    return false;
  }

  return imageData.img.sym_view  != nullptr;
}

QByteArray RequestDispatcher::paintSquiggles( const QString& id, int ncolors, int size, int symGroup, double alpha,
                                              double exponent, double thickness,
                                              double sharpness )
{
  auto& data { getNewImageData( id )};
  paint_squiggles( data,  ncolors,  size,  symGroup,  alpha,  exponent,  thickness,  sharpness );
  return toByteArray( makeImage( data.img ) );
}

QByteArray RequestDispatcher::paintSquiggles( const QString& id, double, bool, bool, bool, int ncolors, int size,
                                              int symGroup, double alpha,  double exponent, double thickness, double sharpness )
{
  return paintSquiggles( id, ncolors,  size, symGroup, alpha, exponent, thickness,  sharpness );
}

QByteArray RequestDispatcher::updateSquiggles( const QString& id, int size, int symGroup )
{
  auto& data{getOldImageData( id )};
  update_squiggles( data, size,  symGroup );
  return toByteArray( makeImage( data.img ) );
}

QByteArray RequestDispatcher::paintClouds( const QString& id, int size, int symmmetryGroup, int col1Red, int col1Green,
                                           int col1Blue, int col2Red, int col2Green, int col2Blue, int col3Red, int col3Green, int col3Blue,
                                           int distributionIndex )
{
  auto& data{getNewImageData( id )};
  paint_clouds( data, size, symmmetryGroup, col1Red, col1Green, col1Blue,  col2Red,
                col2Green, col2Blue, col3Red,  col3Green, col3Blue, distributionIndex );
  return toByteArray( makeImage( data.img ) );
}

QByteArray RequestDispatcher::paintHyperbolicClouds( const QString& id, int size, int fdfIndex, int rotation0,
                                                     int rotation1, int rotation2,
                                                     int rotation3, int projType, int col1Red, int col1Green, int col1Blue, int col2Red, int col2Green, int col2Blue,
                                                     int col3Red, int col3Green, int col3Blue, int distributionIndex )
{
  auto& data{getNewImageData( id )};
  paint_hyperbolic_clouds( data,  size,  fdfIndex,  rotation0,  rotation1,  rotation2,
                           rotation3,  projType,  col1Red,  col1Green,  col1Blue,  col2Red,  col2Green, col2Blue,
                           col3Red,  col3Green,  col3Blue,  distributionIndex );
  return toByteArray( makeImage( data.img ) );
}

QByteArray RequestDispatcher::paintLines( const QString& id, int size, int symmmetryGroup, int ncolors,
                                          const QString& ruleName1, int weight1, bool isPastel1,
                                          const QString& ruleTName, int weight2, bool isPastel2,
                                          const QString& ruleName3, int weight3, bool isPastel3 )
{
  auto& data{getNewImageData( id )};
  paint_lines( data,  size, symmmetryGroup, ncolors, ruleName1, weight1, isPastel1,
               ruleTName, weight2, isPastel2, ruleName3, weight3, isPastel3 );
  return toByteArray( makeImage( data.img ) );
}

QByteArray RequestDispatcher::paintLines( const QString& id, const QString& colorImagePath,  double saturationBoost,
                                          bool useHue, bool useSaturation, bool useLightness,
                                          int size, int symmmetryGroup, int ncolors,
                                          const QString& ruleName1, int weight1, bool isPastel1,
                                          const QString& ruleTName, int weight2, bool isPastel2,
                                          const QString& ruleName3, int weight3, bool isPastel3 )
{
  auto& data{getNewImageData( id )};
  paint_lines( data, colorImagePath,   saturationBoost, useHue,  useSaturation,  useLightness,
               size,  symmmetryGroup,  ncolors, ruleName1, weight1, isPastel1,
               ruleTName, weight2, isPastel2, ruleName3, weight3, isPastel3 );
  return toByteArray( makeImage( data.img ) );
}

QByteArray RequestDispatcher::paintStripes( const QString& id,  int sz, int symGroup, double alpha )
{
  auto& data{getNewImageData( id )};
  paint_stripes( data,  sz, symGroup, alpha );
  return toByteArray( makeImage( data.img ) );
}


QByteArray RequestDispatcher::paintClusters( const QString& id, int sz, int symGroup, double alpha )
{
  auto& data{getNewImageData( id )};
  paint_clusters( data,  sz, symGroup, alpha );
  return toByteArray( makeImage( data.img ) );
}

QByteArray RequestDispatcher::drawWalk( const QString& id, int width, int height, bool balanced, int walkFill )
{
  auto& data{getNewImageData( id )};
  draw_walk( data, width, height, balanced, walkFill );
  return toByteArray( makeImage( data.img ) );
}

QByteArray RequestDispatcher::drawWalk2( const QString& id, int width, int height, bool balanced, int walkFill )
{
  auto& data{getNewImageData( id )};
  draw_walk2( data, width, height, balanced, walkFill );
  return toByteArray( makeImage( data.img ) );
}

QByteArray RequestDispatcher::paintHyperbolicLines( const QString& id, int size, int fdfIndex, int rotation0,
                                                    int rotation1,
                                                    int rotation2,
                                                    int rotation3, int projType, int flipType, float thickness, float sharpness, int ncolors )
{
  auto& data{getNewImageData( id )};
  paint_hyperbolic_lines( data, size,  fdfIndex,  rotation0,  rotation1,  rotation2,
                          rotation3,  projType,  flipType,  thickness,  sharpness,  ncolors );
  return toByteArray( makeImage( data.img ) );
}

QByteArray RequestDispatcher::drawTrap( const QString& id, int colRed, int colGreen, int colBlue, int sz, int symGroup )
{
  auto& data{getNewImageData( id )};
  drawtrap( data, colRed, colGreen, colBlue, sz,  symGroup );
  return toByteArray( makeImage( data.img ) );
}

QByteArray RequestDispatcher::drawQuasiTrap( const QString& id, int colRed, int colGreen, int colBlue, int width,
                                             int height, int symmetry, double quasiperiod )
{
  auto& data{getNewImageData( id )};
  drawquasitrap( data, colRed, colGreen, colBlue, width,  height,  symmetry,  quasiperiod );
  return toByteArray( makeImage( data.img ) );
}

QByteArray RequestDispatcher::drawQuasiTrapPoly( const QString& id, int col1Red, int col1Green, int col1Blue,
                                                 int width, int height, int symmetry, double quasiperiod )
{
  auto& data{getNewImageData( id )};
  drawquasitrap_poly( data, col1Red,  col1Green,  col1Blue, width,  height,  symmetry,  quasiperiod );
  return toByteArray( makeImage( data.img ) );
}

QByteArray RequestDispatcher::paintQuasiperiodicStripes( const QString& id, int size, int fftsize, double alpha )
{
  auto& data{getNewImageData( id )};
  paint_quasiperiodic_stripes( data,  size,  fftsize,  alpha );
  return toByteArray( makeImage( data.img ) );
}

QByteArray RequestDispatcher::caGenerateImage( const QString& id, int width, int height, int ruleIndex, int left,
                                               int top, int right,
                                               int bottom,
                                               int nturns, double intensity )
{
  auto& data{getNewImageData( id )};
  ca_generate_image( data, width,  height,  ruleIndex,  left,  top,  right,  bottom, nturns,  intensity );
  return toByteArray( makeImage( data.img ) );
}

QByteArray RequestDispatcher::caContinue( const QString& id, int nturns, double intensity )
{
  auto& data{getNewImageData( id )};
  ca_continue( data, nturns,  intensity );
  return toByteArray( makeImage( data.img ) );
}



