#include "EulerTourTree.h"
#include "Treap.h"
#include "AgreementTesting.h"
#include "InputGenerator.h"
#include "OutputChecker.h"
#include <iostream>
#include <ctime>
#include "NewickReader.h"
#include <sstream>
#include <queue>

using namespace std;
using namespace chrono;

int main(int argc, char** argv) {
    cout << "Agreement Testing is in progress..." << endl;
    // -i filePath -o filePath -p true/false
    // -s 127 7 3
    vector<string> argVec;
    for(int i = 0; i < argc; ++i) {
        if(i == 0) continue;
        string argStr(argv[i]);
        argVec.emplace_back(argStr);
    }

    srand(time(0));

    if(argVec[0] == "-i"){
        bool enablePromotion = argVec[5] == "true";
        time_t executionTime = 0;

        string inputFilePath = argVec[1];
        NewickReader nr;
        vector<newickFormatParser::inputTreeNode> rawInputTrees = nr.parseNewickFile(inputFilePath);

        rawInputTrees = nr.replaceBlankInternalLabels(rawInputTrees);

        vector<shared_ptr<InputTreeNode>> inputTrees = nr.convertToNativeTreeFormat(rawInputTrees);

        ConstructGraph cg;
        cg.createAssembleGraph(inputTrees);
        shared_ptr<ConstructGraph> constructGraph = cg.getConstructGraph();



        EulerTourTree etTree;
        vector<string> eulerTourSeq = etTree.constructEulerTourTree(constructGraph->getAssembleGraph());

        Treap treap;
        auto nonTreeEdges = etTree.getNonTreeEdges();
        auto root = treap.constructTreap(eulerTourSeq, nonTreeEdges);
        treap.checkInfoCorrectness(root);

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
            cout << "DISAGREEMENT!" << endl;
            cout << "---- execution time ----" << endl;
            cout << ((double) executionTime) / CLOCKS_PER_SEC << endl;
            return 0;
        }
        cout << agreement << endl;
        auto outputRoot = at.getSuperTreeRoot();
        string content = at.convert2Newick(outputRoot, "");
        cout << content << endl;
        string outputFile = argVec[3];
        at.writeToTxtFile(content, outputFile);
        cout << "---- execution time ----" << endl;
        cout << ((double)executionTime)/CLOCKS_PER_SEC << endl;
    } else {
        stringstream numberOfTaxaString(argVec[1]);
        int numberOfTaxa = 0;
        numberOfTaxaString >> numberOfTaxa;

        stringstream numberOfTreesString(argVec[2]);
        int numberOfTrees = 7;
        numberOfTreesString >> numberOfTrees;

        int testTimes = 30;

        stringstream degreeString(argVec[3]);
        int degree = 3;
        degreeString >> degree;

        bool enablePromotion = true;

        long labelCount = 0;
        long labelCount0level = 0;
        unordered_set<string> resultSet;
        time_t executionTime = 0;
        time_t executionTime0level = 0;
        for(int i = 0; i < testTimes; ++i){
            cout << "test " << i+1 << "...." << endl;
            // inspect input generator
            InputGenerator ig;
            ig.generateGeneralInputTrees(numberOfTaxa, numberOfTrees, degree);
            vector<shared_ptr<InputTreeNode>> simulatedInputTrees = ig.getInputTrees();

            ConstructGraph cg;
            cg.createAssembleGraph(simulatedInputTrees);
            shared_ptr<ConstructGraph> constructGraph = cg.getConstructGraph();

            EulerTourTree etTree;
            vector<string> eulerTourSeq = etTree.constructEulerTourTree(constructGraph->getAssembleGraph());

            Treap treap;
            auto nonTreeEdges = etTree.getNonTreeEdges();
            auto root = treap.constructTreap(eulerTourSeq, nonTreeEdges);
            treap.checkInfoCorrectness(root);

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
                cout << "DISAGREEMENT" << endl;
                return 0;
            }
            labelCount += ig.getLabelCount();
            cout << agreement << endl;

            auto outputRoot = at.getSuperTreeRoot();
            string content = at.convert2Newick(outputRoot, "");
            resultSet.emplace(content);
            string fileName = "example" + to_string(i);
            at.writeToTxtFile(content, fileName);
            cout << endl;

            /* construct data structures for 0 levels test*/
            ConstructGraph cg0level;
            cg0level.createAssembleGraph(simulatedInputTrees);
            shared_ptr<ConstructGraph> constructGraph0level = cg0level.getConstructGraph();

            EulerTourTree etTree0level;
            vector<string> eulerTourSeq0level = etTree0level.constructEulerTourTree(constructGraph0level->getAssembleGraph());

            Treap treap0level;
            auto nonTreeEdges0level = etTree0level.getNonTreeEdges();
            auto root0level = treap0level.constructTreap(eulerTourSeq0level, nonTreeEdges0level);
            treap0level.checkInfoCorrectness(root0level);

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
                cout << "DISAGREEMENT" << endl;
                return 0;
            }
            labelCount0level += ig.getLabelCount();
            cout << agreement0level << endl;
            auto outputRoot0level = at0level.getSuperTreeRoot();
            string content0level = at0level.convert2Newick(outputRoot0level, "");
            resultSet.emplace(content0level);
            string fileName0level = "example0level" + to_string(i);
            at0level.writeToTxtFile(content0level, fileName0level);
            cout << endl;
        }
        cout << "---- execution time ----" << endl;
        cout << ((double)executionTime / testTimes)/CLOCKS_PER_SEC << endl;
        cout << "---- execution time of 0 levels ----" << endl;
        cout << ((double)executionTime0level / testTimes)/CLOCKS_PER_SEC << endl;
    }
    return 0;
}
