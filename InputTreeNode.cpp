//
// Created by Lei on 7/5/20.
//

#include "InputTreeNode.h"

using namespace std;

InputTreeNode::InputTreeNode(string inputLabel, int treeIndex) : nodeLabel_(inputLabel), treeIndex_(treeIndex){}

InputTreeNode::InputTreeNode(string inputLabel, vector<shared_ptr<InputTreeNode>> &childVec, int treeIndex) :
nodeLabel_(inputLabel), childVec_(childVec), treeIndex_(treeIndex){}

InputTreeNode::~InputTreeNode() = default;

string InputTreeNode::getNodeLabel() {
    return nodeLabel_;
}

vector<shared_ptr<InputTreeNode>> & InputTreeNode::getChildVec() {
    return childVec_;
}

int InputTreeNode::getTreeIndex() {
    return treeIndex_;
}

void InputTreeNode::addChild(shared_ptr<InputTreeNode> child) {
    childVec_.emplace_back(move(child));
}