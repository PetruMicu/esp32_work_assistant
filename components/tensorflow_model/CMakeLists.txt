idf_component_register(SRCS "src/TensorFlowModel.cpp" "src/model.cc"
                       INCLUDE_DIRS "include"
                       REQUIRES driver header esp-tflite-micro)
