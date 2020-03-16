#!/bin/bash

#rm -rf build && mkdir build
pushd build
cmake -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Release \
    -DTHREADS_PTHREAD_ARG="-pthread" \
    -DENABLE_MKL_DNN=ON \
    -DENABLE_CLDNN=OFF \
    -DTHREADING=OMP \
    -DENABLE_OPENCV=OFF \
    -DENABLE_SSE42=OFF \
    -DENABLE_GNA=OFF \
    -DENABLE_SAMPLES=ON \
    -DCMAKE_INSTALL_PREFIX=/opt/aqnote/install/dldt/2019 \
    -DENABLE_PYTHON=OFF \
    -DPYTHON_EXECUTABLE=/opt/aqnote/python/bin/python3 \
  	-DPYTHON_LIBRARY=/opt/aqnote/python/lib/libpython3.7m.so \
    -DPYTHON_INCLUDE_DIR=/opt/aqnote/python/include/python3.7m \
    ..
popd

make -j6 -C build

# -DCMAKE_INSTALL_PREFIX=/opt/aqnote/dldt \
