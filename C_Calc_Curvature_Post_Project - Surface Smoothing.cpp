#include "C_Calc_Curvature_Post_Project.h"
#include "N_Vector_3n_Calculations.h"
#include <iostream>

void CCalculatingCurvature::Calculate_Face_Centres(void) {
	cout << "Begin calculating face centres..." << endl;

	if (!vertices.empty())
		face_centres.clear(); //Reinitialise face_centres if it contains data from a previous call

	face_centres.resize(faces.size());
	for (int i = 0; i < faces.size(); i++) {
		vector<double> total_face_centre, temp_face_centre;
		total_face_centre.resize(faces[i].size());
		temp_face_centre.resize(faces[i].size());
		for (int j = 0; j < faces[i].size(); j++) {
			if (vertices.empty()) {
				for (int k = 0; k < pre_smoothing_vertices[faces[i][j] - 1].size(); k++)
					total_face_centre[j] += pre_smoothing_vertices[faces[i][k] - 1][j];
			}
			else {
				for (int k = 0; k < vertices[faces[i][j] - 1].size(); k++)
					total_face_centre[j] += vertices[faces[i][k] - 1][j];
			}
		}
		for (int k = 0; k < faces[i].size(); k++)
			temp_face_centre[k] = total_face_centre[k] / faces[i].size();
		face_centres[i].insert(begin(face_centres[i]), begin(temp_face_centre), end(temp_face_centre));
	}

	cout << "Finished calculating face centres." << endl;
}

void CCalculatingCurvature::Calculate_Edge_Centres(bool output_edges_index, int output_frequency) {
	cout << "Begin calculating edge centres..." << endl;
	if (output_edges_index)
		cout << "Total number of edges: " << (faces.size() * 3) << "\t";

	if (!vertices.empty())
		edge_centres.clear(); //Reinitialise edge_centres if it contains data from a previous call

	//edge_centres 0, 1, 2 are coordinates of point, and 3 and 4 are the indices of the two vertices at the end of each edge
	for (int i = 0; i < (faces.size() * 3); i++) { //For each edge on each face (which is a triangle)
		if (output_edges_index) {
			if ((i % output_frequency) == 0)
				cout << i << "\t";
		}
		int face_number, edge_number;
		face_number = i / 3;
		edge_number = i % 3;
		bool new_edge = true;
		if (!edge_centres.empty()) {
			for (int j = 0; j < edge_centres.size(); j++) {
				if (edge_centres[j][3] == faces[face_number][edge_number] - 1) {
					if ((edge_number + 1) < 3) {
						if (edge_centres[j][4] == faces[face_number][edge_number + 1] - 1) {
							new_edge = false;
							break;
						}
					}
					else {
						if (edge_centres[j][4] == faces[face_number][0] - 1) {
							new_edge = false;
							break;
						}
					}
				}
				else if (edge_centres[j][4] == faces[face_number][edge_number] - 1) {
					if ((edge_number + 1) < 3) {
						if (edge_centres[j][3] == faces[face_number][edge_number + 1] - 1) {
							new_edge = false;
							break;
						}
					}
					else {
						if (edge_centres[j][3] == faces[face_number][0] - 1) {
							new_edge = false;
							break;
						}
					}
				}
			}
		}
		if (!new_edge) {
			continue;
		}
		else {
			edge_centres.resize(edge_centres.size() + 1);
			for (int k = 0; k < faces[face_number].size(); k++) {
				if (vertices.empty()) {
					double temp_value;
					if ((edge_number + 1) < 3) {
						temp_value = pre_smoothing_vertices[faces[face_number][edge_number] - 1][k];
						temp_value += pre_smoothing_vertices[faces[face_number][edge_number + 1] - 1][k];
						temp_value = temp_value / 2;
						edge_centres[edge_centres.size() - 1].push_back(temp_value);
					}
					else {
						temp_value = pre_smoothing_vertices[faces[face_number][edge_number] - 1][k];
						temp_value += pre_smoothing_vertices[faces[face_number][0] - 1][k];
						temp_value = temp_value / 2;
						edge_centres[edge_centres.size() - 1].push_back(temp_value);
					}
				}
				else {
					double temp_value;
					if ((edge_number + 1) < 3) {
						temp_value = vertices[faces[face_number][edge_number] - 1][k];
						temp_value += vertices[faces[face_number][edge_number + 1] - 1][k];
						temp_value = temp_value / 2;
						edge_centres[edge_centres.size() - 1].push_back(temp_value);
					}
					else {
						temp_value = vertices[faces[face_number][edge_number] - 1][k];
						temp_value += vertices[faces[face_number][0] - 1][k];
						temp_value = temp_value / 2;
						edge_centres[edge_centres.size() - 1].push_back(temp_value);
					}
				}
			}
			edge_centres[edge_centres.size() - 1].push_back(faces[face_number][edge_number] - 1);
			if ((edge_number + 1) < 3)
				edge_centres[edge_centres.size() - 1].push_back(faces[face_number][edge_number + 1] - 1);
			else
				edge_centres[edge_centres.size() - 1].push_back(faces[face_number][0] - 1);
		}
	}

	if (output_edges_index)
		cout << "Final edge index: " << (faces.size() * 3) - 1 << endl;
	cout << "Finished calculating edge centres." << endl;
}

vector<double> CCalculatingCurvature::Calculate_Triangular_Area(vector<double> point, vector<double> edge_centre, vector<double> face_centre) {
	if (point.size() != 3) {
		cout << "Point size should be 3 but is actually " << point.size() << endl;
		exit(0);
	}
	if (edge_centre.size() != 3) {
		cout << "Edge centre size should be 3 but is actually " << edge_centre.size() << endl;
		exit(0);
	}
	if (face_centre.size() != 3) {
		cout << "Face centre size should be 3 but is actually " << face_centre.size() << endl;
		exit(0);
	}

	vector<double> point_to_face_centre, edge_centre_to_face_centre, triangular_area;
	point_to_face_centre = vec3::Subtract(point, face_centre);
	edge_centre_to_face_centre = vec3::Subtract(edge_centre, face_centre);
	triangular_area = vec3::Scalar_Multiple(vec3::Cross(point_to_face_centre, edge_centre_to_face_centre), 0.5);

	return triangular_area;
}

void CCalculatingCurvature::Calculate_Vertex_Area_Weights_for_Smoothing(void) {
	cout << "Begin calculating vertex area weights for smoothing..." << endl;

	if (!vertices.empty())
		vertice_area_weights.clear(); //Reinitialise vertice_area_weights if it contains data from a previous call

	int zero_vertice_area_weight_counter = 0;
	vertice_area_weights.resize(pre_smoothing_vertices.size()); //Remains constant as pre_smoothing_vertices has same size as vertices
	//Calculate sum of adjacent areas to vertex
	for (int i = 0; i < pre_smoothing_vertices.size(); i++) { //For each vertex (also fine for additional calls as pre_smoothing_vertices is same size as vertices)
		vector<vector<double>> adjacent_face_centres, adjacent_edge_centres;
		//Retrieve adjacent_face_centres
		for (int j = 0; j < faces.size(); j++) {
			for (int k = 0; k < faces[j].size(); k++) {
				if (i == faces[j][k] - 1) { //If vertex forms an edge of this face
					adjacent_face_centres.resize(adjacent_face_centres.size() + 1);
					adjacent_face_centres[adjacent_face_centres.size() - 1].insert(begin(adjacent_face_centres[adjacent_face_centres.size() - 1]), begin(face_centres[j]), end(face_centres[j]));
					adjacent_face_centres[adjacent_face_centres.size() - 1].push_back(j);
				}
			}
		}
		//Retrieve adjcent_edge_centres;
		for (int j = 0; j < edge_centres.size(); j++) {
			if (i == edge_centres[j][3] || i == edge_centres[j][4]) {
				adjacent_edge_centres.resize(adjacent_edge_centres.size() + 1);
				adjacent_edge_centres[adjacent_edge_centres.size() - 1].insert(begin(adjacent_edge_centres[adjacent_edge_centres.size() - 1]), begin(edge_centres[j]), end(edge_centres[j]));
			}
		}
		//Check - ok to continue when triggered?
		int size_1 = adjacent_edge_centres.size(), size_2 = adjacent_face_centres.size();
		if (adjacent_edge_centres.size() != adjacent_face_centres.size())
			cout << "For vertex " << i << ":\tNumber of adjacent faces: " << adjacent_face_centres.size() << "\tNumber of adjacent edges: " << adjacent_edge_centres.size() << "\tDifference: " << size_1 - size_2 << endl;

		//Calculate sum of adjacent areas to vertex
		vector<double> sum_adjacent_areas;
		vector<int> used_edges;
		for (int j = 0; j < 3; j++)
			sum_adjacent_areas.push_back(0);
		for (int j = 0; j < adjacent_face_centres.size(); j++) {
			bool face_already_summed = false;
			for (int k = 0; k < adjacent_edge_centres.size(); k++) {
				bool edge_used = false;
				for (int p = 0; p < used_edges.size(); p++) {
					if (used_edges[p] == k) {
						edge_used = true;
						break;
					}
				}
				//bool edge_already_summed = false;
				for (int l = 0; l < 3; l++) { //Number of edges for each face (as all faces are triangles)
					//if (!edge_already_summed && !face_already_summed) {
					if (!edge_used) {
						if (faces[adjacent_face_centres[j][3]][l] - 1 == adjacent_edge_centres[k][3]) {
							if (l == 0) {
								if (faces[adjacent_face_centres[j][3]][1] - 1 == adjacent_edge_centres[k][4] || faces[adjacent_face_centres[j][3]][2] - 1 == adjacent_edge_centres[k][4]) {
									vector<double> edge_centre_coords, face_centre_coords, singular_area;
									edge_centre_coords.push_back(adjacent_edge_centres[k][0]);
									edge_centre_coords.push_back(adjacent_edge_centres[k][1]);
									edge_centre_coords.push_back(adjacent_edge_centres[k][2]);
									face_centre_coords.push_back(adjacent_face_centres[j][0]);
									face_centre_coords.push_back(adjacent_face_centres[j][1]);
									face_centre_coords.push_back(adjacent_face_centres[j][2]);
									if (vertices.empty())
										singular_area = Calculate_Triangular_Area(pre_smoothing_vertices[i], edge_centre_coords, face_centre_coords);
									else
										singular_area = Calculate_Triangular_Area(vertices[i], edge_centre_coords, face_centre_coords);
									/*
									if (i == 5353 || i == 5480) {
										cout << "i: " << i << "\tSingular area: " << endl;
										Write_List_to_Screen(singular_area);
									}
									*/
									sum_adjacent_areas = vec3::Add(sum_adjacent_areas, singular_area);
									face_already_summed = true;
									//edge_already_summed = true;
									used_edges.push_back(k);
								}
							}
							else if (l == 1) {
								if (faces[adjacent_face_centres[j][3]][0] - 1 == adjacent_edge_centres[k][4] || faces[adjacent_face_centres[j][3]][2] - 1 == adjacent_edge_centres[k][4]) {
									vector<double> edge_centre_coords, face_centre_coords, singular_area;
									edge_centre_coords.push_back(adjacent_edge_centres[k][0]);
									edge_centre_coords.push_back(adjacent_edge_centres[k][1]);
									edge_centre_coords.push_back(adjacent_edge_centres[k][2]);
									face_centre_coords.push_back(adjacent_face_centres[j][0]);
									face_centre_coords.push_back(adjacent_face_centres[j][1]);
									face_centre_coords.push_back(adjacent_face_centres[j][2]);
									if (vertices.empty())
										singular_area = Calculate_Triangular_Area(pre_smoothing_vertices[i], edge_centre_coords, face_centre_coords);
									else
										singular_area = Calculate_Triangular_Area(vertices[i], edge_centre_coords, face_centre_coords);
									sum_adjacent_areas = vec3::Add(sum_adjacent_areas, singular_area);
									face_already_summed = true;
									//edge_already_summed = true;
									used_edges.push_back(k);
								}
							}
							else if (l == 2) {
								if (faces[adjacent_face_centres[j][3]][0] - 1 == adjacent_edge_centres[k][4] || faces[adjacent_face_centres[j][3]][1] - 1 == adjacent_edge_centres[k][4]) {
									vector<double> edge_centre_coords, face_centre_coords, singular_area;
									edge_centre_coords.push_back(adjacent_edge_centres[k][0]);
									edge_centre_coords.push_back(adjacent_edge_centres[k][1]);
									edge_centre_coords.push_back(adjacent_edge_centres[k][2]);
									face_centre_coords.push_back(adjacent_face_centres[j][0]);
									face_centre_coords.push_back(adjacent_face_centres[j][1]);
									face_centre_coords.push_back(adjacent_face_centres[j][2]);
									if (vertices.empty())
										singular_area = Calculate_Triangular_Area(pre_smoothing_vertices[i], edge_centre_coords, face_centre_coords);
									else
										singular_area = Calculate_Triangular_Area(vertices[i], edge_centre_coords, face_centre_coords);
									sum_adjacent_areas = vec3::Add(sum_adjacent_areas, singular_area);
									face_already_summed = true;
									//edge_already_summed = true;
									used_edges.push_back(k);
								}
							}
						}
					}
				}
				if (face_already_summed)
					break;
			}
		}
		vertice_area_weights[i].insert(begin(vertice_area_weights[i]), begin(sum_adjacent_areas), end(sum_adjacent_areas));
	}

	cout << "Finished calculating vertex area weights for smoothing." << endl;
}

void CCalculatingCurvature::Calculate_Smoothing_Vertice_Normals(void) {
	cout << "Begin calculating vertice normals for smoothing..." << endl;

	if (!vertices.empty())
		smoothing_vertice_normals.clear(); //Reinitialise smoothing_vertice_normals if it contains data from a previous call

	vector<double> normal_vector;
	double vector_magnitude;
	smoothing_vertice_normals.resize(vertice_area_weights.size());
	for (int i = 0; i < vertice_area_weights.size(); i++) {
		vector_magnitude = vec3::Magnitude(vertice_area_weights[i]);
		if (vector_magnitude == 0) {
			for (int j = 0; j < 3; j++)
				smoothing_vertice_normals[i].push_back(0);
		}
		else {
			normal_vector = vec3::Scalar_Multiple(vertice_area_weights[i], 1.0 / vector_magnitude);
			smoothing_vertice_normals[i].insert(begin(smoothing_vertice_normals[i]), begin(normal_vector), end(normal_vector));
		}
	}

	cout << "Finished calculating vertice normals for smoothing." << endl;
}

double CCalculatingCurvature::Calculate_Weight_Factor(vector<double> adjacent_area, vector<double> original_area) {
	//Checks
	if (adjacent_area.size() != 3 || original_area.size() != 3) {
		cout << "Sizes of input areas are incorrect so can't calculate weight factor. Sizes should be 3 but are " << adjacent_area.size() << " for the adjacent area and " << original_area.size() << " for the original area." << endl;
		exit(0);
	}

	double mod_adjacent_area, mod_original_area, weight_factor;
	mod_adjacent_area = vec3::Magnitude(adjacent_area);
	mod_original_area = vec3::Magnitude(original_area);
	weight_factor = mod_adjacent_area + (0.3 * mod_original_area);
	return weight_factor;
}

vector<double> CCalculatingCurvature::Calculate_Average_Distance_Vector_for_Movement(vector<double> weight_factors, vector<vector<double>> adjacent_positions, vector<double> original_position) {
	//Checks
	if (weight_factors.size() != adjacent_positions.size()) {
		cout << "Error in input variables: number of weight factors is: " << weight_factors.size() << " and the number of adjacent positions is: " << adjacent_positions.size() << " when they should be equal" << endl;
		exit(0);
	}

	vector<double> temp_v, numerator_summation, distance_vector;
	double sum_of_weights = 0.0;
	for (int i = 0; i < 3; i++)
		numerator_summation.push_back(0);
	for (int i = 0; i < weight_factors.size(); i++) {
		temp_v = vec3::Scalar_Multiple(adjacent_positions[i], weight_factors[i]);
		numerator_summation = vec3::Add(temp_v, numerator_summation);
		sum_of_weights += weight_factors[i];
	}
	temp_v = vec3::Scalar_Multiple(numerator_summation, 1.0 / sum_of_weights);
	distance_vector = vec3::Subtract(temp_v, original_position);
	return distance_vector;
}

vector<double> CCalculatingCurvature::Laplacian_Smoothing(vector<double> old_position, double beta, vector<double> distance_vector, vector<double> normal_vector) {
	//Checks
	if (old_position.size() != 3 || distance_vector.size() != 3 || normal_vector.size() != 3) {
		cout << "Error - some input parameters do not have size of 3.";
		cout << "Size of input parameters: old position: " << old_position.size() << "\tdistance vector: " << distance_vector.size() << "\tnormal vector: " << normal_vector.size() << endl;
		exit(0);
	}

	double scale_factor_1, scale_factor_2;
	vector<double> temp_v1, temp_v2, ans;

	scale_factor_1 = vec3::Dot(distance_vector, normal_vector);
	scale_factor_1 = 0.8 * beta * scale_factor_1;
	temp_v1 = vec3::Scalar_Multiple(normal_vector, scale_factor_1);
	scale_factor_2 = 0.2 * beta;
	temp_v2 = vec3::Scalar_Multiple(distance_vector, scale_factor_2);

	ans = vec3::Add(old_position, temp_v1);
	ans = vec3::Add(ans, temp_v2);
	return ans;
}

void CCalculatingCurvature::Laplacian_Smoothing_Test(vector<double> old_position, double beta, vector<double> distance_vector, vector<double> normal_vector, vector<double> Laplacian_smoothed_position, vector<double> displacement_during_Laplacian_smoothing) {
	//Checks
	if (old_position.size() != 3 || distance_vector.size() != 3 || normal_vector.size() != 3) {
		cout << "Error - some input parameters do not have size of 3.";
		cout << "Size of input parameters: old position: " << old_position.size() << "\tdistance vector: " << distance_vector.size() << "\tnormal vector: " << normal_vector.size() << endl;
		exit(0);
	}

	double scale_factor_1, scale_factor_2;
	vector<double> temp_v1, temp_v2;

	scale_factor_1 = vec3::Dot(distance_vector, normal_vector);
	scale_factor_1 = 0.8 * beta * scale_factor_1;
	temp_v1 = vec3::Scalar_Multiple(normal_vector, scale_factor_1);
	scale_factor_2 = 0.2 * beta;
	temp_v2 = vec3::Scalar_Multiple(distance_vector, scale_factor_2);

	//Calculate Laplacian smoothed position
	Laplacian_smoothed_position.clear();
	Laplacian_smoothed_position = vec3::Add(old_position, temp_v1);
	Laplacian_smoothed_position = vec3::Add(Laplacian_smoothed_position, temp_v2);

	//Calculate displacement during Laplacian smoothing and store in displacement_during_Laplacian_smoothing
	displacement_during_Laplacian_smoothing.clear();
	displacement_during_Laplacian_smoothing = vec3::Subtract(Laplacian_smoothed_position, old_position);
}


vector<double> CCalculatingCurvature::Calculate_Average_Displacement_during_Laplacian_Smoothing(vector<double> weight_factors, vector<vector<double>> original_positions, vector<vector<double>> adjacent_laplacian_positions) {
	//Checks
	if (adjacent_laplacian_positions.size() != original_positions.size() || adjacent_laplacian_positions.size() != weight_factors.size()) {
		cout << "Input data doesn't have the same size of outer dimension so can't calculate the average displacement during Lapalcian smoothing." << endl;
		cout << "Sizes are: weight factors: " << weight_factors.size() << "\toriginal positions: " << original_positions.size() << "\tadjacent positions: " << adjacent_laplacian_positions.size() << endl;
		exit(0);
	}

	vector<double> temp_v, summed_v, average_displacement;
	double denominator = 0.0;
	for (int i = 0; i < 3; i++)
		summed_v.push_back(0);
	for (int i = 0; i < adjacent_laplacian_positions.size(); i++) {
		temp_v = vec3::Subtract(adjacent_laplacian_positions[i], original_positions[i]);
		temp_v = vec3::Scalar_Multiple(temp_v, weight_factors[i]);
		summed_v = vec3::Add(summed_v, temp_v);
		denominator += weight_factors[i];
	}
	average_displacement = vec3::Scalar_Multiple(summed_v, 1.0 / denominator);
	return average_displacement;
}

vector<double> CCalculatingCurvature::Calculate_Average_Displacement_during_Laplacian_Smoothing_Test(vector<double> weight_factors, vector<vector<double>> displacements_during_Laplacian_smoothing) { //Goes with Gaussian_Smoothing_Test()
	//Checks
	if (displacements_during_Laplacian_smoothing.size() != weight_factors.size()) {
		cout << "Input data doesn't have the same size of outer dimension so can't calculate the average displacement during Lapalcian smoothing (Test)." << endl;
		cout << "Sizes are: weight factors: " << weight_factors.size() << "and displacement during Laplacian smoothing: " << displacements_during_Laplacian_smoothing.size() << endl;
		exit(0);
	}

	vector<double> temp_v, summed_v, average_displacement;
	double denominator = 0.0;
	for (int i = 0; i < 3; i++)
		summed_v.push_back(0);
	for (int i = 0; i < displacements_during_Laplacian_smoothing.size(); i++) {
		if (displacements_during_Laplacian_smoothing[i].size() != 3) {
			cout << "Displacements during Laplacian smoothing [" << i << "] size = " << displacements_during_Laplacian_smoothing[i].size() << " when it should be 3 so can't calculate average displacement during laplacian smoothing" << endl;
			exit(0);
		}
		temp_v = vec3::Scalar_Multiple(displacements_during_Laplacian_smoothing[i], weight_factors[i]);
		summed_v = vec3::Add(summed_v, temp_v);
		denominator += weight_factors[i];
	}
	average_displacement = vec3::Scalar_Multiple(summed_v, 1.0 / denominator);
	return average_displacement;
}

vector<double> CCalculatingCurvature::Migrate_Points_after_Laplacian_Smoothing(vector<double> laplacian_position, vector<double> displacement_during_laplacian_smoothing, vector<double> normal_vector) {
	//Checks
	if (laplacian_position.size() != 3 || displacement_during_laplacian_smoothing.size() != 3 || normal_vector.size() != 3) {
		cout << "Error - some input parameters do not have size of 3 so can't perform migration after Laplacian smoothing. ";
		cout << "Size of input parameters: laplacian position: " << laplacian_position.size() << "\tdisplacement vector: " << displacement_during_laplacian_smoothing.size() << "\tnormal vector: " << normal_vector.size() << endl;
		exit(0);
	}

	vector<double> temp_v, migration_vector;
	double scale_factor;
	scale_factor = vec3::Dot(displacement_during_laplacian_smoothing, normal_vector);
	temp_v = vec3::Scalar_Multiple(normal_vector, scale_factor);
	temp_v = vec3::Add(displacement_during_laplacian_smoothing, temp_v);
	temp_v = vec3::Scalar_Multiple(temp_v, 0.3);
	migration_vector = vec3::Subtract(laplacian_position, temp_v);
	return migration_vector;
}

void CCalculatingCurvature::Gaussian_Smoothing(int gaussian_iterations, int iterations_per_step, int output_edges_frequency) {
	if (gaussian_iterations < 0) {
		cout << "Can't have a negative gaussian_iterations!" << endl;
		exit(0);
	}
	if (iterations_per_step < 0) {
		cout << "Can't have a negative iterations_per_step!" << endl;
		exit(0);
	}
	if (output_edges_frequency < 0) {
		cout << "Can't have a negative output_edges_frequency!" << endl;
		exit(0);
	}

	cout << "Begin Gaussian Smoothing..." << endl;

	vector<vector<vector<double>>> list_of_adjacent_original_positions;
	vector<vector<double>> list_of_adjacent_weight_factors, list_of_laplacian_smoothed_positions, list_of_average_displacements_during_laplacian_smoothing;
	vector<vector<int>> adjacent_vertice_indices;
	vector<int> isolated_points_indices, smoothed_points_indices;

	list_of_laplacian_smoothed_positions.resize(pre_smoothing_vertices.size()); //Stores 0 if position isn't smoothed, otherwise stores position
	list_of_average_displacements_during_laplacian_smoothing.resize(pre_smoothing_vertices.size()); //Stores 0 if the position hasn't been smoothed
	adjacent_vertice_indices.resize(pre_smoothing_vertices.size());
	for (int p = 0; p < gaussian_iterations; p++) { //For each iteration of the gaussian smoothing algorithm
		vector<double> laplacian_smoothed_position, average_displacement;
		for (int q = 0; q < iterations_per_step; q++) { //Repeat until the kernel radius of gaussian smoothing (r_gauss) reaches 0
			cout << "Gaussian smoothing iteration: [" << p << "][" << q << "]" << endl;

			Calculate_Face_Centres();
			if (output_edges_frequency != 0)
				Calculate_Edge_Centres(true, output_edges_frequency);
			else
				Calculate_Edge_Centres(false, 0);
			Calculate_Vertex_Area_Weights_for_Smoothing();
			Calculate_Smoothing_Vertice_Normals();
			vertices.resize(pre_smoothing_vertices.size()); //Size kept constant after first iteration, but moved here so vertices.empty() checks can be carried out in functions above

			list_of_adjacent_weight_factors.clear();
			list_of_adjacent_original_positions.clear();
			for (int i = 0; i < pre_smoothing_vertices.size(); i++) { //For each vertex
				//Check if the vertex has adjacent points
				bool isolated_point = true;
				for (int j = 0; j < faces.size(); j++) { //For every face
					//Check there are 3 vertices to each face (ie: each face is a triangle)
					if (faces[j].size() != 3)
						cout << "Error in pre_smoothing_faces - face " << j << " (indexing starting at 0) is not a triangle!" << endl;
					for (int k = 0; k < faces[j].size(); k++) { //For every vertice in each face
						if (i == faces[j][k] - 1) { //If the vertex is within this face
							isolated_point = false; //Vertex is no longer isolated
							//Add the other two vertices onto adjacent_vertices, if they haven't been added already
							if (k == 0) { //If this is the first vertex
								bool new_1 = true, new_2 = true;
								for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
									if (adjacent_vertice_indices[i][m] == faces[j][1] - 1)
										new_1 = false; //Second vertex is not new
									else if (adjacent_vertice_indices[i][m] == faces[j][2] - 1)
										new_2 = false; //Third vertex is not new
									if (!new_1 && !new_2)
										break; //Break if both adjacent vertices are confirmed as not new
								}
								if (new_1)
									adjacent_vertice_indices[i].push_back(faces[j][1] - 1); //Add to list for adjacent vertices for this vertex
								if (new_2)
									adjacent_vertice_indices[i].push_back(faces[j][2] - 1); //Add to list for adjacent vertices for this vertex
							}
							if (k == 1) { //If this is the second vertex
								bool new_0 = true, new_2 = true;
								for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
									if (adjacent_vertice_indices[i][m] == faces[j][0] - 1)
										new_0 = false; //First vertex is not new
									else if (adjacent_vertice_indices[i][m] == faces[j][2] - 1)
										new_2 = false; //Third vertex is not new
									if (!new_0 && !new_2)
										break; //Break if both adjacent vertices are confirmed as not new
								}
								if (new_0)
									adjacent_vertice_indices[i].push_back(faces[j][0] - 1); //Add to list for adjacent vertices for this vertex
								if (new_2)
									adjacent_vertice_indices[i].push_back(faces[j][2] - 1); //Add to list for adjacent vertices for this vertex
							}
							if (k == 2) { //If this is the third vertex
								bool new_0 = true, new_1 = true;
								for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
									if (adjacent_vertice_indices[i][m] == faces[j][0] - 1)
										new_0 = false; //First vertex is not new
									else if (adjacent_vertice_indices[i][m] == faces[j][1] - 1)
										new_1 = false; //Second vertex is not new
									if (!new_0 && !new_1)
										break; //Break if both adjacent vertices are confirmed as not new
								}
								if (new_0)
									adjacent_vertice_indices[i].push_back(faces[j][0] - 1); //Add to list for adjacent vertices for this vertex
								if (new_1)
									adjacent_vertice_indices[i].push_back(faces[j][1] - 1); //Add to list for adjacent vertices for this vertex
							}
						}
					}
				}
				if (isolated_point) { //If point is isolated
					cout << "Isolated point index: " << i << "/" << pre_smoothing_vertices.size() << endl;
					//Copy point to vertices without modification
					vertices[i].insert(begin(vertices[i]), begin(pre_smoothing_vertices[i]), end(pre_smoothing_vertices[i]));
					list_of_laplacian_smoothed_positions[i].push_back(0.0); //Shows that this point has not been smoothed
					list_of_average_displacements_during_laplacian_smoothing[i].push_back(0.0); //Shows that this point has not been smoothed
					if (p == 0 && q == 0)
						isolated_points_indices.push_back(i); //Store this as an isolated point (only when it is first encountered - avoids repeated values)
				}
				else { //If point isn't isolated (so can be smoothed)
					vector<double> weight_factors, average_distance_b_i, normal_vector;
					vector<vector<double>> adjacent_original_vertices_positions;
					adjacent_original_vertices_positions.resize(adjacent_vertice_indices[i].size());
					list_of_adjacent_weight_factors.resize(list_of_adjacent_weight_factors.size() + 1);
					list_of_adjacent_original_positions.resize(list_of_adjacent_original_positions.size() + 1);
					//Calculate average distance (b_i)
					for (int j = 0; j < adjacent_vertice_indices[i].size(); j++) {
						double temp_weight_factor;
						temp_weight_factor = Calculate_Weight_Factor(vertice_area_weights[adjacent_vertice_indices[i][j]], vertice_area_weights[i]);
						weight_factors.push_back(temp_weight_factor);
						adjacent_original_vertices_positions[j].insert(begin(adjacent_original_vertices_positions[j]), begin(pre_smoothing_vertices[adjacent_vertice_indices[i][j]]), end(pre_smoothing_vertices[adjacent_vertice_indices[i][j]]));
					}
					average_distance_b_i = Calculate_Average_Distance_Vector_for_Movement(weight_factors, adjacent_original_vertices_positions, pre_smoothing_vertices[i]);
					//Migrate positions using Laplacian smoothing (o_i to q_i)
					if (p == 0 && q == 0) //If this is the first time the point has been encountered, use pre_smoothing vertices[i]
						laplacian_smoothed_position = Laplacian_Smoothing(pre_smoothing_vertices[i], 0.1, average_distance_b_i, smoothing_vertice_normals[i]);
					else //If this isn't the first time the point has been encountered it has already been smoothed, with is postion stored in vertices[i]
						laplacian_smoothed_position = Laplacian_Smoothing(vertices[i], 0.1, average_distance_b_i, smoothing_vertice_normals[i]);
					//Copy data to outer matrices to move points back later (q_i to p_i)
					list_of_adjacent_weight_factors[list_of_adjacent_weight_factors.size() - 1].insert(begin(list_of_adjacent_weight_factors[list_of_adjacent_weight_factors.size() - 1]), begin(weight_factors), end(weight_factors));
					list_of_adjacent_original_positions[list_of_adjacent_original_positions.size() - 1].insert(begin(list_of_adjacent_original_positions[list_of_adjacent_original_positions.size() - 1]), begin(adjacent_original_vertices_positions), end(adjacent_original_vertices_positions));
					list_of_laplacian_smoothed_positions[i].clear();
					list_of_laplacian_smoothed_positions[i].insert(begin(list_of_laplacian_smoothed_positions[i]), begin(laplacian_smoothed_position), end(laplacian_smoothed_position));
					//Copy smoothed point to vertices
					vertices[i].clear(); //Overwrite previous position (which was just storing posiiton after the last iteration of Laplacian smoothing)
					vertices[i].insert(begin(vertices[i]), begin(laplacian_smoothed_position), end(laplacian_smoothed_position));
					if (p == 0 && q == 0)
						smoothed_points_indices.push_back(i); //Store this as a smoothed point (only when it is first encountered - avoids repeated points)
				}
			}
			//Calculate average distance towards average original positions (d_i)
			vector<vector<vector<double>>> list_of_adjacent_laplacian_smoothed_positions;
			list_of_adjacent_laplacian_smoothed_positions.resize(smoothed_points_indices.size()); //Used for each point that was smoothed
			for (int i = 0; i < smoothed_points_indices.size(); i++) { //For each point that as smoothed
				/*
				for (int j = 0; j < adjacent_vertice_indices.size(); j++) {
					if (list_of_laplacian_smoothed_positions[adjacent_vertice_indices[j]][0] == 0.0)
						continue;
					else {
						list_of_adjacent_laplacian_smoothed_positions.resize(list_of_adjacent_laplacian_smoothed_positions.size() + 1);
						list_of_adjacent_laplacian_smoothed_positions[list_of_adjacent_laplacian_smoothed_positions.size() - 1].insert(begin(list_of_adjacent_laplacian_smoothed_positions[list_of_adjacent_laplacian_smoothed_positions.size() - 1]), begin(list_of_laplacian_smoothed_positions[adjacent_vertice_indices[j]]), end(list_of_laplacian_smoothed_positions[adjacent_vertice_indices[j]]));
					}
				}*/
				//(Using smoothed_points_indices instead of layout above)
				for (int j = 0; j < adjacent_vertice_indices[i].size(); j++) { //For every adjacent point
					bool isolated_adjacent_point = false;
					//Check if the adjacent point is isolated
					for (int k = 0; k < isolated_points_indices.size(); k++) { //For every isolated point
						if (isolated_points_indices[k] == adjacent_vertice_indices[i][j]) { //If the adjacent vertex is an isolated point
							isolated_adjacent_point = true;
							break;
						}
					}
					if (!isolated_adjacent_point) { //If the adjacent point isn't isolated
						list_of_adjacent_laplacian_smoothed_positions[i].resize(list_of_adjacent_laplacian_smoothed_positions[i].size() + 1);
						list_of_adjacent_laplacian_smoothed_positions[i][list_of_adjacent_laplacian_smoothed_positions[i].size() - 1].insert(begin(list_of_adjacent_laplacian_smoothed_positions[i][list_of_adjacent_laplacian_smoothed_positions[i].size() - 1]), begin(list_of_laplacian_smoothed_positions[adjacent_vertice_indices[i][j]]), end(list_of_laplacian_smoothed_positions[adjacent_vertice_indices[i][j]]));
					}
				}
				//Calculate average displacements and store vector
				average_displacement = Calculate_Average_Displacement_during_Laplacian_Smoothing(list_of_adjacent_weight_factors[i], list_of_adjacent_original_positions[i], list_of_adjacent_laplacian_smoothed_positions[i]);
				list_of_average_displacements_during_laplacian_smoothing[i].clear();
				list_of_average_displacements_during_laplacian_smoothing[i].insert(begin(list_of_average_displacements_during_laplacian_smoothing[i]), begin(average_displacement), end(average_displacement));
			}
		}
		//Move vertices back for volume preservation
		vector<double> end_position_for_this_iteration;
		for (int i = 0; i < pre_smoothing_vertices.size(); i++) { //For every vertex
			bool isolated_point = false;
			//Check if point was isolated (and so not smoothed)
			for (int j = 0; j < isolated_points_indices.size(); j++) {
				if (isolated_points_indices[j] == i) {
					isolated_point = true;
					break;
				}
			}
			if (!isolated_point) { //If point wasn't isolated (so was smoothed)
				//Shouldn't be triggered because of isolated_points check but just in case
				/*
				if (list_of_laplacian_smoothed_positions[i][0] == 0.0 || list_of_average_displacements_during_laplacian_smoothing[i][0] == 0.0) {
					cout << "Error: migration after laplacian smoothing triggered when point hasn't been smoothed." << endl;
					exit(0);
				}*/
				//Calculate final position, having been migrated back after Laplacian smoothing, and store in vertices[i]
				end_position_for_this_iteration = Migrate_Points_after_Laplacian_Smoothing(list_of_laplacian_smoothed_positions[i], list_of_average_displacements_during_laplacian_smoothing[i], smoothing_vertice_normals[i]);
				vertices[i].clear(); //Overwrite previous position (which was just storing position after Laplacian smoothed had finished)
				vertices[i].insert(begin(vertices[i]), begin(end_position_for_this_iteration), end(end_position_for_this_iteration));
			}
		}
	}
	cout << "Finished Gaussian [" << gaussian_iterations << ", " << iterations_per_step << "] smoothing." << endl << endl;
}

void CCalculatingCurvature::Gaussian_Smoothing_Test(int gaussian_iterations, int iterations_per_step, int output_edges_frequency) {
	if (gaussian_iterations < 0) {
		cout << "Can't have a negative gaussian_iterations!" << endl;
		exit(0);
	}
	if (iterations_per_step < 0) {
		cout << "Can't have a negative iterations_per_step!" << endl;
		exit(0);
	}
	if (output_edges_frequency < 0) {
		cout << "Can't have a negative output_edges_frequency!" << endl;
		exit(0);
	}

	cout << "Begin Gaussian Smoothing..." << endl;

	vector<vector<vector<double>>> list_of_adjacent_original_positions, list_of_adjacent_single_displacements_during_Laplacian_smoothing;
	vector<vector<double>> list_of_adjacent_weight_factors, list_of_laplacian_smoothed_positions, list_of_average_displacements_during_laplacian_smoothing, list_of_single_displacements_during_Laplacian_smoothing;
	vector<vector<int>> adjacent_vertice_indices;
	vector<int> isolated_points_indices, smoothed_points_indices;

	list_of_laplacian_smoothed_positions.resize(pre_smoothing_vertices.size()); //Stores 0 if position isn't smoothed, otherwise stores position
	list_of_average_displacements_during_laplacian_smoothing.resize(pre_smoothing_vertices.size()); //Stores 0 if the position hasn't been smoothed
	list_of_single_displacements_during_Laplacian_smoothing.resize(pre_smoothing_vertices.size()); //Stores 0 if the posiiton hasn't been smoothed
	adjacent_vertice_indices.resize(pre_smoothing_vertices.size());
	for (int p = 0; p < gaussian_iterations; p++) { //For each iteration of the gaussian smoothing algorithm
		vector<double> laplacian_smoothed_position, average_displacement;
		for (int q = 0; q < iterations_per_step; q++) { //Repeat until the kernel radius of gaussian smoothing (r_gauss) reaches 0
			cout << "Gaussian smoothing iteration: [" << p << "][" << q << "]" << endl;

			Calculate_Face_Centres();
			if (output_edges_frequency != 0)
				Calculate_Edge_Centres(true, output_edges_frequency);
			else
				Calculate_Edge_Centres(false, 0);
			Calculate_Vertex_Area_Weights_for_Smoothing();
			Calculate_Smoothing_Vertice_Normals();
			vertices.resize(pre_smoothing_vertices.size()); //Size kept constant after first iteration, but moved here so vertices.empty() checks can be carried out in functions above

			list_of_adjacent_weight_factors.clear();
			list_of_adjacent_original_positions.clear();
			for (int i = 0; i < pre_smoothing_vertices.size(); i++) { //For each vertex
				//Check if the vertex has adjacent points
				bool isolated_point = true;
				for (int j = 0; j < faces.size(); j++) { //For every face
					//Check there are 3 vertices to each face (ie: each face is a triangle)
					if (faces[j].size() != 3)
						cout << "Error in pre_smoothing_faces - face " << j << " (indexing starting at 0) is not a triangle!" << endl;
					for (int k = 0; k < faces[j].size(); k++) { //For every vertice in each face
						if (i == faces[j][k] - 1) { //If the vertex is within this face
							isolated_point = false; //Vertex is no longer isolated
							//Add the other two vertices onto adjacent_vertices, if they haven't been added already
							if (k == 0) { //If this is the first vertex
								bool new_1 = true, new_2 = true;
								for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
									if (adjacent_vertice_indices[i][m] == faces[j][1] - 1)
										new_1 = false; //Second vertex is not new
									else if (adjacent_vertice_indices[i][m] == faces[j][2] - 1)
										new_2 = false; //Third vertex is not new
									if (!new_1 && !new_2)
										break; //Break if both adjacent vertices are confirmed as not new
								}
								if (new_1)
									adjacent_vertice_indices[i].push_back(faces[j][1] - 1); //Add to list for adjacent vertices for this vertex
								if (new_2)
									adjacent_vertice_indices[i].push_back(faces[j][2] - 1); //Add to list for adjacent vertices for this vertex
							}
							if (k == 1) { //If this is the second vertex
								bool new_0 = true, new_2 = true;
								for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
									if (adjacent_vertice_indices[i][m] == faces[j][0] - 1)
										new_0 = false; //First vertex is not new
									else if (adjacent_vertice_indices[i][m] == faces[j][2] - 1)
										new_2 = false; //Third vertex is not new
									if (!new_0 && !new_2)
										break; //Break if both adjacent vertices are confirmed as not new
								}
								if (new_0)
									adjacent_vertice_indices[i].push_back(faces[j][0] - 1); //Add to list for adjacent vertices for this vertex
								if (new_2)
									adjacent_vertice_indices[i].push_back(faces[j][2] - 1); //Add to list for adjacent vertices for this vertex
							}
							if (k == 2) { //If this is the third vertex
								bool new_0 = true, new_1 = true;
								for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
									if (adjacent_vertice_indices[i][m] == faces[j][0] - 1)
										new_0 = false; //First vertex is not new
									else if (adjacent_vertice_indices[i][m] == faces[j][1] - 1)
										new_1 = false; //Second vertex is not new
									if (!new_0 && !new_1)
										break; //Break if both adjacent vertices are confirmed as not new
								}
								if (new_0)
									adjacent_vertice_indices[i].push_back(faces[j][0] - 1); //Add to list for adjacent vertices for this vertex
								if (new_1)
									adjacent_vertice_indices[i].push_back(faces[j][1] - 1); //Add to list for adjacent vertices for this vertex
							}
						}
					}
				}
				if (isolated_point) { //If point is isolated
					cout << "Isolated point index: " << i << "/" << pre_smoothing_vertices.size() << endl;
					//Copy point to vertices without modification
					vertices[i].insert(begin(vertices[i]), begin(pre_smoothing_vertices[i]), end(pre_smoothing_vertices[i]));
					list_of_laplacian_smoothed_positions[i].push_back(0.0); //Shows that this point has not been smoothed
					list_of_average_displacements_during_laplacian_smoothing[i].push_back(0.0); //Shows that this point has not been smoothed
					list_of_single_displacements_during_Laplacian_smoothing[i].push_back(0.0); //Shows that this point hasn't been smoothed
					if (p == 0 && q == 0)
						isolated_points_indices.push_back(i); //Store this as an isolated point (only when it is first encountered - avoids repeated values)
				}
				else { //If point isn't isolated (so can be smoothed)
					vector<double> weight_factors, average_distance_b_i, normal_vector, laplacian_smoothed_position_test, displament_during_Laplacian_smoothing_test;
					vector<vector<double>> adjacent_original_vertices_positions;
					adjacent_original_vertices_positions.resize(adjacent_vertice_indices[i].size());
					list_of_adjacent_weight_factors.resize(list_of_adjacent_weight_factors.size() + 1);
					list_of_adjacent_original_positions.resize(list_of_adjacent_original_positions.size() + 1);
					//Calculate average distance (b_i)
					for (int j = 0; j < adjacent_vertice_indices[i].size(); j++) {
						double temp_weight_factor;
						temp_weight_factor = Calculate_Weight_Factor(vertice_area_weights[adjacent_vertice_indices[i][j]], vertice_area_weights[i]);
						weight_factors.push_back(temp_weight_factor);
						adjacent_original_vertices_positions[j].insert(begin(adjacent_original_vertices_positions[j]), begin(pre_smoothing_vertices[adjacent_vertice_indices[i][j]]), end(pre_smoothing_vertices[adjacent_vertice_indices[i][j]]));
					}
					average_distance_b_i = Calculate_Average_Distance_Vector_for_Movement(weight_factors, adjacent_original_vertices_positions, pre_smoothing_vertices[i]);
					//Migrate positions using Laplacian smoothing (o_i to q_i)
					if (p == 0 && q == 0) //If this is the first time the point has been encountered, use pre_smoothing vertices[i]
						//laplacian_smoothed_position = Laplacian_Smoothing(pre_smoothing_vertices[i], 0.1, average_distance_b_i, smoothing_vertice_normals[i]);
						Laplacian_Smoothing_Test(pre_smoothing_vertices[i], 0.1, average_distance_b_i, smoothing_vertice_normals[i], laplacian_smoothed_position_test, displament_during_Laplacian_smoothing_test);
					else //If this isn't the first time the point has been encountered it has already been smoothed, with is postion stored in vertices[i]
						//laplacian_smoothed_position = Laplacian_Smoothing(vertices[i], 0.1, average_distance_b_i, smoothing_vertice_normals[i]);
						Laplacian_Smoothing_Test(vertices[i], 0.1, average_distance_b_i, smoothing_vertice_normals[i], laplacian_smoothed_position_test, displament_during_Laplacian_smoothing_test);
					//Copy data to outer matrices to move points back later (q_i to p_i)
					list_of_adjacent_weight_factors[list_of_adjacent_weight_factors.size() - 1].insert(begin(list_of_adjacent_weight_factors[list_of_adjacent_weight_factors.size() - 1]), begin(weight_factors), end(weight_factors));
					list_of_adjacent_original_positions[list_of_adjacent_original_positions.size() - 1].insert(begin(list_of_adjacent_original_positions[list_of_adjacent_original_positions.size() - 1]), begin(adjacent_original_vertices_positions), end(adjacent_original_vertices_positions));
					list_of_laplacian_smoothed_positions[i].clear();
					list_of_laplacian_smoothed_positions[i].insert(begin(list_of_laplacian_smoothed_positions[i]), begin(laplacian_smoothed_position_test), end(laplacian_smoothed_position_test));
					list_of_single_displacements_during_Laplacian_smoothing[i].clear();
					list_of_single_displacements_during_Laplacian_smoothing[i].insert(begin(list_of_single_displacements_during_Laplacian_smoothing[i]), begin(displament_during_Laplacian_smoothing_test), end(displament_during_Laplacian_smoothing_test));
					//Copy smoothed point to vertices
					vertices[i].clear(); //Overwrite previous position (which was just storing posiiton after the last iteration of Laplacian smoothing)
					vertices[i].insert(begin(vertices[i]), begin(laplacian_smoothed_position_test), end(laplacian_smoothed_position_test));
					if (p == 0 && q == 0)
						smoothed_points_indices.push_back(i); //Store this as a smoothed point (only when it is first encountered - avoids repeated points)
				}
			}
			//Calculate average distance towards average original positions (d_i)
			vector<vector<vector<double>>> list_of_adjacent_laplacian_smoothed_positions;
			list_of_adjacent_laplacian_smoothed_positions.resize(smoothed_points_indices.size()); //Used for each point that was smoothed
			for (int i = 0; i < smoothed_points_indices.size(); i++) { //For each point that was smoothed
				list_of_adjacent_single_displacements_during_Laplacian_smoothing.resize(list_of_adjacent_single_displacements_during_Laplacian_smoothing.size() + 1);
				list_of_adjacent_single_displacements_during_Laplacian_smoothing[list_of_adjacent_single_displacements_during_Laplacian_smoothing.size() - 1].resize(adjacent_vertice_indices[i].size());
				for (int j = 0; j < adjacent_vertice_indices[i].size(); j++) { //For every adjacent point
					list_of_adjacent_single_displacements_during_Laplacian_smoothing[i][j].insert(begin(list_of_adjacent_single_displacements_during_Laplacian_smoothing[i][j]), begin(list_of_single_displacements_during_Laplacian_smoothing[adjacent_vertice_indices[i][j]]), end(list_of_single_displacements_during_Laplacian_smoothing[adjacent_vertice_indices[i][j]]));
				}
				//Calculate average displacements and store vector
				//average_displacement = Calculate_Average_Displacement_during_Laplacian_Smoothing(list_of_adjacent_weight_factors[i], list_of_adjacent_original_positions[i], list_of_adjacent_laplacian_smoothed_positions[i]);
				average_displacement = Calculate_Average_Displacement_during_Laplacian_Smoothing_Test(list_of_adjacent_weight_factors[i], list_of_adjacent_single_displacements_during_Laplacian_smoothing[i]);
				list_of_average_displacements_during_laplacian_smoothing[i].clear();
				list_of_average_displacements_during_laplacian_smoothing[i].insert(begin(list_of_average_displacements_during_laplacian_smoothing[i]), begin(average_displacement), end(average_displacement));
			}
		}
		//Move vertices back for volume preservation
		vector<double> end_position_for_this_iteration;
		for (int i = 0; i < pre_smoothing_vertices.size(); i++) { //For every vertex
			bool isolated_point = false;
			//Check if point was isolated (and so not smoothed)
			for (int j = 0; j < isolated_points_indices.size(); j++) {
				if (isolated_points_indices[j] == i) {
					isolated_point = true;
					break;
				}
			}
			if (!isolated_point) { //If point wasn't isolated (so was smoothed)
				//Calculate final position, having been migrated back after Laplacian smoothing, and store in vertices[i]
				end_position_for_this_iteration = Migrate_Points_after_Laplacian_Smoothing(list_of_laplacian_smoothed_positions[i], list_of_average_displacements_during_laplacian_smoothing[i], smoothing_vertice_normals[i]);
				vertices[i].clear(); //Overwrite previous position (which was just storing position after Laplacian smoothed had finished)
				vertices[i].insert(begin(vertices[i]), begin(end_position_for_this_iteration), end(end_position_for_this_iteration));
			}
		}
	}
	cout << "Finished Gaussian [" << gaussian_iterations << ", " << iterations_per_step << "] smoothing." << endl << endl;
}

void CCalculatingCurvature::Gaussian_Smoothing(int gaussian_iterations, int iterations_per_step, int output_edges_frequency, int input_dataset_size, bool continuous_curvature_output, int output_faces_frequency) {
	if (gaussian_iterations < 0) {
		cout << "Can't have a negative gaussian_iterations!" << endl;
		exit(0);
	}
	if (iterations_per_step < 0) {
		cout << "Can't have a negative iterations_per_step!" << endl;
		exit(0);
	}
	if (output_edges_frequency < 0) {
		cout << "Can't have a negative output_edges_frequency!" << endl;
		exit(0);
	}

	cout << "Begin Gaussian Smoothing..." << endl;

	vector<vector<vector<double>>> list_of_adjacent_original_positions;
	vector<vector<double>> list_of_adjacent_weight_factors, list_of_laplacian_smoothed_positions, list_of_average_displacements_during_laplacian_smoothing;
	vector<vector<int>> adjacent_vertice_indices;
	vector<int> isolated_points_indices, smoothed_points_indices;

	list_of_laplacian_smoothed_positions.resize(pre_smoothing_vertices.size()); //Stores 0 if position isn't smoothed, otherwise stores position
	list_of_average_displacements_during_laplacian_smoothing.resize(pre_smoothing_vertices.size()); //Stores 0 if the position hasn't been smoothed
	adjacent_vertice_indices.resize(pre_smoothing_vertices.size());
	for (int p = 0; p < gaussian_iterations; p++) { //For each iteration of the gaussian smoothing algorithm
		vector<double> laplacian_smoothed_position, average_displacement;
		for (int q = 0; q < iterations_per_step; q++) { //Repeat until the kernel radius of gaussian smoothing (r_gauss) reaches 0
			cout << "Gaussian smoothing iteration: [" << p << "][" << q << "]" << endl;

			Calculate_Face_Centres();
			if (output_edges_frequency != 0)
				Calculate_Edge_Centres(true, output_edges_frequency);
			else
				Calculate_Edge_Centres(false, 0);
			Calculate_Vertex_Area_Weights_for_Smoothing();
			Calculate_Smoothing_Vertice_Normals();
			vertices.resize(pre_smoothing_vertices.size()); //Size kept constant after first iteration, but moved here so vertices.empty() checks can be carried out in functions above

			list_of_adjacent_weight_factors.clear();
			list_of_adjacent_original_positions.clear();
			for (int i = 0; i < pre_smoothing_vertices.size(); i++) { //For each vertex
				//Check if the vertex has adjacent points
				bool isolated_point = true;
				for (int j = 0; j < faces.size(); j++) { //For every face
					//Check there are 3 vertices to each face (ie: each face is a triangle)
					if (faces[j].size() != 3)
						cout << "Error in pre_smoothing_faces - face " << j << " (indexing starting at 0) is not a triangle!" << endl;
					for (int k = 0; k < faces[j].size(); k++) { //For every vertice in each face
						if (i == faces[j][k] - 1) { //If the vertex is within this face
							isolated_point = false; //Vertex is no longer isolated
							//Add the other two vertices onto adjacent_vertices, if they haven't been added already
							if (k == 0) { //If this is the first vertex
								bool new_1 = true, new_2 = true;
								for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
									if (adjacent_vertice_indices[i][m] == faces[j][1] - 1)
										new_1 = false; //Second vertex is not new
									else if (adjacent_vertice_indices[i][m] == faces[j][2] - 1)
										new_2 = false; //Third vertex is not new
									if (!new_1 && !new_2)
										break; //Break if both adjacent vertices are confirmed as not new
								}
								if (new_1)
									adjacent_vertice_indices[i].push_back(faces[j][1] - 1); //Add to list for adjacent vertices for this vertex
								if (new_2)
									adjacent_vertice_indices[i].push_back(faces[j][2] - 1); //Add to list for adjacent vertices for this vertex
							}
							if (k == 1) { //If this is the second vertex
								bool new_0 = true, new_2 = true;
								for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
									if (adjacent_vertice_indices[i][m] == faces[j][0] - 1)
										new_0 = false; //First vertex is not new
									else if (adjacent_vertice_indices[i][m] == faces[j][2] - 1)
										new_2 = false; //Third vertex is not new
									if (!new_0 && !new_2)
										break; //Break if both adjacent vertices are confirmed as not new
								}
								if (new_0)
									adjacent_vertice_indices[i].push_back(faces[j][0] - 1); //Add to list for adjacent vertices for this vertex
								if (new_2)
									adjacent_vertice_indices[i].push_back(faces[j][2] - 1); //Add to list for adjacent vertices for this vertex
							}
							if (k == 2) { //If this is the third vertex
								bool new_0 = true, new_1 = true;
								for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
									if (adjacent_vertice_indices[i][m] == faces[j][0] - 1)
										new_0 = false; //First vertex is not new
									else if (adjacent_vertice_indices[i][m] == faces[j][1] - 1)
										new_1 = false; //Second vertex is not new
									if (!new_0 && !new_1)
										break; //Break if both adjacent vertices are confirmed as not new
								}
								if (new_0)
									adjacent_vertice_indices[i].push_back(faces[j][0] - 1); //Add to list for adjacent vertices for this vertex
								if (new_1)
									adjacent_vertice_indices[i].push_back(faces[j][1] - 1); //Add to list for adjacent vertices for this vertex
							}
						}
					}
				}
				if (isolated_point) { //If point is isolated
					//Copy point to vertices without modification
					vertices[i].insert(begin(vertices[i]), begin(pre_smoothing_vertices[i]), end(pre_smoothing_vertices[i]));
					list_of_laplacian_smoothed_positions[i].push_back(0.0); //Shows that this point has not been smoothed
					list_of_average_displacements_during_laplacian_smoothing[i].push_back(0.0); //Shows that this point has not been smoothed
					if (p == 0 && q == 0)
						isolated_points_indices.push_back(i); //Store this as an isolated point (only when it is first encountered - avoids repeated values)
				}
				else { //If point isn't isolated (so can be smoothed)
					vector<double> weight_factors, average_distance_b_i, normal_vector;
					vector<vector<double>> adjacent_original_vertices_positions;
					adjacent_original_vertices_positions.resize(adjacent_vertice_indices[i].size());
					list_of_adjacent_weight_factors.resize(list_of_adjacent_weight_factors.size() + 1);
					list_of_adjacent_original_positions.resize(list_of_adjacent_original_positions.size() + 1);
					//Calculate average distance (b_i)
					for (int j = 0; j < adjacent_vertice_indices[i].size(); j++) {
						double temp_weight_factor;
						temp_weight_factor = Calculate_Weight_Factor(vertice_area_weights[adjacent_vertice_indices[i][j]], vertice_area_weights[i]);
						weight_factors.push_back(temp_weight_factor);
						adjacent_original_vertices_positions[j].insert(begin(adjacent_original_vertices_positions[j]), begin(pre_smoothing_vertices[adjacent_vertice_indices[i][j]]), end(pre_smoothing_vertices[adjacent_vertice_indices[i][j]]));
					}
					average_distance_b_i = Calculate_Average_Distance_Vector_for_Movement(weight_factors, adjacent_original_vertices_positions, pre_smoothing_vertices[i]);
					//Migrate positions using Laplacian smoothing (o_i to q_i)
					if (p == 0 && q == 0) //If this is the first time the point has been encountered, use pre_smoothing vertices[i]
						laplacian_smoothed_position = Laplacian_Smoothing(pre_smoothing_vertices[i], 0.1, average_distance_b_i, smoothing_vertice_normals[i]);
					else //If this isn't the first time the point has been encountered it has already been smoothed, with is postion stored in vertices[i]
						laplacian_smoothed_position = Laplacian_Smoothing(vertices[i], 0.1, average_distance_b_i, smoothing_vertice_normals[i]);
					//Copy data to outer matrices to move points back later (q_i to p_i)
					list_of_adjacent_weight_factors[list_of_adjacent_weight_factors.size() - 1].insert(begin(list_of_adjacent_weight_factors[list_of_adjacent_weight_factors.size() - 1]), begin(weight_factors), end(weight_factors));
					list_of_adjacent_original_positions[list_of_adjacent_original_positions.size() - 1].insert(begin(list_of_adjacent_original_positions[list_of_adjacent_original_positions.size() - 1]), begin(adjacent_original_vertices_positions), end(adjacent_original_vertices_positions));
					list_of_laplacian_smoothed_positions[i].clear();
					list_of_laplacian_smoothed_positions[i].insert(begin(list_of_laplacian_smoothed_positions[i]), begin(laplacian_smoothed_position), end(laplacian_smoothed_position));
					//Copy smoothed point to vertices
					vertices[i].clear(); //Overwrite previous position (which was just storing posiiton after the last iteration of Laplacian smoothing)
					vertices[i].insert(begin(vertices[i]), begin(laplacian_smoothed_position), end(laplacian_smoothed_position));
					if (p == 0 && q == 0)
						smoothed_points_indices.push_back(i); //Store this as a smoothed point (only when it is first encountered - avoids repeated points)
				}
			}
			//Calculate average distance towards average original positions (d_i)
			vector<vector<vector<double>>> list_of_adjacent_laplacian_smoothed_positions;
			list_of_adjacent_laplacian_smoothed_positions.resize(smoothed_points_indices.size()); //Used for each point that was smoothed
			for (int i = 0; i < smoothed_points_indices.size(); i++) { //For each point that as smoothed
				/*
				for (int j = 0; j < adjacent_vertice_indices.size(); j++) {
					if (list_of_laplacian_smoothed_positions[adjacent_vertice_indices[j]][0] == 0.0)
						continue;
					else {
						list_of_adjacent_laplacian_smoothed_positions.resize(list_of_adjacent_laplacian_smoothed_positions.size() + 1);
						list_of_adjacent_laplacian_smoothed_positions[list_of_adjacent_laplacian_smoothed_positions.size() - 1].insert(begin(list_of_adjacent_laplacian_smoothed_positions[list_of_adjacent_laplacian_smoothed_positions.size() - 1]), begin(list_of_laplacian_smoothed_positions[adjacent_vertice_indices[j]]), end(list_of_laplacian_smoothed_positions[adjacent_vertice_indices[j]]));
					}
				}*/
				//(Using smoothed_points_indices instead of layout above)
				for (int j = 0; j < adjacent_vertice_indices[i].size(); j++) { //For every adjacent point
					bool isolated_adjacent_point = false;
					//Check if the adjacent point is isolated
					for (int k = 0; k < isolated_points_indices.size(); k++) { //For every isolated point
						if (isolated_points_indices[k] == adjacent_vertice_indices[i][j]) { //If the adjacent vertex is an isolated point
							isolated_adjacent_point = true;
							break;
						}
					}
					if (!isolated_adjacent_point) { //If the adjacent point isn't isolated
						list_of_adjacent_laplacian_smoothed_positions[i].resize(list_of_adjacent_laplacian_smoothed_positions[i].size() + 1);
						list_of_adjacent_laplacian_smoothed_positions[i][list_of_adjacent_laplacian_smoothed_positions[i].size() - 1].insert(begin(list_of_adjacent_laplacian_smoothed_positions[i][list_of_adjacent_laplacian_smoothed_positions[i].size() - 1]), begin(list_of_laplacian_smoothed_positions[adjacent_vertice_indices[i][j]]), end(list_of_laplacian_smoothed_positions[adjacent_vertice_indices[i][j]]));
					}
				}
				//Calculate average displacements and store vector
				average_displacement = Calculate_Average_Displacement_during_Laplacian_Smoothing(list_of_adjacent_weight_factors[i], list_of_adjacent_original_positions[i], list_of_adjacent_laplacian_smoothed_positions[i]);
				list_of_average_displacements_during_laplacian_smoothing[i].clear();
				list_of_average_displacements_during_laplacian_smoothing[i].insert(begin(list_of_average_displacements_during_laplacian_smoothing[i]), begin(average_displacement), end(average_displacement));
			}
		}
		//Move vertices back for volume preservation
		vector<double> end_position_for_this_iteration;
		for (int i = 0; i < pre_smoothing_vertices.size(); i++) { //For every vertex
			bool isolated_point = false;
			//Check if point was isolated (and so not smoothed)
			for (int j = 0; j < isolated_points_indices.size(); j++) {
				if (isolated_points_indices[j] == i) {
					isolated_point = true;
					break;
				}
			}
			if (!isolated_point) { //If point wasn't isolated (so was smoothed)
				//Shouldn't be triggered because of isolated_points check but just in case
				/*
				if (list_of_laplacian_smoothed_positions[i][0] == 0.0 || list_of_average_displacements_during_laplacian_smoothing[i][0] == 0.0) {
					cout << "Error: migration after laplacian smoothing triggered when point hasn't been smoothed." << endl;
					exit(0);
				}*/
				//Calculate final position, having been migrated back after Laplacian smoothing, and store in vertices[i]
				end_position_for_this_iteration = Migrate_Points_after_Laplacian_Smoothing(list_of_laplacian_smoothed_positions[i], list_of_average_displacements_during_laplacian_smoothing[i], smoothing_vertice_normals[i]);
				vertices[i].clear(); //Overwrite previous position (which was just storing position after Laplacian smoothed had finished)
				vertices[i].insert(begin(vertices[i]), begin(end_position_for_this_iteration), end(end_position_for_this_iteration));
			}
		}

		if (continuous_curvature_output) {
			//Curvature Calculation
			double mean_of_mean_curvature = 0.0, mean_gaussian_curvature = 0.0;
			Calculate_Face_Normals();
			Calculate_Vertice_Normals();
			Calculate_Curvature(true, output_faces_frequency);
			Get_Principal_Curvatures();
			Calculate_Gaussian_Curvatures();
			Calculate_Mean_Curvatures();


			mean_of_mean_curvature = Calculate_Mean<double>(mean_curvature);
			cout << "Mean Curvature: " << mean_of_mean_curvature << endl;
			mean_gaussian_curvature = Calculate_Mean<double>(gaussian_curvature);
			cout << "Mean Gaussian Curvature: " << mean_gaussian_curvature << endl;


			//Spheres Only
			double radius, centre_coordinate, mean;
			centre_coordinate = input_dataset_size / 2; //Integer maths
			//Original radius (with gap/voxel size both of 1) + 1 - top vertice's height post smoothing
			radius = (input_dataset_size - centre_coordinate - 2) + 1 - vertices[0][2];
			cout << "Radius: " << radius << endl;
			Normalise_Gaussian_Curvature_of_Sphere(radius);
			mean = Calculate_Mean<double>(normalised_gaussian_curvature);
			cout << "Mean Normalised Gaussian Curvature: " << mean << endl;
			//test.Output_Data_to_File();
		}
	}
	cout << "Finished Gaussian [" << gaussian_iterations << ", " << iterations_per_step << "] smoothing." << endl << endl;
}

void CCalculatingCurvature::Gaussian_Smoothing(int gaussian_iterations, int iterations_per_step, int output_edges_frequency, int z_value, double z_step, double tolerance) { //Catenoids (with correction)
	if (gaussian_iterations < 0) {
		cout << "Can't have a negative gaussian_iterations!" << endl;
		exit(0);
	}
	if (iterations_per_step < 0) {
		cout << "Can't have a negative iterations_per_step!" << endl;
		exit(0);
	}
	if (output_edges_frequency < 0) {
		cout << "Can't have a negative output_edges_frequency!" << endl;
		exit(0);
	}

	cout << "Begin Gaussian Smoothing..." << endl;

	vector<vector<vector<double>>> list_of_adjacent_original_positions;
	vector<vector<double>> list_of_adjacent_weight_factors, list_of_laplacian_smoothed_positions, list_of_average_displacements_during_laplacian_smoothing;
	vector<vector<int>> adjacent_vertice_indices;
	vector<int> isolated_points_indices, smoothed_points_indices;

	list_of_laplacian_smoothed_positions.resize(pre_smoothing_vertices.size()); //Stores 0 if position isn't smoothed, otherwise stores position
	list_of_average_displacements_during_laplacian_smoothing.resize(pre_smoothing_vertices.size()); //Stores 0 if the position hasn't been smoothed
	adjacent_vertice_indices.resize(pre_smoothing_vertices.size());
	for (int p = 0; p < gaussian_iterations; p++) { //For each iteration of the gaussian smoothing algorithm
		vector<double> laplacian_smoothed_position, average_displacement;
		for (int q = 0; q < iterations_per_step; q++) { //Repeat until the kernel radius of gaussian smoothing (r_gauss) reaches 0
			cout << "Gaussian smoothing iteration: [" << p << "][" << q << "]" << endl;

			Calculate_Face_Centres();
			if (output_edges_frequency != 0)
				Calculate_Edge_Centres(true, output_edges_frequency);
			else
				Calculate_Edge_Centres(false, 0);
			Calculate_Vertex_Area_Weights_for_Smoothing();
			Calculate_Smoothing_Vertice_Normals();
			vertices.resize(pre_smoothing_vertices.size()); //Size kept constant after first iteration, but moved here so vertices.empty() checks can be carried out in functions above

			list_of_adjacent_weight_factors.clear();
			list_of_adjacent_original_positions.clear();
			for (int i = 0; i < pre_smoothing_vertices.size(); i++) { //For each vertex
				//Check if the vertex has adjacent points
				bool isolated_point = true;
				for (int j = 0; j < faces.size(); j++) { //For every face
					//Check there are 3 vertices to each face (ie: each face is a triangle)
					if (faces[j].size() != 3)
						cout << "Error in pre_smoothing_faces - face " << j << " (indexing starting at 0) is not a triangle!" << endl;
					for (int k = 0; k < faces[j].size(); k++) { //For every vertice in each face
						if (i == faces[j][k] - 1) { //If the vertex is within this face
							isolated_point = false; //Vertex is no longer isolated
							//Add the other two vertices onto adjacent_vertices, if they haven't been added already
							if (k == 0) { //If this is the first vertex
								bool new_1 = true, new_2 = true;
								for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
									if (adjacent_vertice_indices[i][m] == faces[j][1] - 1)
										new_1 = false; //Second vertex is not new
									else if (adjacent_vertice_indices[i][m] == faces[j][2] - 1)
										new_2 = false; //Third vertex is not new
									if (!new_1 && !new_2)
										break; //Break if both adjacent vertices are confirmed as not new
								}
								if (new_1)
									adjacent_vertice_indices[i].push_back(faces[j][1] - 1); //Add to list for adjacent vertices for this vertex
								if (new_2)
									adjacent_vertice_indices[i].push_back(faces[j][2] - 1); //Add to list for adjacent vertices for this vertex
							}
							if (k == 1) { //If this is the second vertex
								bool new_0 = true, new_2 = true;
								for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
									if (adjacent_vertice_indices[i][m] == faces[j][0] - 1)
										new_0 = false; //First vertex is not new
									else if (adjacent_vertice_indices[i][m] == faces[j][2] - 1)
										new_2 = false; //Third vertex is not new
									if (!new_0 && !new_2)
										break; //Break if both adjacent vertices are confirmed as not new
								}
								if (new_0)
									adjacent_vertice_indices[i].push_back(faces[j][0] - 1); //Add to list for adjacent vertices for this vertex
								if (new_2)
									adjacent_vertice_indices[i].push_back(faces[j][2] - 1); //Add to list for adjacent vertices for this vertex
							}
							if (k == 2) { //If this is the third vertex
								bool new_0 = true, new_1 = true;
								for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
									if (adjacent_vertice_indices[i][m] == faces[j][0] - 1)
										new_0 = false; //First vertex is not new
									else if (adjacent_vertice_indices[i][m] == faces[j][1] - 1)
										new_1 = false; //Second vertex is not new
									if (!new_0 && !new_1)
										break; //Break if both adjacent vertices are confirmed as not new
								}
								if (new_0)
									adjacent_vertice_indices[i].push_back(faces[j][0] - 1); //Add to list for adjacent vertices for this vertex
								if (new_1)
									adjacent_vertice_indices[i].push_back(faces[j][1] - 1); //Add to list for adjacent vertices for this vertex
							}
						}
					}
				}
				if (isolated_point) { //If point is isolated
					//Copy point to vertices without modification
					vertices[i].insert(begin(vertices[i]), begin(pre_smoothing_vertices[i]), end(pre_smoothing_vertices[i]));
					list_of_laplacian_smoothed_positions[i].push_back(0.0); //Shows that this point has not been smoothed
					list_of_average_displacements_during_laplacian_smoothing[i].push_back(0.0); //Shows that this point has not been smoothed
					if (p == 0 && q == 0)
						isolated_points_indices.push_back(i); //Store this as an isolated point (only when it is first encountered - avoids repeated values)
				}
				else { //If point isn't isolated (so can be smoothed)
					vector<double> weight_factors, average_distance_b_i, normal_vector;
					vector<vector<double>> adjacent_original_vertices_positions;
					adjacent_original_vertices_positions.resize(adjacent_vertice_indices[i].size());
					list_of_adjacent_weight_factors.resize(list_of_adjacent_weight_factors.size() + 1);
					list_of_adjacent_original_positions.resize(list_of_adjacent_original_positions.size() + 1);
					//Calculate average distance (b_i)
					for (int j = 0; j < adjacent_vertice_indices[i].size(); j++) {
						double temp_weight_factor;
						temp_weight_factor = Calculate_Weight_Factor(vertice_area_weights[adjacent_vertice_indices[i][j]], vertice_area_weights[i]);
						weight_factors.push_back(temp_weight_factor);
						adjacent_original_vertices_positions[j].insert(begin(adjacent_original_vertices_positions[j]), begin(pre_smoothing_vertices[adjacent_vertice_indices[i][j]]), end(pre_smoothing_vertices[adjacent_vertice_indices[i][j]]));
					}
					average_distance_b_i = Calculate_Average_Distance_Vector_for_Movement(weight_factors, adjacent_original_vertices_positions, pre_smoothing_vertices[i]);
					//Migrate positions using Laplacian smoothing (o_i to q_i)
					if (p == 0 && q == 0) //If this is the first time the point has been encountered, use pre_smoothing vertices[i]
						laplacian_smoothed_position = Laplacian_Smoothing(pre_smoothing_vertices[i], 0.1, average_distance_b_i, smoothing_vertice_normals[i]);
					else //If this isn't the first time the point has been encountered it has already been smoothed, with is postion stored in vertices[i]
						laplacian_smoothed_position = Laplacian_Smoothing(vertices[i], 0.1, average_distance_b_i, smoothing_vertice_normals[i]);
					//Copy data to outer matrices to move points back later (q_i to p_i)
					list_of_adjacent_weight_factors[list_of_adjacent_weight_factors.size() - 1].insert(begin(list_of_adjacent_weight_factors[list_of_adjacent_weight_factors.size() - 1]), begin(weight_factors), end(weight_factors));
					list_of_adjacent_original_positions[list_of_adjacent_original_positions.size() - 1].insert(begin(list_of_adjacent_original_positions[list_of_adjacent_original_positions.size() - 1]), begin(adjacent_original_vertices_positions), end(adjacent_original_vertices_positions));
					list_of_laplacian_smoothed_positions[i].clear();
					list_of_laplacian_smoothed_positions[i].insert(begin(list_of_laplacian_smoothed_positions[i]), begin(laplacian_smoothed_position), end(laplacian_smoothed_position));
					//Copy smoothed point to vertices
					vertices[i].clear(); //Overwrite previous position (which was just storing posiiton after the last iteration of Laplacian smoothing)
					vertices[i].insert(begin(vertices[i]), begin(laplacian_smoothed_position), end(laplacian_smoothed_position));
					if (p == 0 && q == 0)
						smoothed_points_indices.push_back(i); //Store this as a smoothed point (only when it is first encountered - avoids repeated points)
				}
			}
			//Calculate average distance towards average original positions (d_i)
			vector<vector<vector<double>>> list_of_adjacent_laplacian_smoothed_positions;
			list_of_adjacent_laplacian_smoothed_positions.resize(smoothed_points_indices.size()); //Used for each point that was smoothed
			for (int i = 0; i < smoothed_points_indices.size(); i++) { //For each point that as smoothed
				for (int j = 0; j < adjacent_vertice_indices[i].size(); j++) { //For every adjacent point
					bool isolated_adjacent_point = false;
					//Check if the adjacent point is isolated
					for (int k = 0; k < isolated_points_indices.size(); k++) { //For every isolated point
						if (isolated_points_indices[k] == adjacent_vertice_indices[i][j]) { //If the adjacent vertex is an isolated point
							isolated_adjacent_point = true;
							break;
						}
					}
					if (!isolated_adjacent_point) { //If the adjacent point isn't isolated
						list_of_adjacent_laplacian_smoothed_positions[i].resize(list_of_adjacent_laplacian_smoothed_positions[i].size() + 1);
						list_of_adjacent_laplacian_smoothed_positions[i][list_of_adjacent_laplacian_smoothed_positions[i].size() - 1].insert(begin(list_of_adjacent_laplacian_smoothed_positions[i][list_of_adjacent_laplacian_smoothed_positions[i].size() - 1]), begin(list_of_laplacian_smoothed_positions[adjacent_vertice_indices[i][j]]), end(list_of_laplacian_smoothed_positions[adjacent_vertice_indices[i][j]]));
					}
				}
				//Calculate average displacements and store vector
				average_displacement = Calculate_Average_Displacement_during_Laplacian_Smoothing(list_of_adjacent_weight_factors[i], list_of_adjacent_original_positions[i], list_of_adjacent_laplacian_smoothed_positions[i]);
				list_of_average_displacements_during_laplacian_smoothing[i].clear();
				list_of_average_displacements_during_laplacian_smoothing[i].insert(begin(list_of_average_displacements_during_laplacian_smoothing[i]), begin(average_displacement), end(average_displacement));
			}

			//Correct catenoid layers
			Correct_Layering_for_Catenoids_Post_Smoothing(z_value, z_step, tolerance);
		}
		//Move vertices back for volume preservation
		vector<double> end_position_for_this_iteration;
		for (int i = 0; i < pre_smoothing_vertices.size(); i++) { //For every vertex
			bool isolated_point = false;
			//Check if point was isolated (and so not smoothed)
			for (int j = 0; j < isolated_points_indices.size(); j++) {
				if (isolated_points_indices[j] == i) {
					isolated_point = true;
					break;
				}
			}
			if (!isolated_point) { //If point wasn't isolated (so was smoothed)
				//Calculate final position, having been migrated back after Laplacian smoothing, and store in vertices[i]
				end_position_for_this_iteration = Migrate_Points_after_Laplacian_Smoothing(list_of_laplacian_smoothed_positions[i], list_of_average_displacements_during_laplacian_smoothing[i], smoothing_vertice_normals[i]);
				vertices[i].clear(); //Overwrite previous position (which was just storing position after Laplacian smoothed had finished)
				vertices[i].insert(begin(vertices[i]), begin(end_position_for_this_iteration), end(end_position_for_this_iteration));
			}

		}

		//Correct catenoid layers
		Correct_Layering_for_Catenoids_Post_Smoothing(z_value, z_step, tolerance);
	}

	cout << "Finished Gaussian [" << gaussian_iterations << ", " << iterations_per_step << "] smoothing." << endl << endl;
}

void CCalculatingCurvature::Gaussian_Smoothing(int gaussian_iterations, int iterations_per_step, int output_edges_frequency, int z_value, double alpha, double z_step, double tolerance, string output_curvature_file, int output_faces_frequency, string output_post_smoothing_vertices_filename_prefix) { //Catenoids (with correction)
	if (gaussian_iterations < 0) {
		cout << "Can't have a negative gaussian_iterations!" << endl;
		exit(0);
	}
	if (iterations_per_step < 0) {
		cout << "Can't have a negative iterations_per_step!" << endl;
		exit(0);
	}
	if (output_edges_frequency < 0) {
		cout << "Can't have a negative output_edges_frequency!" << endl;
		exit(0);
	}

	cout << "Begin Gaussian Smoothing..." << endl;

	vector<vector<vector<double>>> list_of_adjacent_original_positions;
	vector<vector<double>> list_of_adjacent_weight_factors, list_of_laplacian_smoothed_positions, list_of_average_displacements_during_laplacian_smoothing;
	vector<vector<int>> adjacent_vertice_indices;
	vector<int> isolated_points_indices, smoothed_points_indices;

	list_of_laplacian_smoothed_positions.resize(pre_smoothing_vertices.size()); //Stores 0 if position isn't smoothed, otherwise stores position
	list_of_average_displacements_during_laplacian_smoothing.resize(pre_smoothing_vertices.size()); //Stores 0 if the position hasn't been smoothed
	adjacent_vertice_indices.resize(pre_smoothing_vertices.size());
	for (int p = 0; p < gaussian_iterations; p++) { //For each iteration of the gaussian smoothing algorithm
		vector<double> laplacian_smoothed_position, average_displacement;
		for (int q = 0; q < iterations_per_step; q++) { //Repeat until the kernel radius of gaussian smoothing (r_gauss) reaches 0
			cout << "Gaussian smoothing iteration: [" << p << "][" << q << "]" << endl;

			Calculate_Face_Centres();
			if (output_edges_frequency != 0)
				Calculate_Edge_Centres(true, output_edges_frequency);
			else
				Calculate_Edge_Centres(false, 0);
			Calculate_Vertex_Area_Weights_for_Smoothing();
			Calculate_Smoothing_Vertice_Normals();
			vertices.resize(pre_smoothing_vertices.size()); //Size kept constant after first iteration, but moved here so vertices.empty() checks can be carried out in functions above

			list_of_adjacent_weight_factors.clear();
			list_of_adjacent_original_positions.clear();
			for (int i = 0; i < pre_smoothing_vertices.size(); i++) { //For each vertex
				//Check if the vertex has adjacent points
				bool isolated_point = true;
				for (int j = 0; j < faces.size(); j++) { //For every face
					//Check there are 3 vertices to each face (ie: each face is a triangle)
					if (faces[j].size() != 3)
						cout << "Error in pre_smoothing_faces - face " << j << " (indexing starting at 0) is not a triangle!" << endl;
					for (int k = 0; k < faces[j].size(); k++) { //For every vertice in each face
						if (i == faces[j][k] - 1) { //If the vertex is within this face
							isolated_point = false; //Vertex is no longer isolated
							//Add the other two vertices onto adjacent_vertices, if they haven't been added already
							if (k == 0) { //If this is the first vertex
								bool new_1 = true, new_2 = true;
								for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
									if (adjacent_vertice_indices[i][m] == faces[j][1] - 1)
										new_1 = false; //Second vertex is not new
									else if (adjacent_vertice_indices[i][m] == faces[j][2] - 1)
										new_2 = false; //Third vertex is not new
									if (!new_1 && !new_2)
										break; //Break if both adjacent vertices are confirmed as not new
								}
								if (new_1)
									adjacent_vertice_indices[i].push_back(faces[j][1] - 1); //Add to list for adjacent vertices for this vertex
								if (new_2)
									adjacent_vertice_indices[i].push_back(faces[j][2] - 1); //Add to list for adjacent vertices for this vertex
							}
							if (k == 1) { //If this is the second vertex
								bool new_0 = true, new_2 = true;
								for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
									if (adjacent_vertice_indices[i][m] == faces[j][0] - 1)
										new_0 = false; //First vertex is not new
									else if (adjacent_vertice_indices[i][m] == faces[j][2] - 1)
										new_2 = false; //Third vertex is not new
									if (!new_0 && !new_2)
										break; //Break if both adjacent vertices are confirmed as not new
								}
								if (new_0)
									adjacent_vertice_indices[i].push_back(faces[j][0] - 1); //Add to list for adjacent vertices for this vertex
								if (new_2)
									adjacent_vertice_indices[i].push_back(faces[j][2] - 1); //Add to list for adjacent vertices for this vertex
							}
							if (k == 2) { //If this is the third vertex
								bool new_0 = true, new_1 = true;
								for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
									if (adjacent_vertice_indices[i][m] == faces[j][0] - 1)
										new_0 = false; //First vertex is not new
									else if (adjacent_vertice_indices[i][m] == faces[j][1] - 1)
										new_1 = false; //Second vertex is not new
									if (!new_0 && !new_1)
										break; //Break if both adjacent vertices are confirmed as not new
								}
								if (new_0)
									adjacent_vertice_indices[i].push_back(faces[j][0] - 1); //Add to list for adjacent vertices for this vertex
								if (new_1)
									adjacent_vertice_indices[i].push_back(faces[j][1] - 1); //Add to list for adjacent vertices for this vertex
							}
						}
					}
				}
				if (isolated_point) { //If point is isolated
					//Copy point to vertices without modification
					vertices[i].insert(begin(vertices[i]), begin(pre_smoothing_vertices[i]), end(pre_smoothing_vertices[i])); //Don't need vertices[i].clear() before because this will only be triggered once
					list_of_laplacian_smoothed_positions[i].push_back(0.0); //Shows that this point has not been smoothed
					list_of_average_displacements_during_laplacian_smoothing[i].push_back(0.0); //Shows that this point has not been smoothed
					if (p == 0 && q == 0)
						isolated_points_indices.push_back(i); //Store this as an isolated point (only when it is first encountered - avoids repeated values)
				}
				else { //If point isn't isolated (so can be smoothed)
					vector<double> weight_factors, average_distance_b_i, normal_vector;
					vector<vector<double>> adjacent_original_vertices_positions;
					adjacent_original_vertices_positions.resize(adjacent_vertice_indices[i].size());
					list_of_adjacent_weight_factors.resize(list_of_adjacent_weight_factors.size() + 1);
					list_of_adjacent_original_positions.resize(list_of_adjacent_original_positions.size() + 1);
					//Calculate average distance (b_i)
					for (int j = 0; j < adjacent_vertice_indices[i].size(); j++) {
						double temp_weight_factor;
						temp_weight_factor = Calculate_Weight_Factor(vertice_area_weights[adjacent_vertice_indices[i][j]], vertice_area_weights[i]);
						weight_factors.push_back(temp_weight_factor);
						adjacent_original_vertices_positions[j].insert(begin(adjacent_original_vertices_positions[j]), begin(pre_smoothing_vertices[adjacent_vertice_indices[i][j]]), end(pre_smoothing_vertices[adjacent_vertice_indices[i][j]]));
					}
					average_distance_b_i = Calculate_Average_Distance_Vector_for_Movement(weight_factors, adjacent_original_vertices_positions, pre_smoothing_vertices[i]);
					//Migrate positions using Laplacian smoothing (o_i to q_i)
					if (p == 0 && q == 0) //If this is the first time the point has been encountered, use pre_smoothing vertices[i]
						laplacian_smoothed_position = Laplacian_Smoothing(pre_smoothing_vertices[i], 0.1, average_distance_b_i, smoothing_vertice_normals[i]);
					else //If this isn't the first time the point has been encountered it has already been smoothed, with is postion stored in vertices[i]
						laplacian_smoothed_position = Laplacian_Smoothing(vertices[i], 0.1, average_distance_b_i, smoothing_vertice_normals[i]);
					//Copy data to outer matrices to move points back later (q_i to p_i)
					list_of_adjacent_weight_factors[list_of_adjacent_weight_factors.size() - 1].insert(begin(list_of_adjacent_weight_factors[list_of_adjacent_weight_factors.size() - 1]), begin(weight_factors), end(weight_factors));
					list_of_adjacent_original_positions[list_of_adjacent_original_positions.size() - 1].insert(begin(list_of_adjacent_original_positions[list_of_adjacent_original_positions.size() - 1]), begin(adjacent_original_vertices_positions), end(adjacent_original_vertices_positions));
					list_of_laplacian_smoothed_positions[i].clear();
					list_of_laplacian_smoothed_positions[i].insert(begin(list_of_laplacian_smoothed_positions[i]), begin(laplacian_smoothed_position), end(laplacian_smoothed_position));
					//Copy smoothed point to vertices
					vertices[i].clear(); //Overwrite previous position (which was just storing posiiton after the last iteration of Laplacian smoothing)
					vertices[i].insert(begin(vertices[i]), begin(laplacian_smoothed_position), end(laplacian_smoothed_position));
					if (p == 0 && q == 0)
						smoothed_points_indices.push_back(i); //Store this as a smoothed point (only when it is first encountered - avoids repeated points)
				}
			}
			//Calculate average distance towards average original positions (d_i)
			vector<vector<vector<double>>> list_of_adjacent_laplacian_smoothed_positions;
			list_of_adjacent_laplacian_smoothed_positions.resize(smoothed_points_indices.size()); //Used for each point that was smoothed
			for (int i = 0; i < smoothed_points_indices.size(); i++) { //For each point that as smoothed
				for (int j = 0; j < adjacent_vertice_indices[i].size(); j++) { //For every adjacent point
					bool isolated_adjacent_point = false;
					//Check if the adjacent point is isolated
					for (int k = 0; k < isolated_points_indices.size(); k++) { //For every isolated point
						if (isolated_points_indices[k] == adjacent_vertice_indices[i][j]) { //If the adjacent vertex is an isolated point
							isolated_adjacent_point = true;
							break;
						}
					}
					if (!isolated_adjacent_point) { //If the adjacent point isn't isolated
						list_of_adjacent_laplacian_smoothed_positions[i].resize(list_of_adjacent_laplacian_smoothed_positions[i].size() + 1);
						list_of_adjacent_laplacian_smoothed_positions[i][list_of_adjacent_laplacian_smoothed_positions[i].size() - 1].insert(begin(list_of_adjacent_laplacian_smoothed_positions[i][list_of_adjacent_laplacian_smoothed_positions[i].size() - 1]), begin(list_of_laplacian_smoothed_positions[adjacent_vertice_indices[i][j]]), end(list_of_laplacian_smoothed_positions[adjacent_vertice_indices[i][j]]));
					}
				}
				//Calculate average displacements and store vector
				average_displacement = Calculate_Average_Displacement_during_Laplacian_Smoothing(list_of_adjacent_weight_factors[i], list_of_adjacent_original_positions[i], list_of_adjacent_laplacian_smoothed_positions[i]);
				list_of_average_displacements_during_laplacian_smoothing[i].clear();
				list_of_average_displacements_during_laplacian_smoothing[i].insert(begin(list_of_average_displacements_during_laplacian_smoothing[i]), begin(average_displacement), end(average_displacement));
			}

			//Correct catenoid layers
			Correct_Layering_for_Catenoids_Post_Smoothing(z_value, z_step, tolerance);
		}
		//Move vertices back for volume preservation
		vector<double> end_position_for_this_iteration;
		for (int i = 0; i < pre_smoothing_vertices.size(); i++) { //For every vertex
			bool isolated_point = false;
			//Check if point was isolated (and so not smoothed)
			for (int j = 0; j < isolated_points_indices.size(); j++) {
				if (isolated_points_indices[j] == i) {
					isolated_point = true;
					break;
				}
			}
			if (!isolated_point) { //If point wasn't isolated (so was smoothed)
				//Calculate final position, having been migrated back after Laplacian smoothing, and store in vertices[i]
				end_position_for_this_iteration = Migrate_Points_after_Laplacian_Smoothing(list_of_laplacian_smoothed_positions[i], list_of_average_displacements_during_laplacian_smoothing[i], smoothing_vertice_normals[i]);
				vertices[i].clear(); //Overwrite previous position (which was just storing position after Laplacian smoothed had finished)
				vertices[i].insert(begin(vertices[i]), begin(end_position_for_this_iteration), end(end_position_for_this_iteration));
			}

		}

		//Correct catenoid layers
		Correct_Layering_for_Catenoids_Post_Smoothing(z_value, z_step, tolerance);

		//Continuous output to Curvautre Overview file
		Append_Continuous_Catneoid_Curvature_Output_to_File(z_value, alpha, z_step, p + 1, iterations_per_step, output_curvature_file, output_faces_frequency); //p + 1 so indexing of iterations starts at 1

		//Write Vertices Post Smoothing to New File
		//Create filename
		string vertices_post_smooting_output_file = output_post_smoothing_vertices_filename_prefix;
		vertices_post_smooting_output_file += to_string(z_value);
		vertices_post_smooting_output_file += "_";
		vertices_post_smooting_output_file += to_string(alpha);
		vertices_post_smooting_output_file.erase(vertices_post_smooting_output_file.find_last_not_of('0') + 1, string::npos); //Remove trailing zeroes
		vertices_post_smooting_output_file.erase(vertices_post_smooting_output_file.find_last_not_of('.') + 1, string::npos); //Remove final '.' if alpha is actually an integer
		vertices_post_smooting_output_file += "_";
		vertices_post_smooting_output_file += to_string(z_step);
		vertices_post_smooting_output_file.erase(vertices_post_smooting_output_file.find_last_not_of('0') + 1, string::npos); //Remove trailing zeroes
		vertices_post_smooting_output_file.erase(vertices_post_smooting_output_file.find_last_not_of('.') + 1, string::npos); //Remove final '.' if z_step is actually an integer
		vertices_post_smooting_output_file += "_";
		vertices_post_smooting_output_file += to_string(p + 1);
		vertices_post_smooting_output_file += "_";
		vertices_post_smooting_output_file += to_string(iterations_per_step);
		vertices_post_smooting_output_file += "_Smoothing.txt";
		//Output file
		Write_2D_Matrix_to_File(vertices, "Vertices Post Smoothing", vertices_post_smooting_output_file);
	}

	cout << "Finished Gaussian [" << gaussian_iterations << ", " << iterations_per_step << "] smoothing." << endl << endl;
}

void CCalculatingCurvature::Skip_Smoothing(void) {
	if (!vertices.empty()) {
		cout << "Can't skip smoothing when smoothing has already been applied." << endl;
		exit(0);
	}

	vertices.resize(pre_smoothing_vertices.size());
	for (int i = 0; i < pre_smoothing_vertices.size(); i++)
		vertices[i].insert(begin(vertices[i]), begin(pre_smoothing_vertices[i]), end(pre_smoothing_vertices[i]));

	cout << "Surface smoothing skipped." << endl << endl;
}

void CCalculatingCurvature::Laplacian_Smoothing(int laplacian_iterations, int output_edges_frequency) {
	if (laplacian_iterations < 0) {
		cout << "Can't have a negative laplacian_iterations!" << endl;
		exit(0);
	}
	if (output_edges_frequency < 0) {
		cout << "Can't have a negative output_edges_frequency!" << endl;
		exit(0);
	}
	cout << "Begin Laplcian Smoothing..." << endl;

	vector<vector<int>> adjacent_vertice_indices;
	vector<double> laplacian_smoothed_position;

	adjacent_vertice_indices.resize(pre_smoothing_vertices.size());
	for (int q = 0; q < laplacian_iterations; q++) { //Repeat until the kernel radius of gaussian smoothing (r_gauss) reaches 0
		cout << "Laplcian smoothing iteration: [" << q << "]" << endl;

		Calculate_Face_Centres();
		if (output_edges_frequency != 0)
			Calculate_Edge_Centres(true, output_edges_frequency);
		else
			Calculate_Edge_Centres(false, 0);
		Calculate_Vertex_Area_Weights_for_Smoothing();
		Calculate_Smoothing_Vertice_Normals();
		vertices.resize(pre_smoothing_vertices.size()); //Size kept constant after first iteration, but moved here so vertices.empty() checks can be carried out in functions above

		for (int i = 0; i < pre_smoothing_vertices.size(); i++) { //For each vertex
			//Check if the vertex has adjacent points
			bool isolated_point = true;
			for (int j = 0; j < faces.size(); j++) { //For every face
				//Check there are 3 vertices to each face (ie: each face is a triangle)
				if (faces[j].size() != 3)
					cout << "Error in pre_smoothing_faces - face " << j << " (indexing starting at 0) is not a triangle!" << endl;
				for (int k = 0; k < faces[j].size(); k++) { //For every vertice in each face
					if (i == faces[j][k] - 1) { //If the vertex is within this face
						isolated_point = false; //Vertex is no longer isolated
						//Add the other two vertices onto adjacent_vertices, if they haven't been added already
						if (k == 0) { //If this is the first vertex
							bool new_1 = true, new_2 = true;
							for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
								if (adjacent_vertice_indices[i][m] == faces[j][1] - 1)
									new_1 = false; //Second vertex is not new
								else if (adjacent_vertice_indices[i][m] == faces[j][2] - 1)
									new_2 = false; //Third vertex is not new
								if (!new_1 && !new_2)
									break; //Break if both adjacent vertices are confirmed as not new
							}
							if (new_1)
								adjacent_vertice_indices[i].push_back(faces[j][1] - 1); //Add to list for adjacent vertices for this vertex
							if (new_2)
								adjacent_vertice_indices[i].push_back(faces[j][2] - 1); //Add to list for adjacent vertices for this vertex
						}
						if (k == 1) { //If this is the second vertex
							bool new_0 = true, new_2 = true;
							for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
								if (adjacent_vertice_indices[i][m] == faces[j][0] - 1)
									new_0 = false; //First vertex is not new
								else if (adjacent_vertice_indices[i][m] == faces[j][2] - 1)
									new_2 = false; //Third vertex is not new
								if (!new_0 && !new_2)
									break; //Break if both adjacent vertices are confirmed as not new
							}
							if (new_0)
								adjacent_vertice_indices[i].push_back(faces[j][0] - 1); //Add to list for adjacent vertices for this vertex
							if (new_2)
								adjacent_vertice_indices[i].push_back(faces[j][2] - 1); //Add to list for adjacent vertices for this vertex
						}
						if (k == 2) { //If this is the third vertex
							bool new_0 = true, new_1 = true;
							for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
								if (adjacent_vertice_indices[i][m] == faces[j][0] - 1)
									new_0 = false; //First vertex is not new
								else if (adjacent_vertice_indices[i][m] == faces[j][1] - 1)
									new_1 = false; //Second vertex is not new
								if (!new_0 && !new_1)
									break; //Break if both adjacent vertices are confirmed as not new
							}
							if (new_0)
								adjacent_vertice_indices[i].push_back(faces[j][0] - 1); //Add to list for adjacent vertices for this vertex
							if (new_1)
								adjacent_vertice_indices[i].push_back(faces[j][1] - 1); //Add to list for adjacent vertices for this vertex
						}
					}
				}
			}
			if (isolated_point) //If point is isolated, copy point to vertices without modification
				vertices[i].insert(begin(vertices[i]), begin(pre_smoothing_vertices[i]), end(pre_smoothing_vertices[i]));
			else { //If point isn't isolated (so can be smoothed)
				vector<double> weight_factors, average_distance_b_i, normal_vector;
				vector<vector<double>> adjacent_original_vertices_positions;
				adjacent_original_vertices_positions.resize(adjacent_vertice_indices[i].size());
				//Calculate average distance (b_i)
				for (int j = 0; j < adjacent_vertice_indices[i].size(); j++) {
					double temp_weight_factor;
					temp_weight_factor = Calculate_Weight_Factor(vertice_area_weights[adjacent_vertice_indices[i][j]], vertice_area_weights[i]);
					weight_factors.push_back(temp_weight_factor);
					adjacent_original_vertices_positions[j].insert(begin(adjacent_original_vertices_positions[j]), begin(pre_smoothing_vertices[adjacent_vertice_indices[i][j]]), end(pre_smoothing_vertices[adjacent_vertice_indices[i][j]]));
				}
				average_distance_b_i = Calculate_Average_Distance_Vector_for_Movement(weight_factors, adjacent_original_vertices_positions, pre_smoothing_vertices[i]);
				//Migrate positions using Laplacian smoothing (o_i to q_i)
				if (q == 0) //If this is the first time the point has been encountered, use pre_smoothing vertices[i]
					laplacian_smoothed_position = Laplacian_Smoothing(pre_smoothing_vertices[i], 0.1, average_distance_b_i, smoothing_vertice_normals[i]);
				else //If this isn't the first time the point has been encountered it has already been smoothed, with is postion stored in vertices[i]
					laplacian_smoothed_position = Laplacian_Smoothing(vertices[i], 0.1, average_distance_b_i, smoothing_vertice_normals[i]);
				//Copy smoothed point to vertices
				vertices[i].clear(); //Overwrite previous position (which was just storing posiiton after the last iteration of Laplacian smoothing)
				vertices[i].insert(begin(vertices[i]), begin(laplacian_smoothed_position), end(laplacian_smoothed_position));
			}
		}
	}

	cout << "Finished Laplacian [" << laplacian_iterations << "] smoothing." << endl << endl;
}

void CCalculatingCurvature::Laplacian_Smoothing(int laplacian_iterations, int output_edges_frequency, bool continuous_curvature_output, int output_faces_frequency) {
	if (laplacian_iterations < 0) {
		cout << "Can't have a negative laplacian_iterations!" << endl;
		exit(0);
	}
	if (output_edges_frequency < 0) {
		cout << "Can't have a negative output_edges_frequency!" << endl;
		exit(0);
	}
	cout << "Begin Laplcian Smoothing..." << endl;

	vector<vector<int>> adjacent_vertice_indices;
	vector<double> laplacian_smoothed_position;

	adjacent_vertice_indices.resize(pre_smoothing_vertices.size());
	for (int q = 0; q < laplacian_iterations; q++) { //Repeat until the kernel radius of gaussian smoothing (r_gauss) reaches 0
		cout << "Laplcian smoothing iteration: [" << q << "]" << endl;

		Calculate_Face_Centres();
		if (output_edges_frequency != 0)
			Calculate_Edge_Centres(true, output_edges_frequency);
		else
			Calculate_Edge_Centres(false, 0);
		Calculate_Vertex_Area_Weights_for_Smoothing();
		Calculate_Smoothing_Vertice_Normals();
		vertices.resize(pre_smoothing_vertices.size()); //Size kept constant after first iteration, but moved here so vertices.empty() checks can be carried out in functions above

		for (int i = 0; i < pre_smoothing_vertices.size(); i++) { //For each vertex
			//Check if the vertex has adjacent points
			bool isolated_point = true;
			for (int j = 0; j < faces.size(); j++) { //For every face
				//Check there are 3 vertices to each face (ie: each face is a triangle)
				if (faces[j].size() != 3)
					cout << "Error in pre_smoothing_faces - face " << j << " (indexing starting at 0) is not a triangle!" << endl;
				for (int k = 0; k < faces[j].size(); k++) { //For every vertice in each face
					if (i == faces[j][k] - 1) { //If the vertex is within this face
						isolated_point = false; //Vertex is no longer isolated
						//Add the other two vertices onto adjacent_vertices, if they haven't been added already
						if (k == 0) { //If this is the first vertex
							bool new_1 = true, new_2 = true;
							for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
								if (adjacent_vertice_indices[i][m] == faces[j][1] - 1)
									new_1 = false; //Second vertex is not new
								else if (adjacent_vertice_indices[i][m] == faces[j][2] - 1)
									new_2 = false; //Third vertex is not new
								if (!new_1 && !new_2)
									break; //Break if both adjacent vertices are confirmed as not new
							}
							if (new_1)
								adjacent_vertice_indices[i].push_back(faces[j][1] - 1); //Add to list for adjacent vertices for this vertex
							if (new_2)
								adjacent_vertice_indices[i].push_back(faces[j][2] - 1); //Add to list for adjacent vertices for this vertex
						}
						if (k == 1) { //If this is the second vertex
							bool new_0 = true, new_2 = true;
							for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
								if (adjacent_vertice_indices[i][m] == faces[j][0] - 1)
									new_0 = false; //First vertex is not new
								else if (adjacent_vertice_indices[i][m] == faces[j][2] - 1)
									new_2 = false; //Third vertex is not new
								if (!new_0 && !new_2)
									break; //Break if both adjacent vertices are confirmed as not new
							}
							if (new_0)
								adjacent_vertice_indices[i].push_back(faces[j][0] - 1); //Add to list for adjacent vertices for this vertex
							if (new_2)
								adjacent_vertice_indices[i].push_back(faces[j][2] - 1); //Add to list for adjacent vertices for this vertex
						}
						if (k == 2) { //If this is the third vertex
							bool new_0 = true, new_1 = true;
							for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
								if (adjacent_vertice_indices[i][m] == faces[j][0] - 1)
									new_0 = false; //First vertex is not new
								else if (adjacent_vertice_indices[i][m] == faces[j][1] - 1)
									new_1 = false; //Second vertex is not new
								if (!new_0 && !new_1)
									break; //Break if both adjacent vertices are confirmed as not new
							}
							if (new_0)
								adjacent_vertice_indices[i].push_back(faces[j][0] - 1); //Add to list for adjacent vertices for this vertex
							if (new_1)
								adjacent_vertice_indices[i].push_back(faces[j][1] - 1); //Add to list for adjacent vertices for this vertex
						}
					}
				}
			}
			if (isolated_point) //If point is isolated, copy point to vertices without modification
				vertices[i].insert(begin(vertices[i]), begin(pre_smoothing_vertices[i]), end(pre_smoothing_vertices[i]));
			else { //If point isn't isolated (so can be smoothed)
				vector<double> weight_factors, average_distance_b_i, normal_vector;
				vector<vector<double>> adjacent_original_vertices_positions;
				adjacent_original_vertices_positions.resize(adjacent_vertice_indices[i].size());
				//Calculate average distance (b_i)
				for (int j = 0; j < adjacent_vertice_indices[i].size(); j++) {
					double temp_weight_factor;
					temp_weight_factor = Calculate_Weight_Factor(vertice_area_weights[adjacent_vertice_indices[i][j]], vertice_area_weights[i]);
					weight_factors.push_back(temp_weight_factor);
					adjacent_original_vertices_positions[j].insert(begin(adjacent_original_vertices_positions[j]), begin(pre_smoothing_vertices[adjacent_vertice_indices[i][j]]), end(pre_smoothing_vertices[adjacent_vertice_indices[i][j]]));
				}
				average_distance_b_i = Calculate_Average_Distance_Vector_for_Movement(weight_factors, adjacent_original_vertices_positions, pre_smoothing_vertices[i]);
				//Migrate positions using Laplacian smoothing (o_i to q_i)
				if (q == 0) //If this is the first time the point has been encountered, use pre_smoothing vertices[i]
					laplacian_smoothed_position = Laplacian_Smoothing(pre_smoothing_vertices[i], 0.1, average_distance_b_i, smoothing_vertice_normals[i]);
				else //If this isn't the first time the point has been encountered it has already been smoothed, with is postion stored in vertices[i]
					laplacian_smoothed_position = Laplacian_Smoothing(vertices[i], 0.1, average_distance_b_i, smoothing_vertice_normals[i]);
				//Copy smoothed point to vertices
				vertices[i].clear(); //Overwrite previous position (which was just storing posiiton after the last iteration of Laplacian smoothing)
				vertices[i].insert(begin(vertices[i]), begin(laplacian_smoothed_position), end(laplacian_smoothed_position));
			}
		}

		if (continuous_curvature_output) {
			//Curvature Calculation
			double mean_of_mean_curvature = 0.0, mean_gaussian_curvature = 0.0;
			Calculate_Face_Normals();
			Calculate_Vertice_Normals();
			Calculate_Curvature(true, output_faces_frequency);
			Get_Principal_Curvatures();
			Calculate_Gaussian_Curvatures();
			Calculate_Mean_Curvatures();

			cout << "Results for Laplacian smoothing iteration [" << q + 1 << "] (indexing starting from 1):" << endl;
			mean_of_mean_curvature = Calculate_Mean<double>(mean_curvature);
			cout << "Mean of Mean Curvature (Whole Dataset): " << mean_of_mean_curvature << endl;
			mean_of_mean_curvature = Calculate_Mean_of_Doubles_List_and_Remove_NANs(mean_curvature);
			cout << "Mean of Mean Curvature (Whole Dataset, Removed NANs): " << mean_of_mean_curvature << endl;
			mean_gaussian_curvature = Calculate_Mean<double>(gaussian_curvature);
			cout << "Mean Gaussian Curvature (Whole Dataset): " << mean_gaussian_curvature << endl;
			mean_gaussian_curvature = Calculate_Mean_of_Doubles_List_and_Remove_NANs(gaussian_curvature);
			cout << "Mean Gaussian Curvature (Whole Dataset, Removed NANs): " << mean_gaussian_curvature << endl;
		}
	}

	cout << "Finished Laplacian [" << laplacian_iterations << "] smoothing." << endl << endl;
}

void CCalculatingCurvature::Laplacian_Smoothing(int laplacian_iterations, int output_edges_frequency, int input_dataset_size, string output_curvature_file, int output_faces_frequency, string output_post_smoothing_vertices_filename_prefix) { //Spheres - continuous output
	if (laplacian_iterations < 0) {
		cout << "Can't have a negative laplacian_iterations!" << endl;
		exit(0);
	}
	if (output_edges_frequency < 0) {
		cout << "Can't have a negative output_edges_frequency!" << endl;
		exit(0);
	}
	cout << "Begin Laplcian Smoothing..." << endl;

	vector<vector<int>> adjacent_vertice_indices;
	vector<double> laplacian_smoothed_position;

	adjacent_vertice_indices.resize(pre_smoothing_vertices.size());
	for (int q = 0; q < laplacian_iterations; q++) { //Repeat until the kernel radius of gaussian smoothing (r_gauss) reaches 0
		cout << "Laplcian smoothing iteration: [" << q << "]" << endl;

		Calculate_Face_Centres();
		if (output_edges_frequency != 0)
			Calculate_Edge_Centres(true, output_edges_frequency);
		else
			Calculate_Edge_Centres(false, 0);
		Calculate_Vertex_Area_Weights_for_Smoothing();
		Calculate_Smoothing_Vertice_Normals();
		vertices.resize(pre_smoothing_vertices.size()); //Size kept constant after first iteration, but moved here so vertices.empty() checks can be carried out in functions above

		for (int i = 0; i < pre_smoothing_vertices.size(); i++) { //For each vertex
			//Check if the vertex has adjacent points
			bool isolated_point = true;
			for (int j = 0; j < faces.size(); j++) { //For every face
				//Check there are 3 vertices to each face (ie: each face is a triangle)
				if (faces[j].size() != 3)
					cout << "Error in pre_smoothing_faces - face " << j << " (indexing starting at 0) is not a triangle!" << endl;
				for (int k = 0; k < faces[j].size(); k++) { //For every vertice in each face
					if (i == faces[j][k] - 1) { //If the vertex is within this face
						isolated_point = false; //Vertex is no longer isolated
						//Add the other two vertices onto adjacent_vertices, if they haven't been added already
						if (k == 0) { //If this is the first vertex
							bool new_1 = true, new_2 = true;
							for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
								if (adjacent_vertice_indices[i][m] == faces[j][1] - 1)
									new_1 = false; //Second vertex is not new
								else if (adjacent_vertice_indices[i][m] == faces[j][2] - 1)
									new_2 = false; //Third vertex is not new
								if (!new_1 && !new_2)
									break; //Break if both adjacent vertices are confirmed as not new
							}
							if (new_1)
								adjacent_vertice_indices[i].push_back(faces[j][1] - 1); //Add to list for adjacent vertices for this vertex
							if (new_2)
								adjacent_vertice_indices[i].push_back(faces[j][2] - 1); //Add to list for adjacent vertices for this vertex
						}
						if (k == 1) { //If this is the second vertex
							bool new_0 = true, new_2 = true;
							for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
								if (adjacent_vertice_indices[i][m] == faces[j][0] - 1)
									new_0 = false; //First vertex is not new
								else if (adjacent_vertice_indices[i][m] == faces[j][2] - 1)
									new_2 = false; //Third vertex is not new
								if (!new_0 && !new_2)
									break; //Break if both adjacent vertices are confirmed as not new
							}
							if (new_0)
								adjacent_vertice_indices[i].push_back(faces[j][0] - 1); //Add to list for adjacent vertices for this vertex
							if (new_2)
								adjacent_vertice_indices[i].push_back(faces[j][2] - 1); //Add to list for adjacent vertices for this vertex
						}
						if (k == 2) { //If this is the third vertex
							bool new_0 = true, new_1 = true;
							for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
								if (adjacent_vertice_indices[i][m] == faces[j][0] - 1)
									new_0 = false; //First vertex is not new
								else if (adjacent_vertice_indices[i][m] == faces[j][1] - 1)
									new_1 = false; //Second vertex is not new
								if (!new_0 && !new_1)
									break; //Break if both adjacent vertices are confirmed as not new
							}
							if (new_0)
								adjacent_vertice_indices[i].push_back(faces[j][0] - 1); //Add to list for adjacent vertices for this vertex
							if (new_1)
								adjacent_vertice_indices[i].push_back(faces[j][1] - 1); //Add to list for adjacent vertices for this vertex
						}
					}
				}
			}
			if (isolated_point) //If point is isolated, copy point to vertices without modification
				vertices[i].insert(begin(vertices[i]), begin(pre_smoothing_vertices[i]), end(pre_smoothing_vertices[i]));
			else { //If point isn't isolated (so can be smoothed)
				vector<double> weight_factors, average_distance_b_i, normal_vector;
				vector<vector<double>> adjacent_original_vertices_positions;
				adjacent_original_vertices_positions.resize(adjacent_vertice_indices[i].size());
				//Calculate average distance (b_i)
				for (int j = 0; j < adjacent_vertice_indices[i].size(); j++) {
					double temp_weight_factor;
					temp_weight_factor = Calculate_Weight_Factor(vertice_area_weights[adjacent_vertice_indices[i][j]], vertice_area_weights[i]);
					weight_factors.push_back(temp_weight_factor);
					adjacent_original_vertices_positions[j].insert(begin(adjacent_original_vertices_positions[j]), begin(pre_smoothing_vertices[adjacent_vertice_indices[i][j]]), end(pre_smoothing_vertices[adjacent_vertice_indices[i][j]]));
				}
				average_distance_b_i = Calculate_Average_Distance_Vector_for_Movement(weight_factors, adjacent_original_vertices_positions, pre_smoothing_vertices[i]);
				//Migrate positions using Laplacian smoothing (o_i to q_i)
				if (q == 0) //If this is the first time the point has been encountered, use pre_smoothing vertices[i]
					laplacian_smoothed_position = Laplacian_Smoothing(pre_smoothing_vertices[i], 0.1, average_distance_b_i, smoothing_vertice_normals[i]);
				else //If this isn't the first time the point has been encountered it has already been smoothed, with is postion stored in vertices[i]
					laplacian_smoothed_position = Laplacian_Smoothing(vertices[i], 0.1, average_distance_b_i, smoothing_vertice_normals[i]);
				//Copy smoothed point to vertices
				vertices[i].clear(); //Overwrite previous position (which was just storing posiiton after the last iteration of Laplacian smoothing)
				vertices[i].insert(begin(vertices[i]), begin(laplacian_smoothed_position), end(laplacian_smoothed_position));
			}
		}

		//Continuous output
		int gaussian_iterations = 0;
		Append_Continuous_Sphere_Curvature_Output_to_File(input_dataset_size, gaussian_iterations, q + 1, output_curvature_file, output_faces_frequency); //q + 1 so coutning iterations starts at 1

		//Write Vertices Post Smoothing to New File
		//Create filename
		string vertices_post_smooting_output_file = output_post_smoothing_vertices_filename_prefix;
		vertices_post_smooting_output_file += to_string(input_dataset_size);
		vertices_post_smooting_output_file += "_";
		vertices_post_smooting_output_file += to_string(gaussian_iterations);
		vertices_post_smooting_output_file += "_";
		vertices_post_smooting_output_file += to_string(q + 1);
		vertices_post_smooting_output_file += "_Smoothing.txt";
		//Output file
		Write_2D_Matrix_to_File(vertices, "Vertices Post Smoothing", vertices_post_smooting_output_file);
	}

	cout << "Finished Laplacian [" << laplacian_iterations << "] smoothing." << endl << endl;
}

void CCalculatingCurvature::Laplacian_Smoothing(int laplacian_iterations, int output_edges_frequency, int z_value, double z_step, double tolerance) {
	if (laplacian_iterations < 0) {
		cout << "Can't have a negative laplacian_iterations!" << endl;
		exit(0);
	}
	if (output_edges_frequency < 0) {
		cout << "Can't have a negative output_edges_frequency!" << endl;
		exit(0);
	}
	cout << "Begin Laplcian Smoothing..." << endl;

	vector<vector<int>> adjacent_vertice_indices;
	vector<double> laplacian_smoothed_position;

	adjacent_vertice_indices.resize(pre_smoothing_vertices.size());
	for (int q = 0; q < laplacian_iterations; q++) { //Repeat until the kernel radius of gaussian smoothing (r_gauss) reaches 0
		cout << "Laplcian smoothing iteration: [" << q << "]" << endl;

		Calculate_Face_Centres();
		if (output_edges_frequency != 0)
			Calculate_Edge_Centres(true, output_edges_frequency);
		else
			Calculate_Edge_Centres(false, 0);
		Calculate_Vertex_Area_Weights_for_Smoothing();
		Calculate_Smoothing_Vertice_Normals();
		vertices.resize(pre_smoothing_vertices.size()); //Size kept constant after first iteration, but moved here so vertices.empty() checks can be carried out in functions above

		for (int i = 0; i < pre_smoothing_vertices.size(); i++) { //For each vertex
			//Check if the vertex has adjacent points
			bool isolated_point = true;
			for (int j = 0; j < faces.size(); j++) { //For every face
				//Check there are 3 vertices to each face (ie: each face is a triangle)
				if (faces[j].size() != 3)
					cout << "Error in pre_smoothing_faces - face " << j << " (indexing starting at 0) is not a triangle!" << endl;
				for (int k = 0; k < faces[j].size(); k++) { //For every vertice in each face
					if (i == faces[j][k] - 1) { //If the vertex is within this face
						isolated_point = false; //Vertex is no longer isolated
						//Add the other two vertices onto adjacent_vertices, if they haven't been added already
						if (k == 0) { //If this is the first vertex
							bool new_1 = true, new_2 = true;
							for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
								if (adjacent_vertice_indices[i][m] == faces[j][1] - 1)
									new_1 = false; //Second vertex is not new
								else if (adjacent_vertice_indices[i][m] == faces[j][2] - 1)
									new_2 = false; //Third vertex is not new
								if (!new_1 && !new_2)
									break; //Break if both adjacent vertices are confirmed as not new
							}
							if (new_1)
								adjacent_vertice_indices[i].push_back(faces[j][1] - 1); //Add to list for adjacent vertices for this vertex
							if (new_2)
								adjacent_vertice_indices[i].push_back(faces[j][2] - 1); //Add to list for adjacent vertices for this vertex
						}
						if (k == 1) { //If this is the second vertex
							bool new_0 = true, new_2 = true;
							for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
								if (adjacent_vertice_indices[i][m] == faces[j][0] - 1)
									new_0 = false; //First vertex is not new
								else if (adjacent_vertice_indices[i][m] == faces[j][2] - 1)
									new_2 = false; //Third vertex is not new
								if (!new_0 && !new_2)
									break; //Break if both adjacent vertices are confirmed as not new
							}
							if (new_0)
								adjacent_vertice_indices[i].push_back(faces[j][0] - 1); //Add to list for adjacent vertices for this vertex
							if (new_2)
								adjacent_vertice_indices[i].push_back(faces[j][2] - 1); //Add to list for adjacent vertices for this vertex
						}
						if (k == 2) { //If this is the third vertex
							bool new_0 = true, new_1 = true;
							for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
								if (adjacent_vertice_indices[i][m] == faces[j][0] - 1)
									new_0 = false; //First vertex is not new
								else if (adjacent_vertice_indices[i][m] == faces[j][1] - 1)
									new_1 = false; //Second vertex is not new
								if (!new_0 && !new_1)
									break; //Break if both adjacent vertices are confirmed as not new
							}
							if (new_0)
								adjacent_vertice_indices[i].push_back(faces[j][0] - 1); //Add to list for adjacent vertices for this vertex
							if (new_1)
								adjacent_vertice_indices[i].push_back(faces[j][1] - 1); //Add to list for adjacent vertices for this vertex
						}
					}
				}
			}
			if (isolated_point) //If point is isolated, copy point to vertices without modification
				vertices[i].insert(begin(vertices[i]), begin(pre_smoothing_vertices[i]), end(pre_smoothing_vertices[i]));
			else { //If point isn't isolated (so can be smoothed)
				vector<double> weight_factors, average_distance_b_i, normal_vector;
				vector<vector<double>> adjacent_original_vertices_positions;
				adjacent_original_vertices_positions.resize(adjacent_vertice_indices[i].size());
				//Calculate average distance (b_i)
				for (int j = 0; j < adjacent_vertice_indices[i].size(); j++) {
					double temp_weight_factor;
					temp_weight_factor = Calculate_Weight_Factor(vertice_area_weights[adjacent_vertice_indices[i][j]], vertice_area_weights[i]);
					weight_factors.push_back(temp_weight_factor);
					adjacent_original_vertices_positions[j].insert(begin(adjacent_original_vertices_positions[j]), begin(pre_smoothing_vertices[adjacent_vertice_indices[i][j]]), end(pre_smoothing_vertices[adjacent_vertice_indices[i][j]]));
				}
				average_distance_b_i = Calculate_Average_Distance_Vector_for_Movement(weight_factors, adjacent_original_vertices_positions, pre_smoothing_vertices[i]);
				//Migrate positions using Laplacian smoothing (o_i to q_i)
				if (q == 0) //If this is the first time the point has been encountered, use pre_smoothing vertices[i]
					laplacian_smoothed_position = Laplacian_Smoothing(pre_smoothing_vertices[i], 0.1, average_distance_b_i, smoothing_vertice_normals[i]);
				else //If this isn't the first time the point has been encountered it has already been smoothed, with is postion stored in vertices[i]
					laplacian_smoothed_position = Laplacian_Smoothing(vertices[i], 0.1, average_distance_b_i, smoothing_vertice_normals[i]);
				//Copy smoothed point to vertices
				vertices[i].clear(); //Overwrite previous position (which was just storing posiiton after the last iteration of Laplacian smoothing)
				vertices[i].insert(begin(vertices[i]), begin(laplacian_smoothed_position), end(laplacian_smoothed_position));
			}
		}

		//Correct catenoid
		Correct_Layering_for_Catenoids_Post_Smoothing(z_value, z_step, tolerance);
	}

	cout << "Finished Laplacian [" << laplacian_iterations << "] smoothing." << endl << endl;
}

void CCalculatingCurvature::Laplacian_Smoothing(int laplacian_iterations, int output_edges_frequency, int z_value, double alpha, double z_step, double tolerance, string output_curvature_file, int output_faces_frequency, string output_post_smoothing_vertices_filename_prefix) {
	if (laplacian_iterations < 0) {
		cout << "Can't have a negative laplacian_iterations!" << endl;
		exit(0);
	}
	if (output_edges_frequency < 0) {
		cout << "Can't have a negative output_edges_frequency!" << endl;
		exit(0);
	}
	cout << "Begin Laplcian Smoothing..." << endl;

	vector<vector<int>> adjacent_vertice_indices;
	vector<double> laplacian_smoothed_position;

	adjacent_vertice_indices.resize(pre_smoothing_vertices.size());
	for (int q = 0; q < laplacian_iterations; q++) { //Repeat until the kernel radius of gaussian smoothing (r_gauss) reaches 0
		cout << "Laplcian smoothing iteration: [" << q << "]" << endl;

		Calculate_Face_Centres();
		if (output_edges_frequency != 0)
			Calculate_Edge_Centres(true, output_edges_frequency);
		else
			Calculate_Edge_Centres(false, 0);
		Calculate_Vertex_Area_Weights_for_Smoothing();
		Calculate_Smoothing_Vertice_Normals();
		vertices.resize(pre_smoothing_vertices.size()); //Size kept constant after first iteration, but moved here so vertices.empty() checks can be carried out in functions above

		for (int i = 0; i < pre_smoothing_vertices.size(); i++) { //For each vertex
			//Check if the vertex has adjacent points
			bool isolated_point = true;
			for (int j = 0; j < faces.size(); j++) { //For every face
				//Check there are 3 vertices to each face (ie: each face is a triangle)
				if (faces[j].size() != 3)
					cout << "Error in pre_smoothing_faces - face " << j << " (indexing starting at 0) is not a triangle!" << endl;
				for (int k = 0; k < faces[j].size(); k++) { //For every vertice in each face
					if (i == faces[j][k] - 1) { //If the vertex is within this face
						isolated_point = false; //Vertex is no longer isolated
						//Add the other two vertices onto adjacent_vertices, if they haven't been added already
						if (k == 0) { //If this is the first vertex
							bool new_1 = true, new_2 = true;
							for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
								if (adjacent_vertice_indices[i][m] == faces[j][1] - 1)
									new_1 = false; //Second vertex is not new
								else if (adjacent_vertice_indices[i][m] == faces[j][2] - 1)
									new_2 = false; //Third vertex is not new
								if (!new_1 && !new_2)
									break; //Break if both adjacent vertices are confirmed as not new
							}
							if (new_1)
								adjacent_vertice_indices[i].push_back(faces[j][1] - 1); //Add to list for adjacent vertices for this vertex
							if (new_2)
								adjacent_vertice_indices[i].push_back(faces[j][2] - 1); //Add to list for adjacent vertices for this vertex
						}
						if (k == 1) { //If this is the second vertex
							bool new_0 = true, new_2 = true;
							for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
								if (adjacent_vertice_indices[i][m] == faces[j][0] - 1)
									new_0 = false; //First vertex is not new
								else if (adjacent_vertice_indices[i][m] == faces[j][2] - 1)
									new_2 = false; //Third vertex is not new
								if (!new_0 && !new_2)
									break; //Break if both adjacent vertices are confirmed as not new
							}
							if (new_0)
								adjacent_vertice_indices[i].push_back(faces[j][0] - 1); //Add to list for adjacent vertices for this vertex
							if (new_2)
								adjacent_vertice_indices[i].push_back(faces[j][2] - 1); //Add to list for adjacent vertices for this vertex
						}
						if (k == 2) { //If this is the third vertex
							bool new_0 = true, new_1 = true;
							for (int m = 0; m < adjacent_vertice_indices[i].size(); m++) { //For each of the currently stored adjacent vertices
								if (adjacent_vertice_indices[i][m] == faces[j][0] - 1)
									new_0 = false; //First vertex is not new
								else if (adjacent_vertice_indices[i][m] == faces[j][1] - 1)
									new_1 = false; //Second vertex is not new
								if (!new_0 && !new_1)
									break; //Break if both adjacent vertices are confirmed as not new
							}
							if (new_0)
								adjacent_vertice_indices[i].push_back(faces[j][0] - 1); //Add to list for adjacent vertices for this vertex
							if (new_1)
								adjacent_vertice_indices[i].push_back(faces[j][1] - 1); //Add to list for adjacent vertices for this vertex
						}
					}
				}
			}
			if (isolated_point) //If point is isolated, copy point to vertices without modification
				vertices[i].insert(begin(vertices[i]), begin(pre_smoothing_vertices[i]), end(pre_smoothing_vertices[i]));
			else { //If point isn't isolated (so can be smoothed)
				vector<double> weight_factors, average_distance_b_i, normal_vector;
				vector<vector<double>> adjacent_original_vertices_positions;
				adjacent_original_vertices_positions.resize(adjacent_vertice_indices[i].size());
				//Calculate average distance (b_i)
				for (int j = 0; j < adjacent_vertice_indices[i].size(); j++) {
					double temp_weight_factor;
					temp_weight_factor = Calculate_Weight_Factor(vertice_area_weights[adjacent_vertice_indices[i][j]], vertice_area_weights[i]);
					weight_factors.push_back(temp_weight_factor);
					adjacent_original_vertices_positions[j].insert(begin(adjacent_original_vertices_positions[j]), begin(pre_smoothing_vertices[adjacent_vertice_indices[i][j]]), end(pre_smoothing_vertices[adjacent_vertice_indices[i][j]]));
				}
				average_distance_b_i = Calculate_Average_Distance_Vector_for_Movement(weight_factors, adjacent_original_vertices_positions, pre_smoothing_vertices[i]);
				//Migrate positions using Laplacian smoothing (o_i to q_i)
				if (q == 0) //If this is the first time the point has been encountered, use pre_smoothing vertices[i]
					laplacian_smoothed_position = Laplacian_Smoothing(pre_smoothing_vertices[i], 0.1, average_distance_b_i, smoothing_vertice_normals[i]);
				else //If this isn't the first time the point has been encountered it has already been smoothed, with is postion stored in vertices[i]
					laplacian_smoothed_position = Laplacian_Smoothing(vertices[i], 0.1, average_distance_b_i, smoothing_vertice_normals[i]);
				//Copy smoothed point to vertices
				vertices[i].clear(); //Overwrite previous position (which was just storing posiiton after the last iteration of Laplacian smoothing)
				vertices[i].insert(begin(vertices[i]), begin(laplacian_smoothed_position), end(laplacian_smoothed_position));
			}
		}

		//Correct catenoid
		Correct_Layering_for_Catenoids_Post_Smoothing(z_value, z_step, tolerance);

		//Continuous output
		int gaussian_iterations = 0;
		Append_Continuous_Catneoid_Curvature_Output_to_File(z_value, alpha, z_step, gaussian_iterations, q + 1, output_curvature_file, output_faces_frequency); //q + 1 so coutning iterations starts at 1

		//Write Vertices Post Smoothing to New File
		//Create filename
		string vertices_post_smooting_output_file = output_post_smoothing_vertices_filename_prefix;
		vertices_post_smooting_output_file += to_string(z_value);
		vertices_post_smooting_output_file += "_";
		vertices_post_smooting_output_file += to_string(alpha);
		vertices_post_smooting_output_file.erase(vertices_post_smooting_output_file.find_last_not_of('0') + 1, string::npos); //Remove trailing zeroes
		vertices_post_smooting_output_file.erase(vertices_post_smooting_output_file.find_last_not_of('.') + 1, string::npos); //Remove final '.' if alpha is actually an integer
		vertices_post_smooting_output_file += "_";
		vertices_post_smooting_output_file += to_string(z_step);
		vertices_post_smooting_output_file.erase(vertices_post_smooting_output_file.find_last_not_of('0') + 1, string::npos); //Remove trailing zeroes
		vertices_post_smooting_output_file.erase(vertices_post_smooting_output_file.find_last_not_of('.') + 1, string::npos); //Remove final '.' if z_step is actually an integer
		vertices_post_smooting_output_file += "_";
		vertices_post_smooting_output_file += to_string(gaussian_iterations);
		vertices_post_smooting_output_file += "_";
		vertices_post_smooting_output_file += to_string(q + 1);
		vertices_post_smooting_output_file += "_Smoothing.txt";
		//Output file
		Write_2D_Matrix_to_File(vertices, "Vertices Post Smoothing", vertices_post_smooting_output_file);
	}

	cout << "Finished Laplacian [" << laplacian_iterations << "] smoothing." << endl << endl;
}

void CCalculatingCurvature::Correct_Layering_for_Catenoids_Post_Smoothing(int z_value, double z_step, double tolerance) {
	cout << "Begin correcting layers for catenoids post smoothing..." << endl;
	//Calculate top layer
	int num_layers = ((z_value / z_step) * 2) + 1 + 2;
	int top_limit = num_layers - 2; //-1 to start indexing layers from 0, and -1 to ensure that the 2nd from top isn't used (so correct coordinates to bottom of this layer)
	//Correct vertices
	int corrected_vertices_counter = 0;
	for (int i = 0; i < vertices.size(); i++) {
		if (vertices[i][2] < 1.0) {
			vertices[i][2] = 1.0;
			corrected_vertices_counter++;
		}
		else if (vertices[i][2] < (1.0 + tolerance)) {
			vertices[i][2] = 1.0;
			corrected_vertices_counter++;
		}
		else if (vertices[i][2] > (top_limit - tolerance)) {
			vertices[i][2] = top_limit;
			corrected_vertices_counter++;
		}
		else if (vertices[i][2] > top_limit) {
			vertices[i][2] = top_limit;
			corrected_vertices_counter++;
		}
	}
	cout << "Post smoothing catenoid correction completed - " << corrected_vertices_counter << " vertices corrected successfully." << endl;
}