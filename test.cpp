#define MIZU_IMPLEMENTATION
#include "operations.hpp"
#include "operations.parallel.hpp"
#include "operations.float.hpp"

#include "ffi/operations.hpp"

MIZU_EXPORT_C void test_print(char* c) {
	printf("%s\n", c);
}

#ifdef _WIN32
MIZU_EXPORT int dll_main(int argc, char** argv)
#else
int main(int argc, char** argv)
#endif
{
	using namespace mizu;

#ifdef _WIN32
	auto path = "./tst_load";
#else
	auto path = "./libtst_load";
#endif
	auto function = "print";
	auto hw = "Hello 世界";
	opcode program[] = {
		opcode{find_label, 200}.set_immediate(label2int("thread")),
		// opcode{ffi::push_type_void},
		// opcode{ffi::push_type_pointer},
		// opcode{ffi::create_interface, 201},

		opcode{load_immediate, 202}.set_host_pointer_lower_immediate(path),
		opcode{load_upper_immediate, 202}.set_host_pointer_upper_immediate(path),
		opcode{load_immediate, 203}.set_host_pointer_lower_immediate(function),
		opcode{load_upper_immediate, 203}.set_host_pointer_upper_immediate(function),
		opcode{load_immediate, 204}.set_host_pointer_lower_immediate(hw),
		opcode{load_upper_immediate, 204}.set_host_pointer_upper_immediate(hw),

		// opcode{ffi::load_library, 202, 202},
		// opcode{ffi::load_library_function, 203, 202, 203},

		opcode{load_immediate, 1}.set_immediate(5),
		opcode{stack_push_immediate}.set_immediate(1),
		opcode{stack_store_i32, 0, 0, 1},
		opcode{load_immediate, 2}.set_immediate(6),
		opcode{add, 3, 2, 1},
		opcode{debug_print, 0, 3},
		opcode{load_immediate, 4}.set_immediate(1),
		opcode{set_if_equal, 4, 4, 0},
		opcode{branch_relative_immediate, 0, 4}.set_branch_immediate(5),
		// opcode{fork_relative_immediate, 5}.set_immediate_signed(5),
		opcode{fork_to, 5, 200},
		opcode{stack_load_i32, 3, 0, 0},
		opcode{debug_print, 0, 3},
		opcode{join_thread, 0, 5, 0},

		opcode{load_immediate, 6}.set_immediate(7),
		opcode{convert_to_f64, 6, 6},
		opcode{convert_to_f64, 7, 3},
		opcode{add_f64, 8, 6, 7},
		opcode{debug_print, 0, 8},
		opcode{halt},

		// Thread
		opcode{label}.set_immediate(label2int("thread")),
		opcode{load_immediate, 1}.set_immediate(22),
		opcode{debug_print, 0, 1},
		// FFI call (print)
		// opcode{add, registers::a(0), 204, 0},
		// opcode{ffi::call, 0, 203, 201},
		opcode{halt},
	};

	{
		registers_and_stack env = {};
		setup_enviornment(env);

		program->op(program, env.memory.data(), env.stack_boundary, env.stack_pointer);
	}

	// auto testPrintPtr = mizu::loader::lookup("test_print");
	// ((void(*)(const char*))testPrintPtr)("Hello World");

	return 0;
}