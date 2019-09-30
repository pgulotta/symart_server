#include "ca_adapter.hpp"
#include "ca.hpp"
#include "imagedata.hpp"
#include <QImage>
#include <QScopedPointer>

static QScopedPointer<ca> ca_sim;

void ca_generate_image( ImageData& imageData, int width, int height, int ruleIndex, int left, int top,
                        int right, int bottom, int nturns, double intensity )
{
  imageData.layers.reset();
  ca_sim.reset( new ca( width, height, ruleIndex ) );
  ca_sim->random_rectangle( left, top, right, bottom );
  imageData.img =  std::make_shared<canvas<color_t>>( ca_sim->generate_image( nturns, intensity ) );
}

void ca_continue( ImageData& imageData, int nturns, double intensity )
{
  imageData.img =  std::make_shared<canvas<color_t>>( ca_sim->generate_image( nturns, intensity ) );
}
