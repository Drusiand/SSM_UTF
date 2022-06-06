/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * functions-grammar.h
 * Copyright (C) 2018 Konstantin Kozlov <mackoel@gmail.com>
 *
 * nlreg is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * nlreg is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _FUNCTIONS_GRAMMAR_H_
#define _FUNCTIONS_GRAMMAR_H_

 // based on github.com:pbharrin/Genetic-Prog.git

#include <string>
#include <vector>

void set_print_trace(int arg);

using namespace std;

class GrammarNode
{
public:
	string label = "GrammarNode";
	int precendence;
	int numChildren;
	GrammarNode* children[2];
	int child_type[2];
	GrammarNode() { children[0] = NULL; children[1] = NULL; } // to be used later
	virtual ~GrammarNode() { } // to be used later
	virtual double eval(vector<double>& inVal) = 0;  //setting the 0 makes it a PURE
	virtual void coprint(std::ostream &out) = 0;
	virtual GrammarNode* prune() = 0;
	virtual GrammarNode* clone() = 0; //make a deep copy of the current tree
	virtual void setScale(vector<double>& a, vector<double>& b) = 0;
	virtual string getLabel() { return label; }
	// Print function. It's virtual, so only one operator << is needed.
	virtual void printOn(ostream& os) { os << label; }
};

// Print operator for all classes that inherit from GrammarNode
inline ostream& operator << (ostream& os, GrammarNode& gnd)
{
	gnd.printOn(os);
	return os;
}

//class for storing constant values
class ConstNode : public GrammarNode {
	double *constVal;
	int constInd;
public:
	ConstNode();
	ConstNode(int preSetVal);
	ConstNode(int ind, double *preSetVal);
	virtual double eval(vector<double>& inVal);
	ConstNode* clone();
	virtual void coprint(std::ostream &out);
	virtual ConstNode* prune();
	virtual void setScale(vector<double>& a, vector<double>& b) { }
	//	virtual string getLabel();
	//	virtual void printOn (ostream& os) { os << "Const" << endl; };
};

//class for using inputs
class InputNode : public GrammarNode {
	int inputIndex;
	string inpname;
	bool scalePresent;
	double scale;
	double offset;
public:
	InputNode();
	InputNode(int inputId, string pname);
	virtual double eval(vector<double>& inVal);
	InputNode* clone();
	virtual void coprint(std::ostream &out);
	virtual InputNode* prune();
	virtual void setScale(vector<double>& a, vector<double>& b) {
		if (inputIndex >= 0) {
			scale = a[inputIndex]; offset = b[inputIndex]; scalePresent = true;
		}
		else if (children[0]) {
			children[0]->setScale(a, b);
		}
	}
	//	void setValues(int inIndex);
	//	virtual string getLabel();
	//	virtual void printOn (ostream& os) { os << "Input" << endl; };
};

//addition
class Add : public GrammarNode {
public:
	Add();
	virtual double eval(vector<double>& inVal);
	Add* clone();
	virtual void coprint(std::ostream &out);
	virtual Add* prune();
	virtual void setScale(vector<double>& a, vector<double>& b) {
		if (children[0]) children[0]->setScale(a, b);
		if (children[1]) children[1]->setScale(a, b);
	}
	//	virtual string getLabel();
	//	virtual void printOn (ostream& os) { os << "Add" << endl; };
};

//subtraction
class Subtract : public GrammarNode {
public:
	Subtract();
	virtual double eval(vector<double>& inVal);
	Subtract* clone();
	virtual void coprint(std::ostream &out);
	virtual Subtract* prune();
	virtual void setScale(vector<double>& a, vector<double>& b) {
		if (children[0]) children[0]->setScale(a, b);
		if (children[1]) children[1]->setScale(a, b);
	}
	//	virtual string getLabel();
	//	virtual void printOn (ostream& os) { os << "Subtract" << endl; };
};

//multiplication
class Multiply : public GrammarNode {
public:
	Multiply();
	virtual double eval(vector<double>& inVal);
	Multiply* clone();
	virtual void coprint(std::ostream &out);
	virtual Multiply* prune();
	virtual void setScale(vector<double>& a, vector<double>& b) {
		if (children[0]) children[0]->setScale(a, b);
		if (children[1]) children[1]->setScale(a, b);
	}
	//	virtual string getLabel();
	//	virtual void printOn (ostream& os) { os << "Multiply" << endl; };
};

//division
class Divide : public GrammarNode {
public:
	Divide();
	virtual double eval(vector<double>& inVal);
	Divide* clone();
	virtual void coprint(std::ostream &out);
	virtual Divide* prune();
	virtual void setScale(vector<double>& a, vector<double>& b) {
		if (children[0]) children[0]->setScale(a, b);
		if (children[1]) children[1]->setScale(a, b);
	}
	//	virtual string getLabel();
	//	virtual void printOn (ostream& os) { os << "Divide" << endl; };
};

//subtraction of const
class InputMinusConst : public GrammarNode {
public:
	InputMinusConst();
	virtual double eval(vector<double>& inVal);
	InputMinusConst* clone();
	virtual void coprint(std::ostream &out);
	virtual InputMinusConst* prune();
	virtual void setScale(vector<double>& a, vector<double>& b) {
		if (children[0]) children[0]->setScale(a, b);
		if (children[1]) children[1]->setScale(a, b);
	}
	//	virtual string getLabel();
	//	virtual void printOn (ostream& os) { os << "InputMinusConst" << endl; };
};

//subtraction from const NEW
class ConstMinusInput : public GrammarNode {
public:
	ConstMinusInput();
	virtual double eval(vector<double>& inVal);
	ConstMinusInput* clone();
	virtual void coprint(std::ostream &out);
	virtual ConstMinusInput* prune();
	virtual void setScale(vector<double>& a, vector<double>& b) {
		if (children[0]) children[0]->setScale(a, b);
		if (children[1]) children[1]->setScale(a, b);
	}
	//	virtual string getLabel();
	//	virtual void printOn (ostream& os) { os << "InputMinusConst" << endl; };
};

//rec subtraction of const
class RecInputMinusConst : public GrammarNode {
public:
	RecInputMinusConst();
	virtual double eval(vector<double>& inVal);
	RecInputMinusConst* clone();
	virtual void coprint(std::ostream &out);
	virtual RecInputMinusConst* prune();
	virtual void setScale(vector<double>& a, vector<double>& b) {
		if (children[0]) children[0]->setScale(a, b);
		if (children[1]) children[1]->setScale(a, b);
	}
	//	virtual string getLabel();
	//	virtual void printOn (ostream& os) { os << "RecInputMinusConst" << endl; };
};

//rec subtraction from const NEW
class RecConstMinusInput : public GrammarNode {
public:
	RecConstMinusInput();
	virtual double eval(vector<double>& inVal);
	RecConstMinusInput* clone();
	virtual void coprint(std::ostream &out);
	virtual RecConstMinusInput* prune();
	virtual void setScale(vector<double>& a, vector<double>& b) {
		if (children[0]) children[0]->setScale(a, b);
		if (children[1]) children[1]->setScale(a, b);
	}
	//	virtual string getLabel();
	//	virtual void printOn (ostream& os) { os << "RecInputMinusConst" << endl; };
};

class GrammarContainer {
public:
	GrammarContainer(vector<string>& measurements, int n_t) : predictors(measurements), tree(n_t) {
		n_trees = n_t;
		n_predictors = predictors.size();
		n_nodes_type_0 = 9;
		n_nodes_type_1 = 1;
		n_nodes_type_2 = 1;
	}
	void build_nth_tree(vector<int>& genotype, vector<double>& conc, int n, double *phenotype, int *phenomask);
	GrammarNode*get_nth_tree(int n) { return tree[n]; }
private:
	vector<string> predictors;
	vector<GrammarNode*> tree;
	int n_trees;
	int n_predictors;
	int n_nodes_type_0;
	int n_nodes_type_1;
	int n_nodes_type_2;
	int last_predictor;
	int last_const;
	GrammarNode* build_tree(vector<int>& genotype, vector<double>& conc, double *phenotype, int *phenomask);
	GrammarNode* find_node(int type, int gen, vector<double>& conc, double *phenotype, int& phenomask);
	GrammarNode* find_node_type_0(int gen, double *phenotype);
	GrammarNode* find_node_type_1(int gen, double conc, double *phenotype);
	GrammarNode* find_node_type_2(int gen, double *phenotype);
};
#endif // _FUNCTIONS_GRAMMAR_H_
