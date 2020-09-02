//
// Created by Lei on 7/29/20.
//

#ifndef AGREEMENTTESTING_TREAPNODE_H
#define AGREEMENTTESTING_TREAPNODE_H

#include <iostream>

using namespace std;

struct TreapNode {
    string label_;
    int key_;
    int priority_;
    int size_;
    bool active_;
    bool activeOccur_;
    bool activeChild_;
    shared_ptr<TreapNode> parent_; //if not used later, will be removed
    shared_ptr<TreapNode> left_;
    shared_ptr<TreapNode> right_;
    TreapNode() = default;
    TreapNode(string label, int key, int priority, bool hasNonTreeEdges, bool activeOccur) : label_(label),
                                                                                                      key_(key),
                                                                                                      priority_(
                                                                                                              priority),
                                                                                                      size_(1),
                                                                                                      active_(
                                                                                                              hasNonTreeEdges),
                                                                                                      activeOccur_(
                                                                                                              activeOccur),
                                                                                                      activeChild_(
                                                                                                              false),
                                                                                                      parent_(nullptr),
                                                                                                      left_(nullptr),
                                                                                                      right_(nullptr) {};
};

#endif //AGREEMENTTESTING_TREAPNODE_H
