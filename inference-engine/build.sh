#!/bin/bash

rm build && mkdir build
pushd build
cmake -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DTHREADS_PTHREAD_ARG="-pthread" \
    -DENABLE_MKL_DNN=ON \
    -DENABLE_CLDNN=ON \
    -DTHREADING=OMP \
    -DENABLE_PYTHON=ON \
    -DENABLE_OPENCV=OFF \
    -DENABLE_SSE42=OFF \
    -DENABLE_GNA=OFF \
    -DENABLE_SAMPLES=ON \
    ..
popd

# -DCMAKE_INSTALL_PREFIX=/opt/aqnote/dldt \