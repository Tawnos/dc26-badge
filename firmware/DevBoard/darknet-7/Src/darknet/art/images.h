#ifndef DCDARKNET_IMAGES_H
#define DCDARKNET_IMAGES_H

namespace cmdc0de {
  struct DCImage
  {
    unsigned int width;
    unsigned int height;
    unsigned int bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
    const char* pixel_data;
  };
}

const cmdc0de::DCImage& getLogo1();
const cmdc0de::DCImage& getLogo2();
const cmdc0de::DCImage& getLogo3();
const cmdc0de::DCImage& getLogo4();

//const cmdc0de::DCImage &getLogoTest();

#endif
