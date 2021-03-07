#pragma once

#include <functional>
#include <string>

#include <src/semi-gcode.hpp>
#include <src/utils.hpp>

enum class upload_instruction_ret {
	keep_going,
	cancel,
	timeout,
};

struct gcode_generation_options {
	double dpi{150.0};
};

using upload_instruction = std::function<upload_instruction_ret(std::string &&gcode, double)>;

namespace gcode {

namespace generator {

/* clang-format off */
template <typename T>
concept generator_type = requires(T object) {
    { object(instruction::dwell{}) } -> std::same_as<std::string>;
    { object(instruction::home{}) } -> std::same_as<std::string>;
    { object(instruction::set_home_position{}) } -> std::same_as<std::string>;
    { object(instruction::laser_off{}) } -> std::same_as<std::string>;
    { object(instruction::laser_on{}) } -> std::same_as<std::string>;
    { object(instruction::move_dpi{}) } -> std::same_as<std::string>;
    { object(instruction::move_mm{}) } -> std::same_as<std::string>;
    { object(instruction::power{}) } -> std::same_as<std::string>;
    { object(instruction::wait_for_movement_finish{}) } -> std::same_as<std::string>;
    { object(std::monostate{}) } -> std::same_as<std::string>;
};
/* clang-format on */

class grbl {
public:
	grbl(const double dpi = 300.0);

	[[nodiscard]] auto operator()(const instruction::dwell v) const noexcept -> std::string;
	[[nodiscard]] auto operator()(const instruction::home) const noexcept -> std::string;
	[[nodiscard]] auto operator()(const instruction::set_home_position) const noexcept -> std::string;
	[[nodiscard]] auto operator()(const instruction::laser_off) const noexcept -> std::string;
	[[nodiscard]] auto operator()(const instruction::laser_on) const noexcept -> std::string;
	[[nodiscard]] auto operator()(const instruction::move_dpi v) const noexcept -> std::string;
	[[nodiscard]] auto operator()(const instruction::move_mm v) const noexcept -> std::string;
	[[nodiscard]] auto operator()(const instruction::power v) const noexcept -> std::string;
	[[nodiscard]] auto operator()(const std::monostate) const noexcept -> std::string;
	[[nodiscard]] auto operator()(const instruction::wait_for_movement_finish) const noexcept -> std::string;

private:
	const double m_precision{calculate_precision(300.0)};
};

} // namespace generator

template <generator::generator_type type = generator::grbl>
void transform(semi::gcodes &&gcodes, const gcode_generation_options &opts, const upload_instruction &instruction) {
	type generator(opts.dpi);
	for (auto index = 0; auto &&gcode : gcodes) {
		switch (instruction(std::visit(generator, gcode), divide(++index, gcodes.size()))) {
			case upload_instruction_ret::keep_going:
				break;
			case upload_instruction_ret::cancel:
				return;
			case upload_instruction_ret::timeout:
				return;
		}
	}
}

} // namespace gcode
