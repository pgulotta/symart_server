#include "wallpapergenerator.hpp"
#include "imagedata.hpp"
#include "randgen.hpp"
#include "generators.hpp"
#include <QImage>
#include <QByteArray>

QByteArray WallpaperGenerator::getWallpaper()
{
    ImageData imageData;
    paint_clusters( imageData, 512, random_symmmetryGroup(), random_real_range_inclusive( 0.5, 2.0 ) );
    return toByteArray( QImage{makeImage( imageData.img )} );
}
