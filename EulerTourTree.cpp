//
// Created by Lei on 7/5/20.
//

#include "EulerTourTree.h"

#include <stack>

vector<string> EulerTourTree::constructEulerTourTree(shared_ptr<InputGraph> assembleGraph) {
    unordered_map<string, bool> visitedLabels;
    unordered_set<string> elementSet;
    unordered_map<string, string> fromMap;

    for_each(assembleGraph->begin(), assembleGraph->end(),
             [&visitedLabels, &elementSet](pair<string, unordered_set<string>> graphNode) {
                 visitedLabels.emplace(graphNode.first, false);
                 elementSet.emplace(graphNode.first);
             });

    stack<string> dfsStack;

    vector<string> eulerTourSeq;
    while (!elementSet.empty()) {
        dfsStack.emplace(*elementSet.begin());
        while (!dfsStack.empty()) {
            string currentLabel = dfsStack.top();
            dfsStack.pop();

            auto visitedLabelsIter = visitedLabels.find(currentLabel);
            if (visitedLabelsIter != visitedLabels.end()) {
                visitedLabelsIter->second = true;
                eulerTourSeq.push_back(currentLabel);
                auto elementSetIter = elementSet.find(currentLabel);
                if (elementSetIter != elementSet.end()) {
                    elementSet.erase(elementSetIter);
                }

                bool allEmpty = false;
                auto assembleGraphIter = assembleGraph->find(currentLabel);
                if (assembleGraphIter != assembleGraph->end()) {
                    for (const string &childLabel : assembleGraph->at(currentLabel)) {
                        visitedLabelsIter = visitedLabels.find(childLabel);
                        if (visitedLabelsIter != visitedLabels.end()) {
                            if (!visitedLabelsIter->second) {
                                allEmpty = true;
                                dfsStack.emplace(childLabel);
                                fromMap.emplace(childLabel, currentLabel);

                                // find an edge which is a tree edge
                                graphEdge edge;
                                if(childLabel.compare(currentLabel) < 0){
                                    edge.endPoint1 = childLabel;
                                    edge.endPoint2 = currentLabel;
                                }
                                else{
                                    edge.endPoint1 = currentLabel;
                                    edge.endPoint2 = childLabel;
                                }

                                treeEdges_.emplace(edge, 0);
                                allEdges_.emplace(edge, 0);
                                break;
                            } else {
                                // construct non-tree edges
                                graphEdge edge;
                                if(childLabel.compare(currentLabel) < 0){
                                    edge.endPoint1 = childLabel;
                                    edge.endPoint2 = currentLabel;
                                }
                                else{
                                    edge.endPoint1 = currentLabel;
                                    edge.endPoint2 = childLabel;
                                }

                                auto treeEdgeIter = treeEdges_.find(edge);
                                if(treeEdgeIter == treeEdges_.end()){
                                    allEdges_.emplace(edge, 0);
                                    auto edgeIter = nonTreeEdges_.find(edge.endPoint1);
                                    if(edgeIter != nonTreeEdges_.end()){
                                        auto levelIter = edgeIter->second.find(0);
                                        if(levelIter != edgeIter->second.end()) levelIter->second.emplace(edge);
                                        else{
                                            unordered_set<graphEdge, graphEdgeHash> tmpSet;
                                            tmpSet.emplace(edge);
                                            edgeIter->second.emplace(0, tmpSet);
                                        }
                                    }
                                    else{
                                        unordered_set<graphEdge, graphEdgeHash> tmpSet;
                                        tmpSet.emplace(edge);
                                        unordered_map<int, unordered_set<graphEdge, graphEdgeHash>> tmpMap;
                                        tmpMap.emplace(0, tmpSet);
                                        nonTreeEdges_.emplace(edge.endPoint1, tmpMap);
                                    }

                                    edgeIter = nonTreeEdges_.find(edge.endPoint2);
                                    if(edgeIter != nonTreeEdges_.end()){
                                        auto levelIter = edgeIter->second.find(0);
                                        if(levelIter != edgeIter->second.end()) levelIter->second.emplace(edge);
                                        else{
                                            unordered_set<graphEdge, graphEdgeHash> tmpSet;
                                            tmpSet.emplace(edge);
                                            edgeIter->second.emplace(0, tmpSet);
                                        }
                                    }
                                    else{
                                        unordered_set<graphEdge, graphEdgeHash> tmpSet;
                                        tmpSet.emplace(edge);
                                        unordered_map<int, unordered_set<graphEdge, graphEdgeHash>> tmpMap;
                                        tmpMap.emplace(0, tmpSet);
                                        nonTreeEdges_.emplace(edge.endPoint2, tmpMap);
                                    }
                                }
                            }
                        }
                    }
                }
                if (!allEmpty) {
                    if (fromMap.find(currentLabel) != fromMap.end()) {
                        dfsStack.emplace(fromMap.at(currentLabel));
                    }
                }
            }
        }
    }
    return eulerTourSeq;
}

void EulerTourTree::printEulerTourTree(const vector<string> &spanningTree) {
    cout << "spanning tree is: " << endl;
    cout << "\t";
    for (const string &label : spanningTree) {
        cout << label << "\t";
    }
    cout << endl;
}

void EulerTourTree::printEdges() {
    cout << "tree edges: " << endl;
    for(auto &edge : treeEdges_){
        cout << edge.first.endPoint1 << " - " << edge.first.endPoint2 << " + " << edge.second << endl;
    }
    cout << "all edges: " << endl;
    for(auto &edge : allEdges_){
        cout << edge.first.endPoint1 << " - " << edge.first.endPoint2 << " + " << edge.second << endl;
    }
    cout << "non tree edges: " << endl;
    for(auto &edge: nonTreeEdges_){
        cout << "major edge is: " << edge.first << endl;
        for(auto &e: edge.second){
            cout << "\t" << e.first << endl;
            for(auto &eg: e.second){
                cout << "\t\t" << eg.endPoint1 << " - " << eg.endPoint2 << endl;
            }
        }
    }
}