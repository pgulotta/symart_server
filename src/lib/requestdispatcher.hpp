#pragma once

#include "imagedata.hpp"
#include "purgeimageshandler.hpp"
#include <map>
#include <tuple>


class QByteArray;
class QImage;
class QString;
struct ImageMetaData;
struct ColorsImage;

class RequestDispatcher
{
public:
  RequestDispatcher( );

  static QString now();

  void purgeOldImages();

  bool canTileImage( const QString& id );

  bool isSymmetricView( const QString& id );

  QByteArray lastGeneratedImage( const QString& id );

  QByteArray hexagonalStretch( const QString& id );

  QByteArray randomizeTiles( const QString& id, int xtiles, int ytiles );

  QByteArray makeHyperbolic( const QString& id, int size,  int projType );

  QByteArray generateWallpaper();

  QImage getColorsImage( const QString& id ) ;

  QByteArray  updateSquiggles( const QString& id, int size, int symGroup );

  QByteArray  paintSquiggles( const QString& id, int ncolors, int size, int symGroup, double alpha, double exponent,
                              double thickness, double sharpness );

  QByteArray  paintSquiggles( const QString& id, double saturationBoost, bool useHue,
                              bool useSaturation, bool useLightness, int ncolors, int size, int symGroup, double alpha, double exponent,
                              double thickness, double sharpness );

  QByteArray paintClouds( const QString& id, int size, int symmmetryGroup, int col1Red, int col1Green, int col1Blue,
                          int col2Red, int col2Green, int col2Blue, int col3Red, int col3Green, int col3Blue, int distributionIndex );


  QByteArray paintHyperbolicClouds( const QString& id, int size, int fdfIndex, int rotation0, int rotation1,
                                    int rotation2, int rotation3, int projType, int col1Red, int col1Green,
                                    int col1Blue, int col2Red, int col2Green, int col2Blue,
                                    int col3Red, int col3Green, int col3Blue, int distributionIndex );

  QByteArray paintLines( const QString& id, int size, int symmmetryGroup, int ncolors,
                         const QString& ruleName1, int weight1, bool isPastel1,
                         const QString& ruleTName, int weight2, bool isPastel2,
                         const QString& ruleName3, int weight3, bool isPastel3 );


  QByteArray paintLines( const QString& id, const QString& colorImagePath,  double saturationBoost,
                         bool useHue, bool useSaturation, bool useLightness,
                         int size, int symmmetryGroup, int ncolors,
                         const QString& ruleName1, int weight1, bool isPastel1,
                         const QString& ruleTName, int weight2, bool isPastel2,
                         const QString& ruleName3, int weight3, bool isPastel3 );

  QByteArray paintStripes( const QString& id, int sz, int symGroup, double alpha );

  QByteArray paintClusters( const QString& id, int sz, int symGroup, double alpha );

  QByteArray paintHyperbolicLines( const QString& id, int size, int fdfIndex, int rotation0, int rotation1,
                                   int rotation2,
                                   int rotation3, int projType, int flipType, float thickness, float sharpness, int ncolors );

  QByteArray paintQuasiperiodicStripes( const QString& id, int size, int fftsize, double alpha );

  QByteArray drawWalk( const QString& id, int width, int height, bool balanced, int walkFill );

  QByteArray drawWalk2( const QString& id,  int width, int height, bool balanced, int walkFill );

  QByteArray drawTrap( const QString& id, int colRed, int colGreen, int colBlue, int sz, int symGroup );

  QByteArray drawQuasiTrap( const QString& id, int colRed, int colGreen, int colBlue, int width, int height, int symmetry,
                            double quasiperiod );

  QByteArray drawQuasiTrapPoly( const QString& id, int col1Red, int col1Green, int col1Blue, int width, int height,
                                int symmetry, double quasiperiod );

  QByteArray caGenerateImage( const QString& id, int width, int height, int ruleIndex, int left, int top, int right,
                              int bottom,
                              int nturns, double intensity );

  QByteArray caContinue( const QString& id, int nturns, double intensity );


private:
  ImageData& getNewImageData( const QString& id );
  ImageData& getOldImageData( const QString& id );
  std::tuple < int, int >purgeOldImageMetaData( const qint64& agedTimeMSecsSinceEpoch );
  std::tuple < int, int >purgeOldColorsImages( const qint64& agedTimeMSecsSinceEpoch );
  std::map<QString, ImageMetaData> mImageDataById;
  std::map<QString, ColorsImage> mColorsImagesById;
  std::unique_ptr<PurgeImagesHandler> mPurgeImagesHandler{new PurgeImagesHandler};

};

struct ImageMetaData {
  ImageData imageData;
  qint64 lastTouched;
};

struct ColorsImage {
  QImage image;
  qint64 lastTouched;
};

