//
// Created by Lei on 8/15/20.
//

#include "GetDecomposition.h"
#include <stack>

using namespace std;

GetDecomposition::GetDecomposition(unordered_map<string, unordered_set<int>> &occurMap,
                                   unordered_map<string, unordered_map<int, unordered_set<string>>> &childMap,
                                   bool enablePromotion, unordered_map<graphEdge, int, graphEdgeHash> allEdge,
                                   unordered_map<graphEdge, int, graphEdgeHash> treeEdges,
                                   unordered_map<string, unordered_map<int, unordered_set<graphEdge, graphEdgeHash>>> nonTreeEdges,
                                   unordered_map<string, unordered_set<string>> adjacentList,
                                   unordered_map<string, unordered_map<int, unordered_map<string, unordered_set<shared_ptr<TreapNode>>>>> levelLabelReferenceMap,
                                   unordered_map<string, shared_ptr<TreapNode>> activeOccurList,
                                   shared_ptr<TreapNode> &treap, int numberOfCount) :
        occurMap_(occurMap),
        childMap_(childMap),
        adjacentList_(adjacentList){
    this->hdt_ = make_shared<HDT>(enablePromotion, move(allEdge), move(treeEdges), move(nonTreeEdges),
                                  move(adjacentList), move(levelLabelReferenceMap), move(activeOccurList), treap,
                                  numberOfCount);
}

shared_ptr<GoodDecomposition>
GetDecomposition::computeDecomposition(vector<string> &position, vector<string> &parentPosition, unordered_map<shared_ptr<TreapNode>, unordered_map<int, unordered_set<string>>> gam,
                                       unordered_map<string, unordered_map<int, shared_ptr<TreapNode>>> comp, bool first) {
    // compute set S and K
    unordered_set<string> toDelete, S;
    unordered_set<int> K;
    unordered_map<string, unordered_set<int>> positionIdx;

    // takes O(k) time
    for (int i = 0; i < position.size(); ++i) {
        if (position[i].empty()) continue;
        auto occurIter = occurMap_.find(position[i]);
        if (occurIter != occurMap_.end()) {
            auto idxIter = occurIter->second.find(i);
            if (idxIter != occurIter->second.end()) {
                occurIter->second.erase(idxIter);
            }
            // when idx set is empty, then corresponding label is exposed
            if (occurIter->second.empty()) {
                S.emplace(position[i]);
                // if the label is "new"
                if (position[i] != parentPosition[i]) {
                    // the label is not deleted from hdt
                    toDelete.emplace(position[i]);
                }
            }
        }
        occurIter = positionIdx.find(position[i]);
        if (occurIter != positionIdx.end()) {
            occurIter->second.emplace(i);
        } else {
            unordered_set<int> tempSet{i};
            positionIdx.emplace(position[i], tempSet);
        }
    }

    for(auto &s : S){
        auto positionIter = positionIdx.find(s);
        for(auto &idx : positionIter->second){
            K.emplace(idx);
        }
    }

    // construct the initial state of the corresponding data structure
    unordered_map<shared_ptr<TreapNode>, unordered_map<int, unordered_set<string>>> gamma;
    unordered_map<string, unordered_map<int, shared_ptr<TreapNode>>> component;
    if(first){
        shared_ptr<TreapNode> dumInitial = make_shared<TreapNode>();
        for(int i = 0; i < position.size(); ++i){
            if(position[i].empty()) continue;
            auto childIter = childMap_.find(position[i]);
            if(childIter != childMap_.end()){
                auto idxIter = childIter->second.find(i);
                if(idxIter != childIter->second.end()){
                    for(auto &child : idxIter->second){
                        auto activeOccurIter = hdt_->levelActiveOccurMap.find(0);
                        if (activeOccurIter != hdt_->levelActiveOccurMap.end()) {
                            auto occurIter = activeOccurIter->second.find(child);
                            if (occurIter != activeOccurIter->second.end()) {
                                shared_ptr<TreapNode> root = dumInitial;
                                auto gammaIter = gamma.find(root);
                                if(gammaIter != gamma.end()){
                                    auto idx = gammaIter->second.find(i);
                                    if(idx != gammaIter->second.end()){
                                        idx->second.emplace(child);
                                    }
                                    else {
                                        unordered_set<string> tempSet{child};
                                        gammaIter->second.emplace(i, tempSet);
                                    }
                                }
                                else {
                                    unordered_set<string> tempSet{child};
                                    unordered_map<int, unordered_set<string>> tempMap{{i, tempSet}};
                                    gamma.emplace(root, tempMap);
                                }

                                auto compIter = component.find(child);
                                if(compIter != component.end()){
                                    compIter->second.emplace(i, root);
                                }
                                else {
                                    unordered_map<int, shared_ptr<TreapNode>> tempMap{{i, root}};
                                    component.emplace(child, tempMap);
                                }
                            }
                        }
                    }
                }
            }
        }
    } else {
        gamma = move(gam);
        component = move(comp);

        for(auto &labelToDelete : toDelete){
            shared_ptr<TreapNode> ptr;
            auto compIter = component.find(labelToDelete);
            if(compIter != component.end()){
                ptr = compIter->second.begin()->second;
                component.erase(compIter);
                if(compIter->second.empty()) component.erase(compIter);
            }
            auto ids = positionIdx.find(labelToDelete);
            if(ids != positionIdx.end()){
                for(auto &idx : ids->second){
                    auto gammaIter = gamma.find(ptr);
                    if(gammaIter != gamma.end()){
                        auto mapIter = gammaIter->second.find(idx);
                        if(mapIter != gammaIter->second.end()){
                            auto eleIter = mapIter->second.find(labelToDelete);
                            if(eleIter != mapIter->second.end()) mapIter->second.erase(eleIter);
                            if(mapIter->second.empty()) gammaIter->second.erase(mapIter);
                        }
                        if(gammaIter->second.empty()) gamma.erase(gammaIter);
                    }
                    auto childIter = childMap_.find(labelToDelete);
                    if(childIter != childMap_.end()){
                        auto id = childIter->second.find(idx);
                        if(id != childIter->second.end()){
                            for(auto &child : id->second){
                                compIter = component.find(child);
                                if(compIter != component.end()){
                                    auto ind = compIter->second.find(idx);
                                    if(ind != compIter->second.end()){
                                        ind->second = ptr;
                                    } else {
                                        compIter->second.emplace(idx, ptr);
                                    }
                                } else {
                                    unordered_map<int, shared_ptr<TreapNode>> tmpMap{{idx, ptr}};
                                    component.emplace(child, tmpMap);
                                }
                                gammaIter = gamma.find(ptr);
                                if(gammaIter != gamma.end()){
                                    auto mapIter = gammaIter->second.find(idx);
                                    if(mapIter != gammaIter->second.end()){
                                        mapIter->second.emplace(child);
                                    } else {
                                        unordered_set<string> tmpSet{child};
                                        gammaIter->second.emplace(idx, tmpSet);
                                    }
                                } else {
                                    unordered_set<string> tmpSet{child};
                                    unordered_map<int, unordered_set<string>> tmpMap{{idx, tmpSet}};
                                    gamma.emplace(ptr, tmpMap);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // remove corresponding edges from the display graph
    for (auto &exposedLabel : S) {
        auto neighborIter = adjacentList_.find(exposedLabel);
        if (neighborIter != adjacentList_.end()) {
            for (auto &nei : neighborIter->second) {
                // only delete edges associated with new labels that are exposed
                auto deleteIter = toDelete.find(exposedLabel);
                if (deleteIter != toDelete.end()) {
                    auto adjIter = adjacentList_.find(nei);
                    if (adjIter != adjacentList_.end()) {
                        auto ngIter = adjIter->second.find(exposedLabel);
                        if (ngIter != adjIter->second.end()) adjIter->second.erase(ngIter);
                    }
                    time_t start = clock();
                    hdt_->hdtDeleteEdge(exposedLabel, nei);
                    time_t end = clock();
                    hdtTimeDuration += end - start;

                    shared_ptr<TreapNode> root1;
                    shared_ptr<TreapNode> root2;
                    shared_ptr<TreapNode> tempNode = make_shared<TreapNode>();
                    auto activeOccurIter = hdt_->levelActiveOccurMap.find(0);
                    if(activeOccurIter != hdt_->levelActiveOccurMap.end()){
                        auto occurIter = activeOccurIter->second.find(exposedLabel);
                        if(occurIter != activeOccurIter->second.end())
                            root1 = hdt_->hdtFindRoot(occurIter->second);
                        occurIter = activeOccurIter->second.find(nei);
                        if(occurIter != activeOccurIter->second.end())
                            root2 = hdt_->hdtFindRoot(occurIter->second);
                    }
                    if(root1->size_ > root2->size_){
                        auto tmp = root1;
                        root1 = root2;
                        root2 = tmp;
                    }
                    unordered_set<string> visited;
                    stack<shared_ptr<TreapNode>> stack;
                    stack.emplace(root1);
                    while(!stack.empty()){
                        auto current = stack.top();
                        stack.pop();
                        if(current->left_ != nullptr) stack.emplace(current->left_);
                        if(current->right_ != nullptr) stack.emplace(current->right_);
                        if(visited.find(current->label_) != visited.end()) continue;
                        visited.emplace(current->label_);
                        auto compIter = component.find(current->label_);
                        if(compIter != component.end()) {
                            for(auto &entry : compIter->second){
//                                cout << current->label_ << "-->" << entry.second << " --> " << tempNode << endl;
                                auto gammaIter = gamma.find(tempNode);
                                if(gammaIter != gamma.end()){
                                    auto idx = gammaIter->second.find(entry.first);
                                    if(idx != gammaIter->second.end()){
                                        idx->second.emplace(current->label_);
                                    } else {
                                        unordered_set<string> tempSet{current->label_};
                                        gammaIter->second.emplace(entry.first, tempSet);
                                    }
                                } else {
                                    unordered_set<string> tempSet{current->label_};
                                    unordered_map<int, unordered_set<string>> tempMap{{entry.first, tempSet}};
                                    gamma.emplace(tempNode, tempMap);
                                }
                                gammaIter = gamma.find(entry.second);
                                if(gammaIter != gamma.end()){
                                    auto idx = gammaIter->second.find(entry.first);
                                    if(idx != gammaIter->second.end()){
                                        auto setIter = idx->second.find(current->label_);
                                        if(setIter != idx->second.end()) idx->second.erase(setIter);
                                        if(idx->second.empty()) gammaIter->second.erase(idx);
                                    }
                                    if(gammaIter->second.empty()) gamma.erase(gammaIter);
                                }
                                entry.second = tempNode;
                            }
                        }
                        else continue;
                    }
                }
            }
        }
    }

    unordered_map<shared_ptr<TreapNode>, unordered_map<int, unordered_set<string>>> gammaCopy = gamma;
    unordered_map<string, unordered_map<int, shared_ptr<TreapNode>>> componentCopy = component;

    string badLabel;
    while (!(badLabel = computeBadLabel(S, component)).empty()) {
        // check badLabel
        cout << "---- check bad label ----" << endl;
        cout << "current badLabel is : " << badLabel << endl;

        // badLabel stores the current bad label
        // compute set K'
        auto positionIdxIter = positionIdx.find(badLabel);
        if (positionIdxIter != positionIdx.end()) {
            auto &kPrime = positionIdxIter->second;

            // compute set Gamma'
            unordered_set<shared_ptr<TreapNode>> gammaPrime;
            for (auto &i : kPrime) {
                auto children = childMap_.find(badLabel);
                if (children != childMap_.end()) {
                    auto idxIter = children->second.find(i);
                    if (idxIter != children->second.end()) {
                        for (auto &child : idxIter->second) {
                            auto compIter = component.find(child);
                            if (compIter != component.end()) {
                                auto idx = compIter->second.find(i);
                                if (idx != compIter->second.end()) {
                                    gammaPrime.emplace(idx->second);
                                }
                            }
                        }
                    }
                }
            }

            // initialize set B
            unordered_map<int, unordered_set<string>> setB;
            shared_ptr<TreapNode> mergeRef;
            for (auto &entry : gammaPrime) {
                if (mergeRef == nullptr) mergeRef = entry;
                auto gammaIter = gamma.find(entry);
                if (gammaIter != gamma.end()) {
                    for (auto &childSet : gammaIter->second) {
                        auto idxIter = setB.find(childSet.first);
                        if (idxIter != setB.end()) {
                            for (auto &children : childSet.second) {
                                idxIter->second.emplace(children);
                                auto compIter = component.find(children);
                                if (compIter != component.end()) {
                                    auto idx = compIter->second.find(childSet.first);
                                    if (idx != compIter->second.end()) {
                                        idx->second = mergeRef;
                                    }
                                }
                            }
                        } else {
                            setB.emplace(childSet.first, childSet.second);
                            for (auto &child : childSet.second) {
                                auto compIter = component.find(child);
                                if (compIter != component.end()) {
                                    auto idx = compIter->second.find(childSet.first);
                                    if (idx != compIter->second.end()) {
                                        idx->second = mergeRef;
                                    }
                                }
                            }
                        }
                    }
                    gamma.erase(gammaIter);
                }
            }
            gamma.emplace(mergeRef, setB);
            mergeRef = nullptr;

            auto sIter = S.find(badLabel);
            if (sIter != S.end()) S.erase(sIter);

            for (auto &i : kPrime) {
                auto kIter = K.find(i);
                if (kIter != K.end()) K.erase(kIter);
            }
        }

        badLabel = "";
    }

    // initialize fancy PI
    shared_ptr<vector<vector<string>>> PI = make_shared<vector<vector<string>>>();
    shared_ptr<vector<unordered_map<shared_ptr<TreapNode>, unordered_map<int, unordered_set<string>>>>> GAMMA =
            make_shared<vector<unordered_map<shared_ptr<TreapNode>, unordered_map<int, unordered_set<string>>>>>();
    shared_ptr<vector<unordered_map<string, unordered_map<int, shared_ptr<TreapNode>>>>> COMPONENT =
            make_shared<vector<unordered_map<string, unordered_map<int, shared_ptr<TreapNode>>>>>();

    for (auto &entry : gamma) {
        vector<string> pi_A(position.size());
        for (int i = 0; i < pi_A.size(); ++i) pi_A[i] = "";

        for (int i = 0; i < pi_A.size(); ++i) {
            string parentLabel = position[i];
            auto setIter = entry.second.find(i);
            if (setIter != entry.second.end()) {
                if (S.find(parentLabel) != S.end()) pi_A[i] = *(setIter->second.begin());
                else pi_A[i] = parentLabel;
            }
        }

        unordered_map<shared_ptr<TreapNode>, unordered_map<int, unordered_set<string>>> tmpGamma;
        unordered_map<string, unordered_map<int, shared_ptr<TreapNode>>> tmpComponent;
        for(int i = 0; i < pi_A.size(); ++i){
            if(!position[i].empty() && position[i] == pi_A[i]){
                auto childIter = childMap_.find(pi_A[i]);
                if(childIter != childMap_.end()){
                    auto idx = childIter->second.find(i);
                    if(idx != childIter->second.end()){
                        for(auto &child : idx->second){
                            auto compIter = componentCopy.find(child);
                            if(compIter != componentCopy.end()){
                                auto idxIter = compIter->second.find(i);
                                if(idxIter != compIter->second.end()){
                                    auto tmpCompIter = tmpComponent.find(child);
                                    if(tmpCompIter != tmpComponent.end()){
                                        auto ind = tmpCompIter->second.find(i);
                                        if(ind != tmpCompIter->second.end()){
                                            ind->second = idxIter->second;
                                        } else {
                                            tmpCompIter->second.emplace(i, idxIter->second);
                                        }
                                    } else {
                                        unordered_map<int, shared_ptr<TreapNode>> tmpMap{{i, idxIter->second}};
                                        tmpComponent.emplace(child, tmpMap);
                                    }
                                    auto tmpGammaIter = tmpGamma.find(idxIter->second);
                                    if(tmpGammaIter != tmpGamma.end()){
                                        auto ind = tmpGammaIter->second.find(i);
                                        if(ind != tmpGammaIter->second.end()){
                                            ind->second.emplace(child);
                                        } else {
                                            unordered_set<string> tmpSet{child};
                                            tmpGammaIter->second.emplace(i, tmpSet);
                                        }
                                    } else {
                                        unordered_set<string> tmpSet{child};
                                        unordered_map<int, unordered_set<string>> tmpMap{{i, tmpSet}};
                                        tmpGamma.emplace(idxIter->second, tmpMap);
                                    }
                                }
                            }
                        }
                    }
                }
                auto compIter = componentCopy.find(pi_A[i]);
                if(compIter != componentCopy.end()){
                    auto idxIter = compIter->second.find(i);
                    if(idxIter != compIter->second.end()){
                        auto tmpCompIter = tmpComponent.find(pi_A[i]);
                        if(tmpCompIter != tmpComponent.end()){
                            auto ind = tmpCompIter->second.find(i);
                            if(ind != tmpCompIter->second.end()){
                                ind->second = idxIter->second;
                            } else {
                                tmpCompIter->second.emplace(i, idxIter->second);
                            }
                        } else {
                            unordered_map<int, shared_ptr<TreapNode>> tmpMap{{i, idxIter->second}};
                            tmpComponent.emplace(pi_A[i], tmpMap);
                        }
                        auto tmpGammaIter = tmpGamma.find(idxIter->second);
                        if(tmpGammaIter != tmpGamma.end()){
                            auto ind = tmpGammaIter->second.find(i);
                            if(ind != tmpGammaIter->second.end()){
                                ind->second.emplace(pi_A[i]);
                            } else {
                                unordered_set<string> tmpSet{pi_A[i]};
                                tmpGammaIter->second.emplace(i, tmpSet);
                            }
                        } else {
                            unordered_set<string> tmpSet{pi_A[i]};
                            unordered_map<int, unordered_set<string>> tmpMap{{i, tmpSet}};
                            tmpGamma.emplace(idxIter->second, tmpMap);
                        }
                    }
                }
            }
            else if(pi_A[i] != position[i] && !pi_A[i].empty()){
                auto compIter = componentCopy.find(pi_A[i]);
                if(compIter != componentCopy.end()){
                    auto idxIter = compIter->second.find(i);
                    if(idxIter != compIter->second.end()){
                        auto tmpCompIter = tmpComponent.find(pi_A[i]);
                        if(tmpCompIter != tmpComponent.end()){
                            auto ind = tmpCompIter->second.find(i);
                            if(ind != tmpCompIter->second.end()){
                                ind->second = idxIter->second;
                            } else {
                                tmpCompIter->second.emplace(i, idxIter->second);
                            }
                        } else {
                            unordered_map<int, shared_ptr<TreapNode>> tmpMap{{i, idxIter->second}};
                            tmpComponent.emplace(pi_A[i], tmpMap);
                        }
                        auto tmpGammaIter = tmpGamma.find(idxIter->second);
                        if(tmpGammaIter != tmpGamma.end()){
                            auto ind = tmpGammaIter->second.find(i);
                            if(ind != tmpGammaIter->second.end()){
                                ind->second.emplace(pi_A[i]);
                            } else {
                                unordered_set<string> tmpSet{pi_A[i]};
                                tmpGammaIter->second.emplace(i, tmpSet);
                            }
                        } else {
                            unordered_set<string> tmpSet{pi_A[i]};
                            unordered_map<int, unordered_set<string>> tmpMap{{i, tmpSet}};
                            tmpGamma.emplace(idxIter->second, tmpMap);
                        }
                    }
                }
            }
        }

        PI->emplace_back(pi_A);
        GAMMA->emplace_back(tmpGamma);
        COMPONENT->emplace_back(tmpComponent);
    }

    return make_shared<GoodDecomposition>(PI, S, GAMMA, COMPONENT);
}

string GetDecomposition::computeBadLabel(unordered_set<string> &S,
                                         unordered_map<string, unordered_map<int, shared_ptr<TreapNode>>> &component) {

    unordered_map<int, unordered_set<shared_ptr<TreapNode>>> appeared;
    for (auto &label : S) {
        auto neiIter = childMap_.find(label);
        if (neiIter != childMap_.end()) {
            for (auto &entry : neiIter->second) {
                int idx = entry.first;
                for (auto &child : entry.second) {
                    auto compIter = component.find(child);
                    if (compIter != component.end()) {
                        auto idxIter = compIter->second.find(idx);
                        if (idxIter != compIter->second.end()) {
                            auto appearIter = appeared.find(idx);
                            if (appearIter != appeared.end()) {
                                auto setIter = appearIter->second.find(idxIter->second);
                                if (setIter != appearIter->second.end())
                                    return label;
                                else {
                                    appearIter->second.emplace(idxIter->second);
                                }
                            } else {
                                unordered_set<shared_ptr<TreapNode>> tempSet{idxIter->second};
                                appeared.emplace(idx, tempSet);
                            }
                        }
                    }
                }
            }
        }
    }
    return "";
}

void GetDecomposition::printAdjacentList() {
    cout << "---- check adjacent list ----" << endl;
    for (auto &entry : adjacentList_) {
        cout << entry.first << " -> ";
        for (auto &ent : entry.second) {
            cout << ent << " ";
        }
        cout << endl;
    }
}