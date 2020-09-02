cmake_minimum_required(VERSION 3.17)
project(ImproveAgreementTesting)

set(CMAKE_CXX_STANDARD 14)

add_executable(ImproveAgreementTesting main.cpp InputTreeNode.h InputTreeNode.cpp ConstructGraph.cpp ConstructGraph.h EulerTourTree.cpp EulerTourTree.h TreapNode.h Treap.cpp Treap.h HelperDataStructure.h HDT.cpp HDT.h AgreementTesting.cpp AgreementTesting.h GetDecomposition.cpp GetDecomposition.h InputGenerator.cpp InputGenerator.h OutputChecker.cpp OutputChecker.h)