#include "Ledo.h"
//#include "extras/PwmPin.h"     // HomeSpan
#define DEBUG_1   // To prevent error due to GPIO 0 during debug mode

// PUBLIC

Ledo::Ledo(uint8_t rpin, uint8_t gpin, uint8_t bpin)
{
    // this->on = new Characteristic::On();
    // this->brightness = new Characteristic::Brightness(100);
    // this->saturation = new Characteristic::Saturation(0);
    // this->hue = new Characteristic::Hue(0);

    this->rpin = rpin;
    this->gpin = gpin;
    this->bpin = bpin;
    pinMode(rpin, OUTPUT);
    pinMode(gpin, OUTPUT);
    #ifndef DEBUG_1
    pinMode(bpin, OUTPUT);
    #endif
    this->set(0, 0, 0);
}

/*
bool Ledo::update()
{
    float hu, sat, bri, r, g, b;
    bool isOn = this->on->getNewVal();

    bri = this->brightness->getNewVal<float>();
    sat = this->saturation->getNewVal<float>();
    hu = this->hue->getNewVal<float>();

    LedPin::HSVtoRGB(hu, sat/100.0, bri/100.0, &r, &g, &b);

    this->red = isOn * r * 100;
    this->green = isOn * g * 100;
    this->blue = isOn * b * 100;

    this->write(false);

    return true;
}
*/


void Ledo::set(uint8_t red, uint8_t green, uint8_t blue)
{
    this->red = red;
    this->green = green;
    this->blue = blue;
    this->write(false);
}

void Ledo::turnOff()
{
    this->set(0, 0, 0);
}

void Ledo::nextRGB()
{
    if (!this->checkWheel())
        return;

    switch (this->state)
    {
        case GREEN_IS_RISING:
            this->green += 5;
            if (this->green == 255)
                this->state = BLUE_IS_FALLING;
            break;

        case BLUE_IS_FALLING:
            this->blue -= 5;
            if (this->blue == 0)
                this->state = RED_IS_RISING;
            break;

        case RED_IS_RISING:
            this->red += 5;
            if (this->red == 255)
                this->state = GREEN_IS_FALLING;
            break;

        case GREEN_IS_FALLING:
            this->green -= 5;
            if (this->green == 0)
                this->state = BLUE_IS_RISING;
            break;

        case BLUE_IS_RISING:
            this->blue += 5;
            if (this->blue == 255)
                this->state = RED_IS_FALLING;
            break;

        case RED_IS_FALLING:
            this->red -= 5;
            if (this->red == 0)
                this->state = GREEN_IS_RISING;
            break;
        
        default:
            return;
    }

    this->write(true);
}


// PRIVATE

bool Ledo::checkWheel()
{
    if (!this->isInWheel)
    {
        this->blue = 255;
        this->red = 0;
        this->green = 0;
        this->write(true);
        this->state = GREEN_IS_RISING;
        return false;
    }

    return true;
}

void Ledo::write(bool isInWheel)
{
    this->isInWheel = isInWheel;
    analogWrite(this->rpin, this->red);
    analogWrite(this->gpin, this->green);
    #ifndef DEBUG_1
    analogWrite(this->bpin, this->blue);
    #endif
}

uint8_t Ledo::onRequestReceive(RequestCode code, uint8_t data)
{
    if (code != TURN_LED)
        return NO_RESPONSE;

    return NO_RESPONSE;
}