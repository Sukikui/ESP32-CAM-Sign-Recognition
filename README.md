# ESP32-CAM-Sign-Recognition

## Overview
The aim of this university project is to implement a CNN (convolutional neural network) for gesture recognition on ESP32-CAM. Keras is used for model's training and TensorFlowLite allows to implement the model on a microcontroler such as ESP32. 
An American Sign Language's data set is used to train the model, several optimizations were made to improve the precision in practice (merging labels, artificial augmentation of the data set).


## CNN architecture
- 3 (Convolution + ReLu + Max-pooling) layers
- 1 Flatten layer
- 1 (Fully-connected + ReLu) layer
- 1 (Fully-connected + Softmax) layer

<img width="400" alt="image" src="https://github.com/Sukikui/ESP32-CAM-Sign-Recognition/assets/97752902/15be1e94-d58c-4327-8a5c-3acef009bcb2">


## How to create binary model's file with TFLite
At the end of training in the notebook, `ASL_256_lite.tflite` is created by TensorFlowLite. To create binary model's `model_data.cc` file to implement in ESP32 source code, entry this command in folder's terminal :
```
xxd -i ASL_256_lite.tflite > model_data.cc
```


## Results
- ~100% with test data (but biased)
- ~70% with C-O-L-Y labels


## Documentations
- Board used : [AI Thinker ESP32-CAM](https://docs.platformio.org/en/latest/boards/espressif32/esp32cam.html#ai-thinker-esp32-cam)
- Data Set : [MNIST ASL](https://www.kaggle.com/datasets/datamunge/sign-language-mnist) 
- CNN modeling and training : [Keras](https://keras.io/api/)
- CNN microcontroler's post-training implementation : [TensorFlowLite](https://www.tensorflow.org/lite/api_docs?hl=fr)

## Special thanks
- [TensorFlowLite (C++ API for ESP32)](https://github.com/tanakamasayuki/Arduino_TensorFlowLite_ESP32)
