//
// Created by Lei on 8/15/20.
//

#include "AgreementTesting.h"
#include <queue>
#include <stack>
#include <fstream>

using namespace std;

AgreementTesting::AgreementTesting(unordered_map<string, unordered_set<int>> &occurMap,
                                   unordered_map<string, unordered_map<int, unordered_set<string>>> &childMap,
                                   bool enablePromotion,
                                   unordered_map<graphEdge, int, graphEdgeHash> allEdge,
                                   unordered_map<graphEdge, int, graphEdgeHash> treeEdges,
                                   unordered_map<string, unordered_map<int, unordered_set<graphEdge, graphEdgeHash>>> nonTreeEdges,
                                   unordered_map<string, unordered_set<string>> adjacentList,
                                   unordered_map<string, unordered_map<int, unordered_map<string, unordered_set<shared_ptr<TreapNode>>>>> levelLabelReferenceMap,
                                   unordered_map<string, shared_ptr<TreapNode>> activeOccurList,
                                   shared_ptr<TreapNode> &treap,
                                   int numberOfCount) {
    getDecomposition_ = make_shared<GetDecomposition>(occurMap, childMap, enablePromotion, allEdge, treeEdges,
                                                      nonTreeEdges, adjacentList, levelLabelReferenceMap,
                                                      activeOccurList, treap, numberOfCount);
}

string AgreementTesting::BuildAST(vector<string> &initialPosition) {
    vector<string> position(initialPosition.size());
    for(int i = 0; i < initialPosition.size(); ++i) position[i] = "";
    shared_ptr<SuperTreeNode> pred = make_shared<SuperTreeNode>();
    unordered_map<shared_ptr<TreapNode>, unordered_map<int, unordered_set<string>>> GAMMA;
    unordered_map<string, unordered_map<int, shared_ptr<TreapNode>>> COMPONENT;
    shared_ptr<Positions> initialPositions = make_shared<Positions>(initialPosition, position, pred, GAMMA, COMPONENT);
    queue<shared_ptr<Positions>> Q;
    Q.emplace(initialPositions);
    bool first = true;

    while (!Q.empty()) {
        int size = Q.size();
        for (int i = 0; i < size; ++i) {
            auto currentPositions = Q.front();
            vector<string> currentPoi = currentPositions->currentPosition_;
            vector<string> parentPoi = currentPositions->parentPosition_;
            Q.pop();

            // <S, PI> = GetDecomposition(pi)
            auto SnPi = getDecomposition_->computeDecomposition(currentPoi, parentPoi, currentPositions->GAMMA, currentPositions->COMPONENT, first);
            if(first) first = false;

            // if S = empty then return disagreement
            if(SnPi->S.empty()) return "DISAGREEMENT";
            // create anode r(pi)
            shared_ptr<SuperTreeNode> currentNode = make_shared<SuperTreeNode>();
            // r(pi).parent = pred
            currentNode->parent = currentPositions->parent_;
            currentPositions->parent_->children.emplace_back(currentNode);
            // add each label from S to the node's label field
            for(auto &l : SnPi->S){
                if(l.substr(0,2) == "f_") continue;
                currentNode->label += (l + "-");
            }
            // add child positions to the queue for further processing
            for(int j = 0; j < SnPi->childPositions->size(); ++j){
                shared_ptr<Positions> childPositions = make_shared<Positions>((*SnPi->childPositions)[j], currentPoi, currentNode, (*SnPi->GAMMA)[j], (*SnPi->COMPONENT)[j]);
                Q.emplace(childPositions);
            }
        }
    }

    if(pred != nullptr){
        root_ = pred->children.front();
    }
    hdtDuration = getDecomposition_->getHDTExecutionTime();
    return "AGREEMENT";
}

void AgreementTesting::printSuperTree() {
    cout << "---- print out the supertree ----" << endl;
    stack<shared_ptr<SuperTreeNode>> stack;
    stack.emplace(root_);
    while(!stack.empty()){
        auto &currentNode = stack.top();
        stack.pop();
        cout << currentNode->label << " -> ";
        for(auto &child : currentNode->children){
            cout << child->label << " | ";
            stack.emplace(child);
        }
        cout << endl;
    }
}

string AgreementTesting::convert2Newick(shared_ptr<SuperTreeNode> &tree, string newick) {
    if(tree != nullptr){
        if(!tree->children.empty()){
            //this tree has the children
            newick += "(";
            for(auto &child : tree->children){
                newick = convert2Newick(child, newick);
                newick += ',';
            }
            if(newick[newick.length()-1] == ','){
                newick = newick.substr(0, newick.length()-1);
            }
            newick += ")";
        }
        if(tree->label[tree->label.length()-1] == '-'){
            newick += tree->label.substr(0, tree->label.length()-1);
        }
    }
    return newick;
}

shared_ptr<SuperTreeNode> AgreementTesting::getSuperTreeRoot() {
    return root_;
}

void AgreementTesting::writeToTxtFile(string &content, string &fileName) {
    fstream file;
    file.open ("../output/"+fileName+".txt", ios::out);
    string newickStr = content + ";\n\r";
    file << newickStr << endl;
    file.close();
}