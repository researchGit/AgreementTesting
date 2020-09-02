//
// Created by Lei on 7/29/20.
//

#include "Treap.h"
#include <stack>

using namespace std;

Treap::Treap() {
    this->keyCount = 0;
}

int Treap::getKeyCount() {
    return keyCount++;
}

void Treap::rotateRight(shared_ptr<TreapNode> &root) {
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

void Treap::rotateLeft(shared_ptr<TreapNode> &root) {
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

void
Treap::insert(shared_ptr<TreapNode> &root, string label, int key, int priority, bool active, bool activeOccur,
              shared_ptr<TreapNode> &parent) {
    if (root == nullptr) {
        root = make_shared<TreapNode>(label, key, priority, active, activeOccur);
        root->parent_ = parent;

        if (activeOccur) activeOccurList.emplace(label, root);

        if (lastVisitedPointer == nullptr) lastVisitedPointer = root;
        else {
            auto labelIter = levelLabelReferenceMap.find(label);
            if (labelIter != levelLabelReferenceMap.end()) {
                auto levelIter = labelIter->second.find(0);
                if (levelIter != labelIter->second.end()) {
                    // find it
                    auto subIter = levelIter->second.find(lastVisitedPointer->label_);
                    if (subIter != levelIter->second.end()) {
                        subIter->second.emplace(lastVisitedPointer);
                    } else {
                        unordered_set<shared_ptr<TreapNode>> tempSet{lastVisitedPointer};
                        levelIter->second.emplace(lastVisitedPointer->label_, tempSet);
                    }
                } else {
                    unordered_set<shared_ptr<TreapNode>> tempSet{lastVisitedPointer};
                    unordered_map<string, unordered_set<shared_ptr<TreapNode>>> tempLabelOccurMap{{lastVisitedPointer->label_, tempSet}};
                    labelIter->second.emplace(0, tempLabelOccurMap);
                }
            } else {
                unordered_set<shared_ptr<TreapNode>> tempSet{lastVisitedPointer};
                unordered_map<string, unordered_set<shared_ptr<TreapNode>>> tempLabelOccurMap{{lastVisitedPointer->label_, tempSet}};
                unordered_map<int, unordered_map<string, unordered_set<shared_ptr<TreapNode>>>> tempLevelLabel{{0, tempLabelOccurMap}};
                levelLabelReferenceMap.emplace(label, tempLevelLabel);
            }

            labelIter = levelLabelReferenceMap.find(lastVisitedPointer->label_);
            if (labelIter != levelLabelReferenceMap.end()) {
                auto levelIter = labelIter->second.find(0);
                if (levelIter != labelIter->second.end()) {
                    // find it
                    auto subIter = levelIter->second.find(label);
                    if (subIter != levelIter->second.end()) {
                        subIter->second.emplace(root);
                    } else {
                        unordered_set<shared_ptr<TreapNode>> tempSet{root};
                        levelIter->second.emplace(label, tempSet);
                    }
                } else {
                    unordered_set<shared_ptr<TreapNode>> tempSet{root};
                    unordered_map<string, unordered_set<shared_ptr<TreapNode>>> tempLabelOccurMap{{label, tempSet}};
                    labelIter->second.emplace(0, tempLabelOccurMap);
                }
            } else {
                unordered_set<shared_ptr<TreapNode>> tempSet{root};
                unordered_map<string, unordered_set<shared_ptr<TreapNode>>> tempLabelOccurMap{{label, tempSet}};
                unordered_map<int, unordered_map<string, unordered_set<shared_ptr<TreapNode>>>> tempLevelLabel{{0, tempLabelOccurMap}};
                levelLabelReferenceMap.emplace(lastVisitedPointer->label_, tempLevelLabel);
            }
            lastVisitedPointer = root;
        }
    } else if (key < root->key_) {
        root->size_++;
        insert(root->left_, label, key, priority, active, activeOccur, root);
        if (root->left_->priority_ < root->priority_) {
            rotateLeft(root);
        }
        if (root->left_ != nullptr) {
            if (root->left_->active_ || root->left_->activeChild_) root->activeChild_ = true;
        }
        if (root->right_ != nullptr) {
            if (root->right_->active_ || root->right_->activeChild_) root->activeChild_ = true;
        }
    } else {
        root->size_++;
        insert(root->right_, label, key, priority, active, activeOccur, root);
        if (root->right_->priority_ < root->priority_) {
            rotateRight(root);
        }
        if (root->left_ != nullptr) {
            if (root->left_->active_ || root->left_->activeChild_) root->activeChild_ = true;
        }
        if (root->right_ != nullptr) {
            if (root->right_->active_ || root->right_->activeChild_) root->activeChild_ = true;
        }
    }
}

shared_ptr<TreapNode> Treap::constructTreap(vector<string> &eulerTourSequence,
                                            unordered_map<string, unordered_map<int, unordered_set<graphEdge, graphEdgeHash>>> &nonTreeEdges) {
    shared_ptr<TreapNode> treapRoot = nullptr;
    unordered_set<string> occurredLabel;
    lastVisitedPointer = nullptr;
    for (string &label : eulerTourSequence) {
        auto occurredIter = occurredLabel.find(label);
        if (occurredIter != occurredLabel.end()) {
            // the current label is not the first occurrence of the corresponding node
            insert(treapRoot, label, getKeyCount(), rand() / 10, false, false, treapRoot);
        } else {
            occurredLabel.emplace(label);
            // indicate that this label is the first occurrence of the corresponding node
            auto edgeIter = nonTreeEdges.find(label);
            if (edgeIter != nonTreeEdges.end()) {
                auto levelIter = edgeIter->second.find(0);
                if (levelIter != edgeIter->second.end()) {
                    if (!levelIter->second.empty())
                        insert(treapRoot, label, getKeyCount(), rand() / 10, true, true, treapRoot);
                    else
                        insert(treapRoot, label, getKeyCount(), rand() / 10, false, true, treapRoot);
                }
            } else {
                insert(treapRoot, label, getKeyCount(), rand() / 10, false, true, treapRoot);
            }
        }
    }
    if (treapRoot != nullptr) treapRoot->parent_ = nullptr;
    return treapRoot;
}

bool Treap::checkInfoCorrectness(shared_ptr<TreapNode> &root) {
    stack<shared_ptr<TreapNode>> stack;
    shared_ptr<TreapNode> tmp = root;
    while (tmp != nullptr) {
        stack.push(tmp);
        tmp = tmp->left_;
    }
    while (!stack.empty()) {
        tmp = stack.top();
        stack.pop();
        bool status = computeSizeRootedOfNode(tmp);
        bool correctnessStatus = checkParentChileRelationship(tmp);
        if (!status) return false;
        if (!correctnessStatus) return false;
        tmp = tmp->right_;
        while (tmp != nullptr) {
            stack.push(tmp);
            tmp = tmp->left_;
        }
    }
    return true;
}

bool Treap::computeSizeRootedOfNode(shared_ptr<TreapNode> &root) {
    stack<shared_ptr<TreapNode>> stack;
    stack.push(root);
    int size = 0;
    while (!stack.empty()) {
        ++size;
        shared_ptr<TreapNode> tmp = stack.top();
        stack.pop();
        if (tmp->left_ != nullptr) stack.push(tmp->left_);
        if (tmp->right_ != nullptr) stack.push(tmp->right_);
    }
    return size == root->size_;
}

bool Treap::checkParentChileRelationship(shared_ptr<TreapNode> &root) {
    stack<shared_ptr<TreapNode>> stack;
    stack.push(root);
    while (!stack.empty()) {
        shared_ptr<TreapNode> tmp = stack.top();
        stack.pop();
        if (tmp->left_ != nullptr) {
            if (tmp != tmp->left_->parent_) return false;
            stack.push(tmp->left_);
        }
        if (tmp->right_ != nullptr) {
            if (tmp != tmp->right_->parent_) return false;
            stack.push(tmp->right_);
        }
    }
    return true;
}

void Treap::printTreap(shared_ptr<TreapNode> &node) {
    stack<shared_ptr<TreapNode>> stack;
    shared_ptr<TreapNode> tmp = node;
    while (tmp != nullptr) {
        stack.push(tmp);
        tmp = tmp->left_;
    }
    while (!stack.empty()) {
        tmp = stack.top();
        stack.pop();
        cout << tmp << " + " << tmp->label_ << " + " << tmp->key_ << " + " << tmp->priority_ << " + " << tmp->size_
             << " + " << tmp->parent_ << " + " << tmp->left_ << " " << tmp->right_ << " + " << tmp->active_
             << " + "
             << tmp->activeChild_ << " + " << tmp->activeOccur_ << endl;
        tmp = tmp->right_;
        while (tmp != nullptr) {
            stack.push(tmp);
            tmp = tmp->left_;
        }
    }
}

void Treap::printLevelLabelReferenceMap() {
    cout << levelLabelReferenceMap.size() << endl;
    for (auto &levelMap : this->levelLabelReferenceMap) {
        cout << "starting label: " << levelMap.first << endl;
        for (auto &referenceMap: levelMap.second) {
            cout << "\t" << "level at " << referenceMap.first << endl;
            for (auto &refs : referenceMap.second) {
                cout << "\t\t" << "ending label: " << refs.first << endl;
                cout << "\t\t\t";
                for (auto &entity : refs.second) {
                    cout << entity << "\t";
                }
                cout << endl;
            }
        }
    }
}

void Treap::printActiveOccurList() {
    for (auto &entity : this->activeOccurList) {
        cout << "label: " << entity.first << " -> " << entity.second << endl;
    }
}
