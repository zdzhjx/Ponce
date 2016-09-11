#pragma once
//Ponce
#include "trigger.hpp"
#include "snapshot.hpp"
//IDA
#include <kernwin.hpp>
//Triton
#include <api.hpp>

#if defined(__i386) || defined(_M_IX86)
//#define X86_32 It is already defined in the preprocessor options
#define TRITON_X86_REG_XSP triton::arch::x86::x86_reg_esp
#define TRITON_REG_XIP triton::arch::x86::x86_reg_eip
#define TRITON_REG_ID_XAX triton::arch::x86::ID_REG_EAX
#define TRITON_X86_REG_XAX triton::arch::x86::x86_reg_eax
#define REG_SIZE 4
#define ARCH arch::ARCH_X86
#define strtol_m strtol
#define HEX_FORMAT "%#x"
#define CPUSIZE 32

#elif defined(__x86_64__) || defined(_M_X64)
//#define X86_64
#define TRITON_X86_REG_XSP triton::arch::x86::x86_reg_rsp
#define TRITON_REG_XIP triton::arch::x86::x86_reg_rip
#define TRITON_REG_ID_XAX triton::arch::x86::ID_REG_RAX
#define TRITON_X86_REG_XAX triton::arch::x86::x86_reg_rax
#define ARCH arch::ARCH_X86_64
#define strtol_m strtoll
#define REG_SIZE 8
#define HEX_FORMAT "%#llX"
#define CPUSIZE 64
#endif

//All the options:
//#define COLOR_TAINTED 0x99FFCE
//#define COLOR_TAINTED_CONDITION 0x00b377
//#define COLOR_EXECUTED_INSTRUCTION 0xe6e6e6
#define SNAPSHOT_DESCRIPTION "Before use the script"
//#define DEBUG true
//#define EXTRADEBUG true
//It enables the instruction tracing the first time a register/memory is tainted
#define ENABLE_STEP_INTO_WHEN_TAINTING true
//#define PAINT_EXECUTED_INSTRUCTIONS true
//It runs the plugin when it is initiallized
#define AUTO_RUN true
//The two different modes, only one can be activated at a time
//#define TAINT 0
//#define SYMBOLIC 1
//This is the current mode
//#define MODE SYMBOLIC
//#define TAINT_ARGV true
//#define TAINT_ARGC true
//#define SKIP_ARGV0 true
//#define TAINT_END_OF_STRING false

//#define ADD_COMMENTS_WITH_CONTROLLED_OPERAND true
//#define RENAME_TAINTED_FUNCTIONS false
#define RENAME_TAINTED_FUNCTIONS_PREFIX "T%03d_"
//#define ADD_COMMENTS_WITH_SYMBOLIC_EXPRESSIONS false



struct action{
	const action_desc_t* action_decs;
	int view_type[44]; // This are the posible views
	bool enable_taint;
	bool enable_symbolic;
	char* menu_path; // this is where the item will be conteined in
};

struct PathConstraint{
	triton::__uint conditionRipId;
	triton::__uint conditionAddr;
	triton::__uint takenAddr;
	triton::__uint notTakenAddr;
	uint bound;

	PathConstraint(triton::__uint conditionRipId, triton::__uint conditionAddr, triton::__uint takenAddr, triton::__uint notTakenAddr, uint bound)
	{
		this->conditionRipId = conditionRipId;
		this->conditionAddr = conditionAddr;
		this->takenAddr = takenAddr;
		this->notTakenAddr = notTakenAddr;
		this->bound = bound;
	}
};

//This struct stores all the global variables used for the current state of the Ponce plugin during execution
//The idea is restore this sctruct when we restore the snapshot
struct runtime_status_t
{
	//This variable stores the number of instructions executed during the tracing
	unsigned int total_number_traced_ins;
	//This variable is used to count how many instructions were executed after the user was asked
	unsigned int current_trace_counter;
	//Is it something tainted or symbolize?
	bool is_something_tainted_or_symbolize;
	//This index is used when we are renaming the tainted funcitons, to know the index
	unsigned int tainted_functions_index;
	//Trigger to enable/disable triton
	Trigger runtimeTrigger;
	//This is the last instruction executed by triton, we need to reference to reanalize if the user taint a register
	triton::arch::Instruction* last_triton_instruction;
	//This variable stores all the path constraints executed before reach the current instruction
	std::vector<PathConstraint> myPathConstraints;
};

extern struct runtime_status_t ponce_runtime_status;

//All the global variables:
extern struct action action_list[];
extern bool hooked;
extern Snapshot snapshot;

//We could use this if we want to keep all the instructions in memory
//extern std::map<triton::__uint, std::list<triton::arch::Instruction *>> instructions_executed_map;

//User options
struct cmdOptionStruct{
	uint64 limitInstructionsTracingMode = 1000;
	uint64 limitTime = 100; //seconds

	//all this variables should be false and initialized in prompt_conf_window in utils.cpp
	bool already_configured = false; // We use this variable to know if the user already configured anything or if this is the first configuration promt
	
	//Only one out of the two will be selected
	bool  use_tainting_engine = false;
	bool  use_symbolic_engine = false;

	bgcolor_t color_tainted = 0x99FFCE;
	bgcolor_t color_tainted_condition = 0x00b377;
	bgcolor_t color_executed_instruction = 0xe6e6e6;

	bool  showDebugInfo = false;
	bool  showExtraDebugInfo = false;
	bool  manageSymbolicIndexing = false;
	bool  enableTracingAsTainted = false;
	bool taintEndOfString = false;
	bool  automatedTainting = false;
	bool  taintArgv = false;
	bool  taintArgv0 = false;
	bool  taintArgc = false;
	bool  taintRecv = false;
	bool  taintFread = false;

	bool addCommentsControlledOperands = false;
	bool RenameTaintedFunctionNames = false;
	bool addCommentsSymbolicExpresions = false;
	bool paintExecutedInstructions= false;
};
extern struct cmdOptionStruct cmdOptions;

class Input
{
public:
	int bound;
	std::vector <triton::arch::MemoryAccess> memOperand;
	std::vector <triton::arch::Register> regOperand;

	//! Constructor.
	Input()
	{
		this->bound = 0;
	}

	//! Destructor.
	~Input()
	{
	}
};