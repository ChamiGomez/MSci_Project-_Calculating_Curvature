#include "C_Calc_Curvature_Post_Project.h"
#include <cmath>
#include <iostream>
#include <algorithm> //includes min_element() and max_element()

vector<int> CCalculatingCurvature::Generate_Sphere_Pixel_Values(int dataset_size, int voxel_size, int gap_size) {
	cout << "Sythetic spherical datset generation beginning..." << endl;
	//Find location of centre and radius
	double centre_coordinate, radius;
	centre_coordinate = dataset_size / 2; //Integer maths, effectively is /2 - 0.5 for odd numbered dataset_sizes
	radius = dataset_size - centre_coordinate - (2 * gap_size);

	//Loop over every point in the dataset to find if it lies within the sphere or not
	pixel_values.clear(); //Shouldn't be necessary as this will be called at the beginning of main but just in case
	pixel_values.resize(dataset_size);
	for (int i = 0; i < dataset_size; i++) {
		pixel_values[i].resize(dataset_size);
		for (int j = 0; j < dataset_size; j++) {
			pixel_values[i][j].resize(dataset_size);
			for (int k = 0; k < dataset_size; k++) {
				double v, edge_value;
				v = pow((i - centre_coordinate), 2) + pow((j - centre_coordinate), 2) + pow((k - centre_coordinate), 2);
				edge_value = pow((radius / voxel_size), 2);
				if (v <= edge_value)
					pixel_values[i][j][k] = 1;
				else
					pixel_values[i][j][k] = 0;
			}
		}
	}

	cout << "Synthetic spherical surface generation completed!" << endl;

	vector<int> dataset_sizes;
	for (int i = 0; i < 3; i++)
		dataset_sizes.push_back(dataset_size);
	return dataset_sizes;
}

/* Old System
vector<vector<double>> CCalculatingCurvature::Generate_Catenoid_Coordinates(double lower_u, double upper_u, double lower_v, double upper_v, double alpha, double step_size) {
	cout << "Begin generating catenoid coordinates..." << endl;

	//Check values
	if (abs(lower_u) + abs(lower_v) > (2 * M_PI)) { //Note: Should be >= but allowed to facilitate plotting to limit not just before limit
		cout << "Invalid parameters passed for u! Difference between lower and upper limits must be no more than 2*PI, but are actually " << abs(lower_u) + abs(upper_u) << endl;
		exit(0);
	}
	if (lower_u >= upper_u) {
		cout << "Lower_u must be < upper_u! Values passed were lower_u: " << lower_u << " and upper_u: " << upper_u << endl;
		exit(0);
	}
	if (lower_v >= upper_v) {
		cout << "Lower_v must be < upper_v! Values passed were lower_v: " << lower_v << " and upper_v: " << upper_v << endl;
		exit(0);
	}
	if (alpha == 0.0) {
		cout << "Invalid parameter passed for alpha! This constant must be non-zero." << endl;
		exit(0);
	}

	vector<vector<double>> catenoid_coordinates;
	vector<double> catenoid_point;
	for (double i = lower_u; i < upper_u + step_size; i = i + step_size) {
		for (double j = lower_v; j < upper_v + step_size; j = j + step_size) {
			//cout << "[" << i << ", " << j << "]\t";
			catenoid_point = Calculate_Point_on_Catenoid(i, j, alpha);
			catenoid_coordinates.resize(catenoid_coordinates.size() + 1);
			catenoid_coordinates[catenoid_coordinates.size() - 1].insert(begin(catenoid_coordinates[catenoid_coordinates.size() - 1]), begin(catenoid_point), end(catenoid_point));
		}
	}

	Write_2D_Matrix_to_Screen<double>(catenoid_coordinates);
	//Write_2D_Matrix_to_File<double>(catenoid_coordinates, "Catenoid Coordinates", "Catenoid_Coordinates_Test.txt");

	cout << "Finished generating catenoid coordinates." << endl;
	return catenoid_coordinates;
}

vector<double> CCalculatingCurvature::Calculate_Point_on_Catenoid(double u, double v, double alpha) {
	double x, y, z;
	x = alpha * cosh(v / alpha) * cos(u);
	y = alpha * cosh(v / alpha) * sin(u);
	z = v;

	vector<double> ans_v;
	ans_v.push_back(x);
	ans_v.push_back(y);
	ans_v.push_back(z);
	return ans_v;
}

vector<int> CCalculatingCurvature::Generate_Catenoid_Pixel_Values(vector<vector<double>> catenoid_coordinates, int decimal_precision, double alpha) {
	cout << "Begin generating catenoid pixel values..." << endl;

	int scale_factor;
	scale_factor = pow(10, decimal_precision);
	cout << "Scale factor for catenoid pixel value generation: " << scale_factor << endl;

	vector<vector<int>> scaled_coordinates;
	scaled_coordinates.resize(catenoid_coordinates.size());
	int min_x = 0, max_x = 0, min_y = 0, max_y = 0, min_z = 0, max_z = 0;
	vector<int> scaled_z_values;

	if (catenoid_coordinates[0].size() != 3) {
		cout << "Error in size of catenoid_coordinates argument! Size should be 3 for (x, y, z) but is actually " << catenoid_coordinates[0].size() << endl;
		exit(0);
	}

	for (int i = 0; i < catenoid_coordinates.size(); i++) {
		scaled_coordinates[i].resize(catenoid_coordinates[i].size());
		for (int j = 0; j < catenoid_coordinates[i].size(); j++) {
			int scaled_value;
			scaled_value = (round(catenoid_coordinates[i][j] * scale_factor));
			scaled_coordinates[i][j] = scaled_value;
			if (i == 0 && j != 2)
				continue;
			if (j == 2) {
				if (i == 0) {
					min_x = scaled_coordinates[0][0];
					max_x = scaled_coordinates[0][0];
					min_y = scaled_coordinates[0][1];
					max_y = scaled_coordinates[0][1];
					min_z = scaled_coordinates[0][2];
					max_z = scaled_coordinates[0][2];
					scaled_z_values.push_back(scaled_coordinates[0][2]);
					continue;
				}
				if (scaled_coordinates[i][0] < min_x)
					min_x = scaled_coordinates[i][0];
				else if (scaled_coordinates[i][0] > max_x)
					max_x = scaled_coordinates[i][0];
				if (scaled_coordinates[i][1] < min_y)
					min_y = scaled_coordinates[i][1];
				else if (scaled_coordinates[i][1] > max_y)
					max_y = scaled_coordinates[i][1];
				if (scaled_coordinates[i][2] < min_z)
					min_z = scaled_coordinates[i][2];
				else if (scaled_coordinates[i][2] > max_z)
					max_z = scaled_coordinates[i][2];
				bool new_z = true;
				for (int p = 0; p < scaled_z_values.size(); p++) {
					if (scaled_z_values[p] == scaled_coordinates[i][2]) {
						new_z = false;
						break;
					}
				}
				if (new_z)
					scaled_z_values.push_back(scaled_coordinates[i][2]);
			}
		}
	}

	cout << endl;
	Write_2D_Matrix_to_Screen<int>(scaled_coordinates);
	Write_List_to_Screen<int>(scaled_z_values);

	vector<int> dataset_size;
	dataset_size.push_back(scaled_z_values.size());
	dataset_size.push_back(max_y - min_y + 3);
	dataset_size.push_back(max_x - min_x + 3);

	cout << "Dataset size: ";
	Write_List_to_Screen<int>(dataset_size);

	vector<vector<int>> shifted_scaled_coordinates;
	shifted_scaled_coordinates.resize(scaled_coordinates.size());
	for (int i = 0; i < scaled_coordinates.size(); i++) {
		shifted_scaled_coordinates[i].resize(scaled_coordinates[i].size());
		for (int p = 0; p < scaled_z_values.size(); p++) {
			if (scaled_coordinates[i][2] == scaled_z_values[p]) {
				shifted_scaled_coordinates[i][2] = p; //Don't start from 1 so top and bottom of catenoid aren't in the pixel_values
				break;
			}
		}
	}
	if (min_x < 0 || min_y < 0) {
		for (int i = 0; i < scaled_coordinates.size(); i++) {
			if (min_x < 0)
				shifted_scaled_coordinates[i][0] = scaled_coordinates[i][0] + abs(min_x) + 1; //Start from 1 to allow for gap around edges of 1 pixel
			else
				shifted_scaled_coordinates[i][0] = scaled_coordinates[i][0];
			if (min_y < 0)
				shifted_scaled_coordinates[i][1] = scaled_coordinates[i][1] + abs(min_y) + 1; //Start from 1 to allow for gap around edges of 1 pixel
			else
				shifted_scaled_coordinates[i][1] = scaled_coordinates[i][1];
		}
	}

	Write_2D_Matrix_to_Screen<int>(shifted_scaled_coordinates);

	pixel_values.clear(); //Shouldn't be necessary as this will be called at the beginning of main but just in case
	//Note: values passed as pixels[z][y][x] as they are aread in form a TIFF file in this format
	pixel_values.resize(dataset_size[0]);
	for (int k = 0; k < dataset_size[0]; k++) {
		pixel_values[k].resize(dataset_size[1]);
		for (int i = 0; i < dataset_size[1]; i++) {
			pixel_values[k][i].resize(dataset_size[2]);
			for (int j = 0; j < dataset_size[2]; j++) {
				int pixel_value;
				pixel_value = Calculate_if_Point_Lies_within_Catenoid(j, i, k, alpha);
				pixel_values[k][i][j] = pixel_value;
			}
		}
	}

	Write_3D_Matrix_to_File<int>(pixel_values, "Pixel Values", "Catenoid_Pixel_Values_Test.txt", false, true);
	return dataset_size;
}*/

vector<int> CCalculatingCurvature::Generate_Catenoid_Pixel_Values(int x_size, int y_size, int z_value, double z_step, double alpha) {
	cout << "Begin generating catenoid coordinates..." << endl;

	//Check values
	if (alpha == 0.0) {
		cout << "Alpha must be a non-zero constant" << endl;
		exit(0);
	}

	//Fixes byte size arithmetic warning
	double centre_x, centre_y, centre_z;
	centre_x = (x_size / 2.0) - 0.5;
	centre_y = (y_size / 2.0) - 0.5;
	centre_z = 0;

	pixel_values.clear(); //Shouldn't be necessary as this will be called at the beginning of main but just in case
	//Note: values passed as pixels[z][y][x] as they are aread in form a TIFF file in this format
	int num_layers = ((z_value / z_step) * 2) + 1;
	pixel_values.resize(num_layers);
	for (double k = -z_value; k < (z_value + (z_step * 0.1)); k += z_step) {
		double z_val;
		z_val = (k + z_value) * (1 / z_step);
		//cout << k << "\t" << round(z_val) << "\n";
		pixel_values[round(z_val)].resize(y_size);
		for (int j = 0; j < y_size; j++) {
			pixel_values[round(z_val)][j].resize(x_size);
			for (int i = 0; i < x_size; i++) {
				int pixel_value;
				pixel_value = Calculate_if_Point_Lies_within_Catenoid(i - centre_x, j - centre_y, k - centre_z, alpha); //Note: passed as (x, y, z, alpha)
				pixel_values[round(z_val)][j][i] = pixel_value;
			}
		}
	}

	//Old check, now should never be triggered
	for (int i = 0; i < pixel_values.size(); i++) {
		if (pixel_values[i].empty()) {
			pixel_values.erase(begin(pixel_values) + i);
			cout << "Deleted " << i << "th element from pixel values (remember that indexing starts at 0)" << endl;
			cout << "This is an old check - this should no longer be triggered!" << endl;
			exit(0);
		}
	}

	vector<int> datset_size;
	datset_size.push_back(pixel_values.size());
	datset_size.push_back(y_size);
	datset_size.push_back(x_size);
	return datset_size;
}

//Returns size of dataset (z, y, and x) as a vector, with top and bottom layer
vector<int> CCalculatingCurvature::Generate_Catenoid_Pixel_Values_with_Edges(int x_size, int y_size, int z_value, double z_step, double alpha) {
	cout << "Begin generating catenoid coordinates..." << endl;

	//Check values
	if (alpha == 0.0) {
		cout << "Alpha must be a non-zero constant" << endl;
		exit(0);
	}

	//Fixes byte size arithmetic warning
	double centre_x, centre_y, centre_z;
	centre_x = (x_size / 2.0) - 0.5;
	centre_y = (y_size / 2.0) - 0.5;
	centre_z = 0;

	pixel_values.clear(); //Shouldn't be necessary as this will be called at the beginning of main but just in case
	//Note: values passed as pixels[z][y][x] as they are aread in form a TIFF file in this format
	int num_layers = ((z_value / z_step) * 2) + 1;
	pixel_values.resize(num_layers + 2); //With edges
	pixel_values[0].resize(y_size);
	pixel_values[pixel_values.size() - 1].resize(y_size);
	for (double k = -z_value - z_step; k < (z_value + z_step + (z_step * 0.1)); k += z_step) {
		if (round(k * (1.0 / z_step)) == round((z_value + z_step) * (1.0 / z_step))) //Skip final layer because pixel_values has already been filled in with zeroes for this layer
			continue;
		double z_val;
		z_val = (k + z_value + z_step) * (1 / z_step);
		pixel_values[round(z_val)].resize(y_size);
		for (int j = 0; j < y_size; j++) {
			pixel_values[round(z_val)][j].resize(x_size);
			if (k == -z_value - z_step) {
				pixel_values[0][j].resize(x_size);
				pixel_values[pixel_values.size() - 1][j].resize(x_size);
			}
			for (int i = 0; i < x_size; i++) {
				//if (k == -z_value - z_step || round(k*(1/z_step)) == round(z_value + z_step*(1/z_step))) {//If this is the top or bottom layer, fill with zeroes
				if (k == -z_value - z_step) { //If this is the first time this loop is reach
					//Fill the first and last layers with zeroes
					pixel_values[0][j][i] = 0;
					pixel_values[pixel_values.size() - 1][j][i] = 0;
				}
				else { //Normal generation of catenoid values for every layer except the top and bottom layers
					int pixel_value;
					pixel_value = Calculate_if_Point_Lies_within_Catenoid(i - centre_x, j - centre_y, k - centre_z, alpha); //Note: passed as (x, y, z, alpha)
					pixel_values[round(z_val)][j][i] = pixel_value;
				}
			}
		}
	}

	//Old check, now should never be triggered
	for (int i = 0; i < pixel_values.size(); i++) {
		if (pixel_values[i].empty()) {
			pixel_values.erase(begin(pixel_values) + i);
			cout << "Deleted " << i << "th element from pixel values (remember that indexing starts at 0)" << endl;
			cout << "This is an old check - this should no longer be triggered!" << endl;
			exit(0);
		}
	}

	vector<int> datset_size;
	datset_size.push_back(pixel_values.size());
	datset_size.push_back(y_size);
	datset_size.push_back(x_size);
	return datset_size;
}

int CCalculatingCurvature::Calculate_if_Point_Lies_within_Catenoid(double x, double y, double z, double alpha) {
	double x_squared, y_squared, r_squared;
	x_squared = pow(x, 2);
	y_squared = pow(y, 2);
	r_squared = pow(alpha, 2) * pow(cosh(z / alpha), 2);

	if (x_squared + y_squared < r_squared)
		return 2;
	else if (x_squared + y_squared == r_squared)
		return 1;
	else if (x_squared + y_squared > r_squared)
		return 0;
}

vector<int> CCalculatingCurvature::Generate_Catenoid_Pixel_Values_New(int z_max, double alpha, double voxel_size) {
	cout << "Begin generating catenoid coordinates..." << endl;

	//Check values
	if (alpha == 0.0) {
		cout << "Alpha must be a non-zero constant" << endl;
		exit(0);
	}

	//Calculate z_values_double
	vector<int> z_values_int;
	vector<double> z_values_double;
	int scaled_z_max, counter = 0;
	scaled_z_max = z_max * (1.0 / voxel_size);
	for (int i = -scaled_z_max; i < (scaled_z_max + 0.5); i++) {
		z_values_int.push_back(i);
		z_values_double.push_back(z_values_int[counter] * voxel_size);
		counter++;
	}

	//Calculate x and y values
	double r_squared_max;
	int r_max, x_and_y_total;
	r_squared_max = pow(alpha, 2) * pow(cosh(z_max / alpha), 2);
	r_max = int(sqrt(r_squared_max)) + 1; //Round down (remove decimal remainder) and then add 1 to include this ie: always round up
	x_and_y_total = (r_max * 2.0) + 1.0;

	//Calculate centre coordinates
	double centre_x, centre_y, centre_z;
	centre_x = (x_and_y_total / 2.0) - 0.5;
	centre_y = (x_and_y_total / 2.0) - 0.5;
	centre_z = 0;

	//Assign pixel values
	pixel_values.clear(); //Shouldn't be necessary as this will be called at the beginning of main but just in case
	//Note: values passed as pixels[z][y][x] as they are aread in form a TIFF file in this format
	pixel_values.resize((scaled_z_max * 2) + 1 + 2); //Resize with edges
	pixel_values[0].resize(x_and_y_total); //Resize bottom layer
	pixel_values[pixel_values.size() - 1].resize(x_and_y_total); //Resize top layer
	for (int k = 0; k < z_values_double.size() + 1; k++) { //For every layer
		pixel_values[k].resize(x_and_y_total); //Resize middle layers
		for (int j = 0; j < x_and_y_total; j++) { //For each row
			pixel_values[k][j].resize(x_and_y_total); //Resize each row
			if (k == 0) { //For the first time rows are iterated over, resize top and bottom layers' rows as well
				pixel_values[0][j].resize(x_and_y_total);
				pixel_values[pixel_values.size() - 1][j].resize(x_and_y_total);
			}
			for (int i = 0; i < x_and_y_total; i++) { //For each value (for each column)
				if (k == 0) { //If this is for the first middle layer, also allocated top and bottom layer
					pixel_values[0][j][i] = 0;
					pixel_values[pixel_values.size() - 1][j][i] = 0;
				}
				else { //Calculate pixel values for middle layers
					int pixel_value;
					pixel_value = Calculate_if_Point_Lies_within_Catenoid(i - centre_x, j - centre_y, z_values_double[k - 1] - centre_z, alpha); //Note: passed as (x, y, z, alpha)
					pixel_values[k][j][i] = pixel_value;
				}
			}
		}
	}

	//Old check, now should never be triggered
	for (int i = 0; i < pixel_values.size(); i++) {
		if (pixel_values[i].empty()) {
			pixel_values.erase(begin(pixel_values) + i);
			cout << "Deleted " << i << "th element from pixel values (remember that indexing starts at 0)" << endl;
			cout << "This is an old check - this should no longer be triggered!" << endl;
			exit(0);
		}
	}

	vector<int> dataset_sizes;
	dataset_sizes.push_back(pixel_values.size());
	dataset_sizes.push_back(x_and_y_total);
	dataset_sizes.push_back(x_and_y_total);
	return dataset_sizes;
}

vector<int> CCalculatingCurvature::Generate_Catenoid_Pixel_Values_New_2(int z_max, double alpha, double voxel_size) {
	cout << "Begin generating catenoid coordinates..." << endl;

	//Check values
	if (alpha == 0.0) {
		cout << "Alpha must be a non-zero constant" << endl;
		exit(0);
	}

	//Calculate z_values_double
	vector<int> z_values_int;
	vector<double> z_values_double;
	int scaled_z_max, counter = 0;
	scaled_z_max = z_max * (1.0 / voxel_size);
	for (int i = -scaled_z_max; i < (scaled_z_max + 0.5); i++) {
		z_values_int.push_back(i);
		z_values_double.push_back(z_values_int[counter] * voxel_size);
		counter++;
	}

	//Calculate x and y values
	double r_squared_max, x_and_y_min;
	int r_scaled_max, x_and_y_total;
	r_squared_max = pow(alpha, 2) * pow(cosh(z_max / alpha), 2);
	r_scaled_max = int(sqrt(r_squared_max) * (1.0 / voxel_size)) + 1; //Round down (remove decimal remainder) and then add 1 to include this ie: always round up
	x_and_y_total = (r_scaled_max * 2.0) + 1.0;
	x_and_y_min = -(sqrt(r_squared_max));
	x_and_y_min = floor(-(sqrt(r_squared_max)) * (1.0 / voxel_size)) * voxel_size;
	vector<double> x_values, y_values;
	for (int i = 0; i < x_and_y_total; i++) {
		x_values.push_back(x_and_y_min + (i * voxel_size));
		y_values.push_back(x_and_y_min + (i * voxel_size));
	}

	//Assign pixel values
	pixel_values.clear(); //Shouldn't be necessary as this will be called at the beginning of main but just in case
	//Note: values passed as pixels[z][y][x] as they are aread in form a TIFF file in this format
	pixel_values.resize((scaled_z_max * 2) + 1 + 2); //Resize with edges
	pixel_values[0].resize(y_values.size()); //Resize bottom layer
	pixel_values[pixel_values.size() - 1].resize(y_values.size()); //Resize top layer
	for (int k = 0; k < z_values_double.size() + 1; k++) { //For every layer
		pixel_values[k].resize(y_values.size()); //Resize middle layers
		for (int j = 0; j < y_values.size(); j++) { //For each row
			pixel_values[k][j].resize(x_values.size()); //Resize each row
			if (k == 0) { //For the first time rows are iterated over, resize top and bottom layers' rows as well
				pixel_values[0][j].resize(x_values.size());
				pixel_values[pixel_values.size() - 1][j].resize(x_values.size());
			}
			for (int i = 0; i < x_values.size(); i++) { //For each value (for each column)
				if (k == 0) { //If this is for the first middle layer, also allocated top and bottom layer
					pixel_values[0][j][i] = 0;
					pixel_values[pixel_values.size() - 1][j][i] = 0;
				}
				else { //Calculate pixel values for middle layers
					int pixel_value;
					pixel_value = Calculate_if_Point_Lies_within_Catenoid(x_values[i], y_values[j], z_values_double[k - 1], alpha); //Note: passed as (x, y, z, alpha)
					pixel_values[k][j][i] = pixel_value;
				}
			}
		}
	}

	//Old check, now should never be triggered
	for (int i = 0; i < pixel_values.size(); i++) {
		if (pixel_values[i].empty()) {
			pixel_values.erase(begin(pixel_values) + i);
			cout << "Deleted " << i << "th element from pixel values (remember that indexing starts at 0)" << endl;
			cout << "This is an old check - this should no longer be triggered!" << endl;
			exit(0);
		}
	}

	vector<int> dataset_sizes;
	dataset_sizes.push_back(pixel_values.size());
	dataset_sizes.push_back(x_and_y_total);
	dataset_sizes.push_back(x_and_y_total);
	return dataset_sizes;
}

vector<int> CCalculatingCurvature::Generate_Catenoid_Pixel_Values_New_3(int z_max, double alpha, double voxel_size) {
	cout << "Begin generating catenoid coordinates..." << endl;

	//Check values
	if (alpha == 0.0) {
		cout << "Alpha must be a non-zero constant" << endl;
		exit(0);
	}
	if (z_max <= 0) {
		cout << "Z_Max must be positive" << endl;
		exit(0);
	}
	if (voxel_size <= 0) {
		cout << "Voxel size must be positive" << endl;
		exit(0);
	}

	//Calculate z_values_double
	vector<int> z_values_int;
	vector<double> z_values_double;
	int scaled_z_max, counter = 0;
	scaled_z_max = z_max * (1.0 / voxel_size);
	for (int i = -scaled_z_max; i < (scaled_z_max + 0.5); i++) {
		z_values_int.push_back(i);
		z_values_double.push_back(z_values_int[counter] * voxel_size);
		counter++;
	}

	//Calculate x and y values
	double r_squared_max, x_and_y_min;
	int r_scaled_max, x_and_y_total;
	r_squared_max = pow(alpha, 2) * pow(cosh(z_max / alpha), 2);
	r_scaled_max = int(sqrt(r_squared_max) * (1.0 / voxel_size)) + 1; //Round down (remove decimal remainder) and then add 1 to include this ie: always round up
	x_and_y_total = (r_scaled_max * 2.0) + 1.0;
	x_and_y_min = -(sqrt(r_squared_max));
	x_and_y_min = floor(-(sqrt(r_squared_max)) * (1.0 / voxel_size)) * voxel_size;
	vector<double> x_values, y_values;
	for (int i = 0; i < x_and_y_total; i++) {
		x_values.push_back(x_and_y_min + (i * voxel_size));
		y_values.push_back(x_and_y_min + (i * voxel_size));
	}

	//Assign pixel values
	pixel_values.clear(); //Shouldn't be necessary as this will be called at the beginning of main but just in case
	//Note: values passed as pixels[z][y][x] as they are aread in from a TIFF file in this format
	pixel_values.resize((scaled_z_max * 2) + 1 + 2); //Resize with edges
	pixel_values[0].resize(y_values.size()); //Resize bottom layer
	pixel_values[pixel_values.size() - 1].resize(y_values.size()); //Resize top layer
	for (int k = 0; k < z_values_double.size() + 1; k++) { //For every layer
		pixel_values[k].resize(y_values.size()); //Resize middle layers
		for (int j = 0; j < y_values.size(); j++) { //For each row
			pixel_values[k][j].resize(x_values.size()); //Resize each row
			if (k == 0) { //For the first time rows are iterated over, resize top and bottom layers' rows as well
				pixel_values[0][j].resize(x_values.size());
				pixel_values[pixel_values.size() - 1][j].resize(x_values.size());
			}
			for (int i = 0; i < x_values.size(); i++) { //For each value (for each column)
				if (k == 0) { //If this is for the first middle layer, also allocated top and bottom layer
					pixel_values[0][j][i] = 0;
					pixel_values[pixel_values.size() - 1][j][i] = 0;
				}
				else { //Calculate pixel values for middle layers
					int pixel_value;
					pixel_value = Calculate_if_Point_Lies_within_Catenoid(x_values[i], y_values[j], z_values_double[k - 1], alpha); //Note: passed as (x, y, z, alpha)
					pixel_values[k][j][i] = pixel_value;
				}
			}
		}
	}

	//Old check, now should never be triggered
	for (int i = 0; i < pixel_values.size(); i++) {
		if (pixel_values[i].empty()) {
			pixel_values.erase(begin(pixel_values) + i);
			cout << "Deleted " << i << "th element from pixel values (remember that indexing starts at 0)" << endl;
			cout << "This is an old check - this should no longer be triggered!" << endl;
			exit(0);
		}
	}

	vector<int> dataset_sizes;
	dataset_sizes.push_back(pixel_values.size());
	dataset_sizes.push_back(x_and_y_total);
	dataset_sizes.push_back(x_and_y_total);
	return dataset_sizes;
}