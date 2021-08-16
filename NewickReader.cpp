//
// Created by Lei on 7/18/21.
//

#include "NewickReader.h"
#include <queue>
#include <unordered_map>

using namespace newickFormatParser;
using namespace std;

vector<newickFormatParser::inputTreeNode> NewickReader::parseNewickFile(string& filePath) {
    fstream inputFile(filePath);

    vector<newickFormatParser::inputTreeNode> rawInputTrees;

    string content;
    while(getline(inputFile, content)){
        auto findIdxOfSemiC = content.find(";");
        if(findIdxOfSemiC != string::npos) content = content.substr(0, findIdxOfSemiC);
        auto inputTree = parseNewickString(content);
        rawInputTrees.emplace_back(inputTree);
    }

    return rawInputTrees;
}

newickFormatParser::inputTreeNode NewickReader::parseNewickString(string &newickString) {
//    cout << newickString << endl;
    string str = move(newickString);
    newickGrammar grammar;
    inputTreeNode tree;
    qi::phrase_parse(str.cbegin(), str.cend(), grammar, qi::space, tree);
    return tree;
}

vector<shared_ptr<InputTreeNode>> NewickReader::convertToNativeTreeFormat(
        vector<newickFormatParser::inputTreeNode> &rawInputTrees) {
    vector<shared_ptr<InputTreeNode>> inputTrees;
    int treeCount = 0;
    for(auto &rawInputTree : rawInputTrees){
        queue<inputTreeNode> q;
        q.emplace(rawInputTree);
        shared_ptr<InputTreeNode> newNode = make_shared<InputTreeNode>(rawInputTree.label_, treeCount);
        unordered_map<string, shared_ptr<InputTreeNode>> nodeMap;
        nodeMap.emplace(rawInputTree.label_, newNode);
        while(!q.empty()){
            auto currentRawNode = q.front();
            q.pop();
            if(currentRawNode.label_.substr(0,2) != "f_") taxaLabels.emplace(currentRawNode.label_);
            auto nodeIter = nodeMap.find(currentRawNode.label_);
            if(nodeIter != nodeMap.end()){
                auto currentNewNode = nodeIter->second;
                for(auto &rawChild : currentRawNode.children_){
                    shared_ptr<InputTreeNode> newChildNode = make_shared<InputTreeNode>(rawChild.label_, treeCount);
                    currentNewNode->addChild(newChildNode);
                    nodeMap.emplace(rawChild.label_, newChildNode);
                    q.emplace(rawChild);
                }
            }
        }
        treeCount++;
        inputTrees.emplace_back(newNode);
    }

    return inputTrees;
}

vector<newickFormatParser::inputTreeNode> NewickReader::replaceBlankInternalLabels(
        vector<newickFormatParser::inputTreeNode> &inputTrees) {
    vector<inputTreeNode> newInputTrees;
    for(auto &root : inputTrees){
        string newick;
        newick = convert2newick(root, newick);
//        cout << newick << endl;
        inputTreeNode newTree = parseNewickString(newick);
        newInputTrees.emplace_back(newTree);
    }
    return newInputTrees;
}

string NewickReader::convert2newick(newickFormatParser::inputTreeNode &root, string newick) {
    if(!root.children_.empty()){
        newick += "(";
        for(auto &child : root.children_) {
            newick = convert2newick(child, newick);
            newick += ",";
        }
        if(newick[newick.length()-1] == ',') newick = newick.substr(0, newick.length() - 1);
        newick += ")";
    }

    string value;
    for(int i = 0; i < root.label_.length(); ++i){
        if(root.label_[i] == ' ') continue;
        value += root.label_[i];
    }
    if(value.length() == 0 && !root.children_.empty()) value = "f_" + to_string(labelCount++);
    else value = root.label_;

    if(root.length_ > 0){
        value += ":";
        value += to_string(root.length_);
    }
    newick += value;
    return newick;
}

void NewickReader::printInputTrees(vector<newickFormatParser::inputTreeNode> &inputTrees) {
    cout << "----------- print out the raw input trees ------------" << endl;
    cout << "# of input trees: " << inputTrees.size() << endl;
    queue<inputTreeNode> q;
    int count = 1;
    for(auto &inputTree : inputTrees){
        cout << "--- tree " << count++ << endl;
        q.emplace(inputTree);
        while(!q.empty()){
            inputTreeNode current = q.front();
            q.pop();
            cout << "current label: " << current.label_ << endl;
            cout << "# of children: " << current.children_.size() << endl;
            cout << "\tchildren: ";
            for(auto &child : current.children_) {
                cout << child.label_ << " ";
                q.emplace(child);
            }
            cout << endl;
        }
    }
}

void NewickReader::printConvertedInputTrees(vector<shared_ptr<InputTreeNode>> &inputTrees) {
    cout << "----------- print out the converted input trees ------------" << endl;
    cout << "# of input trees: " << inputTrees.size() << endl;
    queue<shared_ptr<InputTreeNode>> q;
    int count = 1;
    for(auto &inputTree : inputTrees){
        cout << "--- tree " << count++ << endl;
        q.emplace(inputTree);
        while(!q.empty()){
            auto current = q.front();
            q.pop();
            cout << "current label: " << current->getNodeLabel() << endl;
            cout << "# of children: " << current->getChildVec().size() << endl;
            cout << "\tchildren: ";
            for(auto &child : current->getChildVec()) {
                cout << child->getNodeLabel() << " ";
                q.emplace(child);
            }
            cout << endl;
        }
    }
}
