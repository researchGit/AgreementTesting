//
// Created by Lei on 7/30/20.
//

#include "HDT.h"
#include <iostream>
#include <stack>

using namespace std;

HDT::HDT(bool enablePromotion, unordered_map<graphEdge, int, graphEdgeHash> allEdge,
         unordered_map<graphEdge, int, graphEdgeHash> treeEdges,
         unordered_map<string, unordered_map<int, unordered_set<graphEdge, graphEdgeHash>>> nonTreeEdges,
         unordered_map<string, unordered_set<string>> adjacentList,
         unordered_map<string, unordered_map<int, unordered_map<string, unordered_set<shared_ptr<TreapNode>>>>> levelLabelReferenceMap,
         unordered_map<string, shared_ptr<TreapNode>> activeOccurList, shared_ptr<TreapNode> &treap,
         int numberOfCount) {
    if (enablePromotion) logN = (int) log2((double) treap->size_) + 1;

    cout << "logN is: " << logN << endl;

    this->edgeList = std::move(allEdge);
    this->treeEdges = std::move(treeEdges);
    this->nonTreeEdges = std::move(nonTreeEdges);
    this->adjacentList = std::move(adjacentList);
    this->labelReference = std::move(levelLabelReferenceMap);
    this->activeOccurList = std::move(activeOccurList);
    this->levelActiveOccurMap.emplace(0, this->activeOccurList);
    // TODO: considering general cases that the inputs are not all connected
    unordered_set<shared_ptr<TreapNode>> treapSet{treap};
    this->levelTreapMap.emplace(0, std::move(treapSet));
    this->nodeCount = numberOfCount;
    this->promotionSwitch = enablePromotion;
}

void HDT::hdtDeleteEdge(string u, string v) {
    bool isNonTreeEdge = false;
    int edgeLevel = 0;

    graphEdge edgeDelete;
    if (u < v) {
        edgeDelete.endPoint1 = u;
        edgeDelete.endPoint2 = v;
    } else {
        edgeDelete.endPoint1 = v;
        edgeDelete.endPoint2 = u;
    }

    // query level of this edge
    auto edgeIter = edgeList.find(edgeDelete);
    if (edgeIter != edgeList.end()) {
        edgeLevel = edgeIter->second;
        // firstly query if it is non-tree edge
        auto nonEdgeIter = nonTreeEdges.find(u);
        if (nonEdgeIter != nonTreeEdges.end()) {
            auto levelIter = nonEdgeIter->second.find(edgeLevel);
            if (levelIter != nonEdgeIter->second.end()) {
                auto eIter = levelIter->second.find(edgeDelete);
                if (eIter != levelIter->second.end()) {
                    // find it, delete it directly
                    levelIter->second.erase(eIter);

                    // delete another piece of info
                    nonEdgeIter = nonTreeEdges.find(v);
                    if (nonEdgeIter != nonTreeEdges.end()) {
                        levelIter = nonEdgeIter->second.find(edgeLevel);
                        if (levelIter != nonEdgeIter->second.end()) {
                            eIter = levelIter->second.find(edgeDelete);
                            if (eIter != levelIter->second.end()) {
                                levelIter->second.erase(eIter);

                                // delete this non-tree edge from edgeList
                                edgeList.erase(edgeIter);

                                // delete info from adjacent list
                                adjacencyListDeleteEdge(u, v);
                                adjacencyListDeleteEdge(v, u);

                                isNonTreeEdge = true;
                                return;
                            }
                        }
                    }
                }
            }
        }

        // when this edge is tree edge
        if (!isNonTreeEdge) {
            // firstly delete it from edgeList
            edgeList.erase(edgeIter);

            edgeIter = treeEdges.find(edgeDelete);
            if (edgeIter != treeEdges.end() && edgeLevel == edgeIter->second) {
                treeEdges.erase(edgeIter);

                // delete the edge from adjacent list
                adjacencyListDeleteEdge(u, v);
                adjacencyListDeleteEdge(v, u);
                // delete the edge from spanning trees from level edgeLevel
                for (int i = edgeLevel; i >= 0; --i) {
                    // split the spanning tree
                    hdtSplit(u, v, i);

                    auto lvIter = treapVector.find(i);
                    if (lvIter != treapVector.end()) {
                        treapVector.erase(lvIter);
                    }
                    shared_ptr<cutTreaps> uvTreaps = make_shared<cutTreaps>(treapU, treapV);
                    treapVector.emplace(i, uvTreaps);
                }

                // after delete the edge at each level
                // move on to find a replace edge
                graphEdge replacementEdge(" ", " ");
                int foundAtLevel = -1;
                for (int i = edgeLevel; i >= 0; --i) {
                    auto treapsIter = treapVector.find(i);
                    if (treapsIter != treapVector.end()) {
                        shared_ptr<cutTreaps> tempCutTreaps = treapsIter->second;
                        if (promotionSwitch) {
                            // enable edge promotions
                            // promote tree edges from the smaller spannign tree at level i to i + 1
                            if (i <= logN - 1) {
//                                cout << "here" << endl;
//                                Treap t;
//                                t.printTreap(tempCutTreaps->treap1);
//                                cout << "===========" << endl;
//                                t.printTreap(tempCutTreaps->treap2);
                                promoteTreeEdges(i, tempCutTreaps);
//                                cout << "exit here" << endl;
                            }
                        }

                        if (tempCutTreaps->treap1->size_ < tempCutTreaps->treap2->size_) {
                            replacementEdge = findReplacementEdge(tempCutTreaps->treap1, tempCutTreaps->treap2, i);
                        } else {
                            replacementEdge = findReplacementEdge(tempCutTreaps->treap2, tempCutTreaps->treap1, i);
                        }

                        if (replacementEdge.endPoint1 != " " && replacementEdge.endPoint2 != " ") {
                            foundAtLevel = i;
                            break;
                        }
                    }
                }

                if (replacementEdge.endPoint1 != " " && replacementEdge.endPoint2 != " ") {
                    // if there exist a replacement edge
                    for (int i = edgeLevel; i >= 0; --i) {
                        if (i > foundAtLevel) continue;
                        auto lvMapIter = levelActiveOccurMap.find(i);
                        if (lvMapIter != levelActiveOccurMap.end()) {
                            shared_ptr<TreapNode> root1;
                            shared_ptr<TreapNode> root2;
                            auto mapIter = lvMapIter->second.find(replacementEdge.endPoint1);
                            if (mapIter != lvMapIter->second.end()) root1 = hdtFindRoot(mapIter->second);
                            mapIter = lvMapIter->second.find(replacementEdge.endPoint2);
                            if (mapIter != lvMapIter->second.end()) root2 = hdtFindRoot(mapIter->second);

                            auto lvSetIter = levelTreapMap.find(i);
                            if (lvSetIter != levelTreapMap.end()) {
                                auto setIter = lvSetIter->second.find(root1);
                                if (setIter != lvSetIter->second.end()) lvSetIter->second.erase(setIter);
                                setIter = lvSetIter->second.find(root2);
                                if (setIter != lvSetIter->second.end()) lvSetIter->second.erase(setIter);

                                rerooting(replacementEdge.endPoint1, root1, i);
                                rerooting(replacementEdge.endPoint2, root2, i);
                                shared_ptr<TreapNode> newNode = make_shared<TreapNode>();
                                newNode->label_ = replacementEdge.endPoint1;
                                newNode->active_ = false;
                                newNode->activeChild_ = false;
                                newNode->left_ = nullptr;
                                newNode->right_ = nullptr;
                                newNode->size_ = 1;
                                newNode->priority_ = rand() / 10;
                                newNode->key_ = nodeCount++;
                                newNode->parent_ = nullptr;
                                newNode->activeOccur_ = false;

                                shared_ptr<TreapNode> piece1 = reLinkTreaps(root1, root2, i);
                                shared_ptr<TreapNode> piece2 = reLinkTreaps(piece1, newNode, i);
                                // piece2 is the final treap
                                lvSetIter->second.emplace(piece2);
                            }
                        }
                    }
                }

                treapU = nullptr;
                treapV = nullptr;

                treapVector.clear();
            }
        }
    }
    else {
        cout << "Cannot find the edge in the edge list!" << endl;
    }
}

void HDT::promoteTreeEdges(int edgeLV, shared_ptr<cutTreaps> currentCutTreaps) {
    shared_ptr<TreapNode> smallTreap =
            currentCutTreaps->treap1->size_ < currentCutTreaps->treap2->size_ ? currentCutTreaps->treap1
                                                                              : currentCutTreaps->treap2;
    unordered_set<string> existNode;
    unordered_set<graphEdge, graphEdgeHash> visitedEdge;
    stack<shared_ptr<TreapNode>> tempStack;

    // traverse the entire small treap
    // for each node encountered, create a copy of the node
    // when finish, add the new one to level i + 1, and update corresponding info
    shared_ptr<TreapNode> p = smallTreap;

    while (!tempStack.empty() || p != nullptr) {
        while (p != nullptr) {
            tempStack.push(p);
            p = p->left_;
        }

        if(!tempStack.empty()){
            p = tempStack.top();
            tempStack.pop();

            // handle the node
            auto existIter = existNode.find(p->label_);
            if (existIter == existNode.end()) {
                existNode.emplace(p->label_);

                auto adjIter = adjacentList.find(p->label_);
                if (adjIter != adjacentList.end()) {
                    for (auto adjEdgeIter = adjIter->second.begin(); adjEdgeIter != adjIter->second.end(); adjEdgeIter++) {
                        graphEdge insertEdge;
                        if (p->label_ < *adjEdgeIter) {
                            insertEdge.endPoint1 = p->label_;
                            insertEdge.endPoint2 = *adjEdgeIter;
                        } else {
                            insertEdge.endPoint1 = *adjEdgeIter;
                            insertEdge.endPoint2 = p->label_;
                        }
                        auto visitIter = visitedEdge.find(insertEdge);
                        if (visitIter == visitedEdge.end()) {
                            visitedEdge.emplace(insertEdge);

                            // update edge info in edgeList and treeEdge
                            auto treeEdgeIter = treeEdges.find(insertEdge);
                            if (treeEdgeIter != treeEdges.end()) {
                                if (treeEdgeIter->second == edgeLV) {
                                    treeEdgeIter->second += 1;

                                    treeEdgeIter = edgeList.find(insertEdge);
                                    if (treeEdgeIter != edgeList.end()) {
                                        if (treeEdgeIter->second == edgeLV) treeEdgeIter->second += 1;
                                    }

                                    auto occurLvIter = levelActiveOccurMap.find(edgeLV + 1);
                                    if (occurLvIter != levelActiveOccurMap.end()) {
                                        auto occurIter1 = occurLvIter->second.find(insertEdge.endPoint1);
                                        auto occurIter2 = occurLvIter->second.find(insertEdge.endPoint2);

                                        if (occurIter1 != occurLvIter->second.end() &&
                                            occurIter2 != occurLvIter->second.end()) {
                                            // both nodes have already been in the level i + 1
                                            shared_ptr<TreapNode> rootA = hdtFindRoot(occurIter1->second);
                                            shared_ptr<TreapNode> rootB = hdtFindRoot(occurIter2->second);
                                            auto lvTreapIter = levelTreapMap.find(edgeLV + 1);
                                            if (lvTreapIter != levelTreapMap.end()) {
                                                auto treapIter = lvTreapIter->second.find(rootA);
                                                if (treapIter != lvTreapIter->second.end())
                                                    lvTreapIter->second.erase(treapIter);
                                                treapIter = lvTreapIter->second.find(rootB);
                                                if (treapIter != lvTreapIter->second.end())
                                                    lvTreapIter->second.erase(treapIter);

                                                rerooting(insertEdge.endPoint1, rootA, edgeLV + 1);
                                                rerooting(insertEdge.endPoint2, rootB, edgeLV + 1);

                                                shared_ptr<TreapNode> piece1 = reLinkTreaps(rootA, rootB, edgeLV + 1);

                                                shared_ptr<TreapNode> newCopy = make_shared<TreapNode>();
                                                newCopy->label_ = insertEdge.endPoint1;
                                                newCopy->left_ = nullptr;
                                                newCopy->right_ = nullptr;
                                                newCopy->size_ = 1;
                                                newCopy->priority_ = rand() / 10;
                                                newCopy->key_ = nodeCount++;
                                                newCopy->parent_ = nullptr;
                                                newCopy->activeOccur_ = false;
                                                newCopy->activeChild_ = false;
                                                newCopy->active_ = false;

                                                shared_ptr<TreapNode> piece2 = reLinkTreaps(piece1, newCopy, edgeLV + 1);
                                                lvTreapIter->second.emplace(piece2);
                                            }
                                        } else if (occurIter1 != occurLvIter->second.end() &&
                                                   occurIter2 == occurLvIter->second.end()) {
                                            // only one of two nodes in the level i + 1;
                                            shared_ptr<TreapNode> rootA = hdtFindRoot(occurIter1->second);
                                            auto lvTreapIter = levelTreapMap.find(edgeLV + 1);
                                            if (lvTreapIter != levelTreapMap.end()) {
                                                auto treapIter = lvTreapIter->second.find(rootA);
                                                if (treapIter != lvTreapIter->second.end())
                                                    lvTreapIter->second.erase(treapIter);

                                                shared_ptr<TreapNode> newNode = make_shared<TreapNode>();
                                                newNode->label_ = insertEdge.endPoint2;
                                                newNode->left_ = nullptr;
                                                newNode->right_ = nullptr;
                                                newNode->size_ = 1;
                                                newNode->priority_ = rand() / 10;
                                                newNode->key_ = nodeCount++;
                                                newNode->parent_ = nullptr;
                                                newNode->activeOccur_ = true;
                                                newNode->activeChild_ = false;
                                                newNode->active_ = false;

                                                auto lvfinder = levelActiveOccurMap.find(edgeLV);
                                                if (lvfinder != levelActiveOccurMap.end()) {
                                                    auto strfinder = lvfinder->second.find(insertEdge.endPoint2);
                                                    if (strfinder != lvfinder->second.end())
                                                        newNode->active_ = strfinder->second->active_;
                                                }

                                                occurLvIter->second.emplace(newNode->label_, newNode);

                                                rerooting(insertEdge.endPoint1, rootA, edgeLV + 1);
                                                shared_ptr<TreapNode> piece1 = reLinkTreaps(rootA, newNode, edgeLV + 1);

                                                shared_ptr<TreapNode> newCopy = make_shared<TreapNode>();
                                                newCopy->label_ = insertEdge.endPoint1;
                                                newCopy->left_ = nullptr;
                                                newCopy->right_ = nullptr;
                                                newCopy->size_ = 1;
                                                newCopy->priority_ = rand() / 10;
                                                newCopy->key_ = nodeCount++;
                                                newCopy->parent_ = nullptr;
                                                newCopy->activeOccur_ = false;
                                                newCopy->activeChild_ = false;
                                                newCopy->active_ = false;

                                                shared_ptr<TreapNode> piece2 = reLinkTreaps(piece1, newCopy, edgeLV + 1);

                                                lvTreapIter->second.emplace(piece2);
                                            }
                                        } else if (occurIter1 == occurLvIter->second.end() &&
                                                   occurIter2 != occurLvIter->second.end()) {
                                            // only one of two nodes in the level i + 1
                                            shared_ptr<TreapNode> rootA = hdtFindRoot(occurIter2->second);
                                            auto lvTreapIter = levelTreapMap.find(edgeLV + 1);
                                            if (lvTreapIter != levelTreapMap.end()) {
                                                auto treapIter = lvTreapIter->second.find(rootA);
                                                if (treapIter != lvTreapIter->second.end())
                                                    lvTreapIter->second.erase(treapIter);

                                                shared_ptr<TreapNode> newNode = make_shared<TreapNode>();
                                                newNode->label_ = insertEdge.endPoint1;
                                                newNode->left_ = nullptr;
                                                newNode->right_ = nullptr;
                                                newNode->size_ = 1;
                                                newNode->priority_ = rand() / 10;
                                                newNode->key_ = nodeCount++;
                                                newNode->parent_ = nullptr;
                                                newNode->activeOccur_ = true;
                                                newNode->activeChild_ = false;
                                                newNode->active_ = false;

                                                auto lvfinder = levelActiveOccurMap.find(edgeLV);
                                                if (lvfinder != levelActiveOccurMap.end()) {
                                                    auto strfinder = lvfinder->second.find(insertEdge.endPoint1);
                                                    if (strfinder != lvfinder->second.end())
                                                        newNode->active_ = strfinder->second->active_;
                                                }

                                                occurLvIter->second.emplace(newNode->label_, newNode);

                                                rerooting(insertEdge.endPoint2, rootA, edgeLV + 1);
                                                shared_ptr<TreapNode> piece1 = reLinkTreaps(rootA, newNode, edgeLV + 1);

                                                shared_ptr<TreapNode> newCopy = make_shared<TreapNode>();
                                                newCopy->label_ = insertEdge.endPoint2;
                                                newCopy->left_ = nullptr;
                                                newCopy->right_ = nullptr;
                                                newCopy->size_ = 1;
                                                newCopy->priority_ = rand() / 10;
                                                newCopy->key_ = nodeCount++;
                                                newCopy->parent_ = nullptr;
                                                newCopy->activeOccur_ = false;
                                                newCopy->activeChild_ = false;
                                                newCopy->active_ = false;

                                                shared_ptr<TreapNode> piece2 = reLinkTreaps(piece1, newCopy, edgeLV + 1);

                                                lvTreapIter->second.emplace(piece2);
                                            }
                                        } else {
                                            // both of them are not in the level i + 1
                                            auto lvTreapIter = levelTreapMap.find(edgeLV + 1);
                                            if (lvTreapIter != levelTreapMap.end()) {
                                                shared_ptr<TreapNode> newNodeU = make_shared<TreapNode>();
                                                newNodeU->label_ = insertEdge.endPoint1;
                                                newNodeU->left_ = nullptr;
                                                newNodeU->right_ = nullptr;
                                                newNodeU->size_ = 1;
                                                newNodeU->priority_ = rand() / 10;
                                                newNodeU->key_ = nodeCount++;
                                                newNodeU->parent_ = nullptr;
                                                newNodeU->activeOccur_ = true;
                                                newNodeU->activeChild_ = false;
                                                newNodeU->active_ = false;

                                                auto lvfinder = levelActiveOccurMap.find(edgeLV);
                                                if (lvfinder != levelActiveOccurMap.end()){
                                                    auto strfinder = lvfinder->second.find(insertEdge.endPoint1);
                                                    if (strfinder != lvfinder->second.end())
                                                        newNodeU->active_ = strfinder->second->active_;
                                                }
                                                occurLvIter->second.emplace(newNodeU->label_, newNodeU);

                                                shared_ptr<TreapNode> newNodeV = make_shared<TreapNode>();
                                                newNodeV->label_ = insertEdge.endPoint2;
                                                newNodeV->left_ = nullptr;
                                                newNodeV->right_ = nullptr;
                                                newNodeV->size_ = 1;
                                                newNodeV->priority_ = rand() / 10;
                                                newNodeV->key_ = nodeCount++;
                                                newNodeV->parent_ = nullptr;
                                                newNodeV->activeOccur_ = true;
                                                newNodeV->activeChild_ = false;
                                                newNodeV->active_ = false;

                                                lvfinder = levelActiveOccurMap.find(edgeLV);
                                                if (lvfinder != levelActiveOccurMap.end()) {
                                                    auto strfinder = lvfinder->second.find(insertEdge.endPoint2);
                                                    if (strfinder != lvfinder->second.end())
                                                        newNodeV->active_ = strfinder->second->active_;
                                                }
                                                occurLvIter->second.emplace(newNodeV->label_, newNodeV);

                                                shared_ptr<TreapNode> piece1 = reLinkTreaps(newNodeU, newNodeV, edgeLV + 1);

                                                shared_ptr<TreapNode> newCopy = make_shared<TreapNode>();
                                                newCopy->label_ = insertEdge.endPoint1;
                                                newCopy->left_ = nullptr;
                                                newCopy->right_ = nullptr;
                                                newCopy->size_ = 1;
                                                newCopy->priority_ = rand() / 10;
                                                newCopy->key_ = nodeCount++;
                                                newCopy->parent_ = nullptr;
                                                newCopy->activeOccur_ = false;
                                                newCopy->activeChild_ = false;
                                                newCopy->active_ = false;

                                                shared_ptr<TreapNode> piece2 = reLinkTreaps(piece1, newCopy, edgeLV + 1);

                                                lvTreapIter->second.emplace(piece2);
                                            } else {
                                                shared_ptr<TreapNode> newNodeU = make_shared<TreapNode>();
                                                newNodeU->label_ = insertEdge.endPoint1;
                                                newNodeU->left_ = nullptr;
                                                newNodeU->right_ = nullptr;
                                                newNodeU->size_ = 1;
                                                newNodeU->priority_ = rand() / 10;
                                                newNodeU->key_ = nodeCount++;
                                                newNodeU->parent_ = nullptr;
                                                newNodeU->activeOccur_ = true;
                                                newNodeU->activeChild_ = false;
                                                newNodeU->active_ = false;

                                                auto lvfinder = levelActiveOccurMap.find(edgeLV);
                                                if (lvfinder != levelActiveOccurMap.end()) {
                                                    auto strfinder = lvfinder->second.find(insertEdge.endPoint1);
                                                    if (strfinder != lvfinder->second.end())
                                                        newNodeU->active_ = strfinder->second->active_;
                                                }
                                                occurLvIter->second.emplace(newNodeU->label_, newNodeU);

                                                shared_ptr<TreapNode> newNodeV = make_shared<TreapNode>();
                                                newNodeV->label_ = insertEdge.endPoint2;
                                                newNodeV->left_ = nullptr;
                                                newNodeV->right_ = nullptr;
                                                newNodeV->size_ = 1;
                                                newNodeV->priority_ = rand() / 10;
                                                newNodeV->key_ = nodeCount++;
                                                newNodeV->parent_ = nullptr;
                                                newNodeV->activeOccur_ = true;
                                                newNodeV->activeChild_ = false;
                                                newNodeV->active_ = false;

                                                lvfinder = levelActiveOccurMap.find(edgeLV);
                                                if (lvfinder != levelActiveOccurMap.end()) {
                                                    auto strfinder = lvfinder->second.find(insertEdge.endPoint2);
                                                    if (strfinder != lvfinder->second.end())
                                                        newNodeV->active_ = strfinder->second->active_;
                                                }
                                                occurLvIter->second.emplace(newNodeV->label_, newNodeV);

                                                shared_ptr<TreapNode> piece1 = reLinkTreaps(newNodeU, newNodeV, edgeLV + 1);

                                                shared_ptr<TreapNode> newCopy = make_shared<TreapNode>();
                                                newCopy->label_ = insertEdge.endPoint1;
                                                newCopy->left_ = nullptr;
                                                newCopy->right_ = nullptr;
                                                newCopy->size_ = 1;
                                                newCopy->priority_ = rand() / 10;
                                                newCopy->key_ = nodeCount++;
                                                newCopy->parent_ = nullptr;
                                                newCopy->activeOccur_ = false;
                                                newCopy->activeChild_ = false;
                                                newCopy->active_ = false;

                                                shared_ptr<TreapNode> piece2 = reLinkTreaps(piece1, newCopy, edgeLV + 1);
                                                unordered_set<shared_ptr<TreapNode>> tempSet;
                                                tempSet.emplace(piece2);
                                                levelTreapMap.emplace(edgeLV + 1, tempSet);
                                            }
                                        }
                                    } else {
                                        // this level i + 1 does not exist
                                        auto lvTreapIter = levelTreapMap.find(edgeLV + 1);
                                        if (lvTreapIter != levelTreapMap.end()) {
                                            shared_ptr<TreapNode> newNodeU = make_shared<TreapNode>();
                                            newNodeU->label_ = insertEdge.endPoint1;
                                            newNodeU->left_ = nullptr;
                                            newNodeU->right_ = nullptr;
                                            newNodeU->size_ = 1;
                                            newNodeU->priority_ = rand() / 10;
                                            newNodeU->key_ = nodeCount++;
                                            newNodeU->parent_ = nullptr;
                                            newNodeU->activeOccur_ = true;
                                            newNodeU->activeChild_ = false;
                                            newNodeU->active_ = false;

                                            auto lvfinder = levelActiveOccurMap.find(edgeLV);
                                            if (lvfinder != levelActiveOccurMap.end()) {
                                                auto strfinder = lvfinder->second.find(insertEdge.endPoint1);
                                                if (strfinder != lvfinder->second.end())
                                                    newNodeU->active_ = strfinder->second->active_;
                                            }
                                            unordered_map<string, shared_ptr<TreapNode>> tempOccur;
                                            tempOccur.emplace(newNodeU->label_, newNodeU);

                                            shared_ptr<TreapNode> newNodeV = make_shared<TreapNode>();
                                            newNodeV->label_ = insertEdge.endPoint2;
                                            newNodeV->left_ = nullptr;
                                            newNodeV->right_ = nullptr;
                                            newNodeV->size_ = 1;
                                            newNodeV->priority_ = rand() / 10;
                                            newNodeV->key_ = nodeCount++;
                                            newNodeV->parent_ = nullptr;
                                            newNodeV->activeOccur_ = true;
                                            newNodeV->activeChild_ = false;
                                            newNodeV->active_ = false;

                                            lvfinder = levelActiveOccurMap.find(edgeLV);
                                            if (lvfinder != levelActiveOccurMap.end()) {
                                                auto strfinder = lvfinder->second.find(insertEdge.endPoint2);
                                                if (strfinder != lvfinder->second.end())
                                                    newNodeV->active_ = strfinder->second->active_;
                                            }
                                            tempOccur.emplace(newNodeV->label_, newNodeV);
                                            levelActiveOccurMap.emplace(edgeLV + 1, tempOccur);

                                            shared_ptr<TreapNode> piece1 = reLinkTreaps(newNodeU, newNodeV, edgeLV + 1);

                                            shared_ptr<TreapNode> newCopy = make_shared<TreapNode>();
                                            newCopy->label_ = insertEdge.endPoint1;
                                            newCopy->left_ = nullptr;
                                            newCopy->right_ = nullptr;
                                            newCopy->size_ = 1;
                                            newCopy->priority_ = rand() / 10;
                                            newCopy->key_ = nodeCount++;
                                            newCopy->parent_ = nullptr;
                                            newCopy->activeOccur_ = false;
                                            newCopy->activeChild_ = false;
                                            newCopy->active_ = false;

                                            shared_ptr<TreapNode> piece2 = reLinkTreaps(piece1, newCopy, edgeLV + 1);

                                            lvTreapIter->second.emplace(piece2);
                                        } else {
                                            shared_ptr<TreapNode> newNodeU = make_shared<TreapNode>();
                                            newNodeU->label_ = insertEdge.endPoint1;
                                            newNodeU->left_ = nullptr;
                                            newNodeU->right_ = nullptr;
                                            newNodeU->size_ = 1;
                                            newNodeU->priority_ = rand() / 10;
                                            newNodeU->key_ = nodeCount++;
                                            newNodeU->parent_ = nullptr;
                                            newNodeU->activeOccur_ = true;
                                            newNodeU->activeChild_ = false;
                                            newNodeU->active_ = false;

                                            auto lvfinder = levelActiveOccurMap.find(edgeLV);
                                            if (lvfinder != levelActiveOccurMap.end()) {
                                                auto strfinder = lvfinder->second.find(insertEdge.endPoint1);
                                                if (strfinder != lvfinder->second.end())
                                                    newNodeU->active_ = strfinder->second->active_;
                                            }
                                            unordered_map<string, shared_ptr<TreapNode>> tempOccur;
                                            tempOccur.emplace(newNodeU->label_, newNodeU);

                                            shared_ptr<TreapNode> newNodeV = make_shared<TreapNode>();
                                            newNodeV->label_ = insertEdge.endPoint2;
                                            newNodeV->left_ = nullptr;
                                            newNodeV->right_ = nullptr;
                                            newNodeV->size_ = 1;
                                            newNodeV->priority_ = rand() / 10;
                                            newNodeV->key_ = nodeCount++;
                                            newNodeV->parent_ = nullptr;
                                            newNodeV->activeOccur_ = true;
                                            newNodeV->activeChild_ = false;
                                            newNodeV->active_ = false;

                                            lvfinder = levelActiveOccurMap.find(edgeLV);
                                            if (lvfinder != levelActiveOccurMap.end()) {
                                                auto strfinder = lvfinder->second.find(insertEdge.endPoint2);
                                                if (strfinder != lvfinder->second.end())
                                                    newNodeV->active_ = strfinder->second->active_;
                                            }
                                            tempOccur.emplace(newNodeV->label_, newNodeV);
                                            levelActiveOccurMap.emplace(edgeLV + 1, tempOccur);

                                            shared_ptr<TreapNode> piece1 = reLinkTreaps(newNodeU, newNodeV, edgeLV + 1);

                                            shared_ptr<TreapNode> newCopy = make_shared<TreapNode>();
                                            newCopy->label_ = insertEdge.endPoint1;
                                            newCopy->left_ = nullptr;
                                            newCopy->right_ = nullptr;
                                            newCopy->size_ = 1;
                                            newCopy->priority_ = rand() / 10;
                                            newCopy->key_ = nodeCount++;
                                            newCopy->parent_ = nullptr;
                                            newCopy->activeOccur_ = false;
                                            newCopy->activeChild_ = false;
                                            newCopy->active_ = false;

                                            shared_ptr<TreapNode> piece2 = reLinkTreaps(piece1, newCopy, edgeLV + 1);

                                            unordered_set<shared_ptr<TreapNode>> tempSet;
                                            tempSet.emplace(piece2);
                                            levelTreapMap.emplace(edgeLV + 1, tempSet);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            p = p->right_;
        }
    }
}

void HDT::rerooting(string u, shared_ptr<TreapNode> &tree, int edgeLV) {
    if(tree->left_ == nullptr && tree->right_ == nullptr) return;
    if(u == findLeftmostNode(tree)->label_) return;
    // if u is not the root of the euler tour(not starting at u)
    shared_ptr<TreapNode> uOccur;
    auto lvMapIter = levelActiveOccurMap.find(edgeLV);
    if(lvMapIter != levelActiveOccurMap.end()){
        auto occurIter = lvMapIter->second.find(u);
        if(occurIter != lvMapIter->second.end()) uOccur = occurIter->second;
    }

    shared_ptr<TreapNode> piece1;
    shared_ptr<TreapNode> piece2;
    shared_ptr<TreapNode> mergeTreap;

    // as u is not the leftmost node in the treap, then cut the treap at the left side of the node
    hdtCutTreapAtLeftPosition(tree, uOccur, piece1, piece2);

    shared_ptr<TreapNode> rightmostOfPiece1 = findRightmostNode(piece1);
    shared_ptr<TreapNode> leftmostOfPiece2= findLeftmostNode(piece2);

    // delete the adjacent reference
    auto refIter = labelReference.find(leftmostOfPiece2->label_);
    if(refIter != labelReference.end()){
        auto levelIter = refIter->second.find(edgeLV);
        if(levelIter != refIter->second.end()){
            auto eIter = levelIter->second.find(rightmostOfPiece1->label_);
            if(eIter != levelIter->second.end()) {
                auto setIter = eIter->second.find(rightmostOfPiece1);
                if(setIter != eIter->second.end()) eIter->second.erase(setIter);
            }
        }
    }

    refIter = labelReference.find(rightmostOfPiece1->label_);
    if(refIter != labelReference.end()){
        auto levelIter = refIter->second.find(edgeLV);
        if(levelIter != refIter->second.end()){
            auto eIter = levelIter->second.find(leftmostOfPiece2->label_);
            if(eIter != levelIter->second.end()) {
                if(eIter->second.size() > 1){
                    auto setIter = eIter->second.find(leftmostOfPiece2);
                    if(setIter != eIter->second.end()) eIter->second.erase(setIter);
                }
            }
        }
    }

    // delete the leftmost node in piece1
    // if the piece1 only has one node, then discard it directly, if not, then remove the leftmost node from it
    if(piece1->size_ == 1){
        if(piece1->activeOccur_){
            shared_ptr<TreapNode> rightmostOfPiece2 = findRightmostNode(piece2);
            rightmostOfPiece2->activeOccur_ = true;
            // update activeOccur info
            auto lvIter = levelActiveOccurMap.find(edgeLV);
            if(lvIter != levelActiveOccurMap.end()) {
                auto eIter = lvIter->second.find(rightmostOfPiece2->label_);
                if(eIter != lvIter->second.end()) eIter->second = rightmostOfPiece2;
            }
            if(piece1->active_){
                rightmostOfPiece2->active_ = true;
                // back to the root to update activeChild info
                for(shared_ptr<TreapNode> p = rightmostOfPiece2; p != nullptr; p = p->parent_){
                    if(p->parent_ != nullptr) p->parent_->activeChild_ = true;
                }
            }
        }
        mergeTreap = piece2;
    }
    else {
        // piece1 contains more than 1 nodes
        shared_ptr<TreapNode> subPiece1;
        shared_ptr<TreapNode> subPiece2;
        shared_ptr<TreapNode> leftmostOfPiece1 = findLeftmostNode(piece1);
        hdtCutTreapAtRightPosition(piece1, leftmostOfPiece1, subPiece1, subPiece2);

        if(subPiece1->activeOccur_){
            shared_ptr<TreapNode> rightmostOfPiece2 = findRightmostNode(piece2);
            rightmostOfPiece2->activeOccur_ = true;
            // update activeOccur info
            auto lvIter = levelActiveOccurMap.find(edgeLV);
            if(lvIter != levelActiveOccurMap.end()){
                auto eIter = lvIter->second.find(rightmostOfPiece2->label_);
                if(eIter != lvIter->second.end()) eIter->second = rightmostOfPiece2;
            }
            if(subPiece1->active_){
                rightmostOfPiece2->active_ = true;
                for(shared_ptr<TreapNode> p = rightmostOfPiece2; p != nullptr; p = p->parent_){
                    if(p->parent_ != nullptr) p->parent_->activeChild_ = true;
                }
            }
        }

        shared_ptr<TreapNode> lmNode = findLeftmostNode(subPiece2);
        // delete the adjacent pointers
        auto referenceIter = labelReference.find(subPiece1->label_);
        if(referenceIter != labelReference.end()){
            auto levelIter = referenceIter->second.find(edgeLV);
            if(levelIter != referenceIter->second.end()){
                auto eIter = levelIter->second.find(lmNode->label_);
                if(eIter != levelIter->second.end()){
                    auto setIter = eIter->second.find(lmNode);
                    if(setIter != eIter->second.end()) eIter->second.erase(setIter);
                }
            }
        }

        referenceIter = labelReference.find(lmNode->label_);
        if(referenceIter != labelReference.end()){
            auto levelIter = referenceIter->second.find(edgeLV);
            if(levelIter != referenceIter->second.end()){
                auto eIter = levelIter->second.find(subPiece1->label_);
                if(eIter != levelIter->second.end()){
                    auto setIter = eIter->second.find(subPiece1);
                    if(setIter != eIter->second.end()) eIter->second.erase(setIter);
                }
            }
        }

        // reconnect piece2 and subpiece2
        mergeTreap = reLinkTreaps(piece2, subPiece2, edgeLV);
    }
    shared_ptr<TreapNode> newNode = make_shared<TreapNode>();
    newNode->size_ = 1;
    newNode->label_ = leftmostOfPiece2->label_;
    newNode->active_ = false;
    newNode->activeOccur_ = false;
    newNode->activeChild_ = false;
    newNode->parent_ = nullptr;
    newNode->key_ = nodeCount++;
    newNode->priority_ = rand() / 10;
    newNode->left_ = nullptr;
    newNode->right_ = nullptr;

    tree = reLinkTreaps(mergeTreap, newNode, edgeLV);
}

void HDT::adjacencyListDeleteEdge(string u, string v) {
    auto adjIter = adjacentList.find(u);
    if (adjIter != adjacentList.end()) {
        auto setIter = adjIter->second.find(v);
        if (setIter != adjIter->second.end()) {
            adjIter->second.erase(setIter);
        }
    }
}

void HDT::hdtCutTreap(string u, string v, shared_ptr<TreapNode> &treap, int edgeLV) {
    shared_ptr<TreapNode> occurPointer1;
    shared_ptr<TreapNode> occurPointer2;
    shared_ptr<TreapNode> occurPointer3;
    shared_ptr<TreapNode> occurPointer4;

    auto edgeIter = labelReference.find(u);
    if (edgeIter != labelReference.end()) {
        auto lvIter = edgeIter->second.find(edgeLV);
        if (lvIter != edgeIter->second.end()) {
            auto endIter = lvIter->second.find(v);
            if (endIter != lvIter->second.end()) {
                auto setIter = endIter->second.begin();
                if (setIter != endIter->second.end()) {
                    if (endIter->second.size() == 1) {
                        occurPointer1 = *setIter;
                        occurPointer2 = nullptr;
                    } else if (endIter->second.size() == 2) {
                        occurPointer1 = *(setIter++);
                        occurPointer2 = *setIter;
                    }
                }
            }
        }
    }

    edgeIter = labelReference.find(v);
    if (edgeIter != labelReference.end()) {
        auto lvIter = edgeIter->second.find(edgeLV);
        if (lvIter != edgeIter->second.end()) {
            auto endIter = lvIter->second.find(u);
            if (endIter != lvIter->second.end()) {
                auto setIter = endIter->second.begin();
                if (setIter != endIter->second.end()) {
                    if (endIter->second.size() == 1) {
                        occurPointer3 = *setIter;
                        occurPointer4 = nullptr;
                    } else if (endIter->second.size() == 2) {
                        occurPointer3 = *(setIter++);
                        occurPointer4 = *setIter;
                    }
                }
            }
        }
    }

    shared_ptr<TreapNode> tempForCompare = nullptr;
    // make sure u1 < u2
    if (occurPointer1 != nullptr && occurPointer2 != nullptr) {
        if (isFirstOccurSmallerThanSecondOccur(occurPointer2, occurPointer1)) {
            tempForCompare = occurPointer2;
            occurPointer2 = occurPointer1;
            occurPointer1 = tempForCompare;
//            cout << "here" << endl;
        }
    }

    // make sure v1 < v2
    if (occurPointer3 != nullptr && occurPointer4 != nullptr) {
        if (isFirstOccurSmallerThanSecondOccur(occurPointer4, occurPointer3)) {
            tempForCompare = occurPointer4;
            occurPointer4 = occurPointer3;
            occurPointer3 = tempForCompare;
        }
    }

    // u1 < u2, v1 < v2
    if (occurPointer1 != nullptr && occurPointer3 != nullptr) {
        if (isFirstOccurSmallerThanSecondOccur(occurPointer3, occurPointer1)) {
            tempForCompare = occurPointer3;
            occurPointer3 = occurPointer1;
            occurPointer1 = tempForCompare;

            tempForCompare = occurPointer3;
            occurPointer3 = occurPointer2;
            occurPointer2 = tempForCompare;
        } else {
            tempForCompare = occurPointer3;
            occurPointer3 = occurPointer2;
            occurPointer2 = tempForCompare;

            tempForCompare = occurPointer4;
            occurPointer4 = occurPointer3;
            occurPointer3 = tempForCompare;
        }
    }

    shared_ptr<TreapNode> piece1;
    shared_ptr<TreapNode> piece2;
    shared_ptr<TreapNode> piece3;
    shared_ptr<TreapNode> piece4;

    // delete the adjacent pointers
    auto referenceIter = labelReference.find(occurPointer1->label_);
    if (referenceIter != labelReference.end()) {
        auto levelIter = referenceIter->second.find(edgeLV);
        if (levelIter != referenceIter->second.end()) {
            auto eIter = levelIter->second.find(occurPointer2->label_);
            if (eIter != levelIter->second.end()) {
                auto setIter = eIter->second.find(occurPointer2);
                if (setIter != eIter->second.end())
                    eIter->second.erase(setIter);
            }
        }
    }

    referenceIter = labelReference.find(occurPointer2->label_);
    if (referenceIter != labelReference.end()) {
        auto levelIter = referenceIter->second.find(edgeLV);
        if (levelIter != referenceIter->second.end()) {
            auto eIter = levelIter->second.find(occurPointer1->label_);
            if (eIter != levelIter->second.end()) {
                auto setIter = eIter->second.find(occurPointer1);
                if (setIter == eIter->second.end())
                    eIter->second.erase(setIter);
            }
        }
    }

    hdtCutTreapAtRightPosition(treap, occurPointer1, piece1, piece2);

    if (occurPointer3 != nullptr) {
        auto refIter = labelReference.find(occurPointer3->label_);
        if (refIter != labelReference.end()) {
            auto levelIter = refIter->second.find(edgeLV);
            if (levelIter != refIter->second.end()) {
                auto eIter = levelIter->second.find(occurPointer4->label_);
                if (eIter != levelIter->second.end()) {
                    auto setIter = eIter->second.find(occurPointer4);
                    if (setIter != eIter->second.end()) eIter->second.erase(setIter);
                }
            }
        }

        refIter = labelReference.find(occurPointer4->label_);
        if (refIter != labelReference.end()) {
            auto levelIter = refIter->second.find(edgeLV);
            if (levelIter != refIter->second.end()) {
                auto eIter = levelIter->second.find(occurPointer3->label_);
                if (eIter != levelIter->second.end()) {
                    auto setIter = eIter->second.find(occurPointer3);
                    if (setIter != eIter->second.end()) eIter->second.erase(setIter);
                }
            }
        }
    } else {
        auto refIter = labelReference.find(occurPointer2->label_);
        if (refIter != labelReference.end()) {
            auto levelIter = refIter->second.find(edgeLV);
            if (levelIter != refIter->second.end()) {
                auto eIter = levelIter->second.find(occurPointer4->label_);
                if (eIter != levelIter->second.end()) {
                    auto setIter = eIter->second.find(occurPointer4);
                    if (setIter != eIter->second.end()) eIter->second.erase(setIter);
                }
            }
        }

        refIter = labelReference.find(occurPointer4->label_);
        if (refIter != labelReference.end()) {
            auto levelIter = refIter->second.find(edgeLV);
            if (levelIter != refIter->second.end()) {
                auto eIter = levelIter->second.find(occurPointer2->label_);
                if (eIter != levelIter->second.end()) {
                    auto setIter = eIter->second.find(occurPointer2);
                    if (setIter != eIter->second.end()) eIter->second.erase(setIter);
                }
            }
        }
    }

    hdtCutTreapAtLeftPosition(piece2, occurPointer4, piece3, piece4);

    //piece1 is the leftmost piece, piece3 is the middle one, piece4 is the rightmost one
    //the rightmost node of piece1 is the same as the leftmost node of piece4

    //then delete the leftmost node from the treap piece4 and relink pieces1 and piece4
    //return the relinked piece and piece3 as the resulted treaps

    //look at the piece4, if the treap contains only 1 node, then it must be the same as the rightmost node in piece1
    //if the size of treap piece4 is greater than 1, then cut the treap piece4 at the right position of the leftmost node
    //one of resulted pieces contains that node and needs to be deleted, while another resulting piece will be returned as
    //part of the treap. Then relink two pieces
    if (piece4->size_ == 1) {
        // piece4 contains only one single node
        if (piece4->activeOccur_) {
            shared_ptr<TreapNode> rightmostNode = findRightmostNode(piece1);
            rightmostNode->activeOccur_ = true;
            // udpate activeOccur info
            auto lvIter = levelActiveOccurMap.find(edgeLV);
            if (lvIter != levelActiveOccurMap.end()) {
                auto eIter = lvIter->second.find(rightmostNode->label_);
                if (eIter != lvIter->second.end()) eIter->second = rightmostNode;
            }

            if (piece4->active_) {
                rightmostNode->active_ = true;
                // log(n) time to go back all the way to the root to update activeChild info
                for (auto &p = rightmostNode; p != nullptr; p = p->parent_) {
                    if (p->parent_ != nullptr) p->parent_->activeChild_ = true;
                }
            }
        }

        treapU = piece1;
        treapV = piece3;
    } else {
        // piece4 contains more than 1 single node
        shared_ptr<TreapNode> leftmostNode = findLeftmostNode(piece4);
        shared_ptr<TreapNode> subPiece1;
        shared_ptr<TreapNode> subPiece2;

        hdtCutTreapAtRightPosition(piece4, leftmostNode, subPiece1, subPiece2);

        if (subPiece1->activeOccur_) {
            shared_ptr<TreapNode> rightmostNode = findRightmostNode(piece1);
            rightmostNode->activeOccur_ = true;
            // update activeOccur info
            auto lvIter = levelActiveOccurMap.find(edgeLV);
            if (lvIter != levelActiveOccurMap.end()) {
                auto eIter = lvIter->second.find(rightmostNode->label_);
                if (eIter != lvIter->second.end()) eIter->second = rightmostNode;
            }

            if (subPiece1->active_) {
                rightmostNode->active_ = true;
                for (auto &p = rightmostNode; p != nullptr; p = p->parent_) {
                    if (p->parent_ != nullptr) p->parent_->activeChild_ = true;
                }
            }
        }

        shared_ptr<TreapNode> lmNode = findLeftmostNode(subPiece2);

        auto refIter = labelReference.find(subPiece1->label_);
        if (refIter != labelReference.end()) {
            auto levelIter = refIter->second.find(edgeLV);
            if (levelIter != refIter->second.end()) {
                auto eIter = levelIter->second.find(lmNode->label_);
                if (eIter != levelIter->second.end()) {
                    auto setIter = eIter->second.find(lmNode);
                    if (setIter != eIter->second.end()) eIter->second.erase(setIter);
                }
            }
        }

        refIter = labelReference.find(lmNode->label_);
        if (refIter != labelReference.end()) {
            auto levelIter = refIter->second.find(edgeLV);
            if (levelIter != refIter->second.end()) {
                auto eIter = levelIter->second.find(subPiece1->label_);
                if (eIter != levelIter->second.end()) {
                    auto setIter = eIter->second.find(subPiece1);
                    if (setIter != eIter->second.end()) eIter->second.erase(setIter);
                }
            }
        }

        // reconnect piece1 and subpiece2
        treapU = reLinkTreaps(piece1, subPiece2, edgeLV);
        treapV = piece3;
    }
}

shared_ptr<TreapNode> HDT::reLinkTreaps(shared_ptr<TreapNode> treap1, shared_ptr<TreapNode> treap2, int edgeLV) {
    if (treap1 == nullptr && treap2 == nullptr) return nullptr;
    if (treap2 == nullptr && treap1 != nullptr) return treap1;
    if (treap1 == nullptr && treap2 != nullptr) return treap2;

    // initialize a dummy node
    shared_ptr<TreapNode> dummyNode = make_shared<TreapNode>();
    dummyNode->parent_ = nullptr;
    dummyNode->left_ = treap1;
    dummyNode->right_ = treap2;
    dummyNode->size_ = treap1->size_ + treap2->size_ + 1;
    dummyNode->active_ = false;
    dummyNode->activeOccur_ = false;
    dummyNode->activeChild_ = (treap1->activeChild_ || treap2->activeChild_ || treap1->active_ || treap2->active_);
    treap1->parent_ = dummyNode;
    treap2->parent_ = dummyNode;

    shared_ptr<TreapNode> leftmostTreap2 = findLeftmostNode(treap2);
    shared_ptr<TreapNode> rightmostTreap1 = findRightmostNode(treap1);

    // update adjacent pointers
    auto refIter = labelReference.find(rightmostTreap1->label_);
    if (refIter != labelReference.end()) {
        auto lvIter = refIter->second.find(edgeLV);
        if (lvIter != refIter->second.end()) {
            auto eIter = lvIter->second.find(leftmostTreap2->label_);
            if (eIter != lvIter->second.end()) eIter->second.emplace(leftmostTreap2);
            else {
                unordered_set<shared_ptr<TreapNode>> tempSet;
                tempSet.emplace(leftmostTreap2);
                lvIter->second.emplace(leftmostTreap2->label_, tempSet);
            }
        } else {
            unordered_set<shared_ptr<TreapNode>> tempSet;
            tempSet.emplace(leftmostTreap2);
            unordered_map<string, unordered_set<shared_ptr<TreapNode>>> tempMap;
            tempMap.emplace(leftmostTreap2->label_, tempSet);
            refIter->second.emplace(edgeLV, tempMap);
        }
    } else {
        unordered_set<shared_ptr<TreapNode>> tempSet;
        tempSet.emplace(leftmostTreap2);
        unordered_map<string, unordered_set<shared_ptr<TreapNode>>> tempMap;
        tempMap.emplace(leftmostTreap2->label_, tempSet);
        unordered_map<int, unordered_map<string, unordered_set<shared_ptr<TreapNode>>>> lvMap;
        lvMap.emplace(edgeLV, tempMap);
        labelReference.emplace(rightmostTreap1->label_, lvMap);
    }

    refIter = labelReference.find(leftmostTreap2->label_);
    if (refIter != labelReference.end()) {
        auto lvIter = refIter->second.find(edgeLV);
        if (lvIter != refIter->second.end()) {
            auto eIter = lvIter->second.find(rightmostTreap1->label_);
            if (eIter != lvIter->second.end()) eIter->second.emplace(rightmostTreap1);
            else {
                unordered_set<shared_ptr<TreapNode>> tempSet;
                tempSet.emplace(rightmostTreap1);
                lvIter->second.emplace(rightmostTreap1->label_, tempSet);
            }
        } else {
            unordered_set<shared_ptr<TreapNode>> tempSet;
            tempSet.emplace(rightmostTreap1);
            unordered_map<string, unordered_set<shared_ptr<TreapNode>>> tempMap;
            tempMap.emplace(rightmostTreap1->label_, tempSet);
            refIter->second.emplace(edgeLV, tempMap);
        }
    } else {
        unordered_set<shared_ptr<TreapNode>> tempSet;
        tempSet.emplace(rightmostTreap1);
        unordered_map<string, unordered_set<shared_ptr<TreapNode>>> tempMap;
        tempMap.emplace(rightmostTreap1->label_, tempSet);
        unordered_map<int, unordered_map<string, unordered_set<shared_ptr<TreapNode>>>> lvMap;
        lvMap.emplace(edgeLV, tempMap);
        labelReference.emplace(leftmostTreap2->label_, lvMap);
    }

    while (dummyNode->left_ != nullptr || dummyNode->right_ != nullptr) {
        if (dummyNode->left_ != nullptr && dummyNode->right_ != nullptr) {
            // left child and right both exist
            if (dummyNode->left_->priority_ > dummyNode->right_->priority_) {
                rightRotate(dummyNode);
                dummyNode->size_ -= 1;
                dummyNode = dummyNode->left_;
            } else {
                leftRotate(dummyNode);
                dummyNode->size_ -= 1;
                dummyNode = dummyNode->right_;
            }
        } else if (dummyNode->left_ != nullptr && dummyNode->right_ == nullptr) {
            leftRotate(dummyNode);
            dummyNode->size_ -= 1;
            dummyNode = dummyNode->right_;
        } else if (dummyNode->right_ != nullptr && dummyNode->left_ == nullptr) {
            rightRotate(dummyNode);
            dummyNode->size_ -= 1;
            dummyNode = dummyNode->left_;
        }
    }

    if (dummyNode->parent_->left_ == dummyNode) {
        dummyNode->parent_->left_ = nullptr;
        dummyNode->parent_ = nullptr;
    } else if (dummyNode->parent_->right_ == dummyNode) {
        dummyNode->parent_->right_ = nullptr;
        dummyNode->parent_ = nullptr;
    }

    if (treap1->parent_ == nullptr) return treap1;
    else return treap2;
}

shared_ptr<TreapNode> HDT::findRightmostNode(shared_ptr<TreapNode> &tree) {
    shared_ptr<TreapNode> right;
    for (shared_ptr<TreapNode> cursor = tree; cursor != nullptr; cursor = cursor->right_)
        right = cursor;
    return right;
}

shared_ptr<TreapNode> HDT::findLeftmostNode(shared_ptr<TreapNode> &tree) {
    shared_ptr<TreapNode> left;
    for (shared_ptr<TreapNode> cursor = tree; cursor != nullptr; cursor = cursor->left_)
        left = cursor;
    return left;
}

bool HDT::isFirstOccurSmallerThanSecondOccur(shared_ptr<TreapNode> &occurA, shared_ptr<TreapNode> &occurB) {
    if (occurA == nullptr || occurB == nullptr) return false;
    if (occurA == occurB) return false;

    shared_ptr<TreapNode> temp_first = occurA;
    shared_ptr<TreapNode> temp_second = occurB;

    int dist_first = computeLengthOfPathToRoot(temp_first);
    auto first_sequence = computePathToRoot(dist_first, occurA);

    int dist_second = computeLengthOfPathToRoot(temp_second);
    auto second_sequence = computePathToRoot(dist_second, occurB);

    int mark = 0;
    for (int i = 0; (i < dist_first) && (i < dist_second); ++i) {
        if ((*first_sequence)[i] == (*second_sequence)[i])
            mark++;
        else break;
    }

    bool flag = false;
    if (mark < dist_first) {
        if ((*first_sequence)[mark] == (*first_sequence)[mark]->parent_->left_) flag = true;
    } else {
        if (mark < dist_second) {
            if ((*second_sequence)[mark] == (*second_sequence)[mark]->parent_->right_) flag = true;
        }
    }
    return flag;
}

int HDT::computeLengthOfPathToRoot(shared_ptr<TreapNode> &currentNode) {
    int length = 0;
    for (shared_ptr<TreapNode> temp = currentNode; temp->parent_ != nullptr;) {
        temp = temp->parent_;
        length++;
    }
    return length;
}

shared_ptr<vector<shared_ptr<TreapNode>>> HDT::computePathToRoot(int pathLength, shared_ptr<TreapNode> &currentNode) {
    shared_ptr<vector<shared_ptr<TreapNode>>> pathSequence = make_shared<vector<shared_ptr<TreapNode>>>(
            (unsigned long) pathLength);
    int tempIndex = pathLength;
    for (shared_ptr<TreapNode> temp = currentNode; temp->parent_ != nullptr; temp = temp->parent_)
        (*pathSequence)[--tempIndex] = temp;
    return pathSequence;
}

void HDT::hdtSplit(string u, string v, int edgeLV) {
    auto levelIter = levelActiveOccurMap.find(edgeLV);
    if (levelIter != levelActiveOccurMap.end()) {
        auto uIter = levelIter->second.find(u);
        auto vIter = levelIter->second.find(v);
        if (uIter != levelIter->second.end() && vIter != levelIter->second.end()) {
            shared_ptr<TreapNode> activeOccurU = uIter->second;
            shared_ptr<TreapNode> activeOccurV = vIter->second;

            shared_ptr<TreapNode> root1 = hdtFindRoot(activeOccurU);
            shared_ptr<TreapNode> root2 = hdtFindRoot(activeOccurV);

            if (root1 == root2) {
                auto listIter = levelTreapMap.find(edgeLV);
                if (listIter != levelTreapMap.end()) {
                    auto setIter = listIter->second.find(root1);
                    if (setIter != listIter->second.end()) {
                        listIter->second.erase(setIter);
                    }
                    hdtCutTreap(u, v, root1, edgeLV);
                    listIter->second.emplace(treapU);
                    listIter->second.emplace(treapV);
                }
            }
        }
    }
}

shared_ptr<TreapNode> HDT::hdtFindRoot(shared_ptr<TreapNode> &occur) {
    if (occur == nullptr) {
        return nullptr;
    }
    shared_ptr<TreapNode> temp = nullptr;
    for (auto p = occur; p; p = p->parent_)
        temp = p;
    return temp;
}

void HDT::leftRotate(shared_ptr<TreapNode> &root) {
    shared_ptr<TreapNode> parent = root->parent_;
    int left_right_mark = -1;
    if (parent != nullptr) {
        if (parent->left_ == root) left_right_mark = 0;
        else if (parent->right_ == root) left_right_mark = 1;
    }

    shared_ptr<TreapNode> temp = root->left_;
    root->left_ = temp->right_;

    if (root->left_ != nullptr) {
        root->left_->parent_ = root;
        temp->size_ -= root->left_->size_;
        if (root->left_->active_ || root->left_->activeChild_) root->activeChild_ = true;
        else {
            if (root->right_ != nullptr) {
                if (!(root->right_->active_ || root->right_->activeChild_)) root->activeChild_ = false;
                else root->activeChild_ = true;
            } else root->activeChild_ = false;
        }
    } else {
        if (root->right_ != nullptr) {
            if (!(root->right_->active_ || root->right_->activeChild_)) root->activeChild_ = false;
            else root->activeChild_ = true;
        } else root->activeChild_ = false;
    }

    if (root->left_ != nullptr && root->right_ != nullptr) root->size_ = root->left_->size_ + root->right_->size_ + 1;
    else if (root->left_ != nullptr && root->right_ == nullptr) root->size_ = root->left_->size_ + 1;
    else if (root->left_ == nullptr && root->right_ != nullptr) root->size_ = root->right_->size_ + 1;
    else root->size_ = 1;

    temp->right_ = root;

    if (temp->right_ != nullptr) {
        temp->right_->parent_ = temp;
        if (temp->right_->active_ || temp->right_->activeChild_) temp->activeChild_ = true;
        else {
            if (temp->left_ != nullptr) {
                if (!(temp->left_->active_ || temp->left_->activeChild_)) temp->activeChild_ = false;
                else temp->activeChild_ = true;
            } else {
                temp->activeChild_ = false;
            }
        }
    } else {
        if (temp->left_ != nullptr) {
            if (!(temp->left_->active_ || temp->left_->activeChild_)) temp->activeChild_ = false;
            else temp->activeChild_ = true;
        } else temp->activeChild_ = false;
    }

    if (temp->left_ != nullptr && temp->right_ != nullptr) temp->size_ = temp->left_->size_ + temp->right_->size_ + 1;
    else if (temp->left_ != nullptr && temp->right_ == nullptr) temp->size_ = temp->left_->size_ + 1;
    else if (temp->left_ == nullptr && temp->right_ != nullptr) temp->size_ = temp->right_->size_ + 1;
    else temp->size_ = 1;

    root = temp;
    root->parent_ = parent;
    if (parent != nullptr) {
        if (left_right_mark == 0) parent->left_ = root;
        else if (left_right_mark == 1) parent->right_ = root;
    }
}

void HDT::rightRotate(shared_ptr<TreapNode> &root) {
    shared_ptr<TreapNode> parent = root->parent_;
    int left_right_mark = -1;
    if (parent != nullptr) {
        if (parent->left_ == root) left_right_mark = 0;
        else if (parent->right_ == root) left_right_mark = 1;
    }

    shared_ptr<TreapNode> temp = root->right_;
    root->right_ = temp->left_;

    if (root->right_ != nullptr) {
        root->right_->parent_ = root;
        if (root->right_->active_ || root->right_->activeChild_) root->activeChild_ = true;
        else {
            if (root->left_ != nullptr) {
                if (!(root->left_->active_ || root->left_->activeChild_)) root->activeChild_ = false;
                else root->activeChild_ = true;
            } else root->activeChild_ = false;
        }
    } else {
        if (root->left_ != nullptr) {
            if (!(root->left_->active_ || root->left_->activeChild_)) root->activeChild_ = false;
            else root->activeChild_ = true;
        } else root->activeChild_ = false;
    }

    if (root->left_ != nullptr && root->right_ != nullptr) root->size_ = root->left_->size_ + root->right_->size_ + 1;
    else if (root->left_ != nullptr && root->right_ == nullptr) root->size_ = root->left_->size_ + 1;
    else if (root->left_ == nullptr && root->right_ != nullptr) root->size_ = root->right_->size_ + 1;
    else root->size_ = 1;

    temp->left_ = root;

    if (temp->left_ != nullptr) {
        temp->left_->parent_ = temp;
        if (temp->left_->active_ || temp->left_->activeChild_) temp->activeChild_ = true;
        else {
            if (temp->right_ != nullptr) {
                if (!(temp->right_->active_ || temp->right_->activeChild_)) temp->activeChild_ = false;
                else temp->activeChild_ = true;
            } else {
                temp->activeChild_ = false;
            }
        }
    } else {
        if (temp->right_ != nullptr) {
            if (!(temp->right_->active_ || temp->right_->activeChild_)) temp->activeChild_ = false;
            else temp->activeChild_ = true;
        } else temp->activeChild_ = false;
    }

    if (temp->left_ != nullptr && temp->right_ != nullptr) temp->size_ = temp->left_->size_ + temp->right_->size_ + 1;
    else if (temp->left_ != nullptr && temp->right_ == nullptr) temp->size_ = temp->left_->size_ + 1;
    else if (temp->left_ == nullptr && temp->right_ != nullptr) temp->size_ = temp->right_->size_ + 1;
    else temp->size_ = 1;

    root = temp;
    root->parent_ = parent;
    if (parent != nullptr) {
        if (left_right_mark == 0) parent->left_ = root;
        else if (left_right_mark == 1) parent->right_ = root;
    }
}

void HDT::hdtCutTreapAtLeftPosition(shared_ptr<TreapNode> &currentTreap, shared_ptr<TreapNode> &cutPosition,
                                    shared_ptr<TreapNode> &treapPiece1, shared_ptr<TreapNode> &treapPiece2) {
    if (cutPosition == nullptr) {
        treapPiece1 = nullptr;
        treapPiece2 = nullptr;
        return;
    }

    shared_ptr<TreapNode> dummyNode = make_shared<TreapNode>();
    dummyNode->parent_ = nullptr;
    dummyNode->left_ = nullptr;
    dummyNode->right_ = nullptr;
    dummyNode->size_ = 1;
    dummyNode->activeChild_ = false;
    dummyNode->active_ = false;
    dummyNode->activeOccur_ = false;

    shared_ptr<TreapNode> pioneer;
    if (cutPosition->left_ != nullptr) {
        for (pioneer = cutPosition->left_; pioneer->right_ != nullptr; pioneer = pioneer->right_);
    } else pioneer = nullptr;

    if (pioneer == nullptr) {
        cutPosition->left_ = dummyNode;
        dummyNode->parent_ = cutPosition;
    } else {
        pioneer->right_ = dummyNode;
        dummyNode->parent_ = pioneer;
    }

    // rotate dummy node to the root
    if (dummyNode->parent_ != nullptr) {
        for (auto p = dummyNode->parent_; p != nullptr; p = p->parent_) {
            if (p->right_ == dummyNode) rightRotate(p);
            else if (p->left_ == dummyNode) leftRotate(p);
        }
    }

    if (dummyNode->left_ != nullptr) treapPiece1 = dummyNode->left_;
    else treapPiece1 = nullptr;

    if (dummyNode->right_ != nullptr) treapPiece2 = dummyNode->right_;
    else treapPiece2 = nullptr;

    if (treapPiece1 != nullptr) treapPiece1->parent_ = nullptr;
    if (treapPiece2 != nullptr) treapPiece2->parent_ = nullptr;

    dummyNode->left_ = nullptr;
    dummyNode->right_ = nullptr;
}

void HDT::hdtCutTreapAtRightPosition(shared_ptr<TreapNode> &currentTreap, shared_ptr<TreapNode> &cutPosition,
                                     shared_ptr<TreapNode> &treapPiece1, shared_ptr<TreapNode> &treapPiece2) {
    if (cutPosition == nullptr) {
        treapPiece1 = nullptr;
        treapPiece2 = nullptr;
        return;
    }

    shared_ptr<TreapNode> dummyNode = make_shared<TreapNode>();
    dummyNode->parent_ = nullptr;
    dummyNode->left_ = nullptr;
    dummyNode->right_ = nullptr;
    dummyNode->size_ = 1;
    dummyNode->activeChild_ = false;
    dummyNode->active_ = false;
    dummyNode->activeOccur_ = false;

    shared_ptr<TreapNode> successor;
    if (cutPosition->right_ != nullptr) {
        for (successor = cutPosition->right_; successor->left_ != nullptr; successor = successor->left_);
    } else successor = nullptr;

    if (successor == nullptr) {
        cutPosition->right_ = dummyNode;
        dummyNode->parent_ = cutPosition;
    } else {
        successor->left_ = dummyNode;
        dummyNode->parent_ = successor;
    }

    // rotate dummy node to the root
    if (dummyNode->parent_ != nullptr) {
        for (auto p = dummyNode->parent_; p != nullptr; p = p->parent_) {
            if (p->right_ == dummyNode) rightRotate(p);
            else if (p->left_ == dummyNode) leftRotate(p);
        }
    }

    if (dummyNode->left_ != nullptr) treapPiece1 = dummyNode->left_;
    else treapPiece1 = nullptr;

    if (dummyNode->right_ != nullptr) treapPiece2 = dummyNode->right_;
    else treapPiece2 = nullptr;

    if (treapPiece1 != nullptr) treapPiece1->parent_ = nullptr;
    if (treapPiece2 != nullptr) treapPiece2->parent_ = nullptr;

    dummyNode->left_ = nullptr;
    dummyNode->right_ = nullptr;
}

graphEdge HDT::findReplacementEdge(shared_ptr<TreapNode> &smallTreap, shared_ptr<TreapNode> &largeTreap, int edgeLV) {
    stack<shared_ptr<TreapNode>> tempStack;
    unordered_set<string> visitedNode;
    unordered_set<graphEdge, graphEdgeHash> visitedEdge;

    if(smallTreap != nullptr && (smallTreap->active_ || smallTreap->activeChild_)) tempStack.emplace(smallTreap);

    while(!tempStack.empty()){
        shared_ptr<TreapNode> topNode = tempStack.top();
        tempStack.pop();
        if(topNode->right_ != nullptr && (topNode->right_->active_ || topNode->right_->activeChild_)) tempStack.emplace(topNode->right_);
        if(topNode->left_ != nullptr && (topNode->left_->active_ || topNode->left_->activeChild_)) tempStack.emplace(topNode->left_);
        if(topNode->active_){
            auto visitedIter = visitedNode.find(topNode->label_);
            if(visitedIter != visitedNode.end()) continue;
            visitedNode.emplace(topNode->label_);

            auto endpIter = nonTreeEdges.find(topNode->label_);
            if(endpIter != nonTreeEdges.end()){
                auto lvIter = endpIter->second.find(edgeLV);
                if(lvIter != endpIter->second.end()){
                    for(auto edgeIter = lvIter->second.begin(); edgeIter != lvIter->second.end();){
                        graphEdge foundEdge = *edgeIter;
                        auto edgeVisIter = visitedEdge.find(foundEdge);
                        if(edgeVisIter != visitedEdge.end()){
                            edgeIter++;
                            continue;
                        }
                        visitedEdge.emplace(foundEdge);
                        shared_ptr<TreapNode> root1;
                        shared_ptr<TreapNode> root2;
                        auto lvMapIter = levelActiveOccurMap.find(edgeLV);
                        if(lvMapIter != levelActiveOccurMap.end()){
                            auto occurIter = lvMapIter->second.find(foundEdge.endPoint1);
                            if(occurIter != lvMapIter->second.end()) root1 = hdtFindRoot(occurIter->second);
                            occurIter = lvMapIter->second.find(foundEdge.endPoint2);
                            if(occurIter != lvMapIter->second.end()) root2 = hdtFindRoot(occurIter->second);
                        }
                        if((smallTreap == root1 && largeTreap == root2) || (smallTreap == root2 && largeTreap == root1)){
                            // found a replacement edge
                            auto elistIter = edgeList.find(foundEdge);
                            if(elistIter != edgeList.end()){
                                if(elistIter->second == edgeLV){
                                    treeEdges.emplace(foundEdge, edgeLV);
                                    auto subendpIter = nonTreeEdges.find(foundEdge.endPoint1);
                                    if(subendpIter != nonTreeEdges.end()){
                                        auto sublvIter = subendpIter->second.find(edgeLV);
                                        if(sublvIter != subendpIter->second.end()){
                                            auto nonedgeIter = sublvIter->second.find(foundEdge);
                                            if(nonedgeIter != sublvIter->second.end()){
                                                sublvIter->second.erase(nonedgeIter);
                                            }
                                        }
                                    }
                                    subendpIter = nonTreeEdges.find(foundEdge.endPoint2);
                                    if(subendpIter != nonTreeEdges.end()){
                                        auto sublvIter = subendpIter->second.find(edgeLV);
                                        if(sublvIter != subendpIter->second.end()){
                                            auto nonedgeIter = sublvIter->second.find(foundEdge);
                                            if(nonedgeIter != sublvIter->second.end()){
                                                sublvIter->second.erase(nonedgeIter);
                                            }
                                        }
                                    }
                                }
                            }

                            return foundEdge;
                        }

                        if(edgeLV < logN && smallTreap == root1 && smallTreap == root2){
                            // delete from the current level
                            edgeIter = lvIter->second.erase(edgeIter);
                            // two endpoints are in the small treap
                            // promote it up to level i + 1if edgeLV is smaller than logN
                            auto subendpIter = nonTreeEdges.find(foundEdge.endPoint1);
                            if(subendpIter != nonTreeEdges.end()){
                                auto sublvIter = subendpIter->second.find(edgeLV + 1);
                                if(sublvIter != subendpIter->second.end()){
                                    sublvIter->second.emplace(foundEdge);
                                }
                                else{
                                    unordered_set<graphEdge, graphEdgeHash> tempSet;
                                    tempSet.emplace(foundEdge);
                                    subendpIter->second.emplace(edgeLV + 1, tempSet);
                                }

                                sublvIter = subendpIter->second.find(edgeLV);
                                if(sublvIter != subendpIter->second.end()){
                                    auto neIter = sublvIter->second.find(foundEdge);
                                    if(neIter != sublvIter->second.end()){
                                        sublvIter->second.erase(neIter);
                                    }
                                }
                            }

                            subendpIter = nonTreeEdges.find(foundEdge.endPoint2);
                            if(subendpIter != nonTreeEdges.end()){
                                auto sublvIter = subendpIter->second.find(edgeLV + 1);
                                if(sublvIter != subendpIter->second.end()){
                                    sublvIter->second.emplace(foundEdge);
                                }
                                else{
                                    unordered_set<graphEdge, graphEdgeHash> tempSet;
                                    tempSet.emplace(foundEdge);
                                    subendpIter->second.emplace(edgeLV + 1, tempSet);
                                }

                                sublvIter = subendpIter->second.find(edgeLV);
                                if(sublvIter != subendpIter->second.end()){
                                    auto neIter = sublvIter->second.find(foundEdge);
                                    if(neIter != sublvIter->second.end()){
                                        sublvIter->second.erase(neIter);
                                    }
                                }
                            }

                            auto elistIter = edgeList.find(foundEdge);
                            if(elistIter != edgeList.end()){
                                if(elistIter->second == edgeLV){
                                    elistIter->second += 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    graphEdge returnEdge(" ", " ");
    return returnEdge;
}

void HDT::printTreapVector() {
    Treap t;
    for(auto &e : treapVector){
        cout << "at level: " << e.first << endl;
        cout << "treap1 : ----------" << endl;
        t.printTreap(e.second->treap1);
        cout << "treap2 : ----------" << endl;
        t.printTreap(e.second->treap2);
    }
}

void HDT::printLevelTreapMap() {
    Treap t;
    for(auto &e : levelTreapMap){
        cout << "at level: " << e.first << endl;
        cout << "the treap are: " << endl;
        int count = 0;
        for(auto entry : e.second){
            cout << "the " << count++ << "th: ============>" << endl;
            t.printTreap(entry);
        }
    }
}
