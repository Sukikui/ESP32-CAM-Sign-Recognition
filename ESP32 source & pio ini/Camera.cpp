#include "Camera.h"

Camera::Camera() { }

bool Camera::init()
{
    camera_config_t config;

    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_GRAYSCALE;  //PIXFORMAT_GRAYSCALE;  //PIXFORMAT_JPEG;   //PIXFORMAT_RGB565; 
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.frame_size = FRAMESIZE_96X96;        //FRAMESIZE_QVGA;   //FRAMESIZE_96X96;
    config.jpeg_quality = 12;
    config.fb_count = 2;

    esp_err_t err = esp_camera_init(&config);

    if (err != ESP_OK) 
        Serial.printf("Camera init failed with error 0x%x", err);
    else
        Serial.println("Camera init success");

    return err == ESP_OK;
}

void Camera::capture()
{
    this->fb = esp_camera_fb_get();

    if (this->fb)
    {
        // Mandatory to use new and delete, else buffer overflow error is caused
        this->img_input = new dl::Tensor<uint8_t>();
        this->img_input->set_element(this->fb->buf).set_shape({96, 96, 1});
    }
    else
        Serial.println("Camera capture failed");
}

void Camera::compressCapture()
{
    this->img_output = dl::image::resize_image(*this->img_input, {28, 28, 1}, dl::image::IMAGE_RESIZE_BILINEAR);
}

void Camera::returnCapture()
{
    esp_camera_fb_return(this->fb);
    // Mandatory to use new and delete, else buffer overflow error is caused
    delete this->img_input;
}

uint8_t* Camera::getBuffer()
{
    return this->img_output->get_element_ptr();
}