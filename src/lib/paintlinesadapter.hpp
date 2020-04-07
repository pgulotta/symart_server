#pragma once

#include "symmetric_canvas.hpp"

#include <vector>

class QString;
class color_t;
class ImageData;



const std::vector<QString>& paintlinesRuleNames();

void paint_lines( ImageData& imageData, int size, int symmmetryGroup, int ncolors,
                  const QString& ruleName1, int weight1, bool isPastel1,
                  const QString& ruleTName, int weight2, bool isPastel2,
                  const QString& ruleName3, int weight3, bool isPastel3 );


void paint_lines( ImageData& imageData,  const QString& colorImagePath,  double saturationBoost,
                  bool useHue, bool useSaturation, bool useLightness,
                  int size, int symmmetryGroup, int ncolors,
                  const QString& ruleName1, int weight1, bool isPastel1,
                  const QString& ruleTName, int weight2, bool isPastel2,
                  const QString& ruleName3, int weight3, bool isPastel3 );


