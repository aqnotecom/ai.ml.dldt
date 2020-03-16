#!/bin/bash

source env.sh

ssd_bin=$SAMPLES_BIN/object_detection_sample_ssd
network_fp32=$SAMPLES_MODEL/ssd_inception_v2_coco_2018_01_28/ir/tensorflow/FP32/frozen_inference_graph.xml
network_fp16=$SAMPLES_MODEL/ssd_inception_v2_coco_2018_01_28/ir/tensorflow/FP16/frozen_inference_graph.xml

${ssd_bin} -i $SAMPLES_INPUT/girl.png -m ${network_fp32} -d CPU

#echo "================"
#${ssd_bin} -i $SAMPLES_INPUT/girl.png -m ${network_fp16} -d MYRIAD

exit 0
