#!/bin/bash

source env.sh

BIN=$SAMPLES_BIN/classification_sample
MODEL=$SAMPLES_MODEL/squeezenet_1.1
MODEL_FP16=$MODEL/ir/FP16/squeezenet1.1.xml

echo "================"
$BIN -i $SAMPLES_INPUT/girl.png -m $MODEL_FP16 -d MYRIAD \
    -nt 5 -ni 1000 -pc -p_msg

exit 0
