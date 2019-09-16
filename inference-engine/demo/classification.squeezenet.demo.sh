#!/bin/bash

source env.sh

#$SAMPLES_BIN/classification_sample \
#    -d CPU \
#    -i $SAMPLES_DATA/car.png \
#    -m $SAMPLES_MODEL/squeezenet_1.1/ir/FP32/classification/squeezenet/1.1/caffe/squeezenet1.1.xml

echo "================"
$SAMPLES_BIN/classification_sample \
    -d MYRIAD \
    -i $SAMPLES_INPUT/car.png \
    -m $SAMPLES_MODEL/classification/squeezenet/1.1/ir/caffe/FP16/squeezenet1.1.xml \
    -nt 5 \
    -ni 1000 \
    -pc \
    -p_msg

exit 0
