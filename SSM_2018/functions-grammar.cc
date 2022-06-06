/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * functions-grammar.cc
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

 // based on github.com:pbharrin/Genetic-Prog.git

#include <iostream>
#include <math.h>
#include <QStack>
#include "functions-grammar.h"

#ifndef ARGVAL
#define ARGVAL 0.00390625 /* 1/256 */
#endif

static int PRINT_TRACE = 0;

void set_print_trace(int arg) { PRINT_TRACE = arg; }

using namespace std;

double int_to_double(int arg) {
	/*	int i_arg = ( arg < 22 ) ? (int)floor(exp(arg)) : (int)floor(exp(arg % 21));
		int left;
		unsigned short left16;
		unsigned short right16;
		right16 = (unsigned short)i_arg;
		left = i_arg >> (8 * sizeof(unsigned char));
		left16 = (unsigned short)left;
		double merged = (double)left16/(double)right16;*/
	double argval = ARGVAL;
	double merged = argval * arg;
	return merged;
}

//
//		ConstNode
//
ConstNode::ConstNode() {
	numChildren = 0;
	constVal = new double(rand() / (double)RAND_MAX);
	char str[20] = "";
	//sprintf(str, "Const: %f", constVal);/////////////////////
	label = str;
	precendence = -1;
}

ConstNode::ConstNode(int preSetVal) {
	numChildren = 0;
	constVal = new double(int_to_double(preSetVal));
	char str[20] = "";
	sprintf(str, "Const: %f", (*constVal));
	label = str;
	precendence = -1;
}

ConstNode::ConstNode(int ind, double *preSetVal) {
	numChildren = 0;
	constVal = preSetVal;
	constInd = ind;
	char str[20] = "";
	sprintf(str, "Const: %f", (*constVal));
	label = str;
	precendence = -1;
}

double ConstNode::eval(vector<double>& inVal) {
	return (*constVal);
}

/*
double ConstNode::eval(vector<double>& inVal, double& dvdt) {
	dvdt = 0;
	return constVal;
}
* */

void ConstNode::coprint(std::ostream& out) {
	out << "(" << (*constVal) << ")";
}

ConstNode* ConstNode::prune() {
	return this;
}

ConstNode* ConstNode::clone() {
	ConstNode* retTree = new ConstNode(constInd, constVal);
	return retTree;
}

//
//		InputNode
//
InputNode::InputNode(int inputId, string pname) {
	numChildren = 0;
	scalePresent = false;
	inputIndex = inputId;
	inpname = pname;
	char str[256] = "";
	sprintf(str, "%s[%d]", pname.c_str(), inputIndex);
	//	setValues(inputIndex);
	label = str;
	precendence = -1;
}

InputNode::InputNode() {
	numChildren = 1;
	scalePresent = false;
	inputIndex = -1;
	child_type[0] = 2;
	precendence = -1;
}

double InputNode::eval(vector<double>& inVal) {
	if (inputIndex >= 0) {
		return inVal[inputIndex];
	}
	else if (children[0]) {
		return children[0]->eval(inVal);
	}
	else {
		if (PRINT_TRACE > 1) cerr << "not defined in input" << endl;
		return 0.0;
	}
}

void InputNode::coprint(std::ostream &out) {
	if (inputIndex >= 0) {
		if (scalePresent) {
			out << "((" << inpname << "-" << offset << ")/" << scale << ")";
		}
		else {
			out << inpname;
		}
	}
	else if (children[0]) {
		children[0]->coprint(out);
	}
	else {
		if (PRINT_TRACE > 1) cerr << "not defined in input coprint" << endl;
		out << "(0)";
	}
}

//void InputNode::setValues(int inIndex){
//	char str[20] = "";
//	sprintf(str, "InputVal: %d", inIndex);
//	label = str;
//}

InputNode* InputNode::prune() {
	if (inputIndex < 0 && !children[0]) {
		//		cout << *this << endl;
		delete this;
		return NULL;
	}
	return this;
}

InputNode* InputNode::clone() {
	InputNode* retTree = new InputNode(inputIndex, inpname);
	return retTree;
}

//
//		Add
//
Add::Add() {
	numChildren = 2;
	child_type[0] = 0;
	child_type[1] = 0;
	label = "Add";
	precendence = 6;
}

double Add::eval(vector<double>& inVal) {
	if (children[0] && children[1]) {
		return children[0]->eval(inVal) + children[1]->eval(inVal);
	} else if (children[0]){
		return children[0]->eval(inVal);
	} else if (children[1]){
		return children[1]->eval(inVal);
	} else {
		if (PRINT_TRACE > 1) cerr << "left and right not defined in add"<<endl;
		return 0.0;
	}
}

/*
double Add::eval(vector<double>& inVal, double& dvdt){
	if (children[0] && children[1]){
		double dvdt1, dvdt2, ff;
		ff = children[0]->eval(inVal, dvdt1) + children[1]->eval(inVal, dvdt2);
		dvdt1 = dvdt1 + dvdt2;
		return ff;
	}
	else if (children[0]) {
		return children[0]->eval(inVal);
	}
	else if (children[1]) {
		return children[1]->eval(inVal);
	}
	else {
		if (PRINT_TRACE > 1) cerr << "left and right not defined in add" << endl;
		return 0.0;
	}
}

/*
double Add::eval(vector<double>& inVal, double& dvdt){
	if (children[0] && children[1]){
		double dvdt1, dvdt2, ff;
		ff = children[0]->eval(inVal, dvdt1) + children[1]->eval(inVal, dvdt2);
		dvdt1 = dvdt1 + dvdt2;
		return ff;
	}
	else {
		cerr << "left and right not defined in add"<<endl;
		return -1.0;
	}
}
*/

void Add::coprint(std::ostream &out) {
	if (children[0] && children[1]) {
		out << " (";
		children[0]->coprint(out);
		out << " + ";
		children[1]->coprint(out);
		out << ") ";
	}
	else if (children[0]) {
		out << " (";
		children[0]->coprint(out);
		out << ") ";
	}
	else if (children[1]) {
		out << " (";
		children[1]->coprint(out);
		out << ") ";
	}
	else {
		if (PRINT_TRACE > 1) cerr << "left and right not defined in add" << endl;
		out << " (0";
		out << ") ";
	}
}

Add* Add::prune() {
	if (children[0]) {
		children[0] = children[0]->prune();
	}
	if (children[1]) {
		children[1] = children[1]->prune();
	}
	if (!children[0] && !children[1]) {
		//		cout << *this << endl;
		delete this;
		return NULL;
	}
	return this;
}

Add* Add::clone() {
	Add* retNode = new Add();
	for (int i = 0; i < numChildren; i++) {
		retNode->children[i] = children[i]->clone();
	}
	return retNode;
}

//
//		Subtract
//
Subtract::Subtract() {
	numChildren = 2;
	child_type[0] = 0;
	child_type[1] = 0;
	label = "Subtract";
	precendence = 6;
}

double Subtract::eval(vector<double>& inVal) {
	if (children[0] && children[1]) {
		return children[0]->eval(inVal) - children[1]->eval(inVal);
	}
	else if (children[0]) {
		return children[0]->eval(inVal);
	}
	else if (children[1]) {
		return children[1]->eval(inVal);
	}
	else {
		if (PRINT_TRACE > 1) cerr << "left and right not defined in subtract" << endl;
		return 0.0;
	}
}

void Subtract::coprint(std::ostream &out) {
	if (children[0] && children[1]) {
		out << " (";
		children[0]->coprint(out);
		out << " - ";
		children[1]->coprint(out);
		out << ") ";
	}
	else if (children[0]) {
		out << " (";
		children[0]->coprint(out);
		out << ") ";
	}
	else if (children[1]) {
		out << " (";
		children[1]->coprint(out);
		out << ") ";
	}
	else {
		if (PRINT_TRACE > 1) cerr << "left and right not defined in subtract" << endl;
		out << " (0";
		out << ") ";
	}
}

Subtract* Subtract::prune() {
	if (children[0]) {
		children[0] = children[0]->prune();
	}
	if (children[1]) {
		children[1] = children[1]->prune();
	}
	if (!children[0] && !children[1]) {
		//		cout << *this << endl;
		delete this;
		return NULL;
	}
	return this;
}

Subtract* Subtract::clone() {
	Subtract* retNode = new Subtract();
	for (int i = 0; i < numChildren; i++) {
		retNode->children[i] = children[i]->clone();
	}
	return retNode;
}

//
//		Multiply
//
Multiply::Multiply() {
	numChildren = 2;
	child_type[0] = 0;
	child_type[1] = 0;
	label = "Multiply";
	precendence = 5;
}

double Multiply::eval(vector<double>& inVal) {
	if (children[0] && children[1]) {
		return children[0]->eval(inVal) * children[1]->eval(inVal);
	}
	else if (children[0]) {
		return children[0]->eval(inVal);
	}
	else if (children[1]) {
		return children[1]->eval(inVal);
	}
	else {
		if (PRINT_TRACE > 1) cerr << "left and right not defined in multiply" << endl;
		return 0.0;
	}
}

void Multiply::coprint(std::ostream &out) {
	if (children[0] && children[1]) {
		out << " (";
		children[0]->coprint(out);
		out << " * ";
		children[1]->coprint(out);
		out << ") ";
	}
	else if (children[0]) {
		out << " (";
		children[0]->coprint(out);
		out << ") ";
	}
	else if (children[1]) {
		out << " (";
		children[1]->coprint(out);
		out << ") ";
	}
	else {
		if (PRINT_TRACE > 1) cerr << "left and right not defined in multiply" << endl;
		out << " (0";
		out << ") ";
	}
}

Multiply* Multiply::prune() {
	if (children[0]) {
		children[0] = children[0]->prune();
	}
	if (children[1]) {
		children[1] = children[1]->prune();
	}
	if (!children[0] && !children[1]) {
		//		cout << *this << endl;
		delete this;
		return NULL;
	}
	return this;
}

Multiply* Multiply::clone() {
	Multiply* retNode = new Multiply();
	for (int i = 0; i < numChildren; i++) {
		retNode->children[i] = children[i]->clone();
	}
	return retNode;
}

//
//		Divide
//
Divide::Divide() {
	numChildren = 2;
	child_type[0] = 0;
	child_type[1] = 0;
	label = "Divide";
	precendence = 5;
}

double Divide::eval(vector<double>& inVal) {
	if (children[0] && children[1]) {
		return children[0]->eval(inVal) / children[1]->eval(inVal);
	} else if (children[0]){
		return children[0]->eval(inVal);
	} else if (children[1]){
		return 1/children[1]->eval(inVal);
	} else {
		if (PRINT_TRACE > 1) cerr << "left and right not defined in divide"<<endl;
		return 0.0;
	}
}

/*
double Divide::eval(vector<double>& inVal, double& dvdt){
	if (children[0] && children[1]){
		double dvdt1, dvdt2, ff, ff1, ff2;
		ff1 = children[0]->eval(inVal, dvdt1);
		ff2 = children[0]->eval(inVal, dvdt2);
		ff = ff1 / ff2;
		dvdt = (dvdt1 * ff2 - ff1 * dvdt2) / dvdt2 / dvdt2;
		return ff;
	}
	else if (children[0]) {
		return children[0]->eval(inVal);
	}
	else if (children[1]) {
		return 1 / children[1]->eval(inVal);
	}
	else {
		if (PRINT_TRACE > 1) cerr << "left and right not defined in divide" << endl;
		return 0.0;
	}
}

/*
double Divide::eval(vector<double>& inVal, double& dvdt){
	if (children[0] && children[1]){
		double dvdt1, dvdt2, ff, ff1, ff2;
		ff1 = children[0]->eval(inVal, dvdt1);
		ff2 = children[0]->eval(inVal, dvdt2);
		ff = ff1 / ff2;
		dvdt = (dvdt1 * ff2 - ff1 * dvdt2) / dvdt2 / dvdt2;
		return ff;
	}
	else {
		cerr << "left and right not defined in divide"<<endl;
		return -1.0;
	}
}
*/

void Divide::coprint(std::ostream &out) {
	if (children[0] && children[1]) {
		out << " (";
		children[0]->coprint(out);
		out << " / ";
		children[1]->coprint(out);
		out << ") ";
	}
	else if (children[0]) {
		out << " (";
		children[0]->coprint(out);
		out << ") ";
	}
	else if (children[1]) {
		out << " (1";
		out << " / ";
		children[1]->coprint(out);
		out << ") ";
	}
	else {
		if (PRINT_TRACE > 1) cerr << "left and right not defined in divide" << endl;
		out << " (0";
		out << ") ";
	}
}

Divide* Divide::prune() {
	if (children[0]) {
		children[0] = children[0]->prune();
	}
	if (children[1]) {
		children[1] = children[1]->prune();
	}
	if (!children[0] && !children[1]) {
		//		cout << *this << endl;
		delete this;
		return NULL;
	}
	return this;
}

Divide* Divide::clone() {
	Divide* retNode = new Divide;
	for (int i = 0; i < numChildren; i++) {
		retNode->children[i] = children[i]->clone();
	}
	return retNode;
}

//
//		InputMinusConst
//
InputMinusConst::InputMinusConst() {
	numChildren = 2;
	child_type[0] = 2;
	child_type[1] = 1;
	label = "InputMinusConst";
	precendence = 6;
}

double InputMinusConst::eval(vector<double>& inVal) {
	if (children[0] && children[1]) {
		return children[0]->eval(inVal) - children[1]->eval(inVal);
	} else if (children[0]){
		return children[0]->eval(inVal);
	} else if (children[1]){
		return -children[1]->eval(inVal);
	} else {
		if (PRINT_TRACE > 1) cerr << "left and right not defined in InputMinusConst" << endl;
		return 0.0;
	}
}

/*
double InputMinusConst::eval(vector<double>& inVal, double& dvdt){
	if (children[0] && children[1]){
		double ff, dvdt1, dvdt2;
		ff = children[0]->eval(inVal, dvdt1) - children[1]->eval(inVal, dvdt2);
		dvdt = dvdt1 - dvdt2;
		return ff;
	}
	else if (children[0]) {
		return children[0]->eval(inVal);
	}
	else if (children[1]) {
		return -children[1]->eval(inVal);
	}
	else {
		if (PRINT_TRACE > 1) cerr << "left and right not defined in InputMinusConst" << endl;
		return 0.0;
	}
}

/*
double InputMinusConst::eval(vector<double>& inVal, double& dvdt){
	if (children[0] && children[1]){
		double ff, dvdt1, dvdt2;
		ff = children[0]->eval(inVal, dvdt1) - children[1]->eval(inVal, dvdt2);
		dvdt = dvdt1 - dvdt2;
		return ff;
	}
	else {
		cerr << "left and right not defined in InputMinusConst" << endl;
		return -1.0;
	}
}
*/

void InputMinusConst::coprint(std::ostream &out) {
	if (children[0] && children[1]) {
		out << " (";
		children[0]->coprint(out);
		out << " - ";
		children[1]->coprint(out);
		out << ") ";
	}
	else if (children[0]) {
		out << " (";
		children[0]->coprint(out);
		out << ") ";
	}
	else if (children[1]) {
		out << " (";
		out << " - ";
		children[1]->coprint(out);
		out << ") ";
	}
	else {
		if (PRINT_TRACE > 1) cerr << "left and right not defined in InputMinusConst" << endl;
		out << " (";
		out << " 0 ";
		out << ") ";
	}
}

InputMinusConst* InputMinusConst::prune() {
	if (children[0]) {
		children[0] = children[0]->prune();
	}
	if (children[1]) {
		children[1] = children[1]->prune();
	}
	if (!children[0] && !children[1]) {
		//		cout << *this << endl;
		delete this;
		return NULL;
	}
	return this;
}

InputMinusConst* InputMinusConst::clone() {
	InputMinusConst* retNode = new InputMinusConst();
	for (int i = 0; i < numChildren; i++) {
		retNode->children[i] = children[i]->clone();
	}
	return retNode;
}

//
//		ConstMinusInput
//
ConstMinusInput::ConstMinusInput() {
	numChildren = 2;
	child_type[0] = 2;
	child_type[1] = 1;
	label = "ConstMinusInput";
	precendence = 6;
}

double ConstMinusInput::eval(vector<double>& inVal) {
	if (children[0] && children[1]) {
		return - children[0]->eval(inVal) + children[1]->eval(inVal);
	}
	else if (children[0]) {
		return -children[0]->eval(inVal);
	}
	else if (children[1]) {
		return +children[1]->eval(inVal);
	}
	else {
		if (PRINT_TRACE > 1) cerr << "left and right not defined in InputMinusConst" << endl;
		return 0.0;
	}
}

void ConstMinusInput::coprint(std::ostream &out) {
	if (children[0] && children[1]) {
		out << " (-";
		children[0]->coprint(out);
		out << " + ";
		children[1]->coprint(out);
		out << ") ";
	}
	else if (children[0]) {
		out << " (-";
		children[0]->coprint(out);
		out << ") ";
	}
	else if (children[1]) {
		out << " (";
		children[1]->coprint(out);
		out << ") ";
	}
	else {
		if (PRINT_TRACE > 1) cerr << "left and right not defined in InputMinusConst" << endl;
		out << " (";
		out << " 0 ";
		out << ") ";
	}
}

ConstMinusInput* ConstMinusInput::prune() {
	if (children[0]) {
		children[0] = children[0]->prune();
	}
	if (children[1]) {
		children[1] = children[1]->prune();
	}
	if (!children[0] && !children[1]) {
		//		cout << *this << endl;
		delete this;
		return NULL;
	}
	return this;
}

ConstMinusInput* ConstMinusInput::clone() {
	ConstMinusInput* retNode = new ConstMinusInput();
	for (int i = 0; i < numChildren; i++) {
		retNode->children[i] = children[i]->clone();
	}
	return retNode;
}

//
//		RecInputMinusConst
//
RecInputMinusConst::RecInputMinusConst() {
	numChildren = 2;
	child_type[0] = 2;
	child_type[1] = 1;
	label = "RecInputMinusConst";
	precendence = 6;
}

double RecInputMinusConst::eval(vector<double>& inVal) {
	if (children[0] && children[1]) {
		return 1 / (children[0]->eval(inVal) - children[1]->eval(inVal));
	}
	else if (children[0]) {
		return 1 / children[0]->eval(inVal);
	}
	else if (children[1]) {
		return -1 / children[1]->eval(inVal);
	}
	else {
		if (PRINT_TRACE > 1) cerr << "left and right not defined in RecInputMinusConst" << endl;
		return 1.0;
	}
}

void RecInputMinusConst::coprint(std::ostream &out) {
	if (children[0] && children[1]) {
		out << " (1/(";
		children[0]->coprint(out);
		out << " - ";
		children[1]->coprint(out);
		out << ")) ";
	}
	else if (children[0]) {
		out << " (1/(";
		children[0]->coprint(out);
		out << ")) ";
	}
	else if (children[1]) {
		out << " (-1/(";
		children[1]->coprint(out);
		out << ")) ";
	}
	else {
		if (PRINT_TRACE > 1) cerr << "left and right not defined in InputMinusConst" << endl;
		out << " (1";
		out << ") ";
	}
}

RecInputMinusConst* RecInputMinusConst::prune() {
	if (children[0]) {
		children[0] = children[0]->prune();
	}
	if (children[1]) {
		children[1] = children[1]->prune();
	}
	if (!children[0] && !children[1]) {
		//		cout << *this << endl;
		delete this;
		return NULL;
	}
	return this;
}

RecInputMinusConst* RecInputMinusConst::clone() {
	RecInputMinusConst* retNode = new RecInputMinusConst();
	for (int i = 0; i < numChildren; i++) {
		retNode->children[i] = children[i]->clone();
	}
	return retNode;
}

//
//		RecConstMinusInput
//
RecConstMinusInput::RecConstMinusInput() {
	numChildren = 2;
	child_type[0] = 2;
	child_type[1] = 1;
	label = "RecConstMinusInput";
	precendence = 6;
}

double RecConstMinusInput::eval(vector<double>& inVal) {
	if (children[0] && children[1]) {
		return 1 / (- children[0]->eval(inVal) + children[1]->eval(inVal));
	}
	else if (children[0]) {
		return -1 / children[0]->eval(inVal);
	}
	else if (children[1]) {
		return 1 / children[1]->eval(inVal);
	}
	else {
		if (PRINT_TRACE > 1) cerr << "left and right not defined in RecInputMinusConst" << endl;
		return 1.0;
	}
}

void RecConstMinusInput::coprint(std::ostream &out) {
	if (children[0] && children[1]) {
		out << " (1/(-";
		children[0]->coprint(out);
		out << " + ";
		children[1]->coprint(out);
		out << ")) ";
	}
	else if (children[0]) {
		out << " (-1/(";
		children[0]->coprint(out);
		out << ")) ";
	}
	else if (children[1]) {
		out << " (1/(";
		children[1]->coprint(out);
		out << ")) ";
	}
	else {
		if (PRINT_TRACE > 1) cerr << "left and right not defined in InputMinusConst" << endl;
		out << " (1";
		out << ") ";
	}
}

RecConstMinusInput* RecConstMinusInput::prune() {
	if (children[0]) {
		children[0] = children[0]->prune();
	}
	if (children[1]) {
		children[1] = children[1]->prune();
	}
	if (!children[0] && !children[1]) {
		//		cout << *this << endl;
		delete this;
		return NULL;
	}
	return this;
}

RecConstMinusInput* RecConstMinusInput::clone() {
	RecConstMinusInput* retNode = new RecConstMinusInput();
	for (int i = 0; i < numChildren; i++) {
		retNode->children[i] = children[i]->clone();
	}
	return retNode;
}

/*
 * expr = grule(op(expr, expr), X, (X - Y), 1/(X - Y)),
 * Y = grule(0.1, 0.3, 0.5, 0.7, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0),
 * op = grule(`+`, `-`, `*`, `/`),
 * X = grule(@PREDICTORS@)

 * expr = grule((expr + expr), (expr - expr), (expr * expr), (expr / expr), X, (X - Y), 1/(X - Y)),
 * Y = grule(0.1, 0.3, 0.5, 0.7, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0),
 * X = grule(@PREDICTORS@)


 */


GrammarNode* GrammarContainer::find_node_type_0(int gen, double *phenotype)
{
	GrammarNode*n;
	int ge = gen % n_nodes_type_0;
	//cout << " ge= " << ge << " ";
	switch (ge) {
	case 0:
		n = new Add();
		break;
	case 1:
		n = new Subtract();
		break;
	case 2:
		n = new Multiply();
		break;
	case 3:
		n = new Divide();
		break;
	case 4:
		n = new InputNode();
		break;
	case 5:
			this->last_const = 0;
		n = new InputMinusConst();
		break;
	case 6:
			this->last_const = 0;			
		n = new RecInputMinusConst();
		break;
	case 7:
			this->last_const = this->n_predictors;
		n = new ConstMinusInput();
		break;
	case 8:
			this->last_const = this->n_predictors;
		n = new RecConstMinusInput();
		break;
	default:
		cout << "Error find node type 0" << gen << endl;
	}
	//cout << " ge= " << (long double)ge << " ";
	(*phenotype) = (long double)ge;
	return n;
}

GrammarNode* GrammarContainer::find_node_type_1(int gen, double conc, double *phenotype)
{
	GrammarNode*n;
	(*phenotype) = conc;
	//cout << " conc " << conc << " ";
	n = new ConstNode(gen, phenotype);
	return n;
}

GrammarNode* GrammarContainer::find_node_type_2(int gen, double *phenotype)
{
	GrammarNode*n;
	int ge = gen % n_predictors;
	this->last_predictor = ge + this->last_const;
	n = new InputNode(ge, predictors[ge]);
	//cout << " ge= " << (long double)ge << " ";
	(*phenotype) = (long double)ge;
	return n;
}

GrammarNode* GrammarContainer::find_node(int type, int gen, vector<double>& conc, double *phenotype, int& phenomask)
{
	GrammarNode*n;
	switch (type) {
	case 0:
		phenomask = 0;
		n = find_node_type_0(gen, phenotype);
		break;
	case 1:
		phenomask = 1;
		n = find_node_type_1(gen, conc[this->last_predictor], phenotype);
		break;
	case 2:
		phenomask = 2;
		n = find_node_type_2(gen, phenotype);
		break;
	default:
		cout << "Error find node " << type << endl;
	}
	return n;
}

/* type:
 * 0 - expr
 * 1 - const
 * 2 - predictor
 * (3 - operation?)
 */

GrammarNode* GrammarContainer::build_tree(vector<int>& genotype, vector<double>& conc, double *phenotype, int *phenomask)
{
	struct StackNode {
		GrammarNode*kernel;
		int side;
		int child_type;
		StackNode(GrammarNode*orig, int p, int ct) {
			kernel = orig;
			side = p;
			child_type = ct;
		}
	};
	GrammarNode*root_expr, *child;
	StackNode *curr;
	QStack<StackNode*> stack;
	this->last_predictor = 0;
	this->last_const = 0;
	int i = 0;
	int curr_type = 0;

	root_expr = find_node(curr_type, genotype[i], conc, &phenotype[i], phenomask[i]);
	i++;
	//cout << "g " << genotype[i - 1] << ' ' << i - 1 << ' ' << *root_expr << endl;
	//cout << "p " << phenotype[i - 1] << ' ' << i - 1 << ' ' << *root_expr << endl;

	if (root_expr->numChildren == 2) {
		stack.push(new StackNode(root_expr, 1, root_expr->child_type[1]));
	}
	if (root_expr->numChildren > 0) {
		stack.push(new StackNode(root_expr, 0, root_expr->child_type[0]));
	}
	i = (i < genotype.size()) ? i : 0;
	while (!stack.isEmpty() && i < genotype.size()) {
		curr = stack.pop();
		curr_type = curr->child_type;
		child = find_node(curr_type, genotype[i], conc, &phenotype[i], phenomask[i]);
		i++;
	//	cout << "BEFORE" << endl;
	//	cout << "g " << genotype[i - 1] << ' ' << i - 1 << ' ' << *child << endl;
	//	cout << "p " << phenotype[i - 1] << ' ' << i - 1 << ' ' << *child << endl;
		curr->kernel->children[curr->side] = child;
		if (child->numChildren == 2) {
			stack.push(new StackNode(child, 1, child->child_type[1]));
		}
		if (child->numChildren > 0) {
			stack.push(new StackNode(child, 0, child->child_type[0]));
		}
	//	cout << "AFTER" << endl;

	///	cout << "g " << genotype[i - 1] << ' ' << i - 1 << ' ' << *child << endl;
	//	cout << "p " << phenotype[i - 1] << ' ' << i - 1 << ' ' << *child << endl;

	}

	while (!stack.isEmpty()) {
		curr = stack.pop();
		curr->kernel->children[curr->side] = NULL;
		//delete curr;	
	}

	root_expr = root_expr->prune();
	if (!root_expr) root_expr = new ConstNode(-1, new double(0.0));
	
	return root_expr;
}

void GrammarContainer::build_nth_tree(vector<int>& genotype, vector<double>& conc, int n,  double *phenotype, int *phenomask)
{
	tree[n] = build_tree(genotype, conc, phenotype, phenomask);
	//tree.insert(tree.begin() + n, build_tree(genotype, conc, phenotype, phenomask));
}
