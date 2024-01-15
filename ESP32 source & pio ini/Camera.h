#ifndef CAMERA_H
#define CAMERA_H

#define CAMERA_MODEL_AI_THINKER

#include <Arduino.h>
#include "dl_image.hpp"
#include "esp_camera.h"
#include "camera_pins.h"
#include "soc/soc.h"            // Disable brownout problems
#include "soc/rtc_cntl_reg.h"   // Disable brownout problems

class Camera
{
    private:
        camera_fb_t* fb = nullptr;
        dl::Tensor<uint8_t>* img_input;      // 96x96 grayscale
        dl::Tensor<uint8_t>* img_output;    // 28x28 grayscale

    public:
        Camera();
        bool init();
        void capture();
        void compressCapture();
        void returnCapture();
        uint8_t* getBuffer();
};

#endif // CAMERA_H