#ifndef LEDO_H
#define LEDO_H

#include <stdlib.h>
#include <Arduino.h>
#include "SPISlave.h"


enum RGBState
{
    GREEN_IS_RISING = 0,
    BLUE_IS_FALLING = 1,
    RED_IS_RISING = 2,
    GREEN_IS_FALLING = 3,
    BLUE_IS_RISING = 4,
    RED_IS_FALLING = 5
};

struct Color
{
    private:
        Color(uint8_t code, uint8_t red, uint8_t green, uint8_t blue)
        {
            this->code = code;
            this->red = red;
            this->green = green;
            this->blue = blue;
        }

    public:
        uint8_t code;
        uint8_t red;
        uint8_t green;
        uint8_t blue;

        bool operator==(uint8_t code) const 
        {
            return this->code == code;
        }

   //     const static Color RAINBOW(0x00, 0, 0,0);
   // const static Color RED(0x01, 255, 0, 0);
   // const static Color GREEN(0x02, 0, 255, 0);
   // const static Color BLUE(0x03, 0, 0, 255);
   // const static Color LIGHT_RED(0x04, 255, 0, 70);
   // const static Color CYAN(0x05, 0, 255, 255);
   // const static Color PURPLE(0x06, 60, 0, 255);
   // const static Color WHITE(0x07, 255,255, 255);
};



//Color colorList[7] = {RED, GREEN, BLUE, LIGHT_RED, CYAN, PURPLE, WHITE};



class Ledo : public SPISlaveListener
{
    private:
        uint8_t rpin, gpin, bpin;
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
        bool isInWheel;
        RGBState state = GREEN_IS_RISING;
        void write(bool isInWheel);
        bool checkWheel();

    public:

        //SpanCharacteristic* on;
        //SpanCharacteristic* brightness;
        //SpanCharacteristic* saturation;
        //SpanCharacteristic* hue;

        Ledo(uint8_t rpin, uint8_t gpin, uint8_t bpin);
        //bool update() override;   // HomeSpan

        void set(uint8_t red, uint8_t green, uint8_t blue);
        void setRed();
        void setGreen();
        void setBlue();
        void setWhite();
        void setCyan();
        void setLightRed();
        void turnOff();
        void nextRGB();

        uint8_t onRequestReceive(RequestCode code, uint8_t data) override; 
};

#endif // LEDO_H