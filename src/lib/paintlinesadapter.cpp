#include "paintlinesadapter.hpp"
#include "imagedata.hpp"
#include "paintlines.hpp"
#include "layer.hpp"
#include "color.hpp"
#include <QDebug>
#include <QString>
#include <QImage>
#include <memory>
#include <vector>

using paintfunc = std::function<void( symmetric_canvas<uint8_t> & )>;


static  std::map<QString, paintfunc> paintlinesFuncs{
  { "Arc", generate_smootharc },
  { "Beads", generate_smoothline2_beads },
  { "Cluster", generate_cluster},
  { "Curl", generate_curl  },
  { "Flower", generate_flower },
  { "Fractal", generate_cluster2 },
  { "Granules", generate_granules },
  { "Line", generate_smoothline2 },
  { "Loop", generate_smoothline5  },
  { "Orbit", generate_orbit },
  { "Star", generate_star},
  { "String", generate_open_string  },
  { "Swirl", generate_swirl},
  { "Tree", generate_tree }
};

static std::vector<QString> ruleNames;


const std::vector<QString>& paintlinesRuleNames()
{
  if ( ruleNames.empty() ) {
    for ( const auto& item : paintlinesFuncs ) {
      ruleNames.push_back( item.first );
    }
  }

  qInfo() << "Q_FUNC_INFO" << " size =  " << ruleNames.size();
  return ruleNames ;
}


const paintfunc& paintlinesFunction( const QString& name )
{
  return paintlinesFuncs[name];
}

void paint_lines( ImageData& imageData,  int size, int symmmetryGroup, int ncolors,
                  const QString& ruleName1, int weight1, bool isPastel1,
                  const QString& ruleName2, int weight2, bool isPastel2,
                  const QString& ruleName3, int weight3, bool isPastel3 )
{
  return  paint_lines( imageData, "",  0.0, 0, 0, 0, size,  symmmetryGroup,  ncolors,
                       ruleName1,  weight1,  isPastel1, ruleName2,  weight2,  isPastel2, ruleName3,  weight3,  isPastel3 );
}

void paint_lines( ImageData& imageData,  const QString& colorImagePath,  double saturationBoost,
                  bool useHue, bool useSaturation, bool useLightness,
                  int size, int symmmetryGroup, int ncolors,
                  const QString& ruleName1, int weight1, bool isPastel1,
                  const QString& ruleName2, int weight2, bool isPastel2,
                  const QString& ruleName3, int weight3, bool isPastel3 )
{
  QImage colorImage;

  if ( !colorImagePath.isEmpty() ) {
    colorImage.load( colorImagePath );
  }

  symgroup sg = static_cast<symgroup>( symmmetryGroup );
  imageData.layers = std::make_shared<std::vector<layer>>( ncolors );
  std::vector< QString> rulenames{ruleName1, ruleName2, ruleName3};
  std::vector<int> weights{weight1, weight2, weight3};
  std::vector<bool> isPastel{isPastel1, isPastel2, isPastel3};

  for ( size_t i = 0; i < imageData.layers->size(); i++ ) {
    size_t ruleIndex = static_cast<size_t>( random_weighted( weights ) ) ;
    auto grid_ptr = std::make_shared<symmetric_canvas<uint8_t>>( size, sg );
    paintlinesFunction( rulenames[ruleIndex] )( *grid_ptr );
    ( *imageData.layers )[i].pixels = std::shared_ptr<const canvas<uint8_t>>( grid_ptr, &( grid_ptr->as_canvas() ) );
    ( *imageData.layers )[i].color = generateColor( colorImage, useHue, useSaturation, useLightness, saturationBoost );
    ( *imageData.layers )[i].pastel = isPastel[ruleIndex];
  }

  auto img = std::make_shared<symmetric_canvas<color_t>>( size, sg );
  merge( img->unsafe_get_canvas(), *( imageData.layers ) );
  imageData.img =  img ;
}


