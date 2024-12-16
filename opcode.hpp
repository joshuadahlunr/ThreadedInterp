#pragma once
#include <cassert>
#include <cstdint>
#include <array>

#ifdef _WIN32
    #define MIZU_EXPORT __declspec(dllexport)
#else
    #define MIZU_EXPORT
#endif
#define MIZU_EXPORT_C extern "C" MIZU_EXPORT

namespace mizu {
	using reg_t = uint16_t;
	inline namespace registers {
		constexpr reg_t x(std::size_t i) { return i; }
		constexpr reg_t t(std::size_t i) { return i + 1; assert(i <= 10); }
		constexpr reg_t a(std::size_t i) { return i + 11; }

		constexpr static reg_t zero = x(0);
	}
	using operation_t = void*(*)(struct opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp);

	struct opcode {
		operation_t op;
		reg_t out, a, b;

		opcode& set_immediate(const uint32_t value) { (uint32_t&)a = value; return *this; }
		opcode& set_immediate_signed(const int32_t value) { (int32_t&)a = value; return *this; }
		opcode& set_branch_immediate(const int16_t value) { (int16_t&)b = value; return *this; }

		opcode& set_immediate_f32(const float value) { (float&)a = value; return *this; }
		opcode& set_lower_immediate_f64(const double value) { return set_immediate((uint64_t&)value); }
		opcode& set_upper_immediate_f64(const double value) { return set_immediate(((uint64_t&)value) >> 32); }

		opcode& set_host_pointer_lower_immediate(const void* ptr) { set_immediate((std::size_t)ptr); return *this; }
		opcode& set_host_pointer_upper_immediate(const void* ptr) { set_immediate(((std::size_t)ptr) >> 32); return *this; }
	};

	#define NEXT() return (registers[0] = 0, (++pc)->op(pc, registers, stack, sp))

	constexpr static reg_t memory_size = 1024;
	struct registers_and_stack {
		std::array<uint64_t, memory_size> memory;
		uint8_t* stack_boundary;
		uint8_t* stack_pointer;
	};
	inline void setup_enviornment(registers_and_stack& env) {
		env.memory[0] = 0;
		env.stack_boundary = (uint8_t*)(env.memory.data() + 256);
		env.stack_pointer = (uint8_t*)(env.memory.data() + env.memory.size());
	}
}