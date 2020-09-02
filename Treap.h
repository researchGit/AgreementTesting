//
// Created by Lei on 7/29/20.
//

#ifndef AGREEMENTTESTING_TREAP_H
#define AGREEMENTTESTING_TREAP_H

#include "TreapNode.h"
#include <vector>
#include "HelperDataStructure.h"
#include <unordered_set>
#include <unordered_map>

using namespace std;

class Treap {
public:
    Treap();

    ~Treap() = default;

    void
    insert(shared_ptr<TreapNode> &root, string label, int key, int priority, bool active, bool activeOccur,
           shared_ptr<TreapNode> &parent);

    void printTreap(shared_ptr<TreapNode> &node);

    int getKeyCount();

    bool checkInfoCorrectness(shared_ptr<TreapNode> &root);

    bool checkParentChileRelationship(shared_ptr<TreapNode> &root);

    shared_ptr<TreapNode> constructTreap(vector<string> &eulerTourSequence,
                                         unordered_map<string, unordered_map<int, unordered_set<graphEdge, graphEdgeHash>>> &nonTreeEdges);

    unordered_map<string, unordered_map<int, unordered_map<string, unordered_set<shared_ptr<TreapNode>>>>>
    getLevelLabelReferenceMap() { return levelLabelReferenceMap; }

    unordered_map<string, shared_ptr<TreapNode>> getActiveOccurList() { return activeOccurList; }

    void printLevelLabelReferenceMap();

    void printActiveOccurList();

private:
    void rotateLeft(shared_ptr<TreapNode> &root);

    void rotateRight(shared_ptr<TreapNode> &root);

    bool computeSizeRootedOfNode(shared_ptr<TreapNode> &root);

    int keyCount;

    unordered_map<string, unordered_map<int, unordered_map<string, unordered_set<shared_ptr<TreapNode>>>>> levelLabelReferenceMap;

    shared_ptr<TreapNode> lastVisitedPointer;

    unordered_map<string, shared_ptr<TreapNode>> activeOccurList;
};


#endif //AGREEMENTTESTING_TREAP_H
