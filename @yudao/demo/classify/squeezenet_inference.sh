#!/bin/bash

./build/intel64/Release/classify_squeezenet \
    -d MYRIAD \
    -i /home/aqnote/org.openvino/openvino/@yudao/demo/classify/car.png \
    -m /home/aqnote/org.openvino/openvino/@yudao/demo/classify/ir/public/squeezenet1.1/FP16/squeezenet1.1.xml

exit 0
