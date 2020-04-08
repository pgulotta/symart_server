#pragma once

class QByteArray;
class ImageData;

class WallpaperGenerator
{
public:
  QByteArray getWallpaper( ImageData& imageData );


private:
  void generate_clouds( ImageData& imageData );
  void generate_clusters( ImageData& imageData );
  void generate_lines( ImageData& imageData );
  void generate_trap( ImageData& imageData );
  void generate_quasitrap( ImageData& imageData );
  void generate_quasitrap_poly( ImageData& imageData );
  void generate_squiggles( ImageData& imageData );
  void generate_stripes( ImageData& imageData );
  void generate_walk( ImageData& imageData );
};


