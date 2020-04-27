#!/bin/bash

#rm -rf build && mkdir build
pushd build
cmake -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DTHREADS_PTHREAD_ARG="-pthread" \
    -DENABLE_MKL_DNN=ON \
    -DENABLE_CLDNN=ON \
    -DTHREADING=OMP \
    -DENABLE_SSE42=OFF \
    -DENABLE_GNA=OFF \
    -DCMAKE_INSTALL_PREFIX=/opt/aqnote/install/dldt/2020 \
    -DENABLE_SAMPLES=ON \
    -DENABLE_TESTS=OFF \
    -DENABLE_OPENCV=ON \
    -DENABLE_PYTHON=ON \
    -DPYTHON_EXECUTABLE=/opt/aqnote/python/bin/python3 \
  	-DPYTHON_LIBRARY=/opt/aqnote/python/lib/libpython3.7m.so \
    -DPYTHON_INCLUDE_DIR=/opt/aqnote/python/include/python3.7m \
    ..
popd

#make -j6 -C build
