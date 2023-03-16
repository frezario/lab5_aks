//
// Created by Naz on 3/2/2023.
//
#include "integrals.h"

std::pair<size_t, size_t> integrals::get_interval_size(size_t pts_per_interval, size_t steps_x, size_t steps_y) {
    for (size_t a = 1; a <= pts_per_interval; a++) {
        if (pts_per_interval % a == 0 && a <= steps_x) {
            auto b = pts_per_interval / a;
            if (b < steps_y) return std::make_pair(a, b);
        }
    }
    return std::make_pair(0, 0);
}

