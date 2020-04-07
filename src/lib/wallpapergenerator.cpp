#include "wallpapergenerator.hpp"
#include "paintclouds.hpp"
#include "randgen.hpp"
#include "generators.hpp"
#include "paintlinesadapter.hpp"
#include <QImage>
#include <QByteArray>
#include <QDebug>
#include <exception>


static const int IMAGE_SIZE = 512;

QByteArray WallpaperGenerator::getWallpaper()
{
  int imageType = random_range_inclusive( 0, 3 );
  ImageData imageData;

  try {
    switch ( imageType ) {
    case 0:
      generate_clouds( imageData );
      break;

    case 1:
      generate_clusters( imageData ) ;
      break;

    case 2:
      generate_lines( imageData ) ;
      break;

    case 3:
      generate_trap( imageData ) ;
      break;
    }
  } catch ( const std::exception& e ) {
    qWarning() << Q_FUNC_INFO << e.what();
  }

  return toByteArray( QImage{makeImage( imageData.img )} );
}


void WallpaperGenerator::generate_trap( ImageData& imageData )
{
  qInfo() << Q_FUNC_INFO ;
  color_t color{randomColor()};
  drawtrap( imageData, color.red, color.green, color.blue, IMAGE_SIZE, random_symmmetryGroup() );

}

void WallpaperGenerator::generate_clouds( ImageData& imageData )
{
  qInfo() << Q_FUNC_INFO ;
  color_t color1{randomColor()};
  color_t color2{randomColor()};
  color_t color3{randomColor()};
  paint_clouds( imageData, IMAGE_SIZE, random_symmmetryGroup(),
                color1.red_int(), color1.green_int(), color1.blue_int(),
                color2.red_int(), color2.green_int(), color2.blue_int(),
                color3.red_int(), color3.green_int(), color3.blue_int(),
                random_distributionIndex() );
}

void WallpaperGenerator::generate_clusters( ImageData& imageData )
{
  qInfo() << Q_FUNC_INFO ;
  paint_clusters( imageData, IMAGE_SIZE, random_symmmetryGroup(), random_real_range_inclusive( 0.75, 1.75 ) );
}

void WallpaperGenerator::generate_lines( ImageData& imageData )
{
  qInfo() << Q_FUNC_INFO ;
  auto ruleNames{paintlinesRuleNames()};
  const auto ruleNamesMax{ ruleNames.size() - 1};
  QString rule1 { ruleNames.at( random_range_inclusive( 0, ruleNamesMax ) ) };
  int weight1 {random_range_inclusive( 1, 5 ) };
  bool isPastel1{random_bool()};
  QString rule2 { ruleNames.at( random_range_inclusive( 0, ruleNamesMax ) ) };
  int weight2 {random_range_inclusive( 1, 5 ) };
  bool isPastel2{random_bool()};
  QString rule3 { ruleNames.at( random_range_inclusive( 0, ruleNamesMax ) ) };
  int weight3 {random_range_inclusive( 1, 5 ) };
  bool isPastel3{random_bool()};
  paint_lines( imageData, IMAGE_SIZE, random_symmmetryGroup(), 3,
               rule1, weight1, isPastel1, rule2, weight2, isPastel2, rule3, weight3, isPastel3 );



}

