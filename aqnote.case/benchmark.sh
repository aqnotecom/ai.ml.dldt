#!/bin/bash

source env.sh

$SAMPLES_BIN/benchmark_app \
    -d MYRIAD \
    -i $SAMPLES_INPUT/car.png \
    -m $SAMPLES_MODEL/classification/squeezenet/1.1/ir/caffe/FP16/squeezenet1.1.xml

exit 0
