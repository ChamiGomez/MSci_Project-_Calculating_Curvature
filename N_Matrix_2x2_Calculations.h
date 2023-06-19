#pragma once
#include <vector>
#include <iostream>

using namespace std;

namespace Matrix_2x2_Calculations {
	vector<vector<double>> Add(vector<vector<double>> lhs_matrix, vector<vector<double>> rhs_matrix) {
		if (lhs_matrix.size() != 2 || rhs_matrix.size() != 2) {
			cout << "Matrices do not have 2 outer layers!";
			exit(0);
		}
		vector<vector<double>> ans;
		ans.resize(2);
		for (int i = 0; i < 2; i++) {
			if (lhs_matrix[i].size() != 2 || rhs_matrix[i].size() != 2) {
				cout << "Matrices do not have 2 inner layers!";
				exit(0);
			}
			for (int j = 0; j < 2; j++)
				ans[i].push_back(lhs_matrix[i][j] + rhs_matrix[i][j]);
		}
		return ans;
	}
}

namespace mat2x2 = Matrix_2x2_Calculations;