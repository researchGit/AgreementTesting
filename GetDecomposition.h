//
// Created by Lei on 8/15/20.
//

#ifndef AGREEMENTTESTING_GETDECOMPOSITION_H
#define AGREEMENTTESTING_GETDECOMPOSITION_H

#include "HDT.h"
#include <list>
#include <ctime>

struct GoodDecomposition {
    shared_ptr<vector<vector<string>>> childPositions;
    unordered_set<string> S;
    shared_ptr<vector<unordered_map<shared_ptr<TreapNode>, unordered_map<int, unordered_set<string>>>>> GAMMA;
    shared_ptr<vector<unordered_map<string, unordered_map<int, shared_ptr<TreapNode>>>>> COMPONENT;

    GoodDecomposition(shared_ptr<vector<vector<string>>> childPositions, unordered_set<string> S,
                      shared_ptr<vector<unordered_map<shared_ptr<TreapNode>, unordered_map<int, unordered_set<string>>>>> GAMMA,
                      shared_ptr<vector<unordered_map<string, unordered_map<int, shared_ptr<TreapNode>>>>> COMPONENT)
            : childPositions(
            move(childPositions)), S(move(S)), GAMMA(GAMMA), COMPONENT(COMPONENT) {};

    ~GoodDecomposition() = default;
};

class GetDecomposition {
public:
    GetDecomposition(unordered_map<string, unordered_set<int>> &occurMap,
                     unordered_map<string, unordered_map<int, unordered_set<string>>> &childMap,
                     bool enablePromotion,
                     unordered_map<graphEdge, int, graphEdgeHash> allEdge,
                     unordered_map<graphEdge, int, graphEdgeHash> treeEdges,
                     unordered_map<string, unordered_map<int, unordered_set<graphEdge, graphEdgeHash>>> nonTreeEdges,
                     unordered_map<string, unordered_set<string>> adjacentList,
                     unordered_map<string, unordered_map<int, unordered_map<string, unordered_set<shared_ptr<TreapNode>>>>> levelLabelReferenceMap,
                     unordered_map<string, shared_ptr<TreapNode>> activeOccurList, shared_ptr<TreapNode> &treap,
                     int numberOfCount);

    ~GetDecomposition() = default;

    shared_ptr<GoodDecomposition> computeDecomposition(vector<string> &position, vector<string> &parentPosition, unordered_map<shared_ptr<TreapNode>, unordered_map<int, unordered_set<string>>> gam,
                                                       unordered_map<string, unordered_map<int, shared_ptr<TreapNode>>> comp, bool first);

    void printAdjacentList();

    time_t getHDTExecutionTime(){return hdtTimeDuration;}

private:
    shared_ptr<HDT> hdt_;
    unordered_map<string, unordered_set<int>> &occurMap_;
    unordered_map<string, unordered_set<string>> adjacentList_;
    unordered_map<string, unordered_map<int, unordered_set<string>>> &childMap_;

    string computeBadLabel(unordered_set<string> &S,
                           unordered_map<string, unordered_map<int, shared_ptr<TreapNode>>> &component);

    time_t hdtTimeDuration = 0;
};


#endif //AGREEMENTTESTING_GETDECOMPOSITION_H
