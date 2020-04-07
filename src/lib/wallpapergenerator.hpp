#pragma once

#include "imagedata.hpp"
#include <QByteArray>



class WallpaperGenerator
{
public:
  QByteArray getWallpaper();


private:
  void generate_clouds( ImageData& imageData );
  void generate_clusters( ImageData& imageData );
  void generate_lines( ImageData& imageData );
  void generate_trap( ImageData& imageData );


};


