#include "EulerTourTree.h"
#include "Treap.h"
#include "AgreementTesting.h"
#include "InputGenerator.h"
#include "OutputChecker.h"
#include <iostream>
#include <ctime>

using namespace std;
using namespace chrono;

int main() {
    cout << "Hello, World!" << endl;
    srand(time(0));
    shared_ptr<InputTreeNode> node_1 = make_shared<InputTreeNode>("a", 0);
    shared_ptr<InputTreeNode> node_2 = make_shared<InputTreeNode>("b", 0);
    shared_ptr<InputTreeNode> node_3 = make_shared<InputTreeNode>("c", 0);
    shared_ptr<InputTreeNode> node_4 = make_shared<InputTreeNode>("d", 0);
    vector<shared_ptr<InputTreeNode>> nodeVec1{node_1, node_2};
    shared_ptr<InputTreeNode> node_5 = make_shared<InputTreeNode>("6", nodeVec1, 0);
    vector<shared_ptr<InputTreeNode>> nodeVec2{node_5, node_3};
    shared_ptr<InputTreeNode> node_6 = make_shared<InputTreeNode>("5", nodeVec2, 0);
    vector<shared_ptr<InputTreeNode>> nodeVec3{node_6, node_4};
    shared_ptr<InputTreeNode> node_7 = make_shared<InputTreeNode>("1", nodeVec3, 0);

    shared_ptr<InputTreeNode> node1 = make_shared<InputTreeNode>("b", 1);
    shared_ptr<InputTreeNode> node2 = make_shared<InputTreeNode>("c", 1);
    shared_ptr<InputTreeNode> node3 = make_shared<InputTreeNode>("e", 1);
    shared_ptr<InputTreeNode> node4 = make_shared<InputTreeNode>("f", 1);
    vector<shared_ptr<InputTreeNode>> nodeVec4{node3, node4};
    shared_ptr<InputTreeNode> node5 = make_shared<InputTreeNode>("g", nodeVec4, 1);
    vector<shared_ptr<InputTreeNode>> nodeVec5{node1, node2, node5};
    shared_ptr<InputTreeNode> node6 = make_shared<InputTreeNode>("2", nodeVec5, 1);

    shared_ptr<InputTreeNode> node7 = make_shared<InputTreeNode>("h", 2);
    shared_ptr<InputTreeNode> node8 = make_shared<InputTreeNode>("i", 2);
    shared_ptr<InputTreeNode> node9 = make_shared<InputTreeNode>("j", 2);
    shared_ptr<InputTreeNode> node10 = make_shared<InputTreeNode>("f", 2);
    vector<shared_ptr<InputTreeNode>> nodeVec6{node7};
    vector<shared_ptr<InputTreeNode>> nodeVec7{node8, node9};
    shared_ptr<InputTreeNode> node11 = make_shared<InputTreeNode>("k", nodeVec6, 2);
    shared_ptr<InputTreeNode> node12 = make_shared<InputTreeNode>("7", nodeVec7, 2);
    vector<shared_ptr<InputTreeNode>> nodeVec8{node11, node12, node10};
    shared_ptr<InputTreeNode> node13 = make_shared<InputTreeNode>("g", nodeVec8, 2);

    shared_ptr<InputTreeNode> n1 = make_shared<InputTreeNode>("i", 3);
    shared_ptr<InputTreeNode> n2 = make_shared<InputTreeNode>("j", 3);
    shared_ptr<InputTreeNode> n3 = make_shared<InputTreeNode>("l", 3);
    vector<shared_ptr<InputTreeNode>> nodeVec9{n1, n2, n3};
    shared_ptr<InputTreeNode> n4 = make_shared<InputTreeNode>("4", nodeVec9, 3);

    vector<shared_ptr<InputTreeNode>> inputTrees{node_7, node6, node13, n4};

    int numberOfTaxa = 900;
    int numberOfTrees = 100;
    int testTimes = 30;
    int degree = 3;
    bool enablePromotion = true;
    long labelCount = 0;
    long labelCount0level = 0;
    unordered_set<string> resultSet;
    time_t executionTime = 0;
    time_t executionTime0level = 0;
    time_t hdtDuration = 0;
    time_t hdtDuration0level = 0;
    for(int i = 0; i < testTimes; ++i){
        cout << "test " << i+1 << "...." << endl;
        // inspect input generator
        InputGenerator ig;
//        ig.generateBinaryInputTrees(numberOfTaxa, numberOfTrees);
        ig.generateGeneralInputTrees(numberOfTaxa, numberOfTrees, degree);
        vector<shared_ptr<InputTreeNode>> simulatedInputTrees = ig.getInputTrees();
//        cout << "--- check simulated input trees ----" << endl;
//        for(auto &tree : simulatedInputTrees) {
//            cout << "**** tree ****" << endl;
//            ig.printTree(tree);
//        }

        ConstructGraph cg;
        cg.createAssembleGraph(simulatedInputTrees);
        shared_ptr<ConstructGraph> constructGraph = cg.getConstructGraph();
//        constructGraph->printAssembleGraph();
//        constructGraph->printInitialPosition();
//        constructGraph->printOccurMap();
//        constructGraph->printChildMap();

        EulerTourTree etTree;
        vector<string> eulerTourSeq = etTree.constructEulerTourTree(constructGraph->getAssembleGraph());
//        EulerTourTree::printEulerTourTree(eulerTourSeq);

        Treap treap;
//        etTree.printEdges();
        auto nonTreeEdges = etTree.getNonTreeEdges();
        auto root = treap.constructTreap(eulerTourSeq, nonTreeEdges);
//        cout << root->priority_ << endl;
//        treap.printTreap(root);
        treap.checkInfoCorrectness(root);

//        cout << "print active occurrence list: " << endl;
//        treap.printActiveOccurList();
//
//        cout << "print reference map: " << endl;
//        treap.printLevelLabelReferenceMap();

        unordered_map<string, unordered_set<int>> occurMap = constructGraph->getOccurMap();
        vector<string> position = constructGraph->getInitialPosition();

        auto childMap = constructGraph->getChildMap();
        auto start = clock();
        AgreementTesting at(occurMap, childMap, enablePromotion, etTree.getAllEdges(), etTree.getTreeEdges(), etTree.getNonTreeEdges(),
                            *constructGraph->getAssembleGraph(), treap.getLevelLabelReferenceMap(),
                            treap.getActiveOccurList(), root, eulerTourSeq.size());
        string agreement = at.BuildAST(position);
        auto end = clock();
        auto duration = (end - start);
        executionTime += duration;
        if(agreement != "AGREEMENT") {
            cout << "WRONG!!!!!!!" << endl;
            return 0;
        }
        hdtDuration += at.getHDTDuration();
        labelCount += ig.getLabelCount();
        cout << agreement << endl;
        cout <<" mission complete!" << endl;
//        at.printSuperTree();
//        cout << "---- print out the SuperTree in Newick format ----" << endl;
        auto outputRoot = at.getSuperTreeRoot();
        string content = at.convert2Newick(outputRoot, "");
        resultSet.emplace(content);
//        cout << content << endl;
        string fileName = "example" + to_string(i);
        at.writeToTxtFile(content, fileName);

//        OutputChecker oc;
//        bool isIdentical = oc.isTreesIdentical(outputRoot, simulatedSeedTrees[0]);
//        cout << "---- check if the seed tree is identical to the supertree (they should be identical) ----" << endl;
//        string identical;
//        if(isIdentical) identical = "Correct Results!";
//        else identical = "Wrong Results!";
//        cout << identical << endl;
//        if(identical == "Wrong Results!") {
//            cout << "WRONG!!!!!!!!!!!!!!!!!" << endl;
//            return 0;
//        }
        cout << endl;

        /* construct data structures for 0 levels test*/
        ConstructGraph cg0level;
        cg0level.createAssembleGraph(simulatedInputTrees);
        shared_ptr<ConstructGraph> constructGraph0level = cg0level.getConstructGraph();
//        constructGraph->printAssembleGraph();
//        constructGraph->printInitialPosition();
//        constructGraph->printOccurMap();
//        constructGraph->printChildMap();

        EulerTourTree etTree0level;
        vector<string> eulerTourSeq0level = etTree0level.constructEulerTourTree(constructGraph0level->getAssembleGraph());
//        EulerTourTree::printEulerTourTree(eulerTourSeq);

        Treap treap0level;
//        etTree.printEdges();
        auto nonTreeEdges0level = etTree0level.getNonTreeEdges();
        auto root0level = treap0level.constructTreap(eulerTourSeq0level, nonTreeEdges0level);
//        cout << root->priority_ << endl;
//        treap.printTreap(root);
        treap0level.checkInfoCorrectness(root0level);

//        cout << "print active occurrence list: " << endl;
//        treap.printActiveOccurList();
//
//        cout << "print reference map: " << endl;
//        treap.printLevelLabelReferenceMap();

        unordered_map<string, unordered_set<int>> occurMap0level = constructGraph0level->getOccurMap();
        vector<string> position0level = constructGraph0level->getInitialPosition();

        auto childMap0level = constructGraph0level->getChildMap();
        auto start0level = clock();
        AgreementTesting at0level(occurMap0level, childMap0level, !enablePromotion, etTree0level.getAllEdges(), etTree0level.getTreeEdges(), etTree0level.getNonTreeEdges(),
                            *constructGraph0level->getAssembleGraph(), treap0level.getLevelLabelReferenceMap(),
                            treap0level.getActiveOccurList(), root0level, eulerTourSeq0level.size());
        string agreement0level = at0level.BuildAST(position0level);
        auto end0level = clock();
        auto duration0level = (end0level - start0level);
        executionTime0level += duration0level;
        if(agreement0level != "AGREEMENT") {
            cout << "WRONG!!!!!!!" << endl;
            return 0;
        }
        hdtDuration0level += at0level.getHDTDuration();
        labelCount0level += ig.getLabelCount();
        cout << agreement0level << endl;
        cout <<" mission complete!" << endl;
//        at.printSuperTree();
//        cout << "---- print out the SuperTree in Newick format ----" << endl;
        auto outputRoot0level = at0level.getSuperTreeRoot();
        string content0level = at0level.convert2Newick(outputRoot0level, "");
        resultSet.emplace(content0level);
//        cout << content << endl;
        string fileName0level = "example0level" + to_string(i);
        at0level.writeToTxtFile(content0level, fileName0level);

//        OutputChecker oc;
//        bool isIdentical = oc.isTreesIdentical(outputRoot, simulatedSeedTrees[0]);
//        cout << "---- check if the seed tree is identical to the supertree (they should be identical) ----" << endl;
//        string identical;
//        if(isIdentical) identical = "Correct Results!";
//        else identical = "Wrong Results!";
//        cout << identical << endl;
//        if(identical == "Wrong Results!") {
//            cout << "WRONG!!!!!!!!!!!!!!!!!" << endl;
//            return 0;
//        }
        cout << endl;
    }
    cout << "---- size of result set ----" << endl;
    cout << resultSet.size() << endl;
    cout << "---- execution time ----" << endl;
    cout << ((double)executionTime / testTimes)/CLOCKS_PER_SEC << endl;
    cout << "---- hdt execution time ----" << endl;
    cout << ((double)hdtDuration / testTimes)/CLOCKS_PER_SEC << endl;
    cout << "---- average number of labels ----" << endl;
    cout << labelCount / testTimes << endl;
    cout << "---- execution time of 0 levels ----" << endl;
    cout << ((double)executionTime0level / testTimes)/CLOCKS_PER_SEC << endl;
    cout << "---- hdt execution time 0 level ----" << endl;
    cout << ((double)hdtDuration0level / testTimes)/CLOCKS_PER_SEC << endl;
    cout << "---- average number of labels of 0 levels ----" << endl;
    cout << labelCount0level / testTimes << endl;
    return 0;
}
