//
// This class is only used to check if the seed input tree is identical to the supertree or not
// This is only designed and used for the simulated input generator only
// Using this checker to test similarity of other trees would not give you the correct answers
//

#include "OutputChecker.h"
#include <stack>
#include <unordered_set>
#include <unordered_map>

using namespace std;

bool OutputChecker::isTreesIdentical(shared_ptr<SuperTreeNode> &output, shared_ptr<InputTreeNode> &input) {
    stack<shared_ptr<SuperTreeNode>> outStack;
    outStack.emplace(output);
    unordered_map<string, unordered_set<string>> outputAdj;
    while(!outStack.empty()){
        auto &currentNode = outStack.top();
        outStack.pop();
        string currentLabel = currentNode->label;
        unordered_set<string> tempSet;
        for(auto &child : currentNode->children) {
            outStack.emplace(child);
            tempSet.emplace(child->label.substr(0, child->label.length()-1));
        }
        outputAdj.emplace(currentLabel.substr(0, currentLabel.length()-1), tempSet);
    }

    stack<shared_ptr<InputTreeNode>> inStack;
    inStack.emplace(input);
    while(!inStack.empty()){
        auto &currentNode = inStack.top();
        inStack.pop();
        auto &children = currentNode->getChildVec();
        auto outIter = outputAdj.find(currentNode->getNodeLabel());
        if(outIter != outputAdj.end()){
            if(children.size() == outIter->second.size()){
                for(auto &child : children){
                    inStack.emplace(child);
                    auto childIter = outIter->second.find(child->getNodeLabel());
                    if(childIter != outIter->second.end()) continue;
                    else return false;
                }
            }
            else return false;
        }
        else return false;
    }

    return true;
}
