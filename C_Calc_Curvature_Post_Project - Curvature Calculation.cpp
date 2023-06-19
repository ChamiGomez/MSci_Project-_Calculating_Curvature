#include "C_Calc_Curvature_Post_Project.h"
#include "N_Vector_3n_Calculations.h"
#include "N_Vector_2n_Calculations.h"
#include "N_Matrix_2x2_Calculations.h"
#include <cmath>
#include <iostream>

void CCalculatingCurvature::Calculate_Face_Normals(void) {
	//Output opening statement
	cout << "Beginning calculating face normals..." << endl;

	//Check - clear old variables but should never be called
	edge_1.clear();
	edge_2.clear();
	edge_3.clear();
	face_normals.clear();

	//Calculate edge vectors
	//Calculate edge_1
	edge_1.resize(faces.size());
	for (int i = 0; i < faces.size(); i++) {
		for (int j = 0; j < faces[i].size(); j++)
			edge_1[i].push_back(vertices[faces[i][2] - 1][j] - vertices[faces[i][1] - 1][j]);
	}
	//Calculate edge_2
	edge_2.resize(faces.size());
	for (int i = 0; i < faces.size(); i++) {
		for (int j = 0; j < faces[i].size(); j++)
			edge_2[i].push_back(vertices[faces[i][0] - 1][j] - vertices[faces[i][2] - 1][j]);
	}
	//Calculate edge_3
	edge_3.resize(faces.size());
	for (int i = 0; i < faces.size(); i++) {
		for (int j = 0; j < faces[i].size(); j++)
			edge_2[i].push_back(vertices[faces[i][0] - 1][j] - vertices[faces[i][2] - 1][j]);
	}

	//Calculate normals to edges (using cross product)
	for (int i = 0; i < faces.size(); i++) {
		vector<double> temp_v;
		temp_v = vec3::Cross(edge_1[i], edge_2[i]);
		temp_v = vec3::Normalise(temp_v);
		face_normals.push_back(temp_v);
	}

	//Output closing statement
	cout << "Finished calculating face normals." << endl;
}

void CCalculatingCurvature::Calculate_Vertice_Normals(void) {
	//Output opening statement
	cout << "Beginning calculating vertice normals..." << endl;

	//Check - clear old variables but should never be called
	edge_1_normalised.clear();
	edge_2_normalised.clear();
	edge_3_normalised.clear();

	//Calculate Normalised edges
	for (int i = 0; i < faces.size(); i++) {
		vector<double> temp;
		temp = vec3::Normalise(edge_1[i]);
		edge_1_normalised.push_back(temp);
		temp = vec3::Normalise(edge_2[i]);
		edge_2_normalised.push_back(temp);
		temp = vec3::Normalise(edge_3[i]);
		edge_3_normalised.push_back(temp);
	}

	//Normalisation Procedure: calculate face areas, calculate triangle barycentres, calculate vertice normals

	//Calculate Face Area
	//Calculate Edge Lengths
	vector<double> edge_1_length, edge_2_length, edge_3_length;
	vector<vector<double>> edge_lengths_squared;
	for (int i = 0; i < faces.size(); i++) {
		edge_1_length.push_back(sqrt(pow(edge_1[i][0], 2) + pow(edge_1[i][1], 2) + pow(edge_1[i][2], 2)));
		edge_2_length.push_back(sqrt(pow(edge_2[i][0], 2) + pow(edge_2[i][1], 2) + pow(edge_2[i][2], 2)));
		edge_3_length.push_back(sqrt(pow(edge_3[i][0], 2) + pow(edge_3[i][1], 2) + pow(edge_3[i][2], 2)));
	}		
	edge_lengths_squared.resize(faces.size());
	for (int i = 0; i < faces.size(); i++) {
		edge_lengths_squared[i].push_back(pow(edge_1_length[i], 2));
		edge_lengths_squared[i].push_back(pow(edge_2_length[i], 2));
		edge_lengths_squared[i].push_back(pow(edge_3_length[i], 2));
	}

	//Calculate Face Areas
	vector<double> face_areas;
	for (int i = 0; i < faces.size(); i++) {
		vector<double> temp_edge_normal;
		double temp_face_area;
		temp_edge_normal = vec3::Cross(edge_1[i], edge_2[i]);
		temp_face_area = 0.5 * vec3::Magnitude(temp_edge_normal);
		face_areas.push_back(temp_face_area);
	}

	//Calculate triangle_barycentres
	//Note: Later used to calculate the cot of the angles for the Voronoi area calculation
	//Note: Later is checked to determine if it is inside or outside the triangle
	vector<vector<double>> triangle_barycentres;
	triangle_barycentres.resize(faces.size());
	for (int i = 0; i < faces.size(); i++) {
		triangle_barycentres[i].push_back(edge_lengths_squared[i][0] * (edge_lengths_squared[i][1] + edge_lengths_squared[i][2] - edge_lengths_squared[i][0]));
		triangle_barycentres[i].push_back(edge_lengths_squared[i][1] * (edge_lengths_squared[i][2] + edge_lengths_squared[i][0] - edge_lengths_squared[i][1]));
		triangle_barycentres[i].push_back(edge_lengths_squared[i][2] * (edge_lengths_squared[i][0] + edge_lengths_squared[i][1] - edge_lengths_squared[i][2]));
	}

	//Calculate Vertice Normals
	//Resize member variables (and clear, though shouldn't be required)
	corner_weights.clear();
	corner_weights.resize(faces.size());
	for (int i = 0; i < faces.size(); i++)
		corner_weights[i].resize(3);
	vertice_weights.clear();
	vertice_weights.resize(vertices.size());
	vertice_normals.clear();
	vertice_normals.resize(vertices.size());
	up.clear();
	up.resize(vertices.size());
	vp.clear();
	vp.resize(vertices.size());
	for (int i = 0; i < vertices.size(); i++) {
		vertice_normals[i].resize(3);
		up[i].resize(3);
		vp[i].resize(3); //Not sure if necessary - 31/12/21 for continuous output of curvature in catenoid tests
	}

	//Loop over each face
	double weight_1, weight_2, weight_3;
	for (int i = 0; i < faces.size(); i++) {
		//Calculate weights according to N.Max (1999)
		weight_1 = face_areas[i] / (pow(edge_2_length[i], 2) * pow(edge_3_length[i], 2));
		weight_2 = face_areas[i] / (pow(edge_1_length[i], 2) * pow(edge_3_length[i], 2));
		weight_3 = face_areas[i] / (pow(edge_2_length[i], 2) * pow(edge_1_length[i], 2));


		//Continuously add weight_x and face_normals[i] to running total for vertice_normal[face_x[i]]
		vector<double> temp_1, temp_2, temp_3;
		temp_1 = vec3::Add(vertice_normals[faces[i][0] - 1], vec3::Scalar_Multiple(face_normals[i], weight_1)); //-1 used to correct indices
		vertice_normals[faces[i][0] - 1].clear();
		vertice_normals[faces[i][0] - 1].insert(begin(vertice_normals[faces[i][0] - 1]), begin(temp_1), end(temp_1));
		temp_2 = vec3::Add(vertice_normals[faces[i][1] - 1], vec3::Scalar_Multiple(face_normals[i], weight_2)); //-1 used to correct indices
		vertice_normals[faces[i][1] - 1].clear();
		vertice_normals[faces[i][1] - 1].insert(begin(vertice_normals[faces[i][1] - 1]), begin(temp_2), end(temp_2));
		temp_3 = vec3::Add(vertice_normals[faces[i][2] - 1], vec3::Scalar_Multiple(face_normals[i], weight_3));
		vertice_normals[faces[i][2] - 1].clear();
		vertice_normals[faces[i][2] - 1].insert(begin(vertice_normals[faces[i][2] - 1]), begin(temp_3), end(temp_3));

		//Calculate areas for weights according to Meyer et al. [2002]
		//Check if the tringle is acute, right or obtuse
		double triangle_barycentre_scale;
		if (triangle_barycentres[i][0] <= 0) {
			corner_weights[i][1] = (-0.25 * edge_lengths_squared[i][2] * face_areas[i]) / vec3::Dot(edge_1[i], edge_3[i]);
			corner_weights[i][2] = (-0.25 * edge_lengths_squared[i][1] * face_areas[i]) / vec3::Dot(edge_1[i], edge_2[i]);
			corner_weights[i][0] = face_areas[i] - corner_weights[i][1] - corner_weights[i][2];
		}
		else if (triangle_barycentres[i][1] <= 0) {
			corner_weights[i][2] = (-0.25 * edge_lengths_squared[i][0] * face_areas[i]) / vec3::Dot(edge_2[i], edge_1[i]);
			corner_weights[i][0] = (-0.25 * edge_lengths_squared[i][2] * face_areas[i]) / vec3::Dot(edge_2[i], edge_3[i]);
			corner_weights[i][1] = face_areas[i] - corner_weights[i][0] - corner_weights[i][2];
		}
		else if (triangle_barycentres[i][2] <= 0) {
			corner_weights[i][0] = (-0.25 * edge_lengths_squared[i][1] * face_areas[i]) / vec3::Dot(edge_3[i], edge_2[i]);
			corner_weights[i][1] = (-0.25 * edge_lengths_squared[i][0] * face_areas[i]) / vec3::Dot(edge_3[i], edge_1[i]);
			corner_weights[i][2] = face_areas[i] - corner_weights[i][0] - corner_weights[i][1];
		}
		else {
			triangle_barycentre_scale = 0.5 * face_areas[i] / (triangle_barycentres[i][0] + triangle_barycentres[i][1] + triangle_barycentres[i][2]);
			corner_weights[i][0] = triangle_barycentre_scale * (triangle_barycentres[i][1] + triangle_barycentres[i][2]);
			corner_weights[i][1] = triangle_barycentre_scale * (triangle_barycentres[i][0] + triangle_barycentres[i][2]);
			corner_weights[i][2] = triangle_barycentre_scale * (triangle_barycentres[i][1] + triangle_barycentres[i][0]);
		}

		//Assign vertex weights
		vertice_weights[faces[i][0] - 1] += corner_weights[i][0];
		vertice_weights[faces[i][1] - 1] += corner_weights[i][1];
		vertice_weights[faces[i][2] - 1] += corner_weights[i][2];

		//Calculate initial coordinate system
		up[faces[i][0] - 1].clear();
		up[faces[i][0] - 1].insert(begin(up[faces[i][0] - 1]), begin(edge_3_normalised[i]), end(edge_3_normalised[i]));
		up[faces[i][1] - 1].clear();
		up[faces[i][1] - 1].insert(begin(up[faces[i][1] - 1]), begin(edge_1_normalised[i]), end(edge_1_normalised[i]));
		up[faces[i][2] - 1].clear();
		up[faces[i][2] - 1].insert(begin(up[faces[i][2] - 1]), begin(edge_2_normalised[i]), end(edge_2_normalised[i]));
	}

	//Normalise vertex normals
	for (int i = 0; i < vertices.size(); i++) {
		vector<double> temp_v;
		temp_v = vec3::Normalise(vertice_normals[i]);
		vertice_normals[i].clear();
		vertice_normals.push_back(temp_v);
	}
	//Normalisation procedure completed

	//Calculate initial vertex coordinate system
	for (int i = 0; i < vertices.size(); i++) {
		vector<double> temp_v;
		temp_v = vec3::Cross(up[i], vertice_normals[i]);
		temp_v = vec3::Normalise(temp_v);
		up[i].clear();
		up.push_back(temp_v);
		temp_v = vec3::Cross(vertice_normals[i], up[i]);
		vp[i].clear();
		vp.push_back(temp_v);
	}

	//Output closing statement
	cout << "Finished calculating vertice normals." << endl;
}

void CCalculatingCurvature::Calculate_Curvature(bool output_faces_index, int output_frequency) {
	//Output opening statement
	cout << "Beginning calculating curvature..." << endl;

	//Initialise Outputs: FaceSFM, VertexSFM and the Corner Voronoi Weights (Note: clears just for safety)
	faces_sft.clear();
	faces_sft.resize(faces.size());
	for (int i = 0; i < faces.size(); i++) {
		faces_sft[i].resize(2);
		for (int j = 0; j < 2; j++)
			faces_sft[i][j].resize(2);
	}
	vertices_sft.clear();
	vertices_sft.resize(vertices.size());
	for (int i = 0; i < vertices.size(); i++) {
		vertices_sft[i].resize(2);
		for (int j = 0; j < 2; j++)
			vertices_sft[i][j].resize(2);
	}
	corner_voronoi_weights.clear();
	corner_voronoi_weights.resize(faces.size());

	//Loop for calculating curvature
	vector<double> kn; //Check if needed - no use of this list?
	if (output_faces_index)
		cout << "Total number of faces: " << faces.size() << "\t";
	int collinear_vertices = 0;
	for (int i = 0; i < faces.size(); i++) {
		if (output_faces_index) {
			if ((i % output_frequency) == 0)
				cout << i << "\t";
		}
		//Calculate curvature per face
		//Set face coordinate frame
		vector<double> v;
		v = vec3::Cross(face_normals[i], edge_1_normalised[i]); //u = edge_1_normalised[i]
		v = vec3::Normalise(v);

		//Solve least square problem with form Ax = b
		double A_array[18] = { //Keep as a simple array for now (not higher dimensional) as assigned to gsl_matrix format later
			vec3::Dot(edge_1[i], edge_1_normalised[i]), vec3::Dot(edge_1[i], v), 0,
			0, vec3::Dot(edge_1[i], edge_1_normalised[i]), vec3::Dot(edge_1[i], v),
			vec3::Dot(edge_2[i], edge_1_normalised[i]), vec3::Dot(edge_2[i], v), 0,
			0, vec3::Dot(edge_2[i], edge_1_normalised[i]), vec3::Dot(edge_2[i], v),
			vec3::Dot(edge_3[i], edge_1_normalised[i]), vec3::Dot(edge_3[i], v), 0,
			0, vec3::Dot(edge_3[i], edge_1_normalised[i]), vec3::Dot(edge_3[i], v)
		};

		double b_array[6] = {
			vec3::Dot(vec3::Subtract(vertice_normals[faces[i][2] - 1], vertice_normals[faces[i][1] - 1]), edge_1_normalised[i]),
			vec3::Dot(vec3::Subtract(vertice_normals[faces[i][2] - 1], vertice_normals[faces[i][1] - 1]), v),
			vec3::Dot(vec3::Subtract(vertice_normals[faces[i][0] - 1], vertice_normals[faces[i][2] - 1]), edge_1_normalised[i]),
			vec3::Dot(vec3::Subtract(vertice_normals[faces[i][0] - 1], vertice_normals[faces[i][2] - 1]), v),
			vec3::Dot(vec3::Subtract(vertice_normals[faces[i][1] - 1], vertice_normals[faces[i][0] - 1]), edge_1_normalised[i]),
			vec3::Dot(vec3::Subtract(vertice_normals[faces[i][1] - 1], vertice_normals[faces[i][0] - 1]), v)
		};

		//Access the C arrays above through GSL views
		gsl_matrix_view A = gsl_matrix_view_array(A_array, 6, 3);
		gsl_vector_view b = gsl_vector_view_array(b_array, 6);

		//Allocate memory for decomposition of A = QRP^T
		gsl_vector* tau = gsl_vector_alloc(3);
		gsl_permutation* p = gsl_permutation_alloc(3);
		int signum = 0;
		gsl_vector* norm = gsl_vector_alloc(3);

		//Perform QRPT Decomposition
		bool QRPT_Decomp;
		QRPT_Decomp = gsl_linalg_QRPT_decomp(&A.matrix, tau, p, &signum, norm);

		//Get rank of matrix
		size_t rank;
		rank = gsl_linalg_QRPT_rank(&A.matrix, 0.000001);

		///Check use of rank error and remove?
		//New - check
		if (rank == 0) {
			//cout << "Face " << i + 1 << "/" << faces.size() << " has a rank of 0 so has been skipped" << endl;

			if (collinear_vertices == 0) {
				for (int p = 0; p < 6; p++) {
					for (int q = 0; q < 3; q++)
						cout << A_array[q + (p * 3)] << "\t";
					cout << endl;
				}
			}
			collinear_vertices++;
			continue;
		}
		//else
			//cout << "Face " << i + 1 << "/" << faces.size() << " has a rank of " << rank << endl;

		//Allocate memory for solving using least-squares		
		gsl_vector* x = gsl_vector_alloc(3);
		gsl_vector* residual = gsl_vector_alloc(6);

		//Find least-squares solution and store in x
		bool least_squares;
		least_squares = gsl_linalg_QRPT_lssolve2(&A.matrix, tau, p, &b.vector, rank, x, residual);

		//Assign values
		faces_sft[i][0][0] = gsl_vector_get(x, 0);
		faces_sft[i][0][1] = gsl_vector_get(x, 1);
		faces_sft[i][1][0] = gsl_vector_get(x, 1);
		faces_sft[i][1][1] = gsl_vector_get(x, 2);

		//Check use
		kn.push_back(faces_sft[i][0][0]);

		//Calculate Curvature per Vertex
		//Calculate Voronoi Weights
		corner_voronoi_weights[i].push_back(corner_weights[i][0] / vertice_weights[faces[i][0] - 1]);
		corner_voronoi_weights[i].push_back(corner_weights[i][1] / vertice_weights[faces[i][1] - 1]);
		corner_voronoi_weights[i].push_back(corner_weights[i][2] / vertice_weights[faces[i][2] - 1]);

		//Calculate new coordinate system and project the tensor
		for (int j = 0; j < 3; j++) {
			vector<double> vertice_curvature_tensor_elements, temp_v;
			vertice_curvature_tensor_elements = Project_Curvature_Tensor(edge_1_normalised[i], v, x, faces[i][j] - 1, i); //Note: -1 to correct for indices
			temp_v = vec3::Scalar_Multiple(vertice_curvature_tensor_elements, corner_voronoi_weights[i][j]);

			vector<vector<double>> temp_matrix;
			temp_matrix.resize(2);
			for (int i = 0; i < temp_matrix.size(); i++)
				temp_matrix[i].resize(2);
			temp_matrix[0][0] = temp_v[0];
			temp_matrix[0][1] = temp_v[1];
			temp_matrix[1][0] = temp_v[1];
			temp_matrix[1][1] = temp_v[2];

			vertices_sft[faces[i][j] - 1] = mat2x2::Add(vertices_sft[faces[i][j] - 1], temp_matrix);
		}
		if (output_faces_index) {
			if (i == faces.size() - 1)
				cout << "Final face index: " << i << endl;
		}
	}

	//Output closing statements
	cout << "There were " << faces.size() << " faces: " << faces.size() - collinear_vertices << " have had their curvature calculated and " << collinear_vertices << " have been skipped (due to rank = 0)" << endl;
	cout << "Finished calculating curvature." << endl;
}

void CCalculatingCurvature::Get_Principal_Curvatures(void) {
	//Output opening statement
	cout << "Beginning calculating principal curvatures..." << endl;

	//Clear and resize matrices
	principal_curvatures.clear();
	principal_curvatures.resize(vertices.size()); //NOTE: transposed of matrix in matlab
	principal_direction_1.clear();
	principal_direction_1.resize(vertices.size());
	principal_direction_2.clear();
	principal_direction_2.resize(vertices.size());

	//Could use eigenvalue and eigenvector solver
	//Loop from Rusinkiewicz that also considers direction
	for (int i = 0; i < vertices.size(); i++) {
		vector<double> temp_v;
		temp_v = vec3::Cross(up[i], vp[i]);
		Rotate_Coordinate_System(i, temp_v);
		double ku, kuv, kv, c = 1.0, s = 0.0, tt = 0.0;
		ku = vertices_sft[i][0][0];
		kuv = vertices_sft[i][0][1];
		kv = vertices_sft[i][1][1];

		if (kuv != 0) {
			//Jacobi rotation to diagonalize
			double h;
			h = 0.5 * (kv - ku) / kuv;

			if (h < 0)
				tt = 1 / (h - sqrt(1 + pow(h, 2)));
			else
				tt = 1 / (h + sqrt(1 + pow(h, 2)));

			c = 1 / sqrt(1 + pow(tt, 2));
			s = tt * c;
		}

		double k1, k2;
		k1 = ku - (tt * kuv);
		k2 = kv + (tt * kuv);

		if (abs(k1) >= abs(k2)) {
			vector<double> temp_v1, temp_v2;
			temp_v1 = vec3::Scalar_Multiple(rotated_u, c);
			temp_v2 = vec3::Scalar_Multiple(rotated_v, s);
			temp_v1 = vec3::Subtract(temp_v1, temp_v2);
			principal_direction_1[i].clear();
			principal_direction_1[i].insert(begin(principal_direction_1[i]), begin(temp_v1), end(temp_v1));
		}
		else {
			double temp_d;
			temp_d = k1;
			k1 = k2;
			k2 = temp_d;
			vector<double> temp_v1, temp_v2;
			temp_v1 = vec3::Scalar_Multiple(rotated_u, s);
			temp_v2 = vec3::Scalar_Multiple(rotated_v, c);
			temp_v1 = vec3::Add(temp_v1, temp_v2);
			principal_direction_1[i].clear();
			principal_direction_1[i].insert(begin(principal_direction_1[i]), begin(temp_v1), end(temp_v1));
		}

		temp_v = vec3::Cross(temp_v, principal_direction_1[i]);
		principal_direction_2[i].clear();
		principal_direction_2[i].insert(begin(principal_direction_2[i]), begin(temp_v), end(temp_v));
		principal_curvatures[i].clear();
		principal_curvatures[i].push_back(k1);
		principal_curvatures[i].push_back(k2);
	}

	//Output closing statement
	cout << "Finished calculating principal curvatures." << endl;
}

void CCalculatingCurvature::Calculate_Gaussian_Curvatures(void) {
	cout << "Begin calculating Gaussian curvature..." << endl;
	gaussian_curvature.clear();
	for (int i = 0; i < vertices.size(); i++)
		gaussian_curvature.push_back(principal_curvatures[i][0] * principal_curvatures[i][1]);
	cout << "Finished calculating Gaussian curvature." << endl << endl;
}

void CCalculatingCurvature::Calculate_Mean_Curvatures(void) {
	cout << "Begin calculating mean curvature..." << endl;
	mean_curvature.clear();
	for (int i = 0; i < vertices.size(); i++)
		mean_curvature.push_back((principal_curvatures[i][0] + principal_curvatures[i][1]) / 2.0);
	cout << "Finished calculating mean curvature." << endl << endl;
}

//Background Curvature Calculation Functions
vector<double> CCalculatingCurvature::Project_Curvature_Tensor(vector<double> u, vector<double> v, gsl_vector* x, int vector_index, int face_normal_index) {
	//Rotate Coordinate System to get rotated_u and rotated_v
	Rotate_Coordinate_System(vector_index, face_normals[face_normal_index]);

	//Create old tensor using solution vector x from Ax =b
	vector<vector<double>> Old_Tensor;
	Old_Tensor.resize(2);
	for (int i = 0; i < Old_Tensor.size(); i++)
		Old_Tensor[i].resize(2);
	Old_Tensor[0][0] = gsl_vector_get(x, 0);
	Old_Tensor[0][1] = gsl_vector_get(x, 1);
	Old_Tensor[1][0] = gsl_vector_get(x, 1);
	Old_Tensor[1][1] = gsl_vector_get(x, 2);

	//Calculate doubles used in vector curvature tensor calculations
	vector<double> temp_vec_a, temp_vec_b;
	temp_vec_a.push_back(vec3::Dot(rotated_u, u));
	temp_vec_a.push_back(vec3::Dot(rotated_u, v));
	temp_vec_b.push_back(vec3::Dot(rotated_v, u));
	temp_vec_b.push_back(vec3::Dot(rotated_v, v));

	//Calculate elements for vertice curvature tensor
	vector<double> temp_vector, ans;
	double temp;

	temp_vector = vec2::Matrix_Multiplication_with_Vector(Old_Tensor, temp_vec_a);
	temp = vec2::Dot(temp_vector, temp_vec_a);
	ans.push_back(temp);

	temp_vector = vec2::Matrix_Multiplication_with_Vector(Old_Tensor, temp_vec_b);
	temp = vec2::Dot(temp_vector, temp_vec_a);
	ans.push_back(temp);

	temp_vector = vec2::Matrix_Multiplication_with_Vector(Old_Tensor, temp_vec_b);
	temp = vec2::Dot(temp_vector, temp_vec_b);
	ans.push_back(temp);

	return ans;
}

void CCalculatingCurvature::Rotate_Coordinate_System(int vector_index, vector<double> normal_vector) {
	rotated_u.clear();
	rotated_v.clear();
	rotated_u.insert(begin(rotated_u), begin(up[vector_index]), end(up[vector_index]));
	rotated_v.insert(begin(rotated_v), begin(vp[vector_index]), end(vp[vector_index]));

	vector<double> temp_v, np, perp, dperp;
	double ndot, temp_d;

	temp_v = vec3::Cross(up[vector_index], vp[vector_index]);
	for (int i = 0; i < temp_v.size(); i++)
		np.push_back(temp_v[i] / vec3::Magnitude(temp_v));
	ndot = vec3::Dot(normal_vector, np);

	if (ndot <= -1) {
		cout << "(Rotate Coordinate system) ndot = " << ndot << "\t";
		for (int i = 0; i < rotated_u.size(); i++) {
			rotated_u[i] = -rotated_u[i];
			rotated_v[i] = -rotated_v[i];
		}
	}
	else {
		perp = vec3::Subtract(normal_vector, vec3::Scalar_Multiple(np, ndot));
		temp_v.clear(); //Take out as return for Add_3D_vector is reinitialised?
		temp_v = vec3::Add(np, normal_vector);
		for (int i = 0; i < temp_v.size(); i++)
			dperp.push_back(temp_v[i] / (1 + ndot));

		temp_d = vec3::Dot(perp, rotated_u);
		temp_v.clear(); //Take out?
		temp_v = vec3::Scalar_Multiple(dperp, temp_d);
		rotated_u = vec3::Subtract(rotated_u, temp_v);

		temp_d = vec3::Dot(perp, rotated_v);
		temp_v.clear(); //Take out?
		temp_v = vec3::Scalar_Multiple(dperp, temp_d);
		rotated_v = vec3::Subtract(rotated_v, temp_v);
	}
}

double CCalculatingCurvature::Calculate_Mean_Curvature_for_Catenoids(int z_value, double z_step) {
	cout << "Begin calculating mean curvature..." << endl;
	mean_curvature.clear();

	//Calculate top layer
	int num_layers = ((z_value / z_step) * 2) + 1 + 2;
	int top_limit = num_layers - 2; //-1 to start indexing layers from 0, and -1 to ensure that the 2nd from top isn't used (so correct coordinates to bottom of this layer)

	//Calculate mean curvature (without bottom and top layers)
	double modified_mean_curvature_total = 0.0, modified_mean_curvature_mean_for_dataset = 0.0;
	int vertices_counter_excluding_top_and_bottom_layer = 0;
	for (int i = 0; i < vertices.size(); i++) {
		mean_curvature.push_back((principal_curvatures[i][0] + principal_curvatures[i][1]) / 2.0);
		if (vertices[i][2] == 1.0 || vertices[i][2] == top_limit)
			continue;
		else {
			modified_mean_curvature_total += mean_curvature[i];
			vertices_counter_excluding_top_and_bottom_layer++;
		}
	}

	modified_mean_curvature_mean_for_dataset = modified_mean_curvature_total / double(vertices_counter_excluding_top_and_bottom_layer);
	cout << "Finished calculating mean curvature." << endl << endl;
	return modified_mean_curvature_mean_for_dataset;
}

double CCalculatingCurvature::Calculate_Gaussian_Curvature_for_Catenoids(int z_value, double z_step) {
	cout << "Begin calculating Gaussian curvature..." << endl;
	gaussian_curvature.clear();

	//Calculate top layer
	int num_layers = ((z_value / z_step) * 2) + 1 + 2;
	int top_limit = num_layers - 2; //-1 to start indexing layers from 0, and -1 to ensure that the 2nd from top isn't used (so correct coordinates to bottom of this layer)

	//Calculate mean curvature (without bottom and top layers)
	double modified_gaussian_curvature_total = 0.0, modified_gaussian_curvature_mean_for_dataset = 0.0;
	int vertices_counter_excluding_top_and_bottom_layer = 0;
	for (int i = 0; i < vertices.size(); i++) {
		gaussian_curvature.push_back(principal_curvatures[i][0] * principal_curvatures[i][1]);
		if (vertices[i][2] == 1.0 || vertices[i][2] == top_limit)
			continue;
		else {
			modified_gaussian_curvature_total += gaussian_curvature[i];
			vertices_counter_excluding_top_and_bottom_layer++;
		}
	}

	modified_gaussian_curvature_mean_for_dataset = modified_gaussian_curvature_total / double(vertices_counter_excluding_top_and_bottom_layer);
	cout << "Finished calculating Gaussian curvature." << endl << endl;
	return modified_gaussian_curvature_mean_for_dataset;
}