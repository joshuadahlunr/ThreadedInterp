#pragma once
#include <cstdint>
#include <array>

using reg_t = uint16_t;
using operation_t = void*(*)(struct opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);

struct opcode {
	operation_t op;
	reg_t out, a, b;

	opcode& set_immediate(uint32_t value) { *(uint32_t*)&a = value; return *this; }
	opcode& set_immediate_signed(int32_t value) { *(int32_t*)&a = value; return *this; }
	opcode& set_branch_immediate(int16_t value) { *(int16_t*)&b = value; return *this; }
};

#define NEXT() return (registers[0] = 0, (++pc)->op(pc, registers, stack, sp))

#define MEMORY_SIZE 1024
struct registers_and_stack {
	std::array<uint64_t, MEMORY_SIZE> memory;
	uint8_t* stack_boundary;
	uint8_t* stack_pointer;
};
inline void setup_enviornment(registers_and_stack& env) {
	env.memory[0] = 0;
	env.stack_boundary = (uint8_t*)(env.memory.data() + 256);
	env.stack_pointer = (uint8_t*)(env.memory.data() + env.memory.size());
}