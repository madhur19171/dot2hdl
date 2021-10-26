/*
 * GraphToVerilog.h
 *
 *  Created on: 31-May-2021
 *      Author: madhur
 */

#ifndef GRAPHTOVERILOG_H_
#define GRAPHTOVERILOG_H_

#pragma once

#include "Graph.h"

class GraphToVerilog{
public:
//	GraphToVerilog(DotReader dotReader);
	GraphToVerilog(std::string filen);


	void writeVerilogCode();
	void writeToFile();

	std::string& getVerilogCode() {
		return verilogCode;
	}

private:
	DotReader dotReader;
	std::string tabs = "";
	std::string verilogCode = "";


//	std::vector<std::pair<std::string, int>> allFloatingPointComponents =
//	{{OPERATOR_FADD, 0}, {OPERATOR_FSUB, 0}, {OPERATOR_FMUL, 0}, {OPERATOR_FDIV, 0},
//	{OPERATOR_FOEQ, 0}, {OPERATOR_FONE, 0},
//	{OPERATOR_FOGT, 0}, {OPERATOR_FOGE, 0},
//	{OPERATOR_FOLT, 0}, {OPERATOR_FOLE, 0},
//	{OPERATOR_FEXP, 0}, {OPERATOR_FLOG, 0}, {OPERATOR_FSQRT, 0}};





	std::vector<Component*> topModulePortComponents;

	std::vector<Component*> floatingPointComponents;

	void insertTab();
	void removeTab();
	//Writes port list for top module
	std::string writeTopModulePorts();
	//Generates a list of components which interface with top module port list
	void generateTopModulePortComponents();
	//writes wires that interconnect various modules inside top module
	std::string writeModulePortWires();
	//returns Instantiation all the modules present in the Design
	std::string writeModuleInstantiation();
	//returns verilog code connecting various input and outputs of components together
	std::string writeInputOutputConnections();
	//Writes "module <name>("
	std::string writeTopModuleName();
	//Defines a macro FLOATING_POINT if the design has floating point units
	std::string writeFloatingPointDefinition();
	//Writes "endmodule;"
	std::string writeEndModule();

	void setFloatingPointComponents();
	std::string generateVivadoScript();
};

#endif /* GRAPHTOVERILOG_H_ */
