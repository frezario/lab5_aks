//
// Created by Naz on 3/2/2023.
//
#include "integrals.h"


    std::vector<double> integrals::divide_region(size_t n, double y_start, double y_end) {
        std::vector<double> regions(n + 1);
        for (size_t i = 0; i != n; i++) {
            regions[i] = y_start + (double) i * ((y_end - y_start) / (double) n);
        }
        regions[n] = y_end;
        return regions;
    }

std::pair<size_t, size_t> integrals::get_interval_size(size_t pts_per_interval, size_t steps_x, size_t steps_y) {
    for (size_t a = 1; a <= pts_per_interval; a++) {
        if (pts_per_interval % a == 0 && a <= steps_x) {
            auto b = pts_per_interval / a;
            if (b < steps_y) return std::make_pair(a, b);
        }
    }
    return std::make_pair(0, 0);
}

