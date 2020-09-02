//
// Created by Lei on 7/5/20.
//

#ifndef AGREEMENTTESTING_INPUTTREENODE_H
#define AGREEMENTTESTING_INPUTTREENODE_H

#include <iostream>
#include <vector>

using namespace std;

class InputTreeNode {
public:
    InputTreeNode(string inputLabel, int treeIndex);
    InputTreeNode(string inputLabel, vector<shared_ptr<InputTreeNode>>& childVec, int treeIndex);
    ~InputTreeNode();

    string getNodeLabel() ;
    vector<shared_ptr<InputTreeNode>>& getChildVec() ;
    int getTreeIndex();
    void addChild(shared_ptr<InputTreeNode> child);

private:
    string nodeLabel_;
    int treeIndex_;
    vector<shared_ptr<InputTreeNode>> childVec_;
};

#endif //AGREEMENTTESTING_INPUTTREENODE_H
