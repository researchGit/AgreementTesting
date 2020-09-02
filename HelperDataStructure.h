//
// Created by Lei on 7/30/20.
//

#ifndef AGREEMENTTESTING_HELPERDATASTRUCTURE_H
#define AGREEMENTTESTING_HELPERDATASTRUCTURE_H

#include <iostream>

using namespace std;

struct graphEdge{
    string endPoint1;
    string endPoint2;
    bool operator == (const graphEdge &t) const {
        return endPoint1 == t.endPoint1 && endPoint2 == t.endPoint2;
    }
    graphEdge() = default;
    graphEdge(string a, string b): endPoint1(a), endPoint2(b){};
};

struct graphEdgeHash{
    size_t operator() (const graphEdge &t) const {
        hash<string> hash;
        return hash(t.endPoint1 + t.endPoint2);
    }
};
#endif //AGREEMENTTESTING_HELPERDATASTRUCTURE_H
