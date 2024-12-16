#pragma once

#include <cstdint>
#include <variant>

namespace mizu::ffi::wasm {

	enum class types {
		Void, // -> void
		I64, // -> int64_t
		F64, // -> double
		I32, // -> int32_t
		F32, // -> float
	};

	using variant = std::variant<std::monostate, int64_t, double, int32_t, float>;

}
