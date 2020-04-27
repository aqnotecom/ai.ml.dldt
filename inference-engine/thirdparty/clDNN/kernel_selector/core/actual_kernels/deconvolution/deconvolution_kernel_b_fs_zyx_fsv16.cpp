﻿//
// Copyright (c) 2019-2020 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "deconvolution_kernel_b_fs_zyx_fsv16.h"
#include "kernel_selector_utils.h"

#include <algorithm>

namespace kernel_selector {

static const size_t sub_group_size = 16;

ParamsKey DeconvolutionKernel_b_fs_zyx_fsv16::GetSupportedKey() const {
    ParamsKey k;
    k.EnableInputDataType(Datatype::F32);
    k.EnableOutputDataType(Datatype::F32);
    k.EnableInputWeightsType(WeightsType::F32);
    k.EnableInputDataType(Datatype::F16);
    k.EnableOutputDataType(Datatype::F16);
    k.EnableInputWeightsType(WeightsType::F16);
    k.EnableInputLayout(DataLayout::b_fs_yx_fsv16);
    k.EnableOutputLayout(DataLayout::b_fs_yx_fsv16);
    k.EnableInputLayout(DataLayout::b_fs_zyx_fsv16);
    k.EnableOutputLayout(DataLayout::b_fs_zyx_fsv16);
    k.EnableInputLayout(DataLayout::bs_fs_zyx_bsv16_fsv16);
    k.EnableOutputLayout(DataLayout::bs_fs_zyx_bsv16_fsv16);
    k.EnableTensorOffset();
    k.EnableTensorPitches();
    k.EnableBiasPerFeature();
    k.EnableNonBiasTerm();
    k.EnableBatching();
    k.EnableSubGroup();
    k.EnableSubGroupShort();
    return k;
}

DeconvolutionKernelBase::DispatchData DeconvolutionKernel_b_fs_zyx_fsv16::SetDefault(const deconvolution_params& params) const {
    DispatchData kd = DeconvolutionKernelBase::SetDefault(params);

    const auto& out = params.output;

    auto x = out.X().v;
    auto y = out.Y().v;
    auto z = out.Z().v;
    auto f = Align(out.Feature().v, 16);
    auto b = out.Batch().v;

    if (out.Batch().v % 16 == 0) {
        if (params.depthwise_separable_opt) {
            kd.gws0 = x * y * z;
            kd.gws1 = f;
            kd.gws2 = b / 16;

            kd.lws0 = 1;
            kd.lws1 = sub_group_size;
            kd.lws2 = 1;
        } else {
            kd.gws0 = 64;
            while (kd.gws0 > 16) {
                if (f % kd.gws0 == 0) break;
                kd.gws0 /= 2;
            }
            kd.gws1 = x * y * z;
            kd.gws2 = CeilDiv(b, 16) * (f / kd.gws0) * params.groups;

            kd.lws0 = sub_group_size;
            kd.lws1 = 1;
            kd.lws2 = 1;
        }
    } else {
        size_t x_block_size = 16;
        while (x_block_size > 1) {
            if (x % x_block_size == 0)
               break;
            x_block_size--;
        }
        x_block_size = std::max(x_block_size, (size_t)8);
        if (params.depthwise_separable_opt) {
            kd.gws0 = CeilDiv(x, x_block_size) * y * z;
            kd.gws1 = f;
            kd.gws2 = b;

            kd.lws0 = 1;
            kd.lws1 = sub_group_size;
            kd.lws2 = 1;
        } else {
            kd.gws0 = 64;
            while (kd.gws0 > 16) {
                if (f % kd.gws0 == 0) break;
                kd.gws0 /= 2;
            }
            kd.gws1 = CeilDiv(x, x_block_size) * y * z;
            kd.gws2 = b * (f / kd.gws0);

            kd.lws0 = sub_group_size;
            kd.lws1 = 1;
            kd.lws2 = 1;
        }
    }

    kd.efficiency = FORCE_PRIORITY_2;

    return kd;
}

bool DeconvolutionKernel_b_fs_zyx_fsv16::Validate(const Params& p, const optional_params& o) const {
    if (!DeconvolutionKernelBase::Validate(p, o)) {
        return false;
    }

    return true;
}

JitConstants DeconvolutionKernel_b_fs_zyx_fsv16::GetJitConstants(const deconvolution_params& params) const {
    auto input = params.inputs[0];
    auto output = params.output;
    auto jit = Parent::GetJitConstants(params);

    if (output.Batch().v % 16 == 0) {
        jit.AddConstant(MakeJitConstant("VER_16MB16C", 1));
    } else {
        jit.AddConstant(MakeJitConstant("VER_8OW16C", 1));
    }
    jit.AddConstant(MakeJitConstant("OC_BLOCK", 16));

    if (output.GetDType() == Datatype::F32)
        jit.AddConstant(MakeJitConstant("DT_F32", 1));
    else
        jit.AddConstant(MakeJitConstant("DT_F16", 1));

    auto mb_block = 1;
    auto ic_block = 16;
    auto iw_block = 1;
    auto icb = 64;
    while (icb > 16) {
        if (Align(output.Feature().v, 16) % icb == 0) break;
        icb /= 2;
    }

    if (output.Batch().v % 16 == 0) {
        mb_block = 16;
        jit.AddConstant(MakeJitConstant("MB_BLOCK", mb_block));
        jit.AddConstant(MakeJitConstant("IC_BLOCK", ic_block));
        jit.AddConstant(MakeJitConstant("IW_BLOCK", iw_block));
    } else {
        iw_block = 16;
        while (iw_block > 1) {
            if (output.X().v % iw_block == 0)
                break;
            iw_block--;
        }
        iw_block = std::max(iw_block, 8);
        jit.AddConstant(MakeJitConstant("MB_BLOCK", mb_block));
        jit.AddConstant(MakeJitConstant("IC_BLOCK", ic_block));
        jit.AddConstant(MakeJitConstant("IW_BLOCK", iw_block));
    }
    if (params.depthwise_separable_opt) {
        jit.AddConstant(MakeJitConstant("ICB", params.split));
    } else {
        jit.AddConstant(MakeJitConstant("ICB", icb));
    }
    jit.AddConstant(MakeJitConstant("IWB", CeilDiv(output.X().v, iw_block)));
    jit.AddConstant(MakeJitConstant("MB_LAST", (output.Batch().v / 16) * 16));
    jit.AddConstant(MakeJitConstant("G", params.split));
    jit.AddConstant(MakeJitConstant("DD", params.dilation.z - 1));
    jit.AddConstant(MakeJitConstant("DH", params.dilation.y - 1));
    jit.AddConstant(MakeJitConstant("DW", params.dilation.x - 1));
    jit.AddConstant(MakeJitConstant("SUB_GROUP_SIZE", sub_group_size));
    jit.AddConstant(MakeJitConstant("IS_DW", "DEPTHWISE_SEPARABLE_OPT"));
    jit.AddConstant(MakeJitConstant("BWD_DATA", 1));
    jit.AddConstant(MakeJitConstant("WITH_BIAS", "BIAS_TERM"));

    jit.AddConstant(MakeJitConstant("MB", "OUTPUT_BATCH_NUM"));
    jit.AddConstant(MakeJitConstant("OC", Align(input.Feature().v, 16)));
    jit.AddConstant(MakeJitConstant("OD", "INPUT0_SIZE_Z"));
    jit.AddConstant(MakeJitConstant("OH", "INPUT0_SIZE_Y"));
    jit.AddConstant(MakeJitConstant("OW", "INPUT0_SIZE_X"));
    jit.AddConstant(MakeJitConstant("IC", Align(output.Feature().v, 16)));
    jit.AddConstant(MakeJitConstant("ID", "OUTPUT_SIZE_Z"));
    jit.AddConstant(MakeJitConstant("IH", "OUTPUT_SIZE_Y"));
    jit.AddConstant(MakeJitConstant("IW", "OUTPUT_SIZE_X"));
    jit.AddConstant(MakeJitConstant("KD", "FILTER_SIZE_Z"));
    jit.AddConstant(MakeJitConstant("KH", "FILTER_SIZE_Y"));
    jit.AddConstant(MakeJitConstant("KW", "FILTER_SIZE_X"));
    jit.AddConstant(MakeJitConstant("SD", "STRIDE_SIZE_Z"));
    jit.AddConstant(MakeJitConstant("SH", "STRIDE_SIZE_Y"));
    jit.AddConstant(MakeJitConstant("SW", "STRIDE_SIZE_X"));
    jit.AddConstant(MakeJitConstant("PD", "PADDING_SIZE_Z"));
    jit.AddConstant(MakeJitConstant("PH", "PADDING_SIZE_Y"));
    jit.AddConstant(MakeJitConstant("PW", "PADDING_SIZE_X"));

    jit.AddConstant(MakeJitConstant("OC_FULL", Align(params.inputs[0].Feature().LogicalDimPadded(), 16)));
    jit.AddConstant(MakeJitConstant("OD_FULL", params.inputs[0].Z().LogicalDimPadded()));
    jit.AddConstant(MakeJitConstant("OH_FULL", params.inputs[0].Y().LogicalDimPadded()));
    jit.AddConstant(MakeJitConstant("OW_FULL", params.inputs[0].X().LogicalDimPadded()));

    jit.AddConstant(MakeJitConstant("IC_FULL", Align(params.output.Feature().LogicalDimPadded(), 16)));
    jit.AddConstant(MakeJitConstant("ID_FULL", params.output.Z().LogicalDimPadded()));
    jit.AddConstant(MakeJitConstant("IH_FULL", params.output.Y().LogicalDimPadded()));
    jit.AddConstant(MakeJitConstant("IW_FULL", params.output.X().LogicalDimPadded()));


    DispatchData runInfo = SetDefault(params);
    jit.AddConstant(MakeJitConstant("LWS_0", runInfo.lws0));
    jit.AddConstant(MakeJitConstant("LWS_1", runInfo.lws1));
    jit.AddConstant(MakeJitConstant("LWS_2", runInfo.lws2));

    return jit;
}

}  // namespace kernel_selector
