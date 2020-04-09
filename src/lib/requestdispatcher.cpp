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
#include <QImage>
#include <QBuffer>
#include <QDebug>
#include <QDateTime>
#include <QThreadPool>
#include <shared_mutex>
#include <thread>


std::shared_mutex shared_mut;
using shrd_lck = std::shared_lock<std::shared_mutex>;
using uniq_lck = std::unique_lock<std::shared_mutex>;

ImageData emptyImageData;
QImage emptyImage;

//const qint64  PurgeImagesTimerIntervalMs {900000};  // 15 Minutes =900,000 Milliseconds
const qint64  PurgeImagesTimerIntervalMs   {60000};  // 1 Minute

RequestDispatcher::RequestDispatcher( ) :
  mPurgeImagesHandler{new PurgeImagesHandler}
{
  mPurgeImagesHandler->setAutoDelete( true );
  QThreadPool::globalInstance()->start( mPurgeImagesHandler.get() );
}

ImageData& RequestDispatcher::getImageData( const QString& id )
{
  uniq_lck l {shared_mut, std::defer_lock};

  if ( l.try_lock() ) {
    ImageMetaData& data = mImageDataById[id];
    data.lastTouched = QDateTime::currentMSecsSinceEpoch();
    return data.imageData;
  } else {
    return emptyImageData;
  }
}

void RequestDispatcher::setImageData( const QString& id, ImageData& imageData )
{
  uniq_lck l {shared_mut, std::defer_lock};

  if ( l.try_lock() ) {
    mImageDataById[id].imageData = imageData;
    mImageDataById[id].lastTouched = QDateTime::currentMSecsSinceEpoch();
  }
}

int RequestDispatcher::purgeOldImageMetaData( const qint64& agedTimeMSecsSinceEpoch )
{
  int  removedCounter{0};
  uniq_lck l {shared_mut, std::defer_lock};

  if ( l.try_lock() ) {
    for ( const auto&[key, value] : mImageDataById ) {
      const qint64& lastTouched = value.lastTouched;

      if ( agedTimeMSecsSinceEpoch > lastTouched ) {
        mImageDataById.erase( key );
        removedCounter++;
      }
    }
  }

  return removedCounter;
}

int RequestDispatcher::purgeOldColorsImages( const qint64& agedTimeMSecsSinceEpoch )
{
  int  removedCounter{0};
  uniq_lck l {shared_mut, std::defer_lock};

  if ( l.try_lock() ) {

    for ( const auto&[key, value] : mColorsImagesById ) {
      const qint64 lastTouched = value.lastTouched;

      if ( agedTimeMSecsSinceEpoch > lastTouched ) {
        mColorsImagesById.erase( key );
        removedCounter++;
      }
    }
  }

  return removedCounter;
}

QString RequestDispatcher::now()
{
  return QDateTime::currentDateTime().toString( Qt::DateFormat::ISODateWithMs ) ;
}

void RequestDispatcher::purgeOldImages()
{
  auto agedTimeMSecsSinceEpoch = QDateTime::currentMSecsSinceEpoch();
  QThread::msleep( PurgeImagesTimerIntervalMs );
  int purgedOldImageMetaDatas = purgeOldImageMetaData( agedTimeMSecsSinceEpoch );
  int purgedOldColorsImages = purgeOldColorsImages( agedTimeMSecsSinceEpoch );

  qInfo() << Q_FUNC_INFO << now() <<
          "  Purged old ImageMetaData count = " << purgedOldImageMetaDatas <<
          "  Purged old Colors Images count = " << purgedOldColorsImages;
}

QByteArray RequestDispatcher::generateWallpaper()
{
  WallpaperGenerator generator;
  return generator.getWallpaper( getImageData( "wallpaper" ) );
}

QByteArray RequestDispatcher::lastGeneratedImage( const QString& id )
{
  auto imageData { getImageData( id )};

  if ( imageData.img.data == nullptr )
    return {QByteArray()};

  return toByteArray( QImage{makeImage( imageData.img )} );
}

QByteArray RequestDispatcher::hexagonalStretch( const QString& id )
{
  auto imageData { getImageData( id )};

  if ( imageData.img.data == nullptr )
    return {QByteArray()};

  auto img = hexagonal_stretch( *( imageData.img.sym_view ) );

  auto newImageData{ImageData( std::move( img ), nullptr, false )};

  return toByteArray( QImage{makeImage( newImageData.img )} );
}

QByteArray RequestDispatcher::randomizeTiles( const QString& id, int xtiles, int ytiles )
{
  auto imageData { getImageData( id )};

  if ( imageData.img.data == nullptr )
    return {QByteArray()};

  wrap_canvas<color_t> img = randomize( xtiles, ytiles, *( imageData.img.sym_view ) );

  const auto canvasView = std::make_shared<wrap_canvas<color_t>>( img );

  return toByteArray( makeImage( canvasView ) );
}

QByteArray RequestDispatcher::makeHyperbolic( const QString& id, int size,  int projType )
{
  auto newImageData =   getImageData( id );

  if ( newImageData.img.data == nullptr )
    return {QByteArray()};

  make_hyperbolic( newImageData, projType, size );

  return toByteArray( QImage{makeImage( newImageData.img )} );
}

void RequestDispatcher::loadColorsImage( const QString& id, const QByteArray& byteArray )
{
  qDebug() << Q_FUNC_INFO << "byteArray.size()  =  " << byteArray.size();
  uniq_lck l {shared_mut, std::defer_lock};

  if ( l.try_lock() ) {
    mColorsImagesById[id].image = fromByteArray( byteArray )  ;
    mColorsImagesById[id].lastTouched = QDateTime::currentMSecsSinceEpoch();
  }

  qDebug() << Q_FUNC_INFO << " mColorsImage.isNull()=" << mColorsImagesById[id].image.isNull();
}

QImage RequestDispatcher::getColorsImage( const QString& id )
{
  uniq_lck l {shared_mut, std::defer_lock};

  if ( l.try_lock() ) {
    mColorsImagesById[id].lastTouched = QDateTime::currentMSecsSinceEpoch();
    return  mColorsImagesById[id].image;
  } else {
    return emptyImage;
  }
}

bool RequestDispatcher::canTileImage( const QString& id )
{
  auto imageData { getImageData( id )};

  if ( imageData.img.data == nullptr  ) {
    return false;
  }

  return  imageData.img.wrap_view  != nullptr;
}


bool RequestDispatcher::isSymmetricView( const QString& id )
{
  auto imageData { getImageData( id )};

  if ( imageData.img.data == nullptr  ) {
    return false;
  }

  return imageData.img.sym_view  != nullptr;
}

QByteArray RequestDispatcher::paintSquiggles( const QString& id, int ncolors, int size, int symGroup, double alpha,
                                              double exponent, double thickness,
                                              double sharpness )
{
  paint_squiggles( getImageData( id ),  ncolors,  size,  symGroup,  alpha,  exponent,  thickness,  sharpness );
  return toByteArray( QImage{makeImage( getImageData( id ).img )} );
}

QByteArray RequestDispatcher::paintSquiggles( const QString& id,
                                              double saturationBoost, bool useHue, bool useSaturation,
                                              bool useLightness, int ncolors, int size, int symGroup, double alpha, double exponent,
                                              double thickness, double sharpness )
{
  paint_squiggles( getImageData( id ), saturationBoost, useHue, useSaturation, useLightness, ncolors,
                   size, symGroup,  alpha,  exponent,  thickness,  sharpness, getColorsImage( id ) );
  return toByteArray( QImage{makeImage( getImageData( id ).img )} );
}

QByteArray RequestDispatcher::updateSquiggles( const QString& id, int size, int symGroup )
{
  ImageData& data{getImageData( id )};
  update_squiggles( data, size,  symGroup );
  return toByteArray( QImage{makeImage( data.img )} );
}

QByteArray RequestDispatcher::paintClouds( const QString& id, int size, int symmmetryGroup, int col1Red, int col1Green,
                                           int col1Blue, int col2Red, int col2Green, int col2Blue, int col3Red, int col3Green, int col3Blue,
                                           int distributionIndex )
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

QByteArray RequestDispatcher::drawTrap( const QString& id, int colRed, int colGreen, int colBlue, int sz, int symGroup )
{
  drawtrap( getImageData( id ), colRed, colGreen, colBlue, sz,  symGroup );
  return toByteArray( QImage{makeImage( getImageData( id ).img )} );
}

QByteArray RequestDispatcher::drawQuasiTrap( const QString& id, int colRed, int colGreen, int colBlue, int width,
                                             int height, int symmetry, double quasiperiod )
{
  drawquasitrap( getImageData( id ), colRed, colGreen, colBlue, width,  height,  symmetry,  quasiperiod );
  return toByteArray( QImage{makeImage( getImageData( id ).img )} );
}

QByteArray RequestDispatcher::drawQuasiTrapPoly( const QString& id, int col1Red, int col1Green, int col1Blue,
                                                 int width, int height, int symmetry, double quasiperiod )
{
  drawquasitrap_poly( getImageData( id ), col1Red,  col1Green,  col1Blue, width,  height,  symmetry,  quasiperiod );
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



