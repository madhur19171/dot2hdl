/*
 * GraphToVerilog.cpp
 *
 *  Created on: 31-May-2021
 *      Author: madhur
 */

#include "GraphToVerilog.h"

//Constructor
//GraphToVerilog::GraphToVerilog(DotReader dotReader){
GraphToVerilog::GraphToVerilog(std::string filen){
	//	this->dotReader = dotReader;

	DotReader dotReader(filen);
	dotReader.lineReader();
	this->dotReader = dotReader;
	std::cout << "Dot Read for Verilog" << std::endl;

	std::cout <<"Generating DFS" << std::endl;

	Graph graph(dotReader);
		graph.generateForwardingTable();
//	graph.startBFS();

	verilogCode = "";
	tabs = "";
}


void GraphToVerilog::writeToFile(){

	generateVivadoScript();

	std::string file_n = dotReader.getFileName() + ".v";
	std::ofstream outStream(file_n);
	writeVerilogCode();
	outStream << getVerilogCode() << std::endl;

	outStream.close();
}

//Function that calls other functions to append their generated verilog code snippet
//The sequence in which the sub-functions are called determines in which sequence the snippet
//will appear in the final code.
//Eg. Module Name is the first thing of a verilog code
//Followed by Top Module Ports Input output declarations
//Followed by declaring wires that connect various sub components in the top module
void GraphToVerilog::writeVerilogCode(){

	verilogCode += writeFloatingPointDefinition();

	verilogCode += writeTopModuleName();


	insertTab();
	verilogCode += writeTopModulePorts();
	removeTab();


	insertTab();
	verilogCode += writeModulePortWires();
	removeTab();

	insertTab();
	verilogCode += writeInputOutputConnections();
	removeTab();

	insertTab();
	verilogCode += writeModuleInstantiation();
	removeTab();

	verilogCode += writeEndModule();
}

std::string GraphToVerilog::generateVivadoScript(){
	std::string ret = "";

	setFloatingPointComponents();

	std::ofstream srcTCL, verilogWrappers;
	if(floatingPointComponents.size() != 0){
		srcTCL.open(dotReader.getFileName() + "_floating_point_units.tcl");
		verilogWrappers.open(dotReader.getFileName() + "_floating_point_wrappers.v");
	}

	for(auto it = allFloatingPointComponents.begin(); it != allFloatingPointComponents.end(); it++){
		if((*it).second != 0){
			for(auto jt = floatingPointComponents.begin(); jt != floatingPointComponents.end(); jt++){
				if((*jt)->op == (*it).first){
					srcTCL << ((FloatingArithmeticComponent*)(*jt))->generateVivadoTCLScript();
					srcTCL << std::endl << std::endl;

					verilogWrappers << ((FloatingArithmeticComponent*)(*jt))->generateVerilogWrapper();
					verilogWrappers << std::endl << std::endl << std::endl << std::endl;
					break;
				}
			}
		}
	}

	if(floatingPointComponents.size() != 0){
		srcTCL.close();
	}

	return ret;
}

void GraphToVerilog::setFloatingPointComponents(){
	for(auto it = dotReader.getComponentList().begin(); it != dotReader.getComponentList().end(); it++){
		if((*it)->type == COMPONENT_OPERATOR){
			for(auto jt = allFloatingPointComponents.begin(); jt != allFloatingPointComponents.end(); jt++){
				if((*jt).first == (*it)->op){
					floatingPointComponents.push_back(*it);
					(*jt).second++;
				}
			}
		}
	}
}

std::string GraphToVerilog::writeFloatingPointDefinition(){
	std::string ret = "";

	if(floatingPointComponents.size() != 0){

		ret += "`ifndef FLOATING_POINT\n";
		ret += "\t`define FLOATING_POINT\n";
		ret += "`endif\n\n\n";
	}

	return ret;
}

//Declares the top module.
std::string GraphToVerilog::writeTopModuleName(){
	std::string moduleName;

	std::string filennn = dotReader.getFileName();

	unsigned long pos = filennn.rfind("_optimized");
	if(pos != std::string::npos){
		filennn = substring(filennn, 0, pos);
	}

	moduleName = "module " + filennn + "(\n";

	return moduleName;
}

//This function populates the topModulePortComponents which will contain
//All the components that need to have input output declarations in the top module
//port list
void GraphToVerilog::generateTopModulePortComponents(){
	for(auto it = dotReader.getComponentList().begin(); it != dotReader.getComponentList().end(); it++){
		if((*it)->type == COMPONENT_START ||
				(*it)->type == COMPONENT_END ||
				(*it)->type == COMPONENT_MC ||
				(*it)->type == COMPONENT_LSQ ||
				(*it)->type == COMPONENT_ROUTER){
			topModulePortComponents.push_back((*it));
		}
	}
}

//IO Ports are declared for components which are directly interfacing
//with the outside world.
//The following component types are to be provided IO Ports:
//1. Entry
//2. Exit
//3. LSQ
//4. MC

std::string GraphToVerilog::writeTopModulePorts(){
	std::string topModulePortList;
	generateTopModulePortComponents();

	//Clock and Reset signal are always present in the design ports
	topModulePortList += tabs + "input clk,\n" + tabs + "input rst,\n\n";

	//Writing IO Ports:

	for(auto it = topModulePortComponents.begin(); it != topModulePortComponents.end(); it++){
		topModulePortList += (*it)->getModuleIODeclaration(tabs);
	}

	//Erasing this necessary to get rid of an extra "," just before closing top module port declarations
	topModulePortList = topModulePortList.erase(topModulePortList.size() - 3, 1);

	topModulePortList += ");\n\n";

	return topModulePortList;
}



//This function accesses the getModulePortDeclarations function of each component
//(Because every component needs wires to connect) to generate
//a declaration of all wires of input/output connections of the sub modules
//They include the data, valid and ready signal for all input and outputs of that component
//Additional ports can be added if needed for a particular component by overriding getModulePortDeclarations
//function in a sub class
std::string GraphToVerilog::writeModulePortWires(){
	std::string modulePortWires;

	for(auto it = dotReader.getComponentList().begin(); it != dotReader.getComponentList().end(); it++){
		modulePortWires += (*it)->getModulePortDeclarations(tabs);
		modulePortWires += "\n";
	}

	return modulePortWires;
}


std::string GraphToVerilog::writeModuleInstantiation(){
	std::string moduleInstances = "";

	for(auto it = dotReader.getComponentList().begin(); it != dotReader.getComponentList().end(); it++){
		moduleInstances += (*it)->getModuleInstantiation(tabs);
		moduleInstances += "\n\n";
	}

	return moduleInstances;
}


std::string GraphToVerilog::writeInputOutputConnections(){
	std::string inputoutput = "\n\n";

	for(auto it = dotReader.getComponentList().begin(); it != dotReader.getComponentList().end(); it++){
		inputoutput += (*it)->getInputOutputConnections();
		inputoutput += "\n";
	}

	return inputoutput;
}


std::string GraphToVerilog::writeEndModule(){
	return "endmodule\n";
}


void GraphToVerilog::insertTab(){
	tabs += "\t";
}

void GraphToVerilog::removeTab(){
	if(tabs.size() != 0)
		tabs = tabs.erase(0, 1);
}

