#!/bin/bash

#rm -rf build && mkdir build
pushd build
cmake -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Release \
    -DTHREADS_PTHREAD_ARG="-pthread" \
    -DENABLE_MKL_DNN=ON \
    -DENABLE_CLDNN=ON \
    -DTHREADING=OMP \
    -DENABLE_OPENCV=OFF \
    -DENABLE_SSE42=OFF \
    -DENABLE_GNA=OFF \
    -DENABLE_SAMPLES=ON \
    -DCMAKE_INSTALL_PREFIX=/opt/aqnote/dldt \
    -DENABLE_PYTHON=ON \
    -DPYTHON_EXECUTABLE=/opt/python/bin/python3 \
  	-DPYTHON_LIBRARY=/opt/python/lib/libpython3.7m.so \
    -DPYTHON_INCLUDE_DIR=/opt/python/include/python3.7m \
    /home/aqnote/sourceware/org.opencv/dldt/inference-engine
popd

make -j6 -C build

# -DCMAKE_INSTALL_PREFIX=/opt/aqnote/dldt \
