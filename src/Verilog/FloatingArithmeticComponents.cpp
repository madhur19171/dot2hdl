/*
 * FloatingArithmeticComponents.cpp
 *
 *  Created on: 15-Jul-2021
 *      Author: madhur
 */

#include "ComponentClass.h"

std::string vivado_version = "7.1";

//Constructor for abstract class
FloatingArithmeticComponent::FloatingArithmeticComponent(Component& c){
	index = c.index;
	moduleName = c.moduleName;
	name = c.name;
	instanceName = c.instanceName;
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

	max_latency = 0x0fffffff;

	clk = c.clk;
	rst = c.rst;
}

std::string FloatingArithmeticComponent::getVivadoTCLScript(){
	std::string ret = "";

	ret += "# BEGIN Vivado Commands\n\
set vivado_ver [version -short]\n\
set fpo_ver " + vivado_version + "\n";
	ret +=	"if {[regexp -nocase {2015\\.1.*} $vivado_ver match]} {\n\
	set fpo_ver 7.0\n\
}\n\
create_ip -name floating_point -version $fpo_ver -vendor xilinx.com -library ip -module_name " + moduleName + "_full_dsp_32\n\
# BEGIN Vivado Commands\n\
# BEGIN Vivado Parameters\n\
set_property -dict [list CONFIG.a_precision_type Single \\\n\
							CONFIG.a_tuser_width 1 \\\n\
							CONFIG.add_sub_value " + add_sub + " \\\n\
							CONFIG.b_tuser_width 1 \\\n\
							CONFIG.c_a_exponent_width 8 \\\n\
							CONFIG.c_a_fraction_width 24 \\\n\
							CONFIG.c_compare_operation " + compare_operation + " \\\n\
							CONFIG.c_has_divide_by_zero false \\\n\
							CONFIG.c_has_invalid_op false \\\n\
							CONFIG.c_has_overflow false \\\n\
							CONFIG.c_has_underflow false \\\n\
							CONFIG.c_latency " + std::to_string(latency) + " \\\n\
							CONFIG.c_mult_usage " + dsp48_uage + " \\\n\
							CONFIG.c_optimization Speed_Optimized \\\n\
							CONFIG.c_rate 1 \\\n\
							CONFIG.c_result_exponent_width 8 \\\n\
							CONFIG.c_result_fraction_width 24 \\\n\
							CONFIG.component_name " + moduleName + "_full_dsp_32 \\\n\
							CONFIG.flow_control NonBlocking \\\n\
							CONFIG.has_a_tlast false \\\n\
							CONFIG.has_a_tuser false \\\n\
							CONFIG.has_aclken true \\\n\
							CONFIG.has_aresetn false \\\n\
							CONFIG.has_b_tlast false \\\n\
							CONFIG.has_b_tuser false \\\n\
							CONFIG.has_operation_tlast false \\\n\
							CONFIG.has_operation_tuser false \\\n\
							CONFIG.has_result_tready false \\\n\
							CONFIG.maximum_latency false \\\n\
							CONFIG.operation_tuser_width 1 \\\n\
							CONFIG.operation_type " + operation_type + " \\\n\
							CONFIG.result_precision_type Single \\\n\
							CONFIG.result_tlast_behv Null] -objects [get_ips " + moduleName + "_full_dsp_32] -quiet\n\
# END Vivado Parameters\n\
set_property generate_synth_checkpoint false [get_files " + moduleName + "_full_dsp_32.xci]\n\
generate_target {synthesis simulation} [get_files " + moduleName + "_full_dsp_32.xci]\n";

	return ret;
}


std::string FloatingArithmeticComponent::getVerilogWrapper(){
	std::string ret = "";

	bool singleOp = in.size == 1;

	ret += "module " + moduleName + "_wrapper\n\
#(parameter\n\
    ID         = 1,\n\
    NUM_STAGE  = 7,\n\
    din0_WIDTH = 32,\n\
    din1_WIDTH = 32,\n\
    dout_WIDTH = 32\n\
)(\n\
    input  wire                  clk,\n\
    input  wire                  reset,\n\
    input  wire                  ce,\n\
    input  wire [din0_WIDTH-1:0] din0,\n";

	if(singleOp)
		ret += "//";//Commenting out second input

	ret += "\tinput  wire [din1_WIDTH-1:0] din1,\n\
    output wire [dout_WIDTH-1:0] dout\n\
);\n\
//------------------------Local signal-------------------\n\
wire                  aclk;\n\
wire                  aclken;\n\
wire                  a_tvalid;\n\
wire [31:0]           a_tdata;\n\
wire                  b_tvalid;\n\
wire [31:0]           b_tdata;\n\
wire                  r_tvalid;\n\
wire [31:0]           r_tdata;\n\
reg  [din0_WIDTH-1:0] din0_buf1;\n\
reg  [din1_WIDTH-1:0] din1_buf1;\n\
reg                   ce_r;\n\
wire [dout_WIDTH-1:0] dout_i;\n\
reg  [dout_WIDTH-1:0] dout_r;\n\
//------------------------Instantiation------------------\n";
	ret += moduleName + "_full_dsp_32 " + moduleName + "_full_dsp_32_instance (\n\
    .aclk                 ( aclk ),\n\
    .aclken               ( aclken ),\n\
    .s_axis_a_tvalid      ( a_tvalid ),\n\
    .s_axis_a_tdata       ( a_tdata ),\n";
	if(singleOp)
		ret += "/*";
	ret += "\t.s_axis_b_tvalid      ( b_tvalid ),\n\
    .s_axis_b_tdata       ( b_tdata ),\n";
	if(singleOp)
		ret += "*/";
	ret += "\t.m_axis_result_tvalid ( r_tvalid ),\n\
    .m_axis_result_tdata  ( r_tdata )\n\
);\n\
//------------------------Body---------------------------\n\
assign aclk     = clk;\n\
assign aclken   = ce_r;\n\
assign a_tvalid = 1'b1;\n\
assign a_tdata  = din0_buf1;\n\
assign b_tvalid = 1'b1;\n\
assign b_tdata  = din1_buf1;\n\
assign dout_i   = r_tdata;\n\
\n\
always @(posedge clk) begin\n\
    if (ce) begin\n\
        din0_buf1 <= din0;\n";
	if(singleOp)
		ret+= "//";
	ret += "\t\tdin1_buf1 <= din1;\n\
    end\n\
end\n\
\n\
always @ (posedge clk) begin\n\
    ce_r <= ce;\n\
end\n\
\n\
always @ (posedge clk) begin\n\
    if (ce_r) begin\n\
        dout_r <= dout_i;\n\
    end\n\
end\n\
\n\
assign dout = ce_r?dout_i:dout_r;\n\
endmodule";

	return ret;
}


std::string FloatingArithmeticComponent::getVerilogParameters(){
	std::string ret;

	ret += "#(.INPUTS(" + std::to_string(in.size) + "), .OUTPUTS(" + std::to_string(out.size) + "), ";
	ret += ".DATA_IN_SIZE(" + std::to_string(in.input[0].bit_size == 0 ? 1 : in.input[0].bit_size) + "), ";
	ret += ".DATA_OUT_SIZE(" + std::to_string(out.output[0].bit_size == 0 ? 1 : out.output[0].bit_size) + "), ";
	ret += ".LATENCY(" + std::to_string(latency) + ")) ";
	//0 data size will lead to negative port length in verilog code. So 0 data size has to be made 1.
	return ret;
}



//FADD Component:
FaddComponent::FaddComponent(Component& c) : FloatingArithmeticComponent(c){

	max_latency = 11;

	index = c.index;
	moduleName = "fadd_op";
	name = c.name;
	instanceName =  moduleName + "_" + name;
	type = c.type;
	bbID = c.bbID;
	op = c.op;
	in = c.in;
	out = c.out;
	delay = c.delay;
	latency = c.latency <= max_latency ? latency : max_latency;
	II = c.II;
	slots = c.slots;
	transparent = c.transparent;
	value = c.value;
	io = c.io;
	inputConnections = c.inputConnections;
	outputConnections = c.outputConnections;

	clk = c.clk;
	rst = c.rst;

	dsp48_uage = "Full_Usage";
	add_sub = "Add";
	compare_operation = "Programmable";
	operation_type = "Add_Subtract";
};
std::string FaddComponent::generateVivadoTCLScript(){
	std::string ret = "";
	//	ret += getVivadoTCLScript(vivado_version, "Add_Subtract", "Add", "Programmable");
	ret += getVivadoTCLScript();
	return ret;
}

std::string FaddComponent::generateVerilogWrapper(){
	return getVerilogWrapper();
}


//FSUB Component:
FsubComponent::FsubComponent(Component& c) : FloatingArithmeticComponent(c){

	max_latency = 11;

	index = c.index;
	moduleName = "fsub_op";
	name = c.name;
	instanceName =  moduleName + "_" + name;
	type = c.type;
	bbID = c.bbID;
	op = c.op;
	in = c.in;
	out = c.out;
	delay = c.delay;
	latency = c.latency <= max_latency ? latency : max_latency;
	II = c.II;
	slots = c.slots;
	transparent = c.transparent;
	value = c.value;
	io = c.io;
	inputConnections = c.inputConnections;
	outputConnections = c.outputConnections;

	clk = c.clk;
	rst = c.rst;

	dsp48_uage = "Full_Usage";
	add_sub = "Subtract";
	compare_operation = "Programmable";
	operation_type = "Add_Subtract";
};
std::string FsubComponent::generateVivadoTCLScript(){
	std::string ret = "";
	//	ret += getVivadoTCLScript(vivado_version, "Add_Subtract", "Sub", "Programmable");
	ret += getVivadoTCLScript();
	return ret;
}

std::string FsubComponent::generateVerilogWrapper(){
	return getVerilogWrapper();
}



//FMUL Component:
FmulComponent::FmulComponent(Component& c) : FloatingArithmeticComponent(c){

	max_latency = 8;

	index = c.index;
	moduleName = "fmul_op";
	name = c.name;
	instanceName =  moduleName + "_" + name;
	type = c.type;
	bbID = c.bbID;
	op = c.op;
	in = c.in;
	out = c.out;
	delay = c.delay;
	latency = c.latency <= max_latency ? latency : max_latency;
	II = c.II;
	slots = c.slots;
	transparent = c.transparent;
	value = c.value;
	io = c.io;
	inputConnections = c.inputConnections;
	outputConnections = c.outputConnections;


	clk = c.clk;
	rst = c.rst;

	dsp48_uage = "Full_Usage";
	add_sub = "Both";
	compare_operation = "Programmable";
	operation_type = "Multiply";
};
std::string FmulComponent::generateVivadoTCLScript(){
	std::string ret = "";
	//	ret += getVivadoTCLScript(vivado_version, "Multiply", "Both", "Programmable");
	ret += getVivadoTCLScript();
	return ret;
}

std::string FmulComponent::generateVerilogWrapper(){
	return getVerilogWrapper();
}


//FDIV Component:
FdivComponent::FdivComponent(Component& c) : FloatingArithmeticComponent(c){

	max_latency = 28;

	index = c.index;
	moduleName = "fdiv_op";
	name = c.name;
	instanceName =  moduleName + "_" + name;
	type = c.type;
	bbID = c.bbID;
	op = c.op;
	in = c.in;
	out = c.out;
	delay = c.delay;
	latency = c.latency <= max_latency ? latency : max_latency;
	II = c.II;
	slots = c.slots;
	transparent = c.transparent;
	value = c.value;
	io = c.io;
	inputConnections = c.inputConnections;
	outputConnections = c.outputConnections;


	clk = c.clk;
	rst = c.rst;

	dsp48_uage = "No_Usage";
	add_sub = "Both";
	compare_operation = "Programmable";
	operation_type = "Divide";
};
std::string FdivComponent::generateVivadoTCLScript(){
	std::string ret = "";
	//	ret += getVivadoTCLScript(vivado_version, "Divide", "Both", "Programmable");
	ret += getVivadoTCLScript();
	return ret;
}

std::string FdivComponent::generateVerilogWrapper(){
	return getVerilogWrapper();
}



//FOEQ Component:
FoeqComponent::FoeqComponent(Component& c) : FloatingArithmeticComponent(c){

	max_latency = 2;

	index = c.index;
	moduleName = "fcmp_oeq_op";
	name = c.name;
	instanceName =  moduleName + "_" + name;
	type = c.type;
	bbID = c.bbID;
	op = c.op;
	in = c.in;
	out = c.out;
	delay = c.delay;
	latency = c.latency <= max_latency ? latency : max_latency;
	II = c.II;
	slots = c.slots;
	transparent = c.transparent;
	value = c.value;
	io = c.io;
	inputConnections = c.inputConnections;
	outputConnections = c.outputConnections;


	clk = c.clk;
	rst = c.rst;

	dsp48_uage = "No_Usage";
	add_sub = "Both";
	compare_operation = "Equal";
	operation_type = "Compare";
};
std::string FoeqComponent::generateVivadoTCLScript(){
	std::string ret = "";
	//	ret += getVivadoTCLScript(vivado_version, "Compare", "Both", "Equal");
	ret += getVivadoTCLScript();
	return ret;
}

std::string FoeqComponent::generateVerilogWrapper(){
	return getVerilogWrapper();
}


//FONE Component:
FoneComponent::FoneComponent(Component& c) : FloatingArithmeticComponent(c){

	max_latency = 2;

	index = c.index;
	moduleName = "fcmp_one_op";
	name = c.name;
	instanceName =  moduleName + "_" + name;
	type = c.type;
	bbID = c.bbID;
	op = c.op;
	in = c.in;
	out = c.out;
	delay = c.delay;
	latency = c.latency <= max_latency ? latency : max_latency;
	II = c.II;
	slots = c.slots;
	transparent = c.transparent;
	value = c.value;
	io = c.io;
	inputConnections = c.inputConnections;
	outputConnections = c.outputConnections;


	clk = c.clk;
	rst = c.rst;

	dsp48_uage = "No_Usage";
	add_sub = "Both";
	compare_operation = "Not_Equal";
	operation_type = "Compare";
};
std::string FoneComponent::generateVivadoTCLScript(){
	std::string ret = "";
	//	ret += getVivadoTCLScript(vivado_version, "Compare", "Both", "Not_Equal");
	ret += getVivadoTCLScript();
	return ret;
}

std::string FoneComponent::generateVerilogWrapper(){
	return getVerilogWrapper();
}


//FOGT Component:
FogtComponent::FogtComponent(Component& c) : FloatingArithmeticComponent(c){

	max_latency = 2;

	index = c.index;
	moduleName = "fcmp_ogt_op";
	name = c.name;
	instanceName =  moduleName + "_" + name;
	type = c.type;
	bbID = c.bbID;
	op = c.op;
	in = c.in;
	out = c.out;
	delay = c.delay;
	latency = c.latency <= max_latency ? latency : max_latency;
	II = c.II;
	slots = c.slots;
	transparent = c.transparent;
	value = c.value;
	io = c.io;
	inputConnections = c.inputConnections;
	outputConnections = c.outputConnections;


	clk = c.clk;
	rst = c.rst;

	dsp48_uage = "No_Usage";
	add_sub = "Both";
	compare_operation = "Greater_Than";
	operation_type = "Compare";
};
std::string FogtComponent::generateVivadoTCLScript(){
	std::string ret = "";
	//	ret += getVivadoTCLScript(vivado_version, "Compare", "Both", "Greater_Than");
	ret += getVivadoTCLScript();
	return ret;
}

std::string FogtComponent::generateVerilogWrapper(){
	return getVerilogWrapper();
}


//FOGE Component:
FogeComponent::FogeComponent(Component& c) : FloatingArithmeticComponent(c){

	max_latency = 2;

	index = c.index;
	moduleName = "fcmp_oge_op";
	name = c.name;
	instanceName =  moduleName + "_" + name;
	type = c.type;
	bbID = c.bbID;
	op = c.op;
	in = c.in;
	out = c.out;
	delay = c.delay;
	latency = c.latency <= max_latency ? latency : max_latency;
	II = c.II;
	slots = c.slots;
	transparent = c.transparent;
	value = c.value;
	io = c.io;
	inputConnections = c.inputConnections;
	outputConnections = c.outputConnections;


	clk = c.clk;
	rst = c.rst;

	dsp48_uage = "No_Usage";
	add_sub = "Both";
	compare_operation = "Greater_Than_Or_Equal";
	operation_type = "Compare";
};
std::string FogeComponent::generateVivadoTCLScript(){
	std::string ret = "";
	//	ret += getVivadoTCLScript(vivado_version, "Compare", "Both", "Greater_Than_Or_Equal");
	ret += getVivadoTCLScript();
	return ret;
}

std::string FogeComponent::generateVerilogWrapper(){
	return getVerilogWrapper();
}


//FOLT Component:
FoltComponent::FoltComponent(Component& c) : FloatingArithmeticComponent(c){

	max_latency = 2;

	index = c.index;
	moduleName = "fcmp_olt_op";
	name = c.name;
	instanceName =  moduleName + "_" + name;
	type = c.type;
	bbID = c.bbID;
	op = c.op;
	in = c.in;
	out = c.out;
	delay = c.delay;
	latency = c.latency <= max_latency ? latency : max_latency;
	II = c.II;
	slots = c.slots;
	transparent = c.transparent;
	value = c.value;
	io = c.io;
	inputConnections = c.inputConnections;
	outputConnections = c.outputConnections;


	clk = c.clk;
	rst = c.rst;

	dsp48_uage = "No_Usage";
	add_sub = "Both";
	compare_operation = "Less_Than";
	operation_type = "Compare";
};
std::string FoltComponent::generateVivadoTCLScript(){
	std::string ret = "";
	//	ret += getVivadoTCLScript(vivado_version, "Compare", "Both", "Less_Than");
	ret += getVivadoTCLScript();
	return ret;
}

std::string FoltComponent::generateVerilogWrapper(){
	return getVerilogWrapper();
}


//FOGE Component:
FoleComponent::FoleComponent(Component& c) : FloatingArithmeticComponent(c){

	max_latency = 2;

	index = c.index;
	moduleName = "fcmp_ole_op";
	name = c.name;
	instanceName =  moduleName + "_" + name;
	type = c.type;
	bbID = c.bbID;
	op = c.op;
	in = c.in;
	out = c.out;
	delay = c.delay;
	latency = c.latency <= max_latency ? latency : max_latency;
	II = c.II;
	slots = c.slots;
	transparent = c.transparent;
	value = c.value;
	io = c.io;
	inputConnections = c.inputConnections;
	outputConnections = c.outputConnections;


	clk = c.clk;
	rst = c.rst;

	dsp48_uage = "No_Usage";
	add_sub = "Both";
	compare_operation = "Less_Than_Or_Equal";
	operation_type = "Compare";
};
std::string FoleComponent::generateVivadoTCLScript(){
	std::string ret = "";
	//	ret += getVivadoTCLScript(vivado_version, "Compare", "Both", "Less_Than_Or_Equal");
	ret += getVivadoTCLScript();
	return ret;
}

std::string FoleComponent::generateVerilogWrapper(){
	return getVerilogWrapper();
}


//FEXP Component:
FexpComponent::FexpComponent(Component& c) : FloatingArithmeticComponent(c){

	max_latency = 20;

	index = c.index;
	moduleName = "fexp_op";
	name = c.name;
	instanceName =  moduleName + "_" + name;
	type = c.type;
	bbID = c.bbID;
	op = c.op;
	in = c.in;
	out = c.out;
	delay = c.delay;
	latency = c.latency <= max_latency ? latency : max_latency;
	II = c.II;
	slots = c.slots;
	transparent = c.transparent;
	value = c.value;
	io = c.io;
	inputConnections = c.inputConnections;
	outputConnections = c.outputConnections;


	clk = c.clk;
	rst = c.rst;

	dsp48_uage = "Medium_Usage";
	add_sub = "Both";
	compare_operation = "Programmable";
	operation_type = "Exponential";
};
std::string FexpComponent::generateVivadoTCLScript(){
	std::string ret = "";
	//	ret += getVivadoTCLScript(vivado_version, "Compare", "Both", "Less_Than_Or_Equal");
	ret += getVivadoTCLScript();
	return ret;
}

std::string FexpComponent::generateVerilogWrapper(){
	return getVerilogWrapper();
}


//FLOG Component:
FlogComponent::FlogComponent(Component& c) : FloatingArithmeticComponent(c){

	max_latency = 22;

	index = c.index;
	moduleName = "flog_op";
	name = c.name;
	instanceName =  moduleName + "_" + name;
	type = c.type;
	bbID = c.bbID;
	op = c.op;
	in = c.in;
	out = c.out;
	delay = c.delay;
	latency = c.latency <= max_latency ? latency : max_latency;
	II = c.II;
	slots = c.slots;
	transparent = c.transparent;
	value = c.value;
	io = c.io;
	inputConnections = c.inputConnections;
	outputConnections = c.outputConnections;


	clk = c.clk;
	rst = c.rst;

	dsp48_uage = "Medium_Usage";
	add_sub = "Both";
	compare_operation = "Programmable";
	operation_type = "Logarithm";
};
std::string FlogComponent::generateVivadoTCLScript(){
	std::string ret = "";
	//	ret += getVivadoTCLScript(vivado_version, "Compare", "Both", "Less_Than_Or_Equal");
	ret += getVivadoTCLScript();
	return ret;
}

std::string FlogComponent::generateVerilogWrapper(){
	return getVerilogWrapper();
}


//FSQRT Component:
FsqrtComponent::FsqrtComponent(Component& c) : FloatingArithmeticComponent(c){

	max_latency = 28;

	index = c.index;
	moduleName = "fsqrt_op";
	name = c.name;
	instanceName =  moduleName + "_" + name;
	type = c.type;
	bbID = c.bbID;
	op = c.op;
	in = c.in;
	out = c.out;
	delay = c.delay;
	latency = c.latency <= max_latency ? latency : max_latency;
	II = c.II;
	slots = c.slots;
	transparent = c.transparent;
	value = c.value;
	io = c.io;
	inputConnections = c.inputConnections;
	outputConnections = c.outputConnections;


	clk = c.clk;
	rst = c.rst;

	dsp48_uage = "No_Usage";
	add_sub = "Both";
	compare_operation = "Programmable";
	operation_type = "Square_root";
};
std::string FsqrtComponent::generateVivadoTCLScript(){
	std::string ret = "";
	//	ret += getVivadoTCLScript(vivado_version, "Compare", "Both", "Less_Than_Or_Equal");
	ret += getVivadoTCLScript();
	return ret;
}

std::string FsqrtComponent::generateVerilogWrapper(){
	return getVerilogWrapper();
}


//Template for Floating Point Component
//<Name>Component::<Name>Component(Component& c) : FloatingArithmeticComponent(c){
//
//	max_latency = <Maximum Latency for this Component as given in IP generator>;
//
//	index = c.index;
//	moduleName = "<Module Name>";
//	name = c.name;
//	instanceName =  moduleName + "_" + name;
//	type = c.type;
//	bbID = c.bbID;
//	op = c.op;
//	in = c.in;
//	out = c.out;
//	delay = c.delay;
//	latency = c.latency <= max_latency ? latency : max_latency;
//	II = c.II;
//	slots = c.slots;
//	transparent = c.transparent;
//	value = c.value;
//	io = c.io;
//	inputConnections = c.inputConnections;
//	outputConnections = c.outputConnections;
//
//
//	clk = c.clk;
//	rst = c.rst;
//
//	//The following information can be gathered from the IP Generator
//	dsp48_uage = "<DSP_48 Usage>";
//	add_sub = "<Add or Sub ?>";
//	compare_operation = "<Is it compare ?>";
//	operation_type = "<Type Of operation>";
//};
//std::string <Name>Component::generateVivadoTCLScript(){
//	std::string ret = "";
//	ret += getVivadoTCLScript();
//	return ret;
//}
//
//std::string <Name>Component::generateVerilogWrapper(){
//	return getVerilogWrapper();
//}
