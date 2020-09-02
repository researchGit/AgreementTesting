//
// Created by Lei on 8/15/20.
//

#ifndef AGREEMENTTESTING_AGREEMENTTESTING_H
#define AGREEMENTTESTING_AGREEMENTTESTING_H

#include "GetDecomposition.h"

struct SuperTreeNode {
    string label;
    shared_ptr<SuperTreeNode> parent;
    vector<shared_ptr<SuperTreeNode>> children;

    SuperTreeNode() = default;

    ~SuperTreeNode() = default;
};

struct Positions {
    vector<string> currentPosition_;
    vector<string> parentPosition_;
    shared_ptr<SuperTreeNode> parent_;
    unordered_map<shared_ptr<TreapNode>, unordered_map<int, unordered_set<string>>> GAMMA;
    unordered_map<string, unordered_map<int, shared_ptr<TreapNode>>> COMPONENT;

    Positions(vector<string> currentPosition, vector<string> parentPosition, shared_ptr<SuperTreeNode> parent,
              unordered_map<shared_ptr<TreapNode>, unordered_map<int, unordered_set<string>>> GAMMA,
              unordered_map<string, unordered_map<int, shared_ptr<TreapNode>>> COMPONENT)
            : currentPosition_(currentPosition), parentPosition_(parentPosition), parent_(parent), GAMMA(GAMMA), COMPONENT(COMPONENT) {};

    ~Positions() = default;
};

class AgreementTesting {
public:
    AgreementTesting(unordered_map<string, unordered_set<int>> &occurMap,
                     unordered_map<string, unordered_map<int, unordered_set<string>>> &childMap,
                     bool enablePromotion,
                     unordered_map<graphEdge, int, graphEdgeHash> allEdge,
                     unordered_map<graphEdge, int, graphEdgeHash> treeEdges,
                     unordered_map<string, unordered_map<int, unordered_set<graphEdge, graphEdgeHash>>> nonTreeEdges,
                     unordered_map<string, unordered_set<string>> adjacentList,
                     unordered_map<string, unordered_map<int, unordered_map<string, unordered_set<shared_ptr<TreapNode>>>>> levelLabelReferenceMap,
                     unordered_map<string, shared_ptr<TreapNode>> activeOccurList, shared_ptr<TreapNode> &treap,
                     int numberOfCount);

    ~AgreementTesting() = default;

    string BuildAST(vector<string> &initialPosition);

    void printSuperTree();

    string convert2Newick(shared_ptr<SuperTreeNode> &tree, string newick);

    shared_ptr<SuperTreeNode> getSuperTreeRoot();

    void writeToTxtFile(string &content, string &fileName);

    time_t getHDTDuration(){return hdtDuration;}

private:
    shared_ptr<GetDecomposition> getDecomposition_;
    shared_ptr<SuperTreeNode> root_;
    time_t hdtDuration = 0;
};


#endif //AGREEMENTTESTING_AGREEMENTTESTING_H
