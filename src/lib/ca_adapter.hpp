#pragma once




class ImageData;

void ca_generate_image( ImageData& imageData, int width, int height, int ruleIndex, int left, int top,
                        int right, int bottom, int nturns, double intensity );

void ca_continue( ImageData& imageData, int nturns, double intensity );
