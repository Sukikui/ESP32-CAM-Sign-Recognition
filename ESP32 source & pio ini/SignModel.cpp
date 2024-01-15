#include "SignModel.h"
//#define DEBUG   // Uncomment to print input image

SignModel::SignModel(const void* model_buf)
{
    // Create reporter
    this->reporter = new MicroErrorReporter();
    
    // Import ASL model
    this->model = GetModel(model_buf);

    if (model->version() != TFLITE_SCHEMA_VERSION)
    {
        TF_LITE_REPORT_ERROR(this->reporter, "Model provided is schema version %d not equal "
                             "to supported version %d.\n", this->model->version(), TFLITE_SCHEMA_VERSION);
    }

    // Create tensor arena and set its size (determinate by testing)
    const int tensor_arena_size = 90 * 1024;    // 81 * 1024;
    uint8_t* tensor_arena = (uint8_t *) heap_caps_malloc(tensor_arena_size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

    if (tensor_arena == NULL)
    {
        Serial.println("Couldn't allocate memory for tensor_arena");
        return;
    }

    // Configue resolver
    this->resolver.AddAveragePool2D();
    this->resolver.AddMaxPool2D();
    this->resolver.AddReshape();
    this->resolver.AddFullyConnected();
    this->resolver.AddConv2D();
    this->resolver.AddDepthwiseConv2D();
    this->resolver.AddSoftmax();
    this->resolver.AddQuantize();
    this->resolver.AddDequantize();

    // Create interpreter
    this->interpreter = new MicroInterpreter(this->model, this->resolver, tensor_arena, tensor_arena_size, this->reporter);
    TfLiteStatus status = this->interpreter->AllocateTensors();

    if (status != kTfLiteOk)
    {
        Serial.println("Allocate tensors failed");
        return;
    }

    // Getting of input and output tensors
    this->input = this->interpreter->input(0);
    this->output = this->interpreter->output(0);
}


void SignModel::expect()
{
    // for potential debugging
}


void SignModel::executeInference(uint8_t* normalizedImage)
{
    #ifdef DEBUG
    for (int i = 0; i < INPUT_SIZE; i++)
    {
        this->input->data.uint8[i] = normalizedImage[i];

        Serial.print(this->getContrast(normalizedImage[i]));
        Serial.print(this->getContrast(normalizedImage[i]));
        if ((i+1)%28 == 0)
            Serial.println();
    }
    #endif

    TfLiteStatus status = this->interpreter->Invoke();

    if (status != kTfLiteOk)
    {
        Serial.println("Invoke interpreter failed");
        return;
    }

    this->findArgmax();
}


String SignModel::getLastResult()
{
    return this->signLabels[this->result];
}

float SignModel::getLastPrecision()
{
    return this->precision;
}


// PRIVATE

void SignModel::findArgmax()
{
    uint8_t argmax = 0;
    float precision = 0;

    for (uint8_t i = 0; i < OUTPUT_SIZE; i++)
    {
        float current_precision = this->output->data.f[i];

        #ifdef DEBUG
            Serial.print("out ");
            Serial.print(signLabels[i]);
            Serial.print(" : ");
            Serial.println(current_precision);
        #endif

        if (current_precision > precision)
        {
            argmax = i;
            precision = current_precision;
        }
    }

    this->result = argmax;
    this->precision = precision;
}


// OVERRIDE

uint8_t SignModel::onRequestReceive(RequestCode code, uint8_t data)
{
    if (code == GET_LAST_SIGN)
        return this->result;
    else
        return NO_RESPONSE;
}