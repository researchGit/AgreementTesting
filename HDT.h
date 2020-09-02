//
// Created by Lei on 7/30/20.
//

#ifndef AGREEMENTTESTING_HDT_H
#define AGREEMENTTESTING_HDT_H

#include "Treap.h"
#include <math.h>
#include <vector>
#include "TreapNode.h"

struct cutTreaps {
    shared_ptr<TreapNode> treap1;
    shared_ptr<TreapNode> treap2;

    cutTreaps() = default;

    cutTreaps(shared_ptr<TreapNode> &t1, shared_ptr<TreapNode> &t2) : treap1(t1), treap2(t2) {};
};

class HDT {
public:
    HDT(bool enablePromotion, unordered_map<graphEdge, int, graphEdgeHash> allEdge,
        unordered_map<graphEdge, int, graphEdgeHash> treeEdges,
        unordered_map<string, unordered_map<int, unordered_set<graphEdge, graphEdgeHash>>> nonTreeEdges,
        unordered_map<string, unordered_set<string>> adjacentList,
        unordered_map<string, unordered_map<int, unordered_map<string, unordered_set<shared_ptr<TreapNode>>>>> levelLabelReferenceMap,
        unordered_map<string, shared_ptr<TreapNode>> activeOccurList,
        shared_ptr<TreapNode> &treap,
        int numberOfCount);

    ~HDT() = default;

    shared_ptr<TreapNode> treapU = nullptr;
    shared_ptr<TreapNode> treapV = nullptr;
    unordered_map<int, unordered_map<string, shared_ptr<TreapNode>>> levelActiveOccurMap;

    void hdtDeleteEdge(string u, string v);

    void printTreapVector();

    void printLevelTreapMap();

    shared_ptr<TreapNode> hdtFindRoot(shared_ptr<TreapNode> &occur);

private:
    int logN = 0;
    unordered_map<graphEdge, int, graphEdgeHash> edgeList;
    unordered_map<graphEdge, int, graphEdgeHash> treeEdges;
    unordered_map<string, unordered_map<int, unordered_set<graphEdge, graphEdgeHash>>> nonTreeEdges;
    unordered_map<string, unordered_set<string>> adjacentList;
    unordered_map<string, unordered_map<int, unordered_map<string, unordered_set<shared_ptr<TreapNode>>>>> labelReference;
    unordered_map<string, shared_ptr<TreapNode>> activeOccurList;
    unordered_map<int, unordered_set<shared_ptr<TreapNode>>> levelTreapMap;
    bool promotionSwitch = false;
    int nodeCount = 0;

    void hdtCutTreap(string u, string v, shared_ptr<TreapNode> &treap, int edgeLV);

    bool isFirstOccurSmallerThanSecondOccur(shared_ptr<TreapNode> &occurA, shared_ptr<TreapNode> &occurB);

    int computeLengthOfPathToRoot(shared_ptr<TreapNode> &currentNode);

    shared_ptr<vector<shared_ptr<TreapNode>>> computePathToRoot(int pathLength, shared_ptr<TreapNode> &currentNode);

    void leftRotate(shared_ptr<TreapNode> &root);

    void rightRotate(shared_ptr<TreapNode> &root);

    shared_ptr<TreapNode> findRightmostNode(shared_ptr<TreapNode> &tree);

    shared_ptr<TreapNode> findLeftmostNode(shared_ptr<TreapNode> &tree);

    shared_ptr<TreapNode> reLinkTreaps(shared_ptr<TreapNode> treap1, shared_ptr<TreapNode> treap2, int edgeLV);

    void hdtCutTreapAtRightPosition(shared_ptr<TreapNode> &currentTreap, shared_ptr<TreapNode> &cutPosition,
                                    shared_ptr<TreapNode> &treapPiece1, shared_ptr<TreapNode> &treapPiece2);

    void hdtCutTreapAtLeftPosition(shared_ptr<TreapNode> &currentTreap, shared_ptr<TreapNode> &cutPosition,
                                   shared_ptr<TreapNode> &treapPiece1, shared_ptr<TreapNode> &treapPiece2);

    unordered_map<int, shared_ptr<cutTreaps>> treapVector;

    void promoteTreeEdges(int edgeLV, shared_ptr<cutTreaps> currentCutTreaps);

    void rerooting(string u, shared_ptr<TreapNode> &tree, int edgeLV);

    graphEdge findReplacementEdge(shared_ptr<TreapNode> &smallTreap, shared_ptr<TreapNode> &largeTreap, int edgeLV);

    void hdtSplit(string u, string v, int edgeLV);

    void adjacencyListDeleteEdge(string u, string v);
};


#endif //AGREEMENTTESTING_HDT_H
