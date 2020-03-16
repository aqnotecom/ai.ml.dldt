#!/bin/bash

source env.sh

BIN=$SAMPLES_BIN/object_detection_sample_ssd
MODEL=$SAMPLES_MODEL/ssd_inception_v2_coco_2018_01_28
MODEL_FP32=$MODEL/ir/FP32/frozen_inference_graph.xml
MODEL_FP16=$MODEL/ir/FP16/frozen_inference_graph.xml

# ${BIN} -i $SAMPLES_INPUT/girl.png -m ${MODEL_FP32} -d CPU

echo "================"
${BIN} -i $SAMPLES_INPUT/girl.png -m ${MODEL_FP16} -d MYRIAD

exit 0
