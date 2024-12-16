#pragma once
#include <thread>

#include "opcode.hpp"

namespace mizu { 
	// NOTE: Not a valid operation
	inline std::thread* new_thread(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		registers_and_stack env;
		std::copy(registers, registers + memory_size, env.memory.data());

		return new std::thread([pc, env = std::move(env)]() mutable {
			setup_enviornment(env);
			return pc->op(pc, env.memory.data(), env.stack_boundary, env.stack_pointer);
		});
	}

	inline namespace operations { extern "C" {

		// Interpret register as signed
		inline void* fork_relative(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
			auto threadPC = pc + *(int64_t*)&registers[pc->a];
			registers[pc->out] = (uint64_t)new_thread(threadPC, registers, stack, sp);
			NEXT();
		}

		// Interpret arguments as signed
		inline void* fork_relative_immediate(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
			auto threadPC = pc + *(int32_t*)&pc->a;
			registers[pc->out] = (uint64_t)new_thread(threadPC, registers, stack, sp);
			NEXT();
		}

		inline void* fork_to(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
			auto threadPC = (opcode*)registers[pc->a];
			registers[pc->out] = (uint64_t)new_thread(threadPC, registers, stack, sp);
			NEXT();
		}

		inline void* join_thread(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp){
			std::thread* thread = (std::thread*)registers[pc->a];
			if(thread) {
				if(thread->joinable()) thread->join();
				delete thread; 
			}
			registers[pc->a] = 0;
			NEXT();
		}
	}}
}