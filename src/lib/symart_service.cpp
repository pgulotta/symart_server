#include "symart_service.hpp"
#include "requestdispatcher.hpp"
#include <QImage>
#include <QBuffer>



static RequestDispatcher requestDispatcher;

void purgeOldImages()
{
  requestDispatcher.purgeOldImages();
}

bool toBool( const QString& text )
{
  if ( text == "false" ) {
    return false;
  }

  if ( text == "0" ) {
    return false;
  }

  return true;
}

QString fromBool( bool value )
{
  if ( value == false ) {
    return "false";
  }

  if ( value == 0 ) {
    return "false";
  }

  return "true";
}

QByteArray lastGeneratedImage( const QString& id )
{
  return requestDispatcher.lastGeneratedImage( id );
}

QByteArray hexagonalStretch( const QString& id )
{
  return requestDispatcher.hexagonalStretch( id );
}

QByteArray randomizeTiles( const QString& id,  int xtiles, int ytiles )
{
  return requestDispatcher.randomizeTiles( id, xtiles, ytiles );
}

QByteArray makeHyperbolic( const QString& id,  int size,  int projType )
{
  return requestDispatcher.makeHyperbolic( id, size, projType );
}

void loadColorsImage( const QString& id, const QByteArray& byteArray )
{
  requestDispatcher.loadColorsImage( id, byteArray );
}

bool canTileImage( const QString& id )
{
  return requestDispatcher.canTileImage( id );
}

bool isSymmetricView( const QString& id )
{
  return requestDispatcher.isSymmetricView( id );
}

QByteArray paintSquiggles( const QString& id,  int ncolors, int size, int symGroup, double alpha, double exponent,
                           double thickness, double sharpness )
{
  return  requestDispatcher.paintSquiggles( id,  ncolors, size, symGroup, alpha, exponent, thickness,  sharpness );
}

QByteArray paintSquiggles( const QString& id, double saturationBoost, bool useHue,
                           bool useSaturation, bool useLightness, int ncolors, int size, int symGroup, double alpha, double exponent,
                           double thickness, double sharpness )
{
  return  requestDispatcher.paintSquiggles( id, saturationBoost, useHue, useSaturation, useLightness,
                                            ncolors, size, symGroup, alpha, exponent, thickness,  sharpness );
}

QByteArray updateSquiggles( const QString& id,  int size, int symGroup )
{
  return requestDispatcher.updateSquiggles( id, size, symGroup );
}

QByteArray paintClouds( const QString& id, int size, int symmmetryGroup, int col1Red, int col1Green, int col1Blue,
                        int col2Red, int col2Green, int col2Blue, int col3Red, int col3Green, int col3Blue, int distributionIndex )
{
  return requestDispatcher.paintClouds( id,  size, symmmetryGroup, col1Red,  col1Green, col1Blue,
                                        col2Red,  col2Green,  col2Blue,  col3Red,  col3Green,  col3Blue,  distributionIndex );
}

QByteArray paintHyperbolicClouds( const QString& id,  int size, int fdfIndex, int rotation0, int rotation1,
                                  int rotation2, int rotation3, int projType, int col1Red,
                                  int col1Green, int col1Blue, int col2Red, int col2Green, int col2Blue,
                                  int col3Red, int col3Green, int col3Blue, int distributionIndex )
{
  return requestDispatcher.paintHyperbolicClouds( id,  size, fdfIndex, rotation0,  rotation1, rotation2, rotation3,
                                                  projType, col1Red,
                                                  col1Green, col1Blue, col2Red, col2Green, col2Blue, col3Red, col3Green, col3Blue, distributionIndex );
}

QByteArray paintLines( const QString& id, int size, int symmmetryGroup, int ncolors,
                       const QString& ruleName1, int weight1, bool isPastel1,
                       const QString& ruleName2, int weight2, bool isPastel2,
                       const QString& ruleName3, int weight3, bool isPastel3 )
{
  return requestDispatcher.paintLines( id, size, symmmetryGroup, ncolors, ruleName1,  weight1, isPastel1, ruleName2,
                                       weight2,  isPastel2, ruleName3,  weight3,  isPastel3 );
}


QByteArray paintLines( const QString& id,  const QString& colorImagePath,  double saturationBoost,
                       bool useHue, bool useSaturation, bool useLightness, int size, int symmmetryGroup, int ncolors,
                       const QString& ruleName1, int weight1, bool isPastel1, const QString& ruleName2,
                       int weight2, bool isPastel2, const QString& ruleName3, int weight3, bool isPastel3 )
{
  return requestDispatcher.paintLines( id,  colorImagePath, saturationBoost, useHue,  useSaturation,  useLightness,
                                       size, symmmetryGroup,  ncolors, ruleName1, weight1, isPastel1,  ruleName2, weight2, isPastel2,
                                       ruleName3, weight3, isPastel3 );
}

QByteArray paintStripes( const QString& id,   int sz, int symGroup, double alpha )
{
  return requestDispatcher.paintStripes( id,  sz, symGroup, alpha );
}

QByteArray paintClusters( const QString& id,  int sz, int symGroup, double alpha )
{
  return  requestDispatcher.paintClusters( id,  sz, symGroup, alpha );
}

QByteArray drawWalk( const QString& id,  int width, int height, bool balanced, int walkFill )
{
  return  requestDispatcher.drawWalk( id, width, height, balanced, walkFill );
}

QByteArray drawWalk2( const QString& id,  int width, int height, bool balanced, int walkFill )
{
  return  requestDispatcher.drawWalk2( id, width, height, balanced, walkFill );
}

QByteArray paintHyperbolicLines( const QString& id,  int size, int fdfIndex, int rotation0, int rotation1,
                                 int rotation2, int rotation3, int projType, int flipType, float thickness,
                                 float sharpness, int ncolors )
{
  return requestDispatcher.paintHyperbolicLines( id, size,  fdfIndex,  rotation0,  rotation1,  rotation2,
                                                 rotation3,  projType,  flipType,  thickness,  sharpness,  ncolors );
}

QByteArray drawTrap( const QString& id,  int sz, int symGroup )
{
  return requestDispatcher.drawTrap( id, sz,  symGroup );
}

QByteArray drawQuasiTrap( const QString& id,  int width, int height, int symmetry, double quasiperiod )
{
  return  requestDispatcher.drawQuasiTrap( id, width,  height,  symmetry,  quasiperiod );
}

QByteArray drawQuasiTrapPoly( const QString& id,  int width, int height, int symmetry, double quasiperiod )
{
  return  requestDispatcher.drawQuasiTrapPoly( id, width,  height,  symmetry,  quasiperiod );
}

QByteArray paintQuasiperiodicStripes( const QString& id,  int size, int fftsize, double alpha )
{
  return  requestDispatcher.paintQuasiperiodicStripes( id,  size,  fftsize,  alpha );
}

QByteArray caGenerateImage( const QString& id,  int width, int height, int ruleIndex, int left, int top, int right,
                            int bottom, int nturns, double intensity )
{
  return requestDispatcher.caGenerateImage( id, width,  height,  ruleIndex,  left,  top,  right,  bottom, nturns,
                                            intensity );
}

QByteArray caContinue( const QString& id,  int nturns, double intensity )
{
  return requestDispatcher.caContinue( id, nturns,  intensity );
}

