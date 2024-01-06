#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "model.h"
#include "Macros.hpp"

class TensorFlowModel {
private:
    uint8_t* _tensor_arena;
    const int tensor_arena_size = TENSOR_ARENA_SIZE;
    const tflite::Model* _model;
    tflite::MicroMutableOpResolver<10>* _op_resolver;
    tflite::MicroInterpreter* _interpreter;
    TfLiteTensor* _input;
    TfLiteTensor* _output;
    bool _configured;
public:
    TensorFlowModel();
    ~TensorFlowModel();
    bool init();
    void deinit();
    float* getInput();
    float predict();
};
