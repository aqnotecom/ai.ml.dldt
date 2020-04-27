// Copyright (C) 2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <string>
#include <memory>
#include <queue>
#include <assert.h>

#include <ngraph/function.hpp>

std::pair<bool, std::string> compare_functions(const std::shared_ptr<ngraph::Function> & f1, const std::shared_ptr<ngraph::Function> & f2) {
    /*
     * This function compares two nGraph functions and requires them to have exactly one output
     * + Check nodes types
     * + Check number of inputs
     * + Check shapes
     * - Do not check nodes attributes (requires visitor mechanism to be completed)
     */
    auto f1_results = f1->get_results();
    auto f2_results = f2->get_results();

    assert(f1_results.size() == 1);
    assert(f2_results.size() == 1);

    auto typeInfoToStr = [](const ngraph::Node::type_info_t & typeInfo) {
        return std::string(typeInfo.name) + "/" + std::to_string(typeInfo.version);
    };

    std::queue<std::pair<std::shared_ptr<ngraph::Node>, std::shared_ptr<ngraph::Node> > > q;
    q.push({f1_results[0], f2_results[0]});
    while (!q.empty()) {
        auto node1 = q.front().first;
        auto node2 = q.front().second;
        q.pop();

        if (node1->get_type_info() != node2->get_type_info()) {
            return {false, typeInfoToStr(node1->get_type_info()) + " != " + typeInfoToStr(node2->get_type_info())};
        }

        if (node1->inputs().size() != node2->inputs().size()) {
            return {false, "Number of inputs is different: " + std::to_string(node1->inputs().size()) + " and " + std::to_string(node2->inputs().size())};
        }

        for (int i = 0; i < node1->inputs().size(); ++i) {
            if (node1->input(i).get_shape() != node2->input(i).get_shape()) {
                std::ostringstream out("Different shape detected");
                out << node1->input(i).get_shape() << " and " << node2->input(i).get_shape();
                return {false, out.str()};
            }

            q.push({node1->input_value(i).get_node_shared_ptr(), node2->input_value(i).get_node_shared_ptr()});
        }
    }
    return {true, ""};
}