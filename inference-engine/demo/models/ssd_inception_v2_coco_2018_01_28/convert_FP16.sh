#!/bin/bash

mo_tf.py \
    --data_type=FP16 \
    --input_model ssd_inception_v2_coco_2018_01_28/tensorflow/frozen_inference_graph.pb \
    --output=detection_boxes,detection_scores,num_detections \
    --tensorflow_use_custom_operations_config /opt/intel/computer_vision_sdk/deployment_tools/model_optimizer/extensions/front/tf/ssd_v2_support.json \
    --tensorflow_object_detection_api_pipeline_config ssd_inception_v2_coco_2018_01_28/tensorflow/pipeline.config

exit 0
