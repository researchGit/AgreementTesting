//
// Created by Lei on 7/5/20.
//

#include "ConstructGraph.h"

#include <queue>

using namespace std;

ConstructGraph::ConstructGraph(shared_ptr<InputGraph> &assembleGraph, vector<string> &initialPosition,
                               unordered_map<string, unordered_set<int>> &occurMap,
                               unordered_map<string, unordered_map<int, unordered_set<string>>> &childMap) : assembleGraph_(assembleGraph),
                                                                                      initialPosition_(initialPosition),
                                                                                      occurMap_(occurMap),
                                                                                      childMap_(childMap){}

void ConstructGraph::createAssembleGraph(vector<shared_ptr<InputTreeNode>> &inputTrees) {
    shared_ptr<InputGraph> assembleGraph = make_shared<InputGraph>();
    queue<shared_ptr<InputTreeNode>> processQueue;

    for (shared_ptr<InputTreeNode> &inputTreeNode : inputTrees) {
        processQueue.emplace(inputTreeNode);
        this->initialPosition_.emplace_back(inputTreeNode->getNodeLabel());
        while (!processQueue.empty()) {
            shared_ptr<InputTreeNode> &currentNode = processQueue.front();
            // add each label in the occurMap
            // which is used later to compute the exposure of a node
            auto occurIter = this->occurMap_.find(currentNode->getNodeLabel());
            if (occurIter != this->occurMap_.end()) {
                occurIter->second.emplace(currentNode->getTreeIndex());
            } else {
                unordered_set<int> tempSet{currentNode->getTreeIndex()};
                this->occurMap_.emplace(currentNode->getNodeLabel(), tempSet);
            }

            for (shared_ptr<InputTreeNode> &child : currentNode->getChildVec()) {
                if (assembleGraph->find(currentNode->getNodeLabel()) != assembleGraph->end()) {
                    //find the element
                    assembleGraph->at(currentNode->getNodeLabel()).emplace(child->getNodeLabel());
                } else {
                    unordered_set<string> adjNodeSet;
                    adjNodeSet.emplace(child->getNodeLabel());
                    assembleGraph->emplace(currentNode->getNodeLabel(), adjNodeSet);
                }

                if (assembleGraph->find(child->getNodeLabel()) != assembleGraph->end()) {
                    assembleGraph->at(child->getNodeLabel()).emplace(currentNode->getNodeLabel());
                } else {
                    unordered_set<string> adjNodeSet;
                    adjNodeSet.emplace(currentNode->getNodeLabel());
                    assembleGraph->emplace(child->getNodeLabel(), adjNodeSet);
                }
                processQueue.emplace(child);

                auto childIter = childMap_.find(currentNode->getNodeLabel());
                if(childIter != childMap_.end()){
                    auto idxIter = childIter->second.find(currentNode->getTreeIndex());
                    if(idxIter != childIter->second.end()){
                        idxIter->second.emplace(child->getNodeLabel());
                    }
                    else {
                        unordered_set<string> tempSet{child->getNodeLabel()};
                        childIter->second.emplace(currentNode->getTreeIndex(), tempSet);
                    }
                }
                else {
                    unordered_set<string> tempSet{child->getNodeLabel()};
                    unordered_map<int, unordered_set<string>> tempMap{{currentNode->getTreeIndex(), tempSet}};
                    childMap_.emplace(currentNode->getNodeLabel(), tempMap);
                }
            }
            processQueue.pop();
        }
    }

    this->constructGraph_ = make_shared<ConstructGraph>(assembleGraph, this->initialPosition_, this->occurMap_, this->childMap_);
}

shared_ptr<InputGraph> ConstructGraph::getAssembleGraph() {
    return assembleGraph_;
}

void ConstructGraph::printAssembleGraph() {
    for_each(assembleGraph_->begin(), assembleGraph_->end(), [](pair<string, unordered_set<string>> graphNode) {
        cout << "label is: " << graphNode.first << endl;
        cout << "\t";
        for (const string &childLabel : graphNode.second) {
            cout << childLabel << "\t";
        }
        cout << endl;
    });
}

void ConstructGraph::printInitialPosition() {
    cout << "--------- position size -----------" << endl;
    cout << this->initialPosition_.size() << endl;
    cout << "---- print Initial Position -----" << endl;
    for (int i = 0; i < this->initialPosition_.size(); ++i) {
        cout << this->initialPosition_[i] << " ";
    }
    cout << endl;
}

shared_ptr<ConstructGraph> ConstructGraph::getConstructGraph() {
    return this->constructGraph_;
}

unordered_map<string, unordered_set<int>> ConstructGraph::getOccurMap() {
    return this->occurMap_;
}

vector<string> ConstructGraph::getInitialPosition() {
    return this->initialPosition_;
}

void ConstructGraph::printOccurMap() {
    cout << "------- print OccurMap ----------" << endl;
    for (auto &occur : this->occurMap_) {
        cout << occur.first << " -> ";
        for (auto &idx : occur.second) {
            cout << idx << ",";
        }
        cout << endl;
    }
}

unordered_map<string, unordered_map<int, unordered_set<string>>> ConstructGraph::getChildMap() {
    return this->childMap_;
}

void ConstructGraph::printChildMap() {
    cout << "------ print child map -------" << endl;
    for(auto &child : childMap_){
        cout << child.first << " : " << endl;
        for(auto &entry : child.second){
            cout << "\t" << entry.first << " : ";
            for(auto & ent : entry.second){
                cout << ent << " ";
            }
            cout << endl;
        }
    }
}