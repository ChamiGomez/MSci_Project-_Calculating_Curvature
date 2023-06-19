#pragma once
#include <vector>
#include <iostream>

using namespace std;

namespace Vector_2n_Calculations {
	bool Check_Size(vector<double> input_vector, string calc_name) {
		if (input_vector.size() != 3) {
			cout << "Vector is not required size for " << calc_name << " calculations (2 elements corresponding to x and y)! Size is actually: " << input_vector.size() << endl;
			return false;
		}
		return true;
	}

	bool Check_Size(vector<double> lhs, vector<double> rhs, string calc_name) {
		if (lhs.size() != 3 || rhs.size() != 3) {
			cout << "Vectors are not required size for " << calc_name << " calculations (2 elements for x and y)!" << endl << "LHS vector size: " << lhs.size() << " and RHS vector size: " << rhs.size() << endl;
			return false;
		}
		return true;
	}

	bool Check_Size(vector<vector<double>> matrix, vector<double> input_v, string calc_name) {
		if (matrix.size() != 2 || matrix[0].size() != 2 || matrix[1].size() != 2) {
			cout << "Matrix is not the correct size of 2x2 for " << calc_name << " calculations!" << endl;
			return false;
		}
		else if (input_v.size() != 2) {
			cout << "Vector is not required size for " << calc_name << " calculations (2 elements corresponding to x and y)! Size is actually: " << input_v.size() << endl;
			return false;
		}
		return true;
	}

	double Dot(vector<double> lhs, vector<double> rhs) {
		if (!Check_Size(lhs, rhs, "dot product")) exit(0);

		double ans;
		ans = (lhs[0] * rhs[0]) + (lhs[1] * rhs[1]);
		return ans;
	}

	//Multiplies a 2x2 matrix with a column vector of 2 elements
	vector<double> Matrix_Multiplication_with_Vector(vector<vector<double>> matrix, vector<double> input_v) {
		if (!Check_Size(matrix, input_v, "matrix multiplication with a vector")) exit(0);

		vector<double> ans;
		ans.push_back((matrix[0][0] * input_v[0]) + (matrix[0][1] * input_v[1]));
		ans.push_back((matrix[1][0] * input_v[0]) + (matrix[1][1] * input_v[1]));
		return ans;
	}
}

namespace vec2 = Vector_2n_Calculations;