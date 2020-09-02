//
// Created by Lei on 8/20/20.
//

#ifndef AGREEMENTTESTING_OUTPUTCHECKER_H
#define AGREEMENTTESTING_OUTPUTCHECKER_H

#include "AgreementTesting.h"
#include "InputTreeNode.h"

using namespace std;

class OutputChecker {
public:
    OutputChecker() = default;
    ~OutputChecker() = default;
    bool isTreesIdentical(shared_ptr<SuperTreeNode> &output, shared_ptr<InputTreeNode> &input);
};


#endif //AGREEMENTTESTING_OUTPUTCHECKER_H
