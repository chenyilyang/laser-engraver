#pragma once

#include <externals/common/types.hpp>

namespace instruction {

struct laser_on {};
struct laser_off {};
struct set_home_position {
	float x;
	float y;
};
struct home {};
struct wait_for_movement_finish {};

struct dwell {
	u16 delay;
};

struct move_dpi {
	float x;
	float y;
	u16 power;
};

struct move_mm {
	float x;
	float y;
};

struct power {
	i32 duty;
};
} // namespace instruction
