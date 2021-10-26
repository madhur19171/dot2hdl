/*
 * MergeComponent.cpp
 *
 *  Created on: 13-Jun-2021
 *      Author: madhur
 */

#include "ComponentClass.h"

//Subclass for Entry type component
RouterComponent::RouterComponent(Component& c){
	index = c.index;
	moduleName = "Router";
	name = c.name;
	instanceName = moduleName + "_" + name;
	type = c.type;
	bbID = c.bbID;
	op = c.op;
	in = c.in;
	out = c.out;
	delay = c.delay;
	latency = c.latency;
	II = c.II;
	slots = c.slots;
	transparent = c.transparent;
	value = c.value;
	io = c.io;
	inputConnections = c.inputConnections;
	outputConnections = c.outputConnections;

	clk = c.clk;
	rst = c.rst;

	std::string IOPort_name = name;

	port_data_in = IOPort_name + "_data_in";
	port_valid_in = IOPort_name + "_valid_in";
	port_ready_in = IOPort_name + "_ready_in";

	port_data_out = IOPort_name + "_data_out";
	port_valid_out = IOPort_name + "_valid_out";
	port_ready_out = IOPort_name + "_ready_out";
}


//Returns the input/output declarations for top-module
std::string RouterComponent::getModuleIODeclaration(std::string tabs){
	std::string ret = "";
	//Start has only one input
	ret += tabs + "input " + generateVector(in.input[0].bit_size - 1, 0) + port_data_in + ",\n";
	ret += tabs + "input " + port_valid_in + ",\n";
	ret += tabs + "output " + port_ready_in + ",\n\n";

	ret += tabs + "output " + generateVector(out.output[0].bit_size - 1, 0) + port_data_out + ",\n";
	ret += tabs + "output " + port_valid_out + ",\n";
	ret += tabs + "input " + port_ready_out + ",\n";
	ret += "\n";

	return ret;
}

std::string RouterComponent::getVerilogParameters(){
	std::string ret;
	//This method of generating module parameters will work because Start node has
	//only 1 input and 1 output
	ret += "#(.N(" + std::to_string(getClusterSize()) + "), .INDEX(" + std::to_string(nodes[index].node_id) + "), .INPUTS(" + std::to_string(in.size) + "), .OUTPUTS(" + std::to_string(out.size) + "), ";
	ret += ".DATA_WIDTH(" + std::to_string(in.input[0].bit_size == 0 ? 1 : in.input[0].bit_size) + "), ";
	ret += ".TYPE_WIDTH(2), .REQUEST_WIDTH($clog2(" + std::to_string(out.size) + ")), ";
	ret += ".FlitPerPacket(6), .PhitPerFlit(1), ";
	ret += ".FIFO_DEPTH(" + std::to_string(nodes[index].fifodepth) + ")) ";
	//0 data size will lead to negative port length in verilog code. So 0 data size has to be made 1.
	return ret;
}


std::string RouterComponent::getInputOutputConnections(){
	std::string ret;

	ret += "\tassign " + clk + " = clk;\n";
	ret += "\tassign " + rst + " = rst;\n";

	//First input of Router component is connected to top module IO port
	ret += "\tassign " + inputConnections.at(0).data + " = " + port_data_in + ";\n";
	ret += "\tassign " + inputConnections.at(0).valid + " = " + port_valid_in + ";\n";
	ret += "\tassign " + port_ready_in + " = " + inputConnections.at(0).ready + ";\n";

	//First output of Router component is connected to top module IO port
	ret += "\tassign " + port_data_out + " = " + outputConnections.at(0).data + ";\n";
	ret += "\tassign " + port_valid_out + " = " + outputConnections.at(0).valid + ";\n";
	ret += "\tassign " + outputConnections.at(0).ready + " = " + port_ready_out + ";\n";


	InputConnection inConn;
	OutputConnection outConn;
	Component* connectedToComponent;
	int connectedFromPort, connectedToPort;
	for(auto it = io.begin(); it != io.end(); it++){
		connectedToComponent = (*it).first;
		connectedFromPort = (*it).second.first;
		connectedToPort = (*it).second.second;
		inConn = connectedToComponent->inputConnections[connectedToPort];
		outConn = outputConnections[connectedFromPort];
		ret += connectInputOutput(inConn, outConn);
	}

	return ret;
}

int RouterComponent::getClusterSize(){
	int clusterSize = 0;
	for(int i = 0; i < components_in_netlist; i++){
		if(nodes[i].type == COMPONENT_ROUTER && nodes[i].bbId == bbID)//Routers on same cluster have same bbID
			clusterSize++;
	}
	return clusterSize;
}






