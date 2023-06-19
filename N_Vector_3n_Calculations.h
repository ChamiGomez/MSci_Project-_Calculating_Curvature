#pragma once
#include <vector>
#include <iostream>
#include <cmath>
#include <string>

using namespace std;

namespace Vector_3n_Calculations {
	bool Check_Size(vector<double> input_vector, string calc_name) {
		if (input_vector.size() != 3) {
			cout << "Vector is not required size for " << calc_name << " calculations (3 elements corresponding to x, y and z)! Size is actually: " << input_vector.size() << endl;
			return false;
		}
		return true;
	}

	bool Check_Size(vector<double> lhs, vector<double> rhs, string calc_name) {
		if (lhs.size() != 3 || rhs.size() != 3) {
			cout << "Vectors are not required size for " << calc_name << " calculations (3 elements for x, y and z)!" << endl << "LHS vector size: " << lhs.size() << " and RHS vector size: " << rhs.size() << endl;
			return false;
		}
		return true;
	}

	vector<double> Cross(vector<double> lhs, vector<double> rhs) {
		if (!Check_Size(lhs, rhs, "cross product")) exit(0);

		vector<double> ans;
		ans.resize(3);
		ans[0] = (lhs[1] * rhs[2]) - (lhs[2] * rhs[1]);
		ans[1] = (lhs[2] * rhs[0]) - (lhs[0] * rhs[2]);
		ans[2] = (lhs[0] * rhs[1]) - (lhs[1] * rhs[0]);
		return ans;
	}

	vector<double> Normalise(vector<double> input_vector) {
		if (!Check_Size(input_vector, "normalisation")) exit(0);

		double magnitude;
		vector<double> ans;
		magnitude = sqrt(pow(input_vector[0], 2) + pow(input_vector[1], 2) + pow(input_vector[2], 2));
		ans.push_back(input_vector[0] / magnitude);
		ans.push_back(input_vector[1] / magnitude);
		ans.push_back(input_vector[2] / magnitude);
		return ans;
	}

	double Magnitude(vector<double> input_vector) {
		if (!Check_Size(input_vector, "magnitude")) exit(0);

		double vector_magnitude;
		vector_magnitude = sqrt(pow(input_vector[0], 2) + pow(input_vector[1], 2) + pow(input_vector[2], 2));
		return vector_magnitude;
	}

	vector<double> Add(vector<double> lhs, vector<double> rhs) {
		if (!Check_Size(lhs, rhs, "addition")) exit(0);

		vector<double> ans;
		for (int i = 0; i < 3; i++) ans.push_back(lhs[i] + rhs[i]);
		return ans;
	}

	vector<double> Subtract(vector<double> lhs, vector<double> rhs) {
		if (!Check_Size(lhs, rhs, "subtraction")) exit(0);

		vector<double> ans;
		for (int i = 0; i < 3; i++) ans.push_back(lhs[i] - rhs[i]);
		return ans;
	}

	double Dot(vector<double> lhs, vector<double> rhs) {
		if (!Check_Size(lhs, rhs, "dot product")) exit(0);

		double ans;
		ans = (lhs[0] * rhs[0]) + (lhs[1] * rhs[1]) + (lhs[2] * rhs[2]);
		return ans;
	}

	vector<double> Scalar_Multiple(vector<double> input_vector, double scalar) {
		if (!Check_Size(input_vector, "scalar multiple")) exit(0);

		vector<double> ans;
		for (int i = 0; i < input_vector.size(); i++)
			ans.push_back(input_vector[i] * scalar);
		return ans;
	}
}

namespace vec3 = Vector_3n_Calculations;