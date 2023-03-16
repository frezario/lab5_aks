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

    /* Calculates the integral of the function with given relative and
     * absolute errors. The function must be of two parameters.
     * */

    template<class F>
    double calculate_riemann_sum(F function, double x_start, double x_end, double y_start,
                                 double y_end, size_t steps_x, size_t steps_y,
                                 double shift_x = 0, double shift_y = 0) {
        auto delta_x = (x_end - x_start) / (double) steps_x;
        auto delta_y = (y_end - y_start) / (double) steps_y;
        double result = 0;
        for (size_t step_x = 0; step_x != steps_x; step_x++) {
            for (size_t step_y = 0; step_y != steps_y; step_y++) {
                auto value_of_func = function(x_start + (double) step_x * delta_x + shift_x,
                                              y_start + (double) step_y * delta_y + shift_y);
                result += value_of_func;
            }
        }
        return result;
    }

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

    template<class F>
    std::tuple<double, double, double> calculate_integral(F function, double abs_err, double rel_err,
                                                          double x_start, double x_end, double y_start,
                                                          double y_end, size_t init_steps_x,
                                                          size_t init_steps_y, size_t max_iter) {

        // first step: dividing an interval and computing
        // the initial value of the riemann sum over it
        double first_riemann_sum = 0;
        double second_riemann_sum = 0;
        auto steps_x = init_steps_x;
        auto steps_y = init_steps_y;
        size_t counter = 0;
        do {
            auto delta_x = (x_end - x_start) / (double) steps_x;
            auto delta_y = (y_end - y_start) / (double) steps_y;

            if (counter == 0) {
                first_riemann_sum = delta_x * delta_y *
                                    calculate_riemann_sum(function, x_start, x_end, y_start, y_end, steps_x, steps_y);
            } else {
                first_riemann_sum = second_riemann_sum;
            }

            auto first = calculate_riemann_sum(function, x_start, x_end, y_start, y_end,
                                               steps_x, steps_y, delta_x / 2, 0);
            auto second = calculate_riemann_sum(function, x_start, x_end, y_start, y_end,
                                                steps_x, steps_y, 0, delta_y / 2);
            auto third = calculate_riemann_sum(function, x_start, x_end, y_start, y_end,
                                               steps_x, steps_y, delta_x / 2, delta_y / 2);
            second_riemann_sum = 0.25 * (first_riemann_sum + delta_x * delta_y * (first + second + third));
            steps_x *= 2;
            steps_y *= 2;
            counter++;
        } while ((fabs(second_riemann_sum - first_riemann_sum) > abs_err ||
                  fabs((second_riemann_sum - first_riemann_sum) / second_riemann_sum) > rel_err) &&
                 counter < max_iter);

        return std::make_tuple(second_riemann_sum, fabs(second_riemann_sum - first_riemann_sum),
                               fabs((second_riemann_sum - first_riemann_sum) / second_riemann_sum));
    }

    /*
     * Divides a region into n smaller ones by y coordinate.
     * Returns a vector of pair of form <y_start, y_end>,
     * where y_start means the lower y bound of the smaller region,
     * and y_end - the upper one.
     * */
    std::vector<double> divide_region(size_t n, double y_start, double y_end);

    template<class F>
    std::tuple<double, double, double>
    calculate_integral_parallel(size_t thread_count, F function, double abs_err, double rel_err,
                                double x_start, double x_end, double y_start,
                                double y_end, size_t init_steps_x,
                                size_t init_steps_y, size_t max_iter) {

        // creating the vector of threads to operate with
        std::vector<std::thread> threads(thread_count);

        // creating the vector of parallel results to write into
        std::vector<double> results(thread_count);

        // creating the vector of regions
        auto regions = divide_region(thread_count, y_start, y_end);

        // first step: dividing an interval and computing
        // the initial value of the riemann sum over it
        double first_riemann_sum = 0;
        double second_riemann_sum = 0;
        auto steps_x = init_steps_x;
        // making the number of steps over y be divisible by threads
        auto steps_y = init_steps_y + (thread_count - init_steps_y % thread_count);
        size_t counter = 0;

        auto calculate_and_write = [&results, &function, &regions, &x_start, &x_end, &steps_x, &steps_y, &thread_count]
                (size_t i, double shift_x, double shift_y) {
            size_t new_steps_y = ceil((double) steps_y / (double) thread_count);
            results[i] = calculate_riemann_sum(function, x_start, x_end, regions[i], regions[i + 1], steps_x,
                                               new_steps_y, shift_x, shift_y);
        };

        // Again we're first calculating the first term, then second, then third, and
        // use the obtained optimized formula to obtain result. If the result is
        // not "good" enough, we just try again. But in this case, each call of
        // calculate_riemann_sum() function will be parallelized by the number of
        // thread given.

        do {
            auto delta_x = (x_end - x_start) / (double) steps_x;
            auto delta_y = (y_end - y_start) / (double) steps_y;

            if (counter == 0) {
                // calculating the Riemann sum for the first time

                // calling function calculate_riemann_sum in threads
                for (size_t i = 0; i != threads.size(); i++) {
                    threads[i] = std::thread{calculate_and_write, i, 0, 0};
                }

                // waiting for them to finish

                for (auto &thread: threads) {
                    thread.join();
                }
                first_riemann_sum = delta_x * delta_y *
                                    std::accumulate(results.begin(), results.end(), 0.0);
            } else {
                first_riemann_sum = second_riemann_sum;
            }

            /*
             * Calculating the first term
             * */

            // calling function calculate_riemann_sum in threads
            for (size_t i = 0; i != threads.size(); i++) {
                threads[i] = std::thread{calculate_and_write, i, delta_x / 2, 0};
            }

            // waiting for them to finish

            for (auto &thread: threads) {
                thread.join();
            }

            // adding up results to get first term
            auto first = std::accumulate(results.begin(), results.end(), 0.0);

            /*
             * Calculating the second term
             * */

            // calling function calculate_riemann_sum in threads
            for (size_t i = 0; i != threads.size(); i++) {
                threads[i] = std::thread{calculate_and_write, i, 0, delta_y / 2};
            }

            // waiting for them to finish

            for (auto &thread: threads) {
                thread.join();
            }

            // adding up results to get first term
            auto second = std::accumulate(results.begin(), results.end(), 0.0);

            /*
             * Calculating the third term
             * */

            // calling function calculate_riemann_sum in threads
            for (size_t i = 0; i != threads.size(); i++) {
                threads[i] = std::thread{calculate_and_write, i, delta_x / 2, delta_y / 2};
            }

            // waiting for them to finish

            for (auto &thread: threads) {
                thread.join();
            }

            // adding up results to get first term
            auto third = std::accumulate(results.begin(), results.end(), 0.0);

            second_riemann_sum = 0.25 * (first_riemann_sum + delta_x * delta_y * (first + second + third));
            steps_x *= 2;
            steps_y *= 2;
            counter++;
        } while ((fabs(second_riemann_sum - first_riemann_sum) > abs_err ||
                  fabs((second_riemann_sum - first_riemann_sum) / second_riemann_sum) > rel_err) &&
                 counter < max_iter);

        return std::make_tuple(second_riemann_sum, fabs(second_riemann_sum - first_riemann_sum),
                               fabs((second_riemann_sum - first_riemann_sum) / second_riemann_sum));
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

        if (i_height == 0) {
            std::cerr << "Error: Unable to split the region into the intervals with " << pts_per_interval << " points per interval" << std::endl;
            exit(7);
        }

        auto pool = thread_pool()


    }

}


#endif //INTEGRATE_SERIAL_INTEGRALS_H
