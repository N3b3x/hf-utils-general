/**
 *
 * Nebula Tech Corporation
 *
 * Copyright © 2023 Nebula Tech Corporation. All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public License v3.0 or later.
 *
 */

#ifndef UTILITIES_COMMON_LEASTSQUARESFIT_H_
#define UTILITIES_COMMON_LEASTSQUARESFIT_H_

#include "platform_compat.h"
#include "arm_math.h"
#include <array>
#include <algorithm>

/**
 * @class LeastSquaresFit
 * @brief Specialized class for performing least squares fit on ARM Cortex-M4.
 *
 * This class uses the CMSIS DSP library to perform matrix operations required
 * for the least squares fit. It is designed to be used in an embedded environment
 * where exception handling is not available (hence the use of noexcept).
 */
template<size_t ROWS, size_t COLS>
class LeastSquaresFit {
public:
	LeastSquaresFit() noexcept;
	~LeastSquaresFit() noexcept;

	/**
	 * @brief Set the design matrix A using a one-dimensional array.
	 * @param values Array of floating point values in row-major order to initialize matrix A.
	 */
	void SetA(const float (&values)[ROWS * COLS]) noexcept;

	/**
	 * @brief Set the design matrix A using a one-dimensional array.
	 * @param values Array of floating point values in row-major order to initialize matrix A.
	 */
	void SetA(const std::array<float, ROWS * COLS>& values) noexcept;


	/**
	 * @brief Set the response vector B.
	 * @param values Array of floating point values to initialize vector B.
	 */
	void SetB(const float (&values)[ROWS]) noexcept;

	/**
	 * @brief Set the response vector B.
	 * @param values Array of floating point values to initialize vector B.
	 */
	void SetB(const std::array<float, ROWS>& values) noexcept;


	/**
	 * @brief Update an element in matrix A.
	 * @param row Row index of the element to update.
	 * @param col Column index of the element to update.
	 * @param newValue New value to set at the specified element.
	 * @return true if the update is successful, false if the indices are out of bounds.
	 */
	bool UpdateMatrixAElement(size_t row, size_t col, float newValue) noexcept;

	/**
	 * @brief Update an element in vector B.
	 * @param row Index of the element in vector B to update.
	 * @param newValue New value to set at the specified index.
	 * @return true if the update is successful, false if the index is out of bounds.
	 */
	bool UpdateMatrixBElement(size_t row, float newValue) noexcept;


	/**
	 * @brief Performs the matrix operations to fit the model X = (A^T * A)^-1 * A^T * B.
	 * @return true if the fitting process is successful, false if the matrix A^T * A is singular.
	 */
	bool Fit() noexcept;


	/**
	 * @brief Retrieves the result from the fitting process.
	 * @param result Array where the result X will be stored.
	 */
	void GetResult(float (&result)[COLS]) const noexcept;

	/**
	 * @brief Retrieves the result from the fitting process.
	 * @param result Array where the result X will be stored.
	 */
	void GetResult(std::array<float, COLS>& result) const noexcept;


	/**
	 * @brief Perform the fit operation and retrieve the result in one step.
	 * @param result Array where the result X will be stored.
	 * @return true if the fitting and result retrieval are successful, false otherwise.
	 */
	bool FitAndGetResult(float (&result)[COLS]) noexcept;

	/**
	 * @brief Perform the fit operation and retrieve the result in one step.
	 * @param result Array where the result X will be stored.
	 * @return true if the fitting and result retrieval are successful, false otherwise.
	 */
	bool FitAndGetResult(std::array<float, COLS>& result) noexcept;

private:

	/**
	 * @brief Ensures the handler is initialized.
	 * @return True if the handler is initialized, false otherwise.
	 */
	bool EnsureInitialized() noexcept {
		if (!initialized) {
			initialized = Initialize();
		}
		return initialized;
	}

	/**
	 * @brief Initializes the arm matrices.
	 * @return True if initialization is successful, false otherwise.
	 */
	bool Initialize() noexcept;

	std::array<float, ROWS * COLS> A_data; 		///< Storage for matrix A data.
	std::array<float, COLS> X_data;        		///< Storage for solution vector X.
	std::array<float, ROWS> B_data;        		///< Storage for vector B data.

	std::array<float, COLS * ROWS> AT_data;   	///< Storage for transpose of A 		(A^T).
	std::array<float, COLS * COLS> AT_A_data; 	///< Storage for intermediate matrix 	(A^T * A).
	std::array<float, COLS * COLS> AT_AI_data; 	///< Storage for intermediate matrix 	(A^T * A)^-1.
	std::array<float, COLS> AT_B_data; 			///< Storage for intermediate matrix 	(A^T * B).

	arm_matrix_instance_f32 A_matrix; 		///< CMSIS-DSP matrix instance for A.
	arm_matrix_instance_f32 X_matrix; 		///< CMSIS-DSP matrix instance for X.
	arm_matrix_instance_f32 B_matrix; 		///< CMSIS-DSP matrix instance for B.

	arm_matrix_instance_f32 AT_matrix;   	///< CMSIS-DSP matrix instance for 	(A^T).
	arm_matrix_instance_f32 AT_A_matrix; 	///< CMSIS-DSP matrix instance for 	(A^T * A).
	arm_matrix_instance_f32 AT_AI_matrix; 	///< CMSIS-DSP matrix instance for (A^T * A)^-1.
	arm_matrix_instance_f32 AT_B_matrix;	///< CMSIS-DSP matrix instance for (A^T * B).

	bool initialized;
};

template<size_t ROWS, size_t COLS>
LeastSquaresFit<ROWS, COLS>::LeastSquaresFit() noexcept:
	initialized(false)
{
	/// No code at this time.
}

template<size_t ROWS, size_t COLS>
LeastSquaresFit<ROWS, COLS>::~LeastSquaresFit() noexcept
{
	/// No code at this time.
}

template<size_t ROWS, size_t COLS>
bool LeastSquaresFit<ROWS, COLS>::Initialize() noexcept
{
	arm_mat_init_f32(&A_matrix, ROWS, COLS, A_data.data());
	arm_mat_init_f32(&X_matrix, COLS, 1, X_data.data());
	arm_mat_init_f32(&B_matrix, ROWS, 1, B_data.data());

	arm_mat_init_f32(&AT_matrix, COLS, ROWS, AT_data.data());
	arm_mat_init_f32(&AT_A_matrix, COLS, COLS, AT_A_data.data());
	arm_mat_init_f32(&AT_AI_matrix, COLS, COLS, AT_AI_data.data());
	arm_mat_init_f32(&AT_B_matrix, COLS, 1, AT_B_data.data());

	return true;
}

template<size_t ROWS, size_t COLS>
void LeastSquaresFit<ROWS, COLS>::SetA(const float (&values)[ROWS * COLS]) noexcept {
	std::copy(std::begin(values), std::end(values), A_data.begin());
}

template<size_t ROWS, size_t COLS>
void LeastSquaresFit<ROWS, COLS>::SetA(const std::array<float, ROWS * COLS>& values) noexcept {
	A_data = values;
}

template<size_t ROWS, size_t COLS>
void LeastSquaresFit<ROWS, COLS>::SetB(const float (&values)[ROWS]) noexcept {
	std::copy(std::begin(values), std::end(values), B_data.begin());
}

template<size_t ROWS, size_t COLS>
void LeastSquaresFit<ROWS, COLS>::SetB(const std::array<float, ROWS>& values) noexcept {
	B_data = values;
}

template<size_t ROWS, size_t COLS>
bool LeastSquaresFit<ROWS, COLS>::UpdateMatrixAElement(size_t row, size_t col, float newValue) noexcept {
	if (row < ROWS && col < COLS) {
		A_data[row * COLS + col] = newValue;
		return true;
	}
	return false;
}

template<size_t ROWS, size_t COLS>
bool LeastSquaresFit<ROWS, COLS>::UpdateMatrixBElement(size_t row, float newValue) noexcept {
	if (row < ROWS) {
		B_data[row] = newValue;
		return true;
	}
	return false;
}

template<size_t ROWS, size_t COLS>
bool LeastSquaresFit<ROWS, COLS>::Fit() noexcept {
	/// Only time we need to make sure the arm arrays are initialized is right before Fitting calculations.
	if(EnsureInitialized()) {
		/// Compute (A^T)
		arm_mat_trans_f32(&A_matrix, &AT_matrix);

		/// Compute (A^T * A)
		arm_mat_mult_f32(&AT_matrix, &A_matrix, &AT_A_matrix);

		/// Attempt to compute inverse (A^T * A)^-1
		if (arm_mat_inverse_f32(&AT_A_matrix, &AT_AI_matrix) != ARM_MATH_SUCCESS) {
			return false; /// Matrix A^T * A is singular and cannot be inverted
		}

		/// Compute (A^T * B)
		arm_mat_mult_f32(&AT_matrix, &B_matrix, &AT_B_matrix);

		/// Compute X = (A^T * A)^-1 * (A^T * B)
		if (arm_mat_mult_f32(&AT_AI_matrix, &AT_B_matrix, &X_matrix) != ARM_MATH_SUCCESS) {
			return false; /// Error in matrix multiplication
		}

		return true;
	}
	return false;
}

template<size_t ROWS, size_t COLS>
void LeastSquaresFit<ROWS, COLS>::GetResult(float (&result)[COLS]) const noexcept {
	std::copy(X_data.begin(), X_data.end(), std::begin(result));
}

template<size_t ROWS, size_t COLS>
void LeastSquaresFit<ROWS, COLS>::GetResult(std::array<float, COLS>& result) const noexcept {
    std::copy(X_data.begin(), X_data.end(), result.begin());
}

template<size_t ROWS, size_t COLS>
bool LeastSquaresFit<ROWS, COLS>::FitAndGetResult(float (&result)[COLS]) noexcept {
	if (!Fit()) {
		return false; // Fitting process failed
	}
	GetResult(result);
	return true;
}

template<size_t ROWS, size_t COLS>
bool LeastSquaresFit<ROWS, COLS>::FitAndGetResult(std::array<float, COLS>& result) noexcept {
    if (!Fit()) {
        return false; // Fitting process failed
    }
    GetResult(result);
    return true;
}


#endif /* UTILITIES_COMMON_LEASTSQUARESFIT_H_ */
