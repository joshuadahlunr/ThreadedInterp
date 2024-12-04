#include <cstdint>
#include <cstdio>
#include <string_view>

#include "opcode.hpp"

constexpr uint32_t label2int(std::string_view label) {
	uint32_t out = 0;
	for(size_t i = 0; i < label.size() && i < 4; ++i)
		out |= label[i] << (i * 8);
	return out;
}

extern "C" {
	inline void* label(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		NEXT();
	}

	inline void* find_label(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		auto needle = *(uint32_t*)&pc->a;
		registers[pc->out] = 0;
		for(size_t i = 0; i < MEMORY_SIZE; ++i)
			if(auto l = pc + i; l->op == label && *(uint32_t*)&l->a == needle) {
				registers[pc->out] = (uint64_t)l;
				break;
			}
		if(registers[pc->out] == 0)
			for(size_t i = 0; i < MEMORY_SIZE; ++i)
				if(auto l = pc - i; l->op == label && *(uint32_t*)&l->a == needle) {
					registers[pc->out] = (uint64_t)l;
					break;
				}
		opcode* dbg = (opcode*)registers[pc->out];
		NEXT();
	}

	inline void* halt(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		return nullptr;
	}

	inline void* debug_print(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		constexpr static auto print_binary = [](void* ptr){ // From: https://stackoverflow.com/a/3974138
			uint8_t* b = (uint8_t*)ptr;

			printf("binary = ");
			for(int8_t i = sizeof(uint64_t)-1; i >= 0; --i)
				for(int8_t j = 7; j >= 0; --j) {
					uint8_t byte = (b[i] >> j) & 1;
					printf("%u", byte);
				}
		};

		print_binary(&registers[pc->a]);
		printf(", u64 = %lu, i64 = %ld, f64 = %f\n", registers[pc->a], *(int64_t*)&registers[pc->a], *(double*)&registers[pc->a]);
		NEXT();
	}

	inline void* load_immediate(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = *(uint32_t*)&pc->a;
		NEXT();
	}

	inline void* load_upper_immediate(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = uint64_t(*(uint32_t*)&pc->a) << 32;
		NEXT();
	}

	inline void* convert_to_u64(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		*(uint64_t*)&registers[pc->out] = registers[pc->a];
		NEXT();
	}

	inline void* convert_to_i64(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		*(int64_t*)&registers[pc->out] = registers[pc->a];
		NEXT();
	}

	inline void* convert_to_u32(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		*(uint32_t*)&registers[pc->out] = registers[pc->a];
		NEXT();
	}

	inline void* convert_to_i32(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		*(int32_t*)&registers[pc->out] = registers[pc->a];
		NEXT();
	}

	inline void* convert_to_u16(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		*(uint16_t*)&registers[pc->out] = registers[pc->a];
		NEXT();
	}

	inline void* convert_to_i16(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		*(int16_t*)&registers[pc->out] = registers[pc->a];
		NEXT();
	}

	inline void* convert_to_u8(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		*(uint8_t*)&registers[pc->out] = registers[pc->a];
		NEXT();
	}

	inline void* convert_to_i8(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		*(int8_t*)&registers[pc->out] = registers[pc->a];
		NEXT();
	}

	inline void* stack_load_u64(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = *(uint64_t*)(sp + registers[pc->a]);
		NEXT();
	}

	inline void* stack_load_i64(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = *(int64_t*)(sp + registers[pc->a]);
		NEXT();
	}

	inline void* stack_store_u64(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = *(uint64_t*)(sp + registers[pc->a]) = *(uint64_t*)&registers[pc->b];
		NEXT();
	}

	inline void* stack_store_i64(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = *(uint64_t*)(sp + registers[pc->a]) = *(uint64_t*)&registers[pc->b];
		NEXT();
	}

	inline void* stack_load_u32(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = *(uint32_t*)(sp + registers[pc->a]);
		NEXT();
	}

	inline void* stack_load_i32(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = *(int32_t*)(sp + registers[pc->a]);
		NEXT();
	}

	inline void* stack_store_u32(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = *(uint32_t*)(sp + registers[pc->a]) = *(uint32_t*)&registers[pc->b];
		NEXT();
	}

	inline void* stack_store_i32(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = *(uint32_t*)(sp + registers[pc->a]) = *(uint32_t*)&registers[pc->b];
		NEXT();
	}

	inline void* stack_load_u16(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = *(uint16_t*)(sp + registers[pc->a]);
		NEXT();
	}

	inline void* stack_load_i16(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = *(int16_t*)(sp + registers[pc->a]);
		NEXT();
	}

	inline void* stack_store_u16(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = *(uint16_t*)(sp + registers[pc->a]) = *(uint16_t*)&registers[pc->b];
		NEXT();
	}

	inline void* stack_store_i16(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = *(uint16_t*)(sp + registers[pc->a]) = *(uint16_t*)&registers[pc->b];
		NEXT();
	}

	inline void* stack_load_u8(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = *(uint8_t*)(sp + registers[pc->a]);
		NEXT();
	}

	inline void* stack_load_i8(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = *(int8_t*)(sp + registers[pc->a]);
		NEXT();
	}

	inline void* stack_store_u8(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = *(uint8_t*)(sp + registers[pc->a]) = *(uint8_t*)&registers[pc->b];
		NEXT();
	}

	inline void* stack_store_i8(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = *(uint8_t*)(sp + registers[pc->a]) = *(uint8_t*)&registers[pc->b];
		NEXT();
	}

	inline void* stack_push(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		sp -= registers[pc->a];
		NEXT();
	}

	inline void* stack_push_immediate(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		auto& size = *(uint32_t*)&pc->a;
		sp -= size;
		NEXT();
	}

	inline void* stack_pop(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		sp += registers[pc->a];
		NEXT();
	}

	inline void* stack_pop_immediate(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		auto& size = *(uint32_t*)&pc->a;
		sp += size;
		NEXT();
	}

	// Interpret register as signed
	inline void* jump_relative(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		auto a = registers[pc->a];
		registers[pc->out] = (uint64_t)pc;
		pc += *(int64_t*)&a - 1;
		NEXT();
	}

	// Interpret arguments as signed
	inline void* jump_relative_immediate(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = (uint64_t)pc;
		pc += *(int32_t*)&pc->a - 1;
		NEXT();
	}

	inline void* jump_to(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = (uint64_t)pc;
		pc = (opcode*)registers[pc->a] - 1;
		NEXT();
	}

	// Interpret register as signed
	inline void* branch_relative(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = (uint64_t)pc;
		if(registers[pc->a])
			pc += *(int64_t*)&registers[pc->b] - 1;
		NEXT();
	}

	// Interpret arguments as signed
	inline void* branch_relative_immediate(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = (uint64_t)pc;
		if(registers[pc->a])
			pc += *(int16_t*)&pc->b - 1;
		NEXT();
	}

	inline void* branch_to(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = (uint64_t)pc;
		if(registers[pc->a])
			pc = (opcode*)registers[pc->b] - 1;
		NEXT();
	}

	inline void* set_if_equal(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = registers[pc->a] == registers[pc->b];
		NEXT();
	}

	inline void* set_if_not_equal(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = registers[pc->a] != registers[pc->b];
		NEXT();
	}

	inline void* set_if_less(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = registers[pc->a] < registers[pc->b];
		NEXT();
	}

	inline void* set_if_less_signed(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = *(int64_t*)&registers[pc->a] < *(int64_t*)&registers[pc->b];
		NEXT();
	}

	inline void* set_if_greater_equal(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = registers[pc->a] >= registers[pc->b];
		NEXT();
	}

	inline void* set_if_greater_equal_signed(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = *(int64_t*)&registers[pc->a] >= *(int64_t*)&registers[pc->b];
		NEXT();
	}

	inline void* add(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = registers[pc->a] + registers[pc->b];
		NEXT();
	}

	inline void* subtract(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = registers[pc->a] - registers[pc->b];
		NEXT();
	}

	inline void* multiply(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = registers[pc->a] * registers[pc->b];
		NEXT();
	}

	inline void* divide(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = registers[pc->a] / registers[pc->b];
		NEXT();
	}

	inline void* modulus(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = registers[pc->a] % registers[pc->b];
		NEXT();
	}

	inline void* shift_left(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = registers[pc->a] << registers[pc->b];
		NEXT();
	}

	inline void* shift_right_logical(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = registers[pc->a] >> registers[pc->b];
		NEXT();
	}
	inline void* shift_right_arithmetic(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = *(int64_t*)&registers[pc->a] >> registers[pc->b];
		NEXT();
	}

	inline void* bitwise_xor(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = registers[pc->a] ^ registers[pc->b];
		NEXT();
	}

	inline void* bitwise_and(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = registers[pc->a] & registers[pc->b];
		NEXT();
	}

	inline void* bitwise_or(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers[pc->out] = registers[pc->a] | registers[pc->b];
		NEXT();
	}
}