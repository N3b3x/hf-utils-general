/**
 * @file ParabolicCurveEstimator.h
 * @brief This file contains the ParabolicCurveEstimator class for estimating a parabolic curve using least squares regression.
 */

#ifndef PARABOLICCURVEESTIMATOR_H_
#define PARABOLICCURVEESTIMATOR_H_

#include <iostream>
#include <vector>
#include <cmath>

/**
 * @class ParabolicCurveEstimator
 * @brief A class for estimating a parabolic curve using least squares regression.
 */
class ParabolicCurveEstimator {
public:
    /**
     * @brief Function to add a data point to the curve estimator.
     * @param x The x-coordinate of the data point.
     * @param y The y-coordinate of the data point.
     */
    bool AddDataPoint(double x, double y) {
        if (x_values_.size() >= max_points_) {
            return false;
        }

        x_values_.push_back(x);
        y_values_.push_back(y);

        return true;
    }

    /**
     * @brief Function to estimate a parabolic curve using least squares regression.
     * @param a Output variable to store the coefficient 'a' of the quadratic equation y = ax^2 + bx + c.
     * @param b Output variable to store the coefficient 'b' of the quadratic equation y = ax^2 + bx + c.
     * @param c Output variable to store the coefficient 'c' of the quadratic equation y = ax^2 + bx + c.
     */
    void Estimate(double& a, double& b, double& c) const {
        int n = x_values_.size();
        double sum_x = 0.0, sum_x2 = 0.0, sum_x3 = 0.0, sum_x4 = 0.0;
        double sum_y = 0.0, sum_xy = 0.0, sum_x2y = 0.0;

        for (int i = 0; i < n; ++i) {
            double x_i = x_values_[i];
            double y_i = y_values_[i];

            sum_x += x_i;
            sum_x2 += x_i * x_i;
            sum_x3 += x_i * x_i * x_i;
            sum_x4 += x_i * x_i * x_i * x_i;

            sum_y += y_i;
            sum_xy += x_i * y_i;
            sum_x2y += x_i * x_i * y_i;
        }

        // Using least squares regression to find coefficients a, b, and c of the quadratic equation y = ax^2 + bx + c
        double denominator = n * sum_x2 * sum_x4 + sum_x * sum_x * sum_x2 * sum_x2 + sum_x3 * sum_x * sum_x3 - n * sum_x3 * sum_x3 - sum_x * sum_x * sum_x4 - sum_x2 * sum_x2 * sum_x2;
        a = (n * sum_x2y * sum_x4 + sum_x * sum_y * sum_x2 * sum_x2 + sum_x3 * sum_x * sum_xy - n * sum_x3 * sum_xy - sum_x * sum_y * sum_x4 - sum_x2 * sum_x2 * sum_xy) / denominator;
        b = (n * sum_x2 * sum_x2y + sum_x * sum_x2 * sum_xy + sum_x3 * sum_y - n * sum_x3 * sum_y - sum_x * sum_x2y - sum_x2 * sum_x2 * sum_y) / denominator;
        c = (sum_x2 * sum_x2 * sum_x2y + sum_x * sum_x * sum_xy * sum_x2 * sum_x2 + sum_x3 * sum_x3 * sum_y - sum_x * sum_x * sum_x3 * sum_x2y - sum_x2 * sum_x2 * sum_x3 * sum_y) / denominator;
    }


    /**
     * @brief Overloaded estimate function to get the coefficients from the class directly.
     */
    void Estimate() const {
        Estimate(a_, b_, c_);
    }

    /**
     * @brief Function to get the estimated coefficients of the parabolic curve.
     * @param a Output variable to store the coefficient 'a' of the quadratic equation y = ax^2 + bx + c.
     * @param b Output variable to store the coefficient 'b' of the quadratic equation y = ax^2 + bx + c.
     * @param c Output variable to store the coefficient 'c' of the quadratic equation y = ax^2 + bx + c.
     */
    void GetCoefficients(double& a, double& b, double& c) const {
        a = a_;
        b = b_;
        c = c_;
    }

    /**
     * @brief Function to set the maximum amount of points allowed in the estimator.
     * @param maxPoints The maximum number of data points allowed in the estimator.
     */
    void SetMaxPoints(size_t maxPoints) {
        max_points_ = maxPoints;
    }

private:
    std::vector<double> x_values_;
    std::vector<double> y_values_;
    mutable double a_ = 0.0;
    mutable double b_ = 0.0;
    mutable double c_ = 0.0;
    size_t max_points_ = std::numeric_limits<size_t>::max();
};


#endif /* PARABOLICCURVEESTIMATOR_H_ */
