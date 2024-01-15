// ███████╗███████╗██████╗ ██████╗ ██████╗          ██████╗ █████╗ ███╗   ███╗                    
// ██╔════╝██╔════╝██╔══██╗╚════██╗╚════██╗        ██╔════╝██╔══██╗████╗ ████║                    
// █████╗  ███████╗██████╔╝ █████╔╝ █████╔╝        ██║     ███████║██╔████╔██║                    
// ██╔══╝  ╚════██║██╔═══╝  ╚═══██╗██╔═══╝         ██║     ██╔══██║██║╚██╔╝██║                    
// ███████╗███████║██║     ██████╔╝███████╗        ╚██████╗██║  ██║██║ ╚═╝ ██║                    
// ╚══════╝╚══════╝╚═╝     ╚═════╝ ╚══════╝         ╚═════╝╚═╝  ╚═╝╚═╝     ╚═╝                    
//                                                                                                
// ███████╗██╗██████╗ ███╗   ███╗██╗    ██╗ █████╗ ██████╗ ███████╗                               
// ██╔════╝██║██╔══██╗████╗ ████║██║    ██║██╔══██╗██╔══██╗██╔════╝                               
// █████╗  ██║██████╔╝██╔████╔██║██║ █╗ ██║███████║██████╔╝█████╗                                 
// ██╔══╝  ██║██╔══██╗██║╚██╔╝██║██║███╗██║██╔══██║██╔══██╗██╔══╝                                 
// ██║     ██║██║  ██║██║ ╚═╝ ██║╚███╔███╔╝██║  ██║██║  ██║███████╗                               
// ╚═╝     ╚═╝╚═╝  ╚═╝╚═╝     ╚═╝ ╚══╝╚══╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝                                     
//
//
//                                                                
//  ██╗███╗   ██╗███████╗ █████╗         ██╗  ██╗   ██╗ ██████╗ ███╗   ██╗         ██████╗ ███████╗                                                    
//  ██║████╗  ██║██╔════╝██╔══██╗        ██║  ╚██╗ ██╔╝██╔═══██╗████╗  ██║        ██╔════╝ ██╔════╝                                                    
//  ██║██╔██╗ ██║███████╗███████║        ██║   ╚████╔╝ ██║   ██║██╔██╗ ██║        ██║  ███╗█████╗                                                      
//  ██║██║╚██╗██║╚════██║██╔══██║        ██║    ╚██╔╝  ██║   ██║██║╚██╗██║        ██║   ██║██╔══╝                                                      
//  ██║██║ ╚████║███████║██║  ██║        ███████╗██║   ╚██████╔╝██║ ╚████║        ╚██████╔╝███████╗                                                    
//  ╚═╝╚═╝  ╚═══╝╚══════╝╚═╝  ╚═╝        ╚══════╝╚═╝    ╚═════╝ ╚═╝  ╚═══╝         ╚═════╝ ╚══════╝                                                    
//                                                                                                                                                     
//  ██████╗ ██████╗ ████████╗        ██████╗  ██████╗ ██████╗ ██████╗                                                                                  
//  ██╔══██╗██╔══██╗╚══██╔══╝        ╚════██╗██╔═████╗╚════██╗╚════██╗                                                                                 
//  ██████╔╝██████╔╝   ██║            █████╔╝██║██╔██║ █████╔╝ █████╔╝                                                                                 
//  ██╔═══╝ ██╔══██╗   ██║           ██╔═══╝ ████╔╝██║██╔═══╝  ╚═══██╗                                                                                 
//  ██║     ██║  ██║   ██║           ███████╗╚██████╔╝███████╗██████╔╝                                                                                 
//  ╚═╝     ╚═╝  ╚═╝   ╚═╝           ╚══════╝ ╚═════╝ ╚══════╝╚═════╝                                                                                 

                                                                                                                                                                                                                   
#include "p_det_model.h"
#include "Arduino.h"
#include "SignModel.h"
#include "SPISlave.h"
#include "Camera.h"
#include "Ledo.h"
#include "Beeper.h"
#include "Flash.h"
#include "SD_MMC.h"

#define MISO        HSPI_IOMUX_PIN_NUM_MISO
#define MOSI        HSPI_IOMUX_PIN_NUM_MOSI
#define CLK         HSPI_IOMUX_PIN_NUM_CLK
#define CS          HSPI_IOMUX_PIN_NUM_CS

#define BUZZER      GPIO_NUM_16

#define RED         GPIO_NUM_4
#define GREEN       GPIO_NUM_2
#define BLUE        GPIO_NUM_0

#define FLASH       GPIO_NUM_4
//#define FLASH_IS_USING
//#define SPI_IS_USING


SignModel* model;
String lastSign = "0";
SPISlave* slave;
Camera* camera;
Ledo* led;
Beeper* beeper;
Flash* flash;


void setup() 
{
  // Disable brownout
	WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  // For being able to use some GPIOs
  SD_MMC.begin("/sdcard", true);

  Serial.begin(115200);
  camera = new Camera();

  if (!camera->init())
  {
    delay(1000);
    esp_cpu_reset(0);
  }

  model = new SignModel(ASL_1_lite_tflite);
  beeper = new Beeper(BUZZER);
  led = new Ledo(RED, GREEN, BLUE);

  #ifdef FLASH_IS_USING
  flash = new Flash(FLASH);
  #endif

  #ifdef SPI_IS_USING
  slave = new SPISlave(MISO, MOSI, CLK, CS);
  slave->addListener(model);
  slave->addListener(beeper);
  slave->addListener(led);

    #ifdef FLASH_IS_USING
    slave->addListener(flash);
    #endif

  slave->startListening();
  #endif
}


void loop() 
{ 
  camera->capture();
  camera->compressCapture();
  model->executeInference(camera->getBuffer());
  camera->returnCapture();

  Serial.print("Detected sign : ");
  Serial.println(model->getLastResult());

  delay(100);
}