//
// Created by Lei on 7/5/20.
//

#ifndef AGREEMENTTESTING_CONSTRUCTGRAPH_H
#define AGREEMENTTESTING_CONSTRUCTGRAPH_H

#include "InputTreeNode.h"

#include <unordered_map>
#include <unordered_set>

using namespace std;
using InputGraph = unordered_map<string, unordered_set<string>>;

class ConstructGraph {
public:
    ConstructGraph() = default;

    explicit ConstructGraph(shared_ptr<InputGraph> &assembleGraph, vector<string> &initialPosition,
                            unordered_map<string, unordered_set<int>> &occurMap,
                            unordered_map<string, unordered_map<int, unordered_set<string>>> &childMap);

    ~ConstructGraph() = default;

    void createAssembleGraph(vector<shared_ptr<InputTreeNode>> &inputTrees);

    shared_ptr<InputGraph> getAssembleGraph();

    shared_ptr<ConstructGraph> getConstructGraph();

    unordered_map<string, unordered_set<int>> getOccurMap();

    vector<string> getInitialPosition();

    unordered_map<string, unordered_map<int, unordered_set<string>>> getChildMap();

    void printAssembleGraph(shared_ptr<InputGraph> aGraph);

    void printInitialPosition();

    void printOccurMap();

    void printChildMap();

private:
    shared_ptr<ConstructGraph> constructGraph_;
    shared_ptr<InputGraph> assembleGraph_;
    vector<string> initialPosition_;
    unordered_map<string, unordered_set<int>> occurMap_;
    unordered_map<string, unordered_map<int, unordered_set<string>>> childMap_;
};


#endif //AGREEMENTTESTING_CONSTRUCTGRAPH_H
