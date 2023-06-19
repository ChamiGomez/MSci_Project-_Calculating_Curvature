#include "C_Calc_Curvature_Post_Project.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm> //Includes sort()

void CCalculatingCurvature::Normalise_Gaussian_Curvature_of_Sphere(double radius) {
	cout << "Begin normalising gaussian curvature of sphere..." << endl;

	double radius_squared;
	radius_squared = pow(radius, 2);

	for (int i = 0; i < gaussian_curvature.size(); i++)
		normalised_gaussian_curvature.push_back(gaussian_curvature[i] * radius_squared);

	cout << "Finished normalising gaussian curvature of sphere" << endl;
}

template
void CCalculatingCurvature::Create_Histogram<int>(vector<int> list, double error_bar, string output_file, bool output_histogram_properties);
template
void CCalculatingCurvature::Create_Histogram<double>(vector<double> list, double error_bar, string output_file, bool output_histogram_properties);
template <typename data_type>
void CCalculatingCurvature::Create_Histogram(vector<data_type> list, double error_bar, string output_file, bool output_histogram_properties) {
	cout << "Begin calculating histogram..." << endl;

	sort(begin(list), end(list));

	vector<vector<double>> histogram;
	bool unique = true;
	double max_error_bar_used = 0.0;
	int exact_value_repeated_counter = 0, error_bar_used_counter = 0;
	for (int i = 0; i < list.size(); i++) {
		if (histogram.empty()) {
			histogram.resize(1);
			histogram[0].push_back(list[i]);
			histogram[0].push_back(1);
			continue; ///take out?
		}
		else {
			for (int j = 0; j < histogram.size(); j++) {
				if (list[i] == histogram[j][0]) {
					exact_value_repeated_counter++;
					histogram[j][1] = histogram[j][1] + 1;
					unique = false;
					break;
				}
				else if (abs(list[i] - histogram[j][0]) < error_bar) {
					error_bar_used_counter++;
					if (output_histogram_properties) {
						if (abs(list[i] - histogram[j][0]) > max_error_bar_used)
							max_error_bar_used = abs(list[i] - histogram[j][0]);
					}
					histogram[j][1] = histogram[j][1] + 1;
					unique = false;
					break;
				}
				else
					unique = true;
			}
			if (unique) {
				histogram.resize(histogram.size() + 1);
				histogram[histogram.size() - 1].push_back(list[i]);
				histogram[histogram.size() - 1].push_back(1);
			}
		}
	}

	cout << "Histogram discretisation complete." << endl;
	if (output_histogram_properties) {
		cout << "There were " << exact_value_repeated_counter << " exact repeated values, " << error_bar_used_counter << " values that were within the error bar and " << histogram.size() << " unique values." << endl;
		cout << "Value of error bar: " << error_bar << "\tValue of max error margin used: " << max_error_bar_used << endl;
	}

	//Write_2D_Matrix_to_Screen<double>(histogram);

	cout << "Beginning to write histogram to file..." << endl;
	fstream Data_Out;
	do {
		Data_Out.open(output_file, fstream::out);
		if (Data_Out.fail())
			cout << "Error opening file for writing!" << endl;
		else
			cout << "File opened for writing successfully..." << endl;
	} while (Data_Out.fail());

	for (int i = 0; i < histogram.size(); i++) {
		if (i < histogram.size() - 1)
			Data_Out << histogram[i][0] << "\t" << histogram[i][1] << "\n";
		else if (i == histogram.size() - 1)
			Data_Out << histogram[i][0] << "\t" << histogram[i][1];
	}
	cout << "Finished writting histogram to file." << endl;

	cout << "Finished creating histogram." << endl;
}

vector<vector<double>> CCalculatingCurvature::Create_Histogram(string input_file, char delimiter, double error_bar, bool output_histogram_properties) {
	cout << "Begin creating histogram..." << endl;

	//Declare initial local vairables
	ifstream data_in;						//Declare the stream that will input the data
	data_in.open(input_file, fstream::in);
	if (data_in.fail()) {
		cout << "Error opening file" << endl << endl;
		exit(0);
	}
	else
		cout << "File opened for reading successfully..." << endl;

	//Declare subsequent local variables
	vector<double> list_data;					//Used for reading in data
	string word;								//Used for storing each word as a string
	double element_value;						//Used for storing each word as a double
	while (getline(data_in, word, delimiter))	//While there are words to read in (seperated by the delimiter), store them in 'word'
	{
		element_value = stod(word);			//Convert the word into a double
		list_data.push_back(element_value);	//Store the double in list_data
	}
	cout << "Data has been read from file successfully..." << endl;

	sort(begin(list_data), end(list_data));

	cout << "Begin calculating histogram..." << endl;
	vector<vector<double>> histogram;
	bool unique = true;
	double max_error_bar_used = 0.0;
	int exact_value_repeated_counter = 0, error_bar_used_counter = 0;
	for (int i = 0; i < list_data.size(); i++) {
		if (histogram.empty()) {
			histogram.resize(1);
			histogram[0].push_back(list_data[i]);
			histogram[0].push_back(1);
			continue;
		}
		else {
			for (int j = 0; j < histogram.size(); j++) {
				if (list_data[i] == histogram[j][0]) {
					exact_value_repeated_counter++;
					histogram[j][1] = histogram[j][1] + 1;
					unique = false;
					break;
				}
				else if (abs(list_data[i] - histogram[j][0]) < error_bar) {
					error_bar_used_counter++;
					if (output_histogram_properties) {
						if (abs(list_data[i] - histogram[j][0]) > max_error_bar_used)
							max_error_bar_used = abs(list_data[i] - histogram[j][0]);
					}
					histogram[j][1] = histogram[j][1] + 1;
					unique = false;
					break;
				}
				else
					unique = true;
			}
			if (unique) {
				histogram.resize(histogram.size() + 1);
				histogram[histogram.size() - 1].push_back(list_data[i]);
				histogram[histogram.size() - 1].push_back(1);
			}
		}
	}

	cout << "Histogram discretisation complete." << endl;
	if (output_histogram_properties) {
		cout << "There were " << list_data.size() << " values that were read in which were split into " << exact_value_repeated_counter << " exact repeated values, " << error_bar_used_counter << " values that were within the error bar and " << histogram.size() << " unique values." << endl;
		cout << "Value of error bar: " << error_bar << "\tValue of max error margin used: " << max_error_bar_used << endl;
	}

	cout << "Finished creating histogram." << endl;
	return histogram;
}

template
double CCalculatingCurvature::Calculate_Mean<int>(vector<int>);
template
double CCalculatingCurvature::Calculate_Mean<double>(vector<double>);
template <typename data_type>
double CCalculatingCurvature::Calculate_Mean(vector<data_type> list) {
	cout << "Begin calculating mean..." << endl;

	data_type total = 0;
	for (int i = 0; i < list.size(); i++)
		total += list[i];
	double ans;
	ans = total / list.size();

	cout << "Finished calculating mean." << endl;
	return ans;
}

double CCalculatingCurvature::Calculate_Mean(string input_file, char delimiter) {
	cout << "Begin calculating mean..." << endl;

	//Declare initial local vairables
	ifstream data_in;						//Declare the stream that will input the data
	data_in.open(input_file, fstream::in);
	if (data_in.fail()) {
		cout << "Error opening file" << endl << endl;
		exit(0);
	}
	else
		cout << "File opened for reading successfully..." << endl;

	//Declare subsequent local variables
	vector<double> list_data;					//Used for reading in data
	string word;								//Used for storing each word as a string
	double element_value;						//Used for storing each word as a double
	while (getline(data_in, word, delimiter))	//While there are words to read in (seperated by the delimiter), store them in 'word'
	{
		element_value = stod(word);			//Convert the word into a double
		list_data.push_back(element_value);	//Store the double in list_data
	}
	cout << "Data has been read from file successfully..." << endl;

	double total = 0;
	for (int i = 0; i < list_data.size(); i++)
		total += list_data[i];
	double ans;
	ans = total / list_data.size();

	cout << "Finished calculating mean." << endl;
	return ans;
}

double CCalculatingCurvature::Calculate_Mean_of_Doubles_List_and_Remove_NANs(vector<double> list) {
	cout << "Begin calculating mean..." << endl;

	//Remove NANs
	list.erase(std::remove_if(std::begin(list), std::end(list), [](const auto& value) { return std::isnan(value); }), std::end(list));

	double total = 0;
	for (int i = 0; i < list.size(); i++)
		total += list[i];
	double ans;
	ans = total / list.size();

	cout << "Finished calculating mean." << endl;
	return ans;
}

vector<double> CCalculatingCurvature::Retrieve_Max_Principal_Curvature(int z_value, double z_step) { //Used for Catenoid error analysis
	cout << "Begin retrieving maximum principal curvatures..." << endl;

	//Check principal curvatures have been calculated
	if (principal_curvatures.empty()) {
		cout << "Can only retrieve max principal curvature when principal curvatures have been calculated." << endl;
		exit(0);
	}

	//Calculate top layer
	int num_layers = ((z_value / z_step) * 2) + 1 + 2;
	int top_limit = num_layers - 2; //-1 to start indexing layers from 0, and -1 to ensure that the 2nd from top isn't used (so correct coordinates to bottom of this layer)

	//Retrieves maximum as the positive curvature is the curvature of the catenoid - if not, retrieves the least negative (ie: highest not max absolute value)
	vector<double> max_principal_curvatures;
	for (int i = 0; i < vertices.size(); i++) {
		if (principal_curvatures[i].size() != 2) {
			cout << "Incorrect format of principal curvatures." << endl;
			exit(0);
		}
		/*
		//Save largest value
		if (principal_curvatures[i][0] > principal_curvatures[i][1])
			max_principal_curvatures.push_back(principal_curvatures[i][0]);
		else if (principal_curvatures[i][0] < principal_curvatures[i][1])
			max_principal_curvatures.push_back(principal_curvatures[i][1]);
		else //Values are equal
			max_principal_curvatures.push_back(principal_curvatures[i][0]);
		*/
		if (vertices[i][2] == 1.0 || vertices[i][2] == top_limit)
			continue;
		else {
			double max_principal_curvature = 0.0;
			max_principal_curvature = max(principal_curvatures[i][0], principal_curvatures[i][1]);
			max_principal_curvatures.push_back(max_principal_curvature);
		}
	}

	cout << "Finished retrieving maximum principal curvatures." << endl;
	return max_principal_curvatures;
}

void CCalculatingCurvature::Calculate_Max_Principal_Curvature_for_Catenoids(int z_max, double alpha, double z_step, int max_gauss_iterations, int max_laplacian_iterations, string filename_prefix, int output_faces_frequency) {
	cout << "Begin calculating mean of maximum principal curvature..." << endl;

	for (int i = 0; i < (max_gauss_iterations + 1); i++) {
		for (int j = 1; j < (max_laplacian_iterations + 1); j++) {
			//Create filenames
			//Post smoothing vertices
			string vertices_post_smooting_input_file = filename_prefix;
			vertices_post_smooting_input_file += to_string(z_max);
			vertices_post_smooting_input_file += "_";
			vertices_post_smooting_input_file += to_string(alpha);
			vertices_post_smooting_input_file.erase(vertices_post_smooting_input_file.find_last_not_of('0') + 1, string::npos); //Remove trailing zeroes
			vertices_post_smooting_input_file.erase(vertices_post_smooting_input_file.find_last_not_of('.') + 1, string::npos); //Remove final '.' if alpha is actually an integer
			vertices_post_smooting_input_file += "_";
			vertices_post_smooting_input_file += to_string(z_step);
			vertices_post_smooting_input_file.erase(vertices_post_smooting_input_file.find_last_not_of('0') + 1, string::npos); //Remove trailing zeroes
			vertices_post_smooting_input_file.erase(vertices_post_smooting_input_file.find_last_not_of('.') + 1, string::npos); //Remove final '.' if z_step is actually an integer
			vertices_post_smooting_input_file += "_";
			vertices_post_smooting_input_file += to_string(i);
			vertices_post_smooting_input_file += "_";
			vertices_post_smooting_input_file += to_string(j);
			vertices_post_smooting_input_file += "_Smoothing.txt";

			//Faces
			string faces_intput_file = filename_prefix;
			faces_intput_file += to_string(z_max);
			faces_intput_file += "_";
			faces_intput_file += to_string(alpha);
			faces_intput_file.erase(faces_intput_file.find_last_not_of('0') + 1, string::npos); //Remove trailing zeroes
			faces_intput_file.erase(faces_intput_file.find_last_not_of('.') + 1, string::npos); //Remove final '.' if alpha is actually an integer
			faces_intput_file += "_";
			faces_intput_file += to_string(z_step);
			faces_intput_file.erase(faces_intput_file.find_last_not_of('0') + 1, string::npos); //Remove trailing zeroes
			faces_intput_file.erase(faces_intput_file.find_last_not_of('.') + 1, string::npos); //Remove final '.' if z_step is actually an integer
			faces_intput_file += "_Faces.txt";

			//Read in vertices post smoothing file and faces file
			cout << "Catenoid " << z_max << "," << alpha << "," << z_step << " [" << i << "][" << j << "]..." << endl;
			vertices = Read_2D_Double_Matrix_from_Text_File(vertices_post_smooting_input_file, "Post Smoothing Vertices");
			faces = Read_2D_Integer_Matrix_from_Text_File(faces_intput_file, "Faces");

			//Calculate curvature
			vector<double> max_principal_curvatures;
			double mean_max_principal_curvature;
			Calculate_Face_Normals();
			Calculate_Vertice_Normals();
			Calculate_Curvature(true, output_faces_frequency);
			Get_Principal_Curvatures();
			max_principal_curvatures = Retrieve_Max_Principal_Curvature(z_max, z_step);
			cout << "Number of vertices (whole dataset): " << vertices.size() << "\tNumber of vertices (excluding top and bottom layers): " << max_principal_curvatures.size() << endl;
			mean_max_principal_curvature = Calculate_Mean<double>(max_principal_curvatures);

			//Output result to screen before writing to file
			cout << "Mean of Maximum Principal Curvatures: " << mean_max_principal_curvature << endl;

			//Append results to output
			//Create output filename
			string output_file = filename_prefix;
			output_file += "Maximum_Principal_Curvatures.txt";
			cout << output_file << endl;
			//Output
			fstream file;
			file.open(output_file, fstream::out | fstream::in);  //Won't create file
			if (file.is_open()) { //If file already exists
				//file.clear();
				//file.open(output_file, fstream::app | fstream::out); //Open in append mode
				file.close();
				ofstream out_file;
				out_file.open(output_file, ofstream::app);
				if (out_file.fail()) {
					cout << "Error opening file for writing (in append mode)!" << endl << endl;
					exit(0);
				}
				else {
					cout << "File opened for writing (in append mode) successfully..." << endl;
					//Output catenoid details and mean max principal curvature
					out_file << z_max << "\t" << alpha << "\t" << z_step << "\t" << i << "\t" << j << "\t" << mean_max_principal_curvature << "\n";
				}
			}
			else { // If file doesn't already exist
				file.close();
				ofstream out_file;
				out_file.open(output_file, ofstream::app);
				if (out_file.fail()) {
					cout << "Error creating new file for writing!" << endl << endl;
					exit(0);
				}
				else {
					cout << "File created and opened for writing successfully..." << endl;
					//Output catenoid details and mean max principal curvature
					out_file << z_max << "\t" << alpha << "\t" << z_step << "\t" << i << "\t" << j << "\t" << mean_max_principal_curvature << "\n";
				}
			}

			cout << "Results written (in append mode) to file successfully." << endl;
		}
	}

	cout << "Finished calculating mean of maximum principal curvature." << endl;
}