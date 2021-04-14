#!/usr/bin/env bash

# Copyright (C) 2018-2019 Intel Corporation
# SPDX-License-Identifier: Apache-2.0

ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

. "$ROOT_DIR/utils.sh"

usage() {
    echo "Classification demo using public SqueezeNet topology"
    echo "-d name     specify the target device to infer on; CPU, GPU, FPGA, HDDL or MYRIAD are acceptable. Sample will look for a suitable plugin for device specified"
    echo "-help            print help message"
    exit 1
}

trap 'error ${LINENO}' ERR

target="MYRIAD"

# parse command line options
while [[ $# -gt 0 ]]
do
key="$1"

case $key in
    -h | -help | --help)
    usage
    ;;
    -d)
    target="$2"
    echo target = "${target}"
    shift
    ;;
    -sample-options)
    sampleoptions="$2 $3 $4 $5 $6"
    echo sample-options = "${sampleoptions}"
    shift
    ;;
    *)
    # unknown option
    ;;
esac
shift
done

target_precision="FP16"

printf "target_precision = ${target_precision}\n"

project_home=/home/aqnote/org.openvino/openvino/@yudao/demo/classify
models_path="$project_home/models"
models_cache="$project_home/cache"
irs_path="$project_home/ir"

model_name="squeezenet1.1"

target_image_path="$ROOT_DIR/car.png"

run_again="Then run the script again\n\n"
dashes="\n\n###################################################\n\n"


# if [ -e "$ROOT_DIR/../../bin/setupvars.sh" ]; then
#     setupvars_path="$ROOT_DIR/../../bin/setupvars.sh"
# else
#     printf "Error: setupvars.sh is not found\n"
# fi

# if ! . $setupvars_path ; then
#     printf "Unable to run ./setupvars.sh. Please check its presence. ${run_again}"
#     exit 1
# fi

# Step 1: 下载模型
printf "${dashes}"
printf "\n\nDownloading the Caffe model and the prototxt"

cur_path=$PWD
python_binary=python3
pip_binary=pip3

downloader_dir="/home/aqnote/org.openvino/open_model_zoo/tools/downloader"
downloader_path="$downloader_dir/downloader.py"

model_dir=$("$python_binary" "$downloader_dir/info_dumper.py" --name "$model_name" |
    "$python_binary" -c 'import sys, json; print(json.load(sys.stdin)[0]["subdirectory"])')

print_and_run "$python_binary" "$downloader_path" --name "$model_name" --output_dir "${models_path}" --cache_dir "${models_cache}"

## Step 2: 转化为ie对应的模型
ir_dir="${irs_path}/${model_dir}/${target_precision}"

if [ ! -e "$ir_dir" ]; then
    printf "${dashes}"
    printf "Convert a model with Model Optimizer\n\n"

    mo_path="/home/aqnote/org.openvino/openvino/model-optimizer/mo.py"

    export PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION=cpp
    print_and_run "$python_binary" "$downloader_dir/converter.py" --mo "$mo_path" --name "$model_name" -d "$models_path" -o "$irs_path" --precisions "$target_precision"
else
    printf "\n\nTarget folder ${ir_dir} already exists. Skipping IR generation  with Model Optimizer."
    printf "If you want to convert a model again, remove the entire ${ir_dir} folder. ${run_again}"
fi

# Step 3: 编译代码
printf "${dashes}"
printf "Build Inference Engine classify\n\n"

OS_PATH=$(uname -m)
NUM_THREADS="-j2"

if [ $OS_PATH == "x86_64" ]; then
  OS_PATH="intel64"
  NUM_THREADS="-j8"
fi

classify_path=$project_home/source
build_dir=$project_home/build
binaries_dir="${build_dir}/${OS_PATH}/Release"

if [ -e $build_dir/CMakeCache.txt ]; then
	rm -rf $build_dir/CMakeCache.txt
fi
mkdir -p $build_dir
cd $build_dir
cmake -DCMAKE_BUILD_TYPE=Release $classify_path

make $NUM_THREADS classify_squeezenet

# Step 4: 运行
printf "${dashes}"
printf "Run Inference Engine classification sample\n\n"

cd $binaries_dir

cp -f $ROOT_DIR/${model_name}.labels ${ir_dir}/

print_and_run $build_dir/intel64/Release/classify_squeezenet -d "$target" -i "$target_image_path" -m "${ir_dir}/${model_name}.xml" ${sampleoptions}

printf "${dashes}"

printf "Demo completed successfully.\n\n"
