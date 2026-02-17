/**
 * @file ParabolicCurveEstimator.h
 * @brief This file contains the ParabolicCurveEstimator class for estimating a parabolic curve using least squares regression.
 */

#ifndef PARABOLICCURVEESTIMATOR_H_
#define PARABOLICCURVEESTIMATOR_H_

#include <array>
#include <cstddef>

/**
 * @class ParabolicCurveEstimator
 * @brief Estimates a parabolic curve (y = ax^2 + bx + c) using least squares regression.
 *
 * Uses fixed-capacity std::array storage so no heap allocation is required.
 *
 * @tparam MaxPoints  Maximum number of data points the estimator can hold.
 */
template <size_t MaxPoints = 64>
class ParabolicCurveEstimator {
public:
    /**
     * @brief Add a data point to the curve estimator.
     * @param x The x-coordinate of the data point.
     * @param y The y-coordinate of the data point.
     * @return true if added, false if at capacity.
     */
    bool AddDataPoint(double x, double y) {
        if (count_ >= MaxPoints) {
            return false;
        }
        x_values_[count_] = x;
        y_values_[count_] = y;
        ++count_;
        return true;
    }

    /**
     * @brief Clear all data points.
     */
    void ClearPoints() {
        count_ = 0;
    }

    /**
     * @brief Returns the current number of stored data points.
     */
    [[nodiscard]] size_t Size() const { return count_; }

    /**
     * @brief Returns the maximum capacity.
     */
    [[nodiscard]] static constexpr size_t Capacity() { return MaxPoints; }

    /**
     * @brief Estimate a parabolic curve using least squares regression.
     * @param[out] a Coefficient of x^2.
     * @param[out] b Coefficient of x.
     * @param[out] c Constant term.
     */
    void Estimate(double& a, double& b, double& c) const {
        int n = static_cast<int>(count_);
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

        double denominator = n * sum_x2 * sum_x4 + sum_x * sum_x * sum_x2 * sum_x2 + sum_x3 * sum_x * sum_x3 - n * sum_x3 * sum_x3 - sum_x * sum_x * sum_x4 - sum_x2 * sum_x2 * sum_x2;
        a = (n * sum_x2y * sum_x4 + sum_x * sum_y * sum_x2 * sum_x2 + sum_x3 * sum_x * sum_xy - n * sum_x3 * sum_xy - sum_x * sum_y * sum_x4 - sum_x2 * sum_x2 * sum_xy) / denominator;
        b = (n * sum_x2 * sum_x2y + sum_x * sum_x2 * sum_xy + sum_x3 * sum_y - n * sum_x3 * sum_y - sum_x * sum_x2y - sum_x2 * sum_x2 * sum_y) / denominator;
        c = (sum_x2 * sum_x2 * sum_x2y + sum_x * sum_x * sum_xy * sum_x2 * sum_x2 + sum_x3 * sum_x3 * sum_y - sum_x * sum_x * sum_x3 * sum_x2y - sum_x2 * sum_x2 * sum_x3 * sum_y) / denominator;
    }

    /**
     * @brief Estimate and store coefficients internally.
     */
    void Estimate() const {
        Estimate(a_, b_, c_);
    }

    /**
     * @brief Get the internally stored coefficients.
     * @param[out] a Coefficient of x^2.
     * @param[out] b Coefficient of x.
     * @param[out] c Constant term.
     */
    void GetCoefficients(double& a, double& b, double& c) const {
        a = a_;
        b = b_;
        c = c_;
    }

private:
    std::array<double, MaxPoints> x_values_{};
    std::array<double, MaxPoints> y_values_{};
    size_t count_ = 0;
    mutable double a_ = 0.0;
    mutable double b_ = 0.0;
    mutable double c_ = 0.0;
};

#endif /* PARABOLICCURVEESTIMATOR_H_ */
