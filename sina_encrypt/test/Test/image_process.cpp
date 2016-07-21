#include "image_process.h"
#include <QImage>
#include <QRgb>
#include <QColor>

void ProcessImage()
{
  QImage image;
  image.load("I:\\HL_Work\\test\\test\\2.png");
  QRgb color; 
  QColor oldcolor;

  for (int x = 0 ; x < image.width(); ++x)
  {
    for (int y = 0; y < image.height(); ++y)
    {
      oldcolor = QColor(image.pixel(x,y));
      int gray_rgb = 125;//= (oldcolor.red() + oldcolor.green() + oldcolor.blue()) / 3;
      image.setPixel(x,y, qRgb(gray_rgb,gray_rgb,gray_rgb));
    }
  }

   image.save("I:\\HL_Work\\test\\test\\1a.png");
}

