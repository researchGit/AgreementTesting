//
// Created by Lei on 7/18/21.
//

#ifndef IMPROVEAGREEMENTTESTING_NEWICKREADER_H
#define IMPROVEAGREEMENTTESTING_NEWICKREADER_H

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "InputTreeNode.h"
#include <unordered_set>

using namespace std;

namespace newickFormatParser {
    struct inputTreeNode;

    typedef std::vector<inputTreeNode> childrenVector;

    struct inputTreeNode {
        string label_;
        childrenVector children_;
        double length_;
    };
}

BOOST_FUSION_ADAPT_STRUCT(
        newickFormatParser::inputTreeNode,
        (newickFormatParser::childrenVector, children_)
                (std::string, label_)
                (double, length_)
)

namespace newickFormatParser {
    namespace qi = boost::spirit::qi;
    namespace phoenix = boost::phoenix;

    struct newickGrammar : qi::grammar<std::string::const_iterator, inputTreeNode()> {
    public:
        newickGrammar() : newickGrammar::base_type(tree) {
            using phoenix::at_c;
            using phoenix::push_back;

            label %= qi::lexeme[+(qi::char_ - ':' - ')' - ',')];
            branch_length %= ':' >> qi::double_;
            subtree = -descendant_list[at_c<0>(qi::_val) = qi::_1]
                    >> -label[at_c<1>(qi::_val) = qi::_1]
                    >> -branch_length[at_c<2>(qi::_val) = qi::_1];

            descendant_list = '(' >> subtree[push_back(qi::_val, qi::_1)]
                                  >> *(',' >> subtree[push_back(qi::_val, qi::_1)])
                                  >> ')';
            tree %= subtree >> ';';
        }

    private:
        qi::rule<std::string::const_iterator, inputTreeNode()> tree, subtree;
        qi::rule<std::string::const_iterator, vector<inputTreeNode>()> descendant_list;
        qi::rule<std::string::const_iterator, double()> branch_length;
        qi::rule<std::string::const_iterator, std::string()> label;
    };
}

class NewickReader {
public:
    NewickReader() = default;

    ~NewickReader() = default;

    vector<newickFormatParser::inputTreeNode> parseNewickFile(string& filePath);

    newickFormatParser::inputTreeNode parseNewickString(string& newickString);

    void printInputTrees(vector<newickFormatParser::inputTreeNode> &inputTrees);

    void printConvertedInputTrees(vector<shared_ptr<InputTreeNode>> &inputTrees);

    vector<newickFormatParser::inputTreeNode> replaceBlankInternalLabels(
            vector<newickFormatParser::inputTreeNode> &inputTrees);

    vector<shared_ptr<InputTreeNode>> convertToNativeTreeFormat(vector<newickFormatParser::inputTreeNode> &rawInputTrees);

    int getTaxaLabelsCount(){return taxaLabels.size();}

private:
    string convert2newick(newickFormatParser::inputTreeNode &root, string newick);

    int labelCount = 0;

    unordered_set<string> taxaLabels;
};


#endif //IMPROVEAGREEMENTTESTING_NEWICKREADER_H
