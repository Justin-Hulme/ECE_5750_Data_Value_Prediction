#include <cinttypes>
#include "cvp.h"
#include "mypredictor.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include <unordered_map>

#define TABLE_ADDRESS_WIDTH 16
constexpr int TABLE_SIZE = 1 << TABLE_ADDRESS_WIDTH;

#define CONF_THRESH 1
#define MAX_CONF 10000

bool last_value = false;
bool stride = false;

// struct to hold verification table data
struct VerificationTableEntry
{
    uint64_t data;
    int64_t stride;
    uint16_t conf;
};

VerificationTableEntry verification_table[TABLE_SIZE];

// unordered_map to map sequence number to pc
std::unordered_map<uint64_t, uint64_t> SeqNumToPC;

// Global branch and path history
static uint64_t ghr = 0, phist = 0;

// Load/Store address history
static uint64_t addrHist = 0;

uint64_t hash_address(uint64_t address){
    uint64_t lower_bits = address & ((1U << TABLE_ADDRESS_WIDTH) - 1);
    uint64_t next_bits = (address >> TABLE_ADDRESS_WIDTH) & ((1U << (TABLE_ADDRESS_WIDTH)) - 1);
    return lower_bits ^ next_bits;
}

uint8_t get_tag(uint64_t address) {
    return static_cast<uint8_t>(address);
}

bool getPrediction(uint64_t seq_no, uint64_t pc, uint8_t piece, uint64_t& predicted_value) {
    if (stride){
        uint64_t hashed_address = hash_address(pc);
        uint8_t pc_tag = get_tag(pc);

        auto &entry = verification_table[hashed_address];

        // if the entry is greater that the threshold or this is the first piece
        if (entry.conf >= CONF_THRESH && piece == 0){
            // predict a value
            predicted_value = entry.data + entry.stride;
            return true;
        }
        else {
            // don't predict
            return false;
        }
    } else if (last_value){
    
    }
    return false;
}

void speculativeUpdate(uint64_t seq_no,        		// dynamic micro-instruction # (starts at 0 and increments indefinitely)
                       bool eligible,			    // true: instruction is eligible for value prediction. false: not eligible.
		               uint8_t prediction_result,	// 0: incorrect, 1: correct, 2: unknown (not revealed)
		               uint64_t pc,
		               uint64_t next_pc,
		               InstClass insn,
		               uint8_t piece,
		               uint64_t src1,
		               uint64_t src2,
		               uint64_t src3,
		               uint64_t dst) {
    if (stride){
        bool isCondBr = insn == condBranchInstClass;
        bool isIndBr = insn == uncondIndirectBranchInstClass;

        // update the branch history
        if(isCondBr)
            ghr = (ghr << 1) | (pc + 4 != next_pc);

        // update the path history
        if(isIndBr)
            phist = (phist << 4) | (next_pc & 0x3);

        // only map the sequence number to the oc if it is a specific instruction type, eligible, and this is the first piece
        if ((insn == loadInstClass || insn == aluInstClass || insn == slowAluInstClass) && eligible && piece == 0){
            SeqNumToPC.insert({seq_no, pc});
        }
    } else if (last_value){

    }
}

void updatePredictor(uint64_t seq_no,		    // dynamic micro-instruction #
		             uint64_t actual_addr,	    // load or store address (0xdeadbeef if not a load or store instruction)
		             uint64_t actual_value,	    // value of destination register (0xdeadbeef if instr. is not eligible for value prediction)
		             uint64_t actual_latency) {	// actual execution latency of instruction
    if (stride){
        // if seq_no is not in the hash table then skip
        auto it = SeqNumToPC.find(seq_no);
        if (it == SeqNumToPC.end()) return;

        // get pc and remove the association
        uint64_t pc = it->second;
        SeqNumToPC.erase(it);

        // update the address history
        if(actual_addr != 0xdeadbeef) {
            addrHist = (addrHist << 4) | actual_addr;
        }

        // if the the instruction is eligible for value prediction
        if(actual_value != 0xdeadbeef){
            // get the entry from the pc
            uint64_t hashed_address = hash_address(pc);
            auto &entry = verification_table[hashed_address];

            // calculate the new stride
            int64_t new_stride = actual_value - entry.data;

            // if the new stride is the same as the old stride then the value was accurately predicted.
            if (new_stride == entry.stride) {
                // increase the confidence
                entry.conf = (entry.conf == MAX_CONF) ? MAX_CONF : entry.conf + 1;
            } else {
                // decrease the confidence
                entry.conf = (entry.conf > 0) ? entry.conf - 1 : 0;

                // update the stride when the confidence drops too low
                if (entry.conf == 0) {
                    entry.stride = new_stride;
                }
            }

            // update the entry value
            entry.data = actual_value;
        }
    } else if (last_value){

    }
}

void beginPredictor(int argc_other, char **argv_other) {
    if (argc_other > 0){
        printf("CONTESTANT ARGUMENTS:\n");

        if (strcmp(argv_other[0], "last_value") == 0){
            last_value = true;
        } else if (strcmp(argv_other[0], "stride") == 0){
            stride = true;
        } else {
            printf("Please include the type of prediction you want as the last argument");
            exit(2);
        }
    }

    for (int i = 0; i < argc_other; i++)
        printf("\targv_other[%d] = %s\n", i, argv_other[i]);

    // prefill the verification table with all 0s
    for (int i = 0; i < TABLE_SIZE; i ++){
        verification_table[i] = {0, 0, 0};
    }
}

void endPredictor() {
	printf("CONTESTANT OUTPUT--------------------------\n");
	printf("--------------------------\n");
}
