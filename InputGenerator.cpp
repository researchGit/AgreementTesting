//
// Created by Lei on 8/19/20.
//

#include "InputGenerator.h"
#include <queue>
#include <random>
#include <chrono>

using namespace std;

void InputGenerator::generateBinaryInputTrees(int numberOfTaxa, int numberOfTrees) {
    int treeCount = 0;
    int taxaCount = 0;
    vector<shared_ptr<InputTreeNode>> nodes;
    // construct a binary seed tree
    shared_ptr<InputTreeNode> seedRoot = make_shared<InputTreeNode>("label_" + to_string(taxaCount++), treeCount);
    queue<shared_ptr<InputTreeNode>> q;
    q.emplace(seedRoot);
    while (!q.empty()) {
        auto currentNode = q.front();
        q.pop();
        if (taxaCount < numberOfTaxa) {
            shared_ptr<InputTreeNode> left = make_shared<InputTreeNode>("label_" + to_string(taxaCount++), treeCount);
            currentNode->addChild(left);
            q.emplace(left);
            nodes.emplace_back(left);
        }
        if (taxaCount < numberOfTaxa) {
            shared_ptr<InputTreeNode> right = make_shared<InputTreeNode>("label_" + to_string(taxaCount++), treeCount);
            currentNode->addChild(right);
            q.emplace(right);
            nodes.emplace_back(right);
        }
    }
    inputTrees.emplace_back(seedRoot);

//    cout << "---- inspect seed tree ----" << endl;
//    printTree(seedRoot);

    while (++treeCount < numberOfTrees) {
        inputTrees.emplace_back(constructSubTree(nodes, seedRoot, treeCount));
    }
}

void InputGenerator::generateGeneralInputTrees(int numberOfTaxa, int numberOfTrees, int degree) {
    int treeCount = -1;
    int taxaCount = 0;
    vector<shared_ptr<InputTreeNode>> nodes;
    unordered_set<string> availableLabels;
    unordered_set<string> usedLabels;
    // construct a binary seed tree
    string generatedLabel = "label_" + to_string(taxaCount++);
    shared_ptr<InputTreeNode> seedRoot = make_shared<InputTreeNode>(generatedLabel, treeCount);
    queue<shared_ptr<InputTreeNode>> q;
    q.emplace(seedRoot);
    availableLabels.emplace(generatedLabel);
    while (!q.empty()) {
        auto currentNode = q.front();
        q.pop();
        if (taxaCount < numberOfTaxa) {
            for (int i = 0; i < degree; ++i) {
                generatedLabel = "label_" + to_string(taxaCount++);
                shared_ptr<InputTreeNode> childNode = make_shared<InputTreeNode>(generatedLabel, treeCount);
                currentNode->addChild(childNode);
                q.emplace(childNode);
                nodes.emplace_back(childNode);
                availableLabels.emplace(generatedLabel);
            }
        }
    }
    labelC = taxaCount;
    seedTrees.emplace_back(seedRoot);

//    cout << "---- inspect seed tree ----" << endl;
//    printTree(seedRoot);

    while (++treeCount < numberOfTrees) {
        if (treeCount + 1 == numberOfTrees)
            inputTrees.emplace_back(
                    constructGeneralSubTree(nodes, seedRoot, treeCount, degree, availableLabels, usedLabels, true));
        else
            inputTrees.emplace_back(
                    constructGeneralSubTree(nodes, seedRoot, treeCount, degree, availableLabels, usedLabels, false));
    }
}

shared_ptr<InputTreeNode> InputGenerator::constructGeneralSubTree(vector<shared_ptr<InputTreeNode>> nodes,
                                                                  shared_ptr<InputTreeNode> &seed, int treeCount,
                                                                  int degree,
                                                                  unordered_set<string> &availableLabels,
                                                                  unordered_set<string> &usedLabels,
                                                                  bool lastTree) {
//    cout << "---- check sizes ----" << endl;
//    cout << availableLabels.size() << endl;
//    cout << usedLabels.size() << endl;

    unordered_set<string> subset;
    if (usedLabels.empty()) {
        unsigned ranSeed = chrono::system_clock::now().time_since_epoch().count();
        shuffle(nodes.begin(), nodes.end(), default_random_engine(ranSeed));
        int len = (int) availableLabels.size() / 10;
        for (int i = 0; i < len; ++i) {
            string label = nodes[i]->getNodeLabel();
            subset.emplace(label);
            usedLabels.emplace(label);
            auto labelsIter = availableLabels.find(label);
            if (labelsIter != availableLabels.end()) availableLabels.erase(labelsIter);
        }
    } else if (availableLabels.empty()) {
        unsigned ranSeed = chrono::system_clock::now().time_since_epoch().count();
        shuffle(nodes.begin(), nodes.end(), default_random_engine(ranSeed));
        int len = (int) nodes.size() - 2;
        int randNum = len <= 0 ? (int)nodes.size() : rand() % len + 2;
        for (int i = 0; i < randNum; ++i) {
            subset.emplace(nodes[i]->getNodeLabel());
        }
    } else {
        if(lastTree){
            int usedLen = (int) usedLabels.size() - 2;
            int usedRand = usedLen <= 0 ? (int)usedLabels.size() : rand() % usedLen + 2;
            vector<string> used;
            for (auto &l : usedLabels) used.emplace_back(l);
            unsigned ranSeed = chrono::system_clock::now().time_since_epoch().count();
            shuffle(used.begin(), used.end(), default_random_engine(ranSeed));
            for (int i = 0; i < usedRand; ++i) subset.emplace(used[i]);
            for (auto &l : availableLabels) subset.emplace(l);
        } else {
//            cout << " i am here" << endl;
            int availableLen = (int) availableLabels.size() - 2;
            int availableRand = availableLen <= 0 ? (int)availableLabels.size() : rand() % availableLen + 2;
            int usedLen = (int) usedLabels.size() - 2;
            int usedRand = usedLen <= 0 ? (int)usedLabels.size() : rand() % usedLen + 2;
            vector<string> available;
            for (auto &l : availableLabels) available.emplace_back(l);
            vector<string> used;
            for (auto &l : usedLabels) used.emplace_back(l);
//            cout << "here?" << endl;

//            cout << "check used" << endl;
//            for (auto &l : used) cout << l << " ";
//            cout << endl;
//
//            cout << "check available" << endl;
//            for (auto &l : available) cout << l << " ";
//            cout << endl;

            unsigned ranSeed = chrono::system_clock::now().time_since_epoch().count();
            shuffle(used.begin(), used.end(), default_random_engine(ranSeed));
            for (int i = 0; i < usedRand; ++i) subset.emplace(used[i]);
            ranSeed = chrono::system_clock::now().time_since_epoch().count();
            shuffle(available.begin(), available.end(), default_random_engine(ranSeed));
            for (int i = 0; i < availableRand; ++i) {
                string label = available[i];
                subset.emplace(label);
                usedLabels.emplace(label);
                auto labelsIter = availableLabels.find(label);
                if (labelsIter != availableLabels.end()) availableLabels.erase(labelsIter);
            }
        }
    }

//    cout << "---- inspect subset ----" << endl;
//    for (auto &ele : subset) cout << ele << " ";
//    cout << endl;

    shared_ptr<InputTreeNode> subtreeRoot = extractGeneralSubTree(subset, seed, treeCount, degree);

//    cout <<"---- inspect subtree ----" << endl;
//    printSubtree(subtreeRoot);

    return subtreeRoot;
}

void InputGenerator::printTree(shared_ptr<InputTreeNode> &root) {
    queue<shared_ptr<InputTreeNode>> q;
    q.emplace(root);
    while (!q.empty()) {
        auto currentNode = q.front();
        q.pop();
        cout << currentNode->getNodeLabel() << " -> ";
        for (auto &child : currentNode->getChildVec()) {
            cout << child->getNodeLabel() << " ";
            q.emplace(child);
        }
        cout << endl;
    }
}

vector<shared_ptr<InputTreeNode>> InputGenerator::getInputTrees() {
    return inputTrees;
}

shared_ptr<InputTreeNode>
InputGenerator::constructSubTree(vector<shared_ptr<InputTreeNode>> nodes, shared_ptr<InputTreeNode> &seed,
                                 int treeCount) {
    int len = (int) nodes.size() - 2;
    int randNum = rand() % len + 2;
    unordered_set<string> subset;
    unsigned ranSeed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(nodes.begin(), nodes.end(), default_random_engine(ranSeed));
    for (int i = 0; i < randNum; ++i) subset.emplace(nodes[i]->getNodeLabel());

//    cout << "---- inspect subset ----" << endl;
//    for(auto &ele : subset) cout << ele << " ";
//    cout << endl;

    shared_ptr<InputTreeNode> subtreeRoot = extractSubTree(subset, seed, treeCount);

//    cout <<"---- inspect subtree ----" << endl;
//    printSubtree(subtreeRoot);

    return subtreeRoot;
}

shared_ptr<InputTreeNode> InputGenerator::extractGeneralSubTree(unordered_set<string> &subset,
                                                                shared_ptr<InputTreeNode> &node, int treeCount,
                                                                int degree) {
    if (node == nullptr) return nullptr;
    shared_ptr<InputTreeNode> treeNode;
    auto subsetIter = subset.find(node->getNodeLabel());
    if (subsetIter != subset.end()) {
        // need to construct the node for this label
        treeNode = make_shared<InputTreeNode>(*subsetIter, treeCount);
        subset.erase(subsetIter);
    }

    vector<shared_ptr<InputTreeNode>> childNodes(degree);
    for (int i = 0; i < degree; ++i) childNodes[i] = nullptr;
    for (int i = 0; i < node->getChildVec().size(); ++i) {
        childNodes[i] = node->getChildVec()[i];
    }

    vector<shared_ptr<InputTreeNode>> returnNodes;
    int numberOfNonNullptr = 0;
    for (auto cNode : childNodes) {
        auto childNode = extractGeneralSubTree(subset, cNode, treeCount, degree);
        if (childNode != nullptr) {
            ++numberOfNonNullptr;
            returnNodes.emplace_back(childNode);
        }
    }

    if (numberOfNonNullptr >= 2) {
        if (treeNode == nullptr) {
//            treeNode = make_shared<InputTreeNode>(node->getNodeLabel(), treeCount);
            treeNode = make_shared<InputTreeNode>("label_" + to_string(labelC++), treeCount);
        }
        for (auto &rNode : returnNodes) {
            treeNode->addChild(rNode);
        }
    } else if (numberOfNonNullptr == 1) {
        if (treeNode == nullptr) return returnNodes[0];
        treeNode->addChild(returnNodes[0]);
    }

    return treeNode;
}

shared_ptr<InputTreeNode> InputGenerator::extractSubTree(unordered_set<string> &subset,
                                                         shared_ptr<InputTreeNode> &node, int treeCount) {
    if (node == nullptr) return nullptr;
    shared_ptr<InputTreeNode> treeNode;
    auto subsetIter = subset.find(node->getNodeLabel());
    if (subsetIter != subset.end()) {
        // need to construct the node for this label
        treeNode = make_shared<InputTreeNode>(*subsetIter, treeCount);
        subset.erase(subsetIter);
    }
    shared_ptr<InputTreeNode> leftNode = (node->getChildVec().empty() ? nullptr : node->getChildVec().front());
    shared_ptr<InputTreeNode> rightNode = (node->getChildVec().size() > 1 ? node->getChildVec().back() : nullptr);

    auto left = extractSubTree(subset, leftNode, treeCount);
    auto right = extractSubTree(subset, rightNode, treeCount);
    if (left != nullptr && right != nullptr) {
        if (treeNode == nullptr) {
            treeNode = make_shared<InputTreeNode>(node->getNodeLabel(), treeCount);
        }
        treeNode->addChild(left);
        treeNode->addChild(right);
    } else if (left != nullptr) {
        if (treeNode == nullptr) return left;
        treeNode->addChild(left);
    } else if (right != nullptr) {
        if (treeNode == nullptr) return right;
        treeNode->addChild(right);
    }
    return treeNode;
}

void InputGenerator::printSubtree(shared_ptr<InputTreeNode> &node) {
    if (node == nullptr) return;
    cout << node->getNodeLabel() << " -> ";
    for (auto &child : node->getChildVec()) {
        cout << child->getNodeLabel() << " ";
    }
    cout << endl;
    for (auto &child : node->getChildVec()) {
        printSubtree(child);
    }
}

vector<shared_ptr<InputTreeNode>> InputGenerator::getSeedTrees() {
    return seedTrees;
}