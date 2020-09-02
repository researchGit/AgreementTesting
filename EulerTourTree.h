//
// Created by Lei on 7/5/20.
//

#ifndef AGREEMENTTESTING_EULERTOURTREE_H
#define AGREEMENTTESTING_EULERTOURTREE_H

#include "ConstructGraph.h"
#include "HelperDataStructure.h"
#include <unordered_map>
#include <unordered_set>

class EulerTourTree {
public:
    EulerTourTree() = default;

    ~EulerTourTree() = default;

    vector<string> constructEulerTourTree(shared_ptr<InputGraph> assembleGraph);

    static void printEulerTourTree(const vector<string> &EulerTourTree);

    unordered_map<graphEdge, int, graphEdgeHash> getTreeEdges() { return treeEdges_; }

    unordered_map<graphEdge, int, graphEdgeHash> getAllEdges() { return allEdges_; }

    unordered_map<string, unordered_map<int, unordered_set<graphEdge, graphEdgeHash>>>
    getNonTreeEdges() { return nonTreeEdges_; }

    void printEdges();

private:
    unordered_map<graphEdge, int, graphEdgeHash> treeEdges_;
    unordered_map<graphEdge, int, graphEdgeHash> allEdges_;
    unordered_map<string, unordered_map<int, unordered_set<graphEdge, graphEdgeHash>>> nonTreeEdges_;
};


#endif //AGREEMENTTESTING_EULERTOURTREE_H
