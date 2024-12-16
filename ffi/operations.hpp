#pragma once

#include "../opcode.hpp"
#include "dynamic_loader.hpp"

#include <ffi.h>
#include <vector>


namespace mizu::ffi { 

	static thread_local std::vector<ffi_type*> current_types = {};
	
	template<bool has_return>
	inline void* call_impl(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
		auto func = FFI_FN(registers[pc->a]);
		auto interface_ = (ffi_cif*)registers[pc->b];

		std::vector<void*> args; args.reserve(interface_->nargs);
		for(size_t i = 0; i < interface_->nargs; ++i)
			args.emplace_back(&registers[registers::a(0) + i]);
		
		if constexpr(has_return)
			ffi_call(interface_, func, &registers[pc->out], args.data());
		else ffi_call(interface_, func, nullptr, args.data());

		NEXT();
	}
	
	inline namespace operations { extern "C" {

		inline void* push_type_void(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
			current_types.push_back(&ffi_type_void);
			NEXT();
		}

		inline void* push_type_pointer(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
			current_types.push_back(&ffi_type_pointer);
			NEXT();
		}

		inline void* create_interface(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
			auto interface_ = (ffi_cif*)(registers[pc->out] = (size_t)new ffi_cif{});

			ffi_status status = ffi_prep_cif(interface_, FFI_DEFAULT_ABI, current_types.size() - 1, current_types[0], current_types.data() + 1);
			if (status != FFI_OK) throw std::runtime_error("ffi_prep_cif failed: " + std::to_string(status));

			current_types.clear();
			NEXT();
		}

		inline void* call(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
			return call_impl<false>(pc, registers, stack, sp);
		}

		inline void* call_with_return(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
			return call_impl<true>(pc, registers, stack, sp);
		}

		inline void* load_library(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
			auto path = (const char*)registers[pc->a];
			try {
				registers[pc->out] = (size_t)loader::load_shared(path, false);
			} catch(loader::error) {
				registers[pc->out] = (size_t)loader::load_shared(path, true);
			}
			NEXT();
		}

		inline void* load_library_function(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp) {
			auto lib = (loader::library*)registers[pc->a];
			auto name = (const char*)registers[pc->b];
			registers[pc->out] = (size_t)loader::lookup(lib, name);
			NEXT();
		}
	}}
}