//
// Created by Lei on 8/19/20.
//

#ifndef AGREEMENTTESTING_INPUTGENERATOR_H
#define AGREEMENTTESTING_INPUTGENERATOR_H

#include "InputTreeNode.h"
#include <unordered_set>

class InputGenerator {
public:
    InputGenerator() = default;

    ~InputGenerator() = default;

    void generateBinaryInputTrees(int numberOfTaxa, int numberOfTrees);

    void generateGeneralInputTrees(int numberOfTaxa, int numberOfTrees, int degree);

    void printTree(shared_ptr<InputTreeNode> &root);

    vector<shared_ptr<InputTreeNode>> getInputTrees();

    vector<shared_ptr<InputTreeNode>> getSeedTrees();

    int getLabelCount(){return labelC;}

private:
    shared_ptr<InputTreeNode>
    constructSubTree(vector<shared_ptr<InputTreeNode>> nodes, shared_ptr<InputTreeNode> &seed, int treeCount);

    shared_ptr<InputTreeNode>
    constructGeneralSubTree(vector<shared_ptr<InputTreeNode>> nodes, shared_ptr<InputTreeNode> &seed, int treeCount,
                            int degree, unordered_set<string> &availableLabels, unordered_set<string> &usedLabels,
                            bool lastTree);

    shared_ptr<InputTreeNode>
    extractSubTree(unordered_set<string> &subset, shared_ptr<InputTreeNode> &node, int treeCount);

    shared_ptr<InputTreeNode>
    extractGeneralSubTree(unordered_set<string> &subset, shared_ptr<InputTreeNode> &node, int treeCount, int degree);

    void printSubtree(shared_ptr<InputTreeNode> &node);

    vector<shared_ptr<InputTreeNode>> inputTrees;

    vector<shared_ptr<InputTreeNode>> seedTrees;

    int labelC = 0;

    void writeToFile(string path, string content);
};


#endif //AGREEMENTTESTING_INPUTGENERATOR_H
