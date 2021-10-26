/*
 * tFIFOComponent.cpp
 *
 *  Created on: 20-Jun-2021
 *      Author: madhur
 */


#include "ComponentClass.h"

//Subclass for Entry type component
FIFOComponent::FIFOComponent(Component& c){
	index = c.index;
	if(c.type == COMPONENT_TFIFO)
		moduleName = "transpFIFO_node";
	else if(c.type == COMPONENT_NFIFO)
		moduleName = "nontranspFIFO_node";
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
}

std::string FIFOComponent::getVerilogParameters(){
	std::string ret;
	//This method of generating module parameters will work because Start node has
	//only 1 input and 1 output
	ret += "#(.INPUTS(1), .OUTPUTS(1), ";
	ret += ".DATA_IN_SIZE(" + std::to_string(in.input[0].bit_size == 0 ? 1 : in.input[0].bit_size) + "), ";
	ret += ".DATA_OUT_SIZE(" + std::to_string(out.output[0].bit_size == 0 ? 1 : out.output[0].bit_size) + "), ";
	ret += ".FIFO_DEPTH(" + std::to_string(nodes[index].slots) + ")) ";
	//0 data size will lead to negative port length in verilog code. So 0 data size has to be made 1.
	return ret;
}

