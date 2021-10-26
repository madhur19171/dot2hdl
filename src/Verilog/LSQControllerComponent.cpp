/*
 * LSQControllerComponent.cpp
 *
 *  Created on: 24-Jun-2021
 *      Author: madhur
 */


#include "ComponentClass.h"

//Subclass for Entry type component
LSQControllerComponent::LSQControllerComponent(Component& c){
	index = c.index;
	if(c.op == OPERATOR_READ_LSQ)
		moduleName = "lsq_load_op";
	else if(c.op == OPERATOR_WRITE_LSQ)
		moduleName = "lsq_store_op";
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

std::string LSQControllerComponent::getVerilogParameters(){
	std::string ret;
	//This method of generating module parameters will work because Start node has
	//only 1 input and 1 output
	ret += "#(.INPUTS(2), .OUTPUTS(2), ";
	ret += ".ADDRESS_SIZE(" + std::to_string(in.input[1].bit_size == 0 ? 1 : in.input[1].bit_size) + "), ";
	ret += ".DATA_SIZE(" + std::to_string(in.input[0].bit_size == 0 ? 1 : in.input[0].bit_size) + ")) ";
	//0 data size will lead to negative port length in verilog code. So 0 data size has to be made 1.
	return ret;
}
