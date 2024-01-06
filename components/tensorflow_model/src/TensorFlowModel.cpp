#include "TensorFlowModel.hpp"
#include "tensorflow/lite/micro/micro_interpreter.h"

TensorFlowModel::TensorFlowModel() {
    _model = nullptr;
    _op_resolver = nullptr;
    _interpreter = nullptr;
    _input = nullptr;
    _configured = false;
}

TensorFlowModel::~TensorFlowModel() {
    if (true == _configured) {
        printf("Warning: Forgot to deinit TensorFlowModel\n");
        deinit();
    }
}

bool TensorFlowModel::init() {
    TfLiteStatus status;

    if (!_tensor_arena) {
        printf("Error: Arena memory not allocated\n");
    }

    _tensor_arena = (uint8_t*)malloc(TENSOR_ARENA_SIZE * sizeof(uint8_t));
    if (!_tensor_arena)
    {
        printf("Error: Could not allocate arena\n");
    }

    _model = ::tflite::GetModel(converted_model_tflite);
    if (_model->version() != TFLITE_SCHEMA_VERSION) {
        printf("Model provided is schema version %ld not equal "
        "to supported version %d.\n",
        _model->version(), TFLITE_SCHEMA_VERSION);
        delete(_model);
    }
    /*The _model has 10 layers (9 hidden + 1 output)*/
    _op_resolver = new tflite::MicroMutableOpResolver<10>();

    if (kTfLiteOk != _op_resolver->AddConv2D()) {
        delete(_op_resolver);
        return false;
    }
    if (kTfLiteOk != _op_resolver->AddMaxPool2D()) {
        delete(_op_resolver);
        return false;
    }
    if (kTfLiteOk != _op_resolver->AddFullyConnected()) {
        delete(_op_resolver);
        return false;
    }
    if (kTfLiteOk != _op_resolver->AddMul()) {
        delete(_op_resolver);
        return false;
    }
    if (kTfLiteOk != _op_resolver->AddAdd()) {
        delete(_op_resolver);
        return false;
    }
    if (kTfLiteOk != _op_resolver->AddLogistic()) {
        delete(_op_resolver);
        return false;
    }
    if (kTfLiteOk != _op_resolver->AddReshape()) {
        delete(_op_resolver);
        return false;
    }
    if (kTfLiteOk != _op_resolver->AddQuantize()) {
        delete(_op_resolver);
        return false;
    }
    if (kTfLiteOk != _op_resolver->AddDequantize()) {
        delete(_op_resolver);
        return false;
    }

    // Build an interpreter to run the model with.
    _interpreter = new tflite::MicroInterpreter(_model, *_op_resolver, _tensor_arena,
                                     tensor_arena_size);

    /*Allocate memory for model's tensor using memory from _tensor_arena*/
    status = _interpreter->AllocateTensors();
    if (kTfLiteOk != status)
    {
        printf("AllocateTensors() failed\n");
        delete(_interpreter);
        return false;
    }

    /*Address to model's input*/
    _input = _interpreter->input(0);
    /*Address to model's output*/
    _output = _interpreter->output(0);

    size_t used_bytes = _interpreter->arena_used_bytes();
    printf("Used bytes %d\n", used_bytes);
    _configured = true;

    return true;
}

void TensorFlowModel::deinit() {
    if (true == _configured) {
        delete(_model);
        delete(_op_resolver);
        delete(_interpreter);
        free(_tensor_arena);
        _model = nullptr;
        _op_resolver = nullptr;
        // _tensor_arena = nullptr;
        _interpreter = nullptr;
        _input = nullptr;
        _configured = false;
    }
}

float* TensorFlowModel::getInput() {
    return _input->data.f;
}

float TensorFlowModel::predict() {
    TfLiteStatus status;

    if (true == _configured){
        status = _interpreter->Invoke();

        if (kTfLiteOk != status) {
            printf("Invoke failed\n");
            return 0.0;
        }

        return _output->data.f[0U];
    }

    printf("Warning: Model not initialized\n");
    return 0.0;

}