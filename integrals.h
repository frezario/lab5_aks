//
// Created by Naz on 2/25/2023.
//

#ifndef INTEGRATE_SERIAL_INTEGRALS_H
#define INTEGRATE_SERIAL_INTEGRALS_H

#include <cstddef>
#include <cmath>
#include <vector>
#include <thread>
#include <numeric>
#include <iostream>
#include "thread_pool.h"

namespace integrals {

    /*
     * Defining interval class in header because of using template class
     * for a function that uses this class (otherwise, it'll be incomplete)
     * */
    class Interval {
    public:
        double x0, y0;
        double dx, dy;
        size_t Nx, Ny;
    };

    template<class F>
    double calculate_riemann_sum_interval(F function, const Interval &interval) {
        double result = 0;
        for (size_t step_y = 0; step_y != interval.Ny; step_y++) {
            for (size_t step_x = 0; step_x != interval.Nx; step_x++) {
                auto value_of_func = function(interval.x0 + (double) step_x * interval.dx,
                                              interval.y0 + (double) step_y * interval.dy);
                result += value_of_func;
            }
        }
        return result;
    }

    std::pair<size_t, size_t> get_interval_size(size_t pts_per_interval, size_t steps_x, size_t steps_y);

    template<class F>
    std::tuple<double, double, double>
    calculate_integral(size_t thread_count, size_t pts_per_interval, F function, double abs_err,
                                      double rel_err,
                                      double x_start, double x_end, double y_start,
                                      double y_end, size_t init_steps_x,
                                      size_t init_steps_y, size_t max_iter) {

        double first_riemann_sum = 0;
        double second_riemann_sum = 0;

        auto steps_y = init_steps_y;
        auto steps_x = init_steps_x;

        auto sizes = get_interval_size(pts_per_interval, steps_x, steps_y);
        auto i_height = sizes.first;
        auto i_width = sizes.second;

        size_t counter = 0;

        if (i_height == 0) {
            std::cerr << "Error: Unable to split the region into the intervals with " << pts_per_interval << " points per interval" << std::endl;
            exit(7);
        }

        auto pool = thread_pool(thread_count);

        do {
            if (counter != 0) {
                sizes = get_interval_size(pts_per_interval, steps_x, steps_y);
                i_height = sizes.first;
                i_width = sizes.second;
            }
            first_riemann_sum = second_riemann_sum;
            second_riemann_sum = 0;
            auto delta_x = (x_end - x_start) / (double) steps_x;
            auto delta_y = (y_end - y_start) / (double) steps_y;

            // creating intervals to give consumers
            size_t num_of_intervals = 0;
            std::vector<std::future<double>> futures;
            for (size_t step_y = 0; step_y < steps_y; step_y += i_height) {
                for (size_t step_x = 0; step_x < steps_x; step_x += i_width) {
                    auto width = std::min(i_width, steps_x - step_x);
                    auto height = std::min(i_height, steps_y - step_y);
                    auto i = Interval{x_start + (double)step_x * delta_x, y_start + (double)step_y * delta_y, delta_x, delta_y, width, height};
                    num_of_intervals++;
                    futures.push_back(pool.submit(std::bind(&calculate_riemann_sum_interval<F>, function, i)));
                    //second_riemann_sum += pool.submit(&calculate_riemann_sum_interval, function, i).get();
                }
            }
            for (size_t i = 0; i<num_of_intervals; ++i){
                second_riemann_sum += futures[i].get();
            }

            second_riemann_sum *= delta_x * delta_y;

            steps_x *= 2;
            steps_y *= 2;
            counter++;
        } while ((fabs(second_riemann_sum - first_riemann_sum) > abs_err ||
                  fabs((second_riemann_sum - first_riemann_sum) / second_riemann_sum) > rel_err) &&
                 counter <= max_iter);

        return std::make_tuple(second_riemann_sum, fabs(second_riemann_sum - first_riemann_sum),
                               fabs((second_riemann_sum - first_riemann_sum) / second_riemann_sum));
    }
}


#endif //INTEGRATE_SERIAL_INTEGRALS_H
