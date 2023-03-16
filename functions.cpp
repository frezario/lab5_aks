//
// Created by Naz on 3/16/2023.
//

#include "functions.h"

double functions::f1(double x1, double x2) {
    double sum = 0.0;
    for (int i = -2; i <= 2; i++) {
        for (int j = -2; j <= 2; j++) {
            sum += 1 / (5 * (i + 2) + j + 3 + pow((x1 - 16 * j), 6) + pow((x2 - 16 * i), 6));
        }
    }
    sum += 0.002;
    return 1 / sum;
}

double functions::f2(double x1, double x2) {
    double a = 20;
    double b = 0.2;
    double c = 2 * pi;
    auto first_term = -a * exp(-b * sqrt(0.5 * (x1 * x1 + x2 * x2)));
    auto second_term = -exp(0.5 * (cos(c * x1) + cos(c * x2)));
    return first_term + second_term + a + exp(1);
}

double functions::f3(double x1, double x2) {
    size_t m = 5;
    double a1[]{1, 2, 1, 1, 5};
    double a2[]{4, 5, 1, 2, 4};
    double c[]{2, 1, 4, 7, 2};

    double sum = 0;
    for (size_t i = 0; i != m; i++) {
        auto pattern = (x1 - a1[i]) * (x1 - a1[i]) + (x2 - a2[i]) * (x2 - a2[i]);
        auto first_factor = c[i] * exp(-1 / pi * pattern);
        auto second_factor = cos(pi * pattern);
        sum += first_factor * second_factor;
    }
    return -sum;
}
