#include "operations.hpp"
#include "operations.parallel.hpp"

int main(int argc, char** argv) {
	opcode program[] = {
		opcode{find_label, 200}.set_immediate(label2int("thread")),

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
		opcode{halt},

		// Thread
		opcode{label}.set_immediate(label2int("thread")),
		opcode{load_immediate, 1}.set_immediate(22),
		opcode{debug_print, 0, 1},
		opcode{halt},
	};

	{
		registers_and_stack env = {};
		setup_enviornment(env);

		program->op(program, env.memory.data(), env.stack_boundary, env.stack_pointer);
	}
}