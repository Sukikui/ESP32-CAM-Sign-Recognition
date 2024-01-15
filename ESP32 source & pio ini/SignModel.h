#ifndef SIGNMODEL_H
#define SIGNMODEL_H

#include <Arduino.h>
#include "SPISlave.h"
#include <TensorFlowLite_ESP32.h>
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"

using namespace tflite;

#define INPUT_SIZE      (28*28)
#define OUTPUT_SIZE     4


/*
 * American Sign Language Recognition Model
 * ========================================
 *
 * input : grayscale 28*28 image                (1 pixel -> {0:255})
 * output : 5 (merge of 24) softmax sign        (1 sign -> [0, 1])
 *
 * https://www.kaggle.com/code/madz2000/cnn-using-keras-100-accuracy
 */
class SignModel : public SPISlaveListener
{
    private:
        const tflite::Model* model;
        MicroErrorReporter* reporter;
        MicroMutableOpResolver<9> resolver;
        MicroInterpreter* interpreter;
        TfLiteTensor* input;
        TfLiteTensor* output;

        uint8_t result = 1;     // H by default
        float precision = 0;

        /*
        const char* signLabels[OUTPUT_SIZE] =
            {"A", "B", "C", "D", "E", "F", "G", "H", "I", "K",
            "L", "M","N", "O", "P", "Q", "R", "S", "T", "U",
            "V", "W", "X", "Y"};
        */

        const char* signLabels[OUTPUT_SIZE] = {"C", "H", "L", "Y"};
        String contrast[7] = {"-", "+", "=", "*", "M", "H", "#"};

        String getContrast(uint8_t pixel)
        {
            if (pixel >= 255-36)
                return contrast[6];
            else if (pixel >= 255-2*36)
                return contrast[5];
            else if (pixel >= 255-3*36)
                return contrast[4];
            else if (pixel >= 255-4*36)
                return contrast[3];
            else if (pixel >= 255-5*36)
                return contrast[2];
            else if (pixel >= 255-6*36)
                return contrast[1];
            else
                return contrast[0];
        }

        void findArgmax();

    public:
        SignModel(const void* model_buf);
        void expect();
        void executeInference(uint8_t* input);
        String getLastResult();
        float getLastPrecision();

        uint8_t onRequestReceive(RequestCode code, uint8_t data) override; 
};

#endif  // SIGNMODEL_H