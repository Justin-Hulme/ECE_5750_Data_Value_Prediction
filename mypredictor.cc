#include <cinttypes>
#include "cvp.h"
#include "mypredictor.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include <unordered_map>

/* Psuedo Code

    table to index into
    hashing function to index into
    read from and write to table

    take current pc and history and look up value in table and that is predicted value

*/

#define TABLE_ADDRESS_WIDTH 16
constexpr int TABLE_SIZE = 1 << TABLE_ADDRESS_WIDTH;

struct VerificationTableEntry
{
    uint8_t valid;
    uint64_t data;
};

std::unordered_map<uint64_t, uint64_t> SeqNumToPC;
uint64_t table_misses = 0;

VerificationTableEntry verification_table[TABLE_SIZE];
// uint64_t classification_table[TABLE_SIZE];

// Global branch and path history
static uint64_t ghr = 0, phist = 0;

// Load/Store address history
static uint64_t addrHist = 0;

uint64_t hash_address(uint64_t address){
    uint64_t lower_bits = address & ((1U << TABLE_ADDRESS_WIDTH) - 1);
    uint64_t next_bits = (address >> TABLE_ADDRESS_WIDTH) & ((1U << (TABLE_ADDRESS_WIDTH)) - 1);
    return lower_bits ^ next_bits;
}

bool getPrediction(uint64_t seq_no, uint64_t pc, uint8_t piece, uint64_t& predicted_value) {

    // basic prediction
    uint64_t hashed_address = hash_address(pc);

    VerificationTableEntry entry = verification_table[hashed_address];

    if (entry.valid == 1){
        predicted_value = entry.data;
        // printf("Predicted Value: %" PRIu64 "\n", entry.data);
        return true;
    }
    else {
        printf("table miss\n");
        return false;
    }
}

void speculativeUpdate(uint64_t seq_no,        		// dynamic micro-instruction # (starts at 0 and increments indefinitely)
                       bool eligible,			// true: instruction is eligible for value prediction. false: not eligible.
		               uint8_t prediction_result,	// 0: incorrect, 1: correct, 2: unknown (not revealed)
		               // Note: can assemble local and global branch history using pc, next_pc, and insn.
		               uint64_t pc,
		               uint64_t next_pc,
		               InstClass insn,
		               uint8_t piece,
		               // Note: up to 3 logical source register specifiers, up to 1 logical destination register specifier.
		               // 0xdeadbeef means that logical register does not exist.
		               // May use this information to reconstruct architectural register file state (using log. reg. and value at updatePredictor()).
		               uint64_t src1,
		               uint64_t src2,
		               uint64_t src3,
		               uint64_t dst) {
    
    // In this example, we will only attempt to predict ALU/LOAD/SLOWALU 

    // bool isPredictable = insn == aluInstClass || insn == loadInstClass || insn == slowAluInstClass;

    // It's an instruction we are interested in predicting, update the first table history
    // Note that some other type of predictors may not want to update at this time if the
    // prediction is unknown to be correct or incorrect

    // At this point, any branch-related information is architectural, i.e.,
    // updating the GHR/LHRs is safe.
    bool isCondBr = insn == condBranchInstClass;
    bool isIndBr = insn == uncondIndirectBranchInstClass;

    // Infrastructure provides perfect branch prediction.
    // As a result, the branch-related histories can be updated now.
    if(isCondBr)
        ghr = (ghr << 1) | (pc + 4 != next_pc);

    if(isIndBr)
	phist = (phist << 4) | (next_pc & 0x3);

    if (insn == loadInstClass || insn == storeInstClass){
        SeqNumToPC.insert({seq_no, pc});
    }
}

void updatePredictor(uint64_t seq_no,		// dynamic micro-instruction #
		             uint64_t actual_addr,	// load or store address (0xdeadbeef if not a load or store instruction)
		             uint64_t actual_value,	// value of destination register (0xdeadbeef if instr. is not eligible for value prediction)
		             uint64_t actual_latency) {	// actual execution latency of instruction

    // It is now safe to update the address history register
    //if(insn == loadInstClass || insn == storeInstClass) 

    uint64_t pc = SeqNumToPC[seq_no];
    SeqNumToPC.erase(seq_no);

    if(pc != 0xdeadbeef) {
        addrHist = (addrHist << 4) | actual_addr;

        uint64_t hashed_address = hash_address(actual_addr);

        verification_table[hashed_address].data = actual_value;
        verification_table[hashed_address].valid = 1;
    }
}

void beginPredictor(int argc_other, char **argv_other) {
    if (argc_other > 0)
        printf("CONTESTANT ARGUMENTS:\n");

    for (int i = 0; i < argc_other; i++)
        printf("\targv_other[%d] = %s\n", i, argv_other[i]);

    for (int i = 0; i < TABLE_SIZE; i++){
        verification_table[i].valid = 0;
    }
}

void endPredictor() {
	printf("CONTESTANT OUTPUT--------------------------\n");
	printf("--------------------------\n");

    uint64_t length = SeqNumToPC.size();
    

    printf("\nLinked List Length: %" PRIu64 "\n", length);
}
