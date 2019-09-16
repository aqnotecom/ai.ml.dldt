#!/bin/bash

source evn.sh

$SAMPLES_BIN/security_barrier_camera_demo \
    -d CPU \
    -d_va CPU \
    -d_lpr CPU \
    -i $SAMPLES_INPUT/car_1.bmp \
    -m $SAMPLES_MODEL/intel_models/vehicle-license-plate-detection-barrier-0106/FP16/vehicle-license-plate-detection-barrier-0106.xml \
    -m_va $SAMPLES_MODEL/intel_models/vehicle-attributes-recognition-barrier-0039/FP16/vehicle-attributes-recognition-barrier-0039.xml \
    -m_lpr $SAMPLES_MODEL/intel_models/license-plate-recognition-barrier-0001/FP16/license-plate-recognition-barrier-0001.xml

echo "================"
$SAMPLES_BIN/security_barrier_camera_demo \
    -d MYRIAD \
    -d_va MYRIAD \
    -d_lpr MYRIAD \
    -i $SAMPLES_INPUT/car_1.bmp \
    -m $SAMPLES_MODEL/intel_models/vehicle-license-plate-detection-barrier-0106/FP16/vehicle-license-plate-detection-barrier-0106.xml \
    -m_va $SAMPLES_MODEL/intel_models/vehicle-attributes-recognition-barrier-0039/FP16/vehicle-attributes-recognition-barrier-0039.xml \
    -m_lpr $SAMPLES_MODEL/intel_models/license-plate-recognition-barrier-0001/FP16/license-plate-recognition-barrier-0001.xml


exit 0
