#pragma once

#include "../opcode.hpp"

#ifdef MIZU_IMPLEMENTATION
	#include "dynamic_loader.hpp"

	#if defined(__EMSCRIPTEN__) || defined(__WASI__)
		#include "wasm/gen/calls.hpp"
	#else
		#include <ffi.h>
	#endif
	#include <vector>
#endif


namespace mizu::ffi {
#ifdef MIZU_IMPLEMENTATION
	#if defined(__EMSCRIPTEN__) || defined(__WASI__)
		static thread_local std::vector<wasm::types> current_types = {};
	#else
		static thread_local std::vector<ffi_type*> current_types = {};
	#endif
#endif


	template<bool has_return>
	void* call_impl(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
	{
#if defined(__EMSCRIPTEN__) || defined(__WASI__)
		auto func = (void*)registers[pc->a];
		auto interface_ = (std::vector<wasm::types>*)registers[pc->b];

		wasm::variant res;
		switch(interface_->size()) {
			break; case 0: throw std::runtime_error("Function interfaces must at least specify the return type!");
			break; case 1: res = wasm::call(interface_->at(0), func);
			break; case 2: res = wasm::call(interface_->at(0), func, interface_->at(1), registers[registers::a(0)]);
			break; case 3: res = wasm::call(interface_->at(0), func, interface_->at(1), registers[registers::a(0)], interface_->at(2), registers[registers::a(1)]);
			break; case 4: res = wasm::call(interface_->at(0), func, interface_->at(1), registers[registers::a(0)], interface_->at(2), registers[registers::a(1)],
				interface_->at(3), registers[registers::a(2)]);
			break; case 5: res = wasm::call(interface_->at(0), func, interface_->at(1), registers[registers::a(0)], interface_->at(2), registers[registers::a(1)],
				interface_->at(3), registers[registers::a(2)], interface_->at(4), registers[registers::a(3)]);
			// break; case 6: res = wasm::call(interface_->at(0), func, interface_->at(1), registers[registers::a(0)], interface_->at(2), registers[registers::a(1)],
			// 	interface_->at(3), registers[registers::a(2)], interface_->at(4), registers[registers::a(3)], interface_->at(5), registers[registers::a(4)]);
			break; default: throw std::runtime_error("The WASM Trampoline currently only supports functions with a maximum of 4 arguments!");
		}

		auto& reg = registers[pc->out];
		if constexpr(has_return) std::visit([&reg](auto v) {
			if constexpr(!std::is_same_v<decltype(v), std::monostate>)
				reg = *(uint64_t*)&v;
		}, res);
#else
		auto func = FFI_FN(registers[pc->a]);
		auto interface_ = (ffi_cif*)registers[pc->b];

		std::vector<void*> args; args.reserve(interface_->nargs);
		for(size_t i = 0; i < interface_->nargs; ++i)
			args.emplace_back(&registers[registers::a(0) + i]);

		if constexpr(has_return)
			ffi_call(interface_, func, &registers[pc->out], args.data());
		else ffi_call(interface_, func, nullptr, args.data());
#endif
		NEXT();
	}
#else // MIZU_IMPLEMENTATION
		;
#endif // MIZU_IMPLEMENTATION

	inline namespace operations { extern "C" {

		void* push_type_void(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
#if defined(__EMSCRIPTEN__) || defined(__WASI__)
			current_types.push_back(wasm::types::Void);
#else
			current_types.push_back(&ffi_type_void);
#endif
			NEXT();
		}
#else // MIZU_IMPLEMENTATION
		;
#endif // MIZU_IMPLEMENTATION

		void* push_type_pointer(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
#if defined(__EMSCRIPTEN__) || defined(__WASI__)
			if constexpr(sizeof(void*) == 4)
				current_types.push_back(wasm::types::I32);
			else if constexpr(sizeof(void*) == 8)
				current_types.push_back(wasm::types::I64);
#else
			current_types.push_back(&ffi_type_pointer);
#endif
			NEXT();
		}
#else // MIZU_IMPLEMENTATION
		;
#endif // MIZU_IMPLEMENTATION

		void* create_interface(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
#if defined(__EMSCRIPTEN__) || defined(__WASI__)
			if(current_types.empty()) throw std::runtime_error("Function interfaces must at least specify the return type!");
			if(current_types.size() > 5) throw std::runtime_error("The WASM Trampoline currently only supports functions with a maximum of 4 arguments!");
			auto interface_ = (std::vector<wasm::types>*)(registers[pc->out] = (size_t)new std::vector<wasm::types>{});
			*interface_ = std::move(current_types);
#else
			auto interface_ = (ffi_cif*)(registers[pc->out] = (size_t)new ffi_cif{});

			ffi_status status = ffi_prep_cif(interface_, FFI_DEFAULT_ABI, current_types.size() - 1, current_types[0], current_types.data() + 1);
			if (status != FFI_OK) throw std::runtime_error("ffi_prep_cif failed: " + std::to_string(status));
#endif

			current_types.clear();
			NEXT();
		}
#else // MIZU_IMPLEMENTATION
		;
#endif // MIZU_IMPLEMENTATION

		void* call(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
			return call_impl<false>(pc, registers, stack, sp);
		}
#else // MIZU_IMPLEMENTATION
		;
#endif // MIZU_IMPLEMENTATION

		void* call_with_return(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
			return call_impl<true>(pc, registers, stack, sp);
		}
#else // MIZU_IMPLEMENTATION
		;
#endif // MIZU_IMPLEMENTATION

		void* load_library(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
#if defined(__EMSCRIPTEN__) || defined(__WASI__)
			throw std::runtime_error("WASM doesn't currently support dynamic libraries");
#else
			auto path = (const char*)registers[pc->a];
			try {
				registers[pc->out] = (size_t)loader::load_shared(path, false);
			} catch(loader::error) {
				registers[pc->out] = (size_t)loader::load_shared(path, true);
			}
#endif
			NEXT();
		}
#else // MIZU_IMPLEMENTATION
		;
#endif // MIZU_IMPLEMENTATION

		void* load_library_function(opcode* pc, uint64_t* registers, uint8_t* stack, uint8_t* sp)
#ifdef MIZU_IMPLEMENTATION
		{
			auto lib = (loader::library*)registers[pc->a];
#if defined(__EMSCRIPTEN__) || defined(__WASI__)
			if(lib != nullptr) throw std::runtime_error("WASM doesn't support finding functions outside the current executable!");
#else
			auto name = (const char*)registers[pc->b];
			registers[pc->out] = (size_t)loader::lookup(name, lib);
#endif
			NEXT();
		}
#else // MIZU_IMPLEMENTATION
		;
#endif // MIZU_IMPLEMENTATION
	}}
}