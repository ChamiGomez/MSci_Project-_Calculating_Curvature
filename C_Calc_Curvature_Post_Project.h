#pragma once
#include <vector>
#include <tiffio.h>
#include <string>
#include <gsl/gsl_linalg.h>

using namespace std;

class CCalculatingCurvature {
public:
	//Surface Detection Variables
	vector<vector<vector<int>>> pixel_values;
	vector<vector<vector<double>>> isosurface_coordinates;
	vector<vector<vector<vector<double>>>> triangles;
	vector<vector<double>> pre_smoothing_vertices;
	vector<vector<int>> faces;

	//Temp Surface Detection Variables (for single cube)
	vector<int> vertices_values;
	vector<vector<double>> vertices_coordinates;

	//Curvature Calculation Variables
	vector<vector<vector<double>>> faces_sft, vertices_sft; //Faces' and Vertices' Second Fundamental Tensor
	vector<vector<double>> vertices, edge_1, edge_2, edge_3, face_normals, edge_1_normalised, edge_2_normalised, edge_3_normalised, corner_weights, vertice_normals, up, vp, corner_voronoi_weights, principal_curvatures, principal_direction_1, principal_direction_2; //principal_curvatures is transposed of corresponding matlab matrix
	vector<double> vertice_weights, rotated_u, rotated_v, gaussian_curvature, normalised_gaussian_curvature, mean_curvature; //For Rotate_Coordinate_System: used within Calculate_Curvature and Get_Principal_Curvatures;

	//Surface Smoothing Variables
	vector<vector<double>> face_centres, edge_centres, vertice_area_weights, smoothing_vertice_normals;

	//Data Handling
	fstream Preliminary_Output_File_Checks(string filename);
	fstream Preliminary_Output_File_Checks(void);
	template<typename data_type>
	void Write_List_to_File(vector<data_type> output_list, string list_name, string output_file);
	template<typename data_type>
	void Write_2D_Matrix_to_File(vector<vector<data_type>> matrix_2D, string matrix_name, string output_file);
	template<typename data_type>
	void Write_3D_Matrix_to_File(vector<vector<vector<data_type>>> matrix_3D, string matrix_name, string output_file, bool output_directory_number, bool spaces_between_directories);
	template<typename data_type>
	void Write_4D_Matrix_to_File(vector<vector<vector<vector<data_type>>>> matrix_4D, string matrix_name, string output_file);
	template<typename data_type>
	void Write_List_to_Screen(vector<data_type> list);
	template<typename data_type>
	void Write_2D_Matrix_to_Screen(vector<vector<data_type>> matrix);
	template<typename data_type>
	void Write_3D_Matrix_to_Screen(vector<vector<vector<data_type>>> matrix_3D);
	template<typename data_type>
	void Read_2D_Matrix_from_File(string input_file, vector<vector<data_type>> matrix_store, string matrix_name); //Doesn't work
	/*template<typename data_type>
	vector<vector<vector<data_type>>> Read_3D_Matrix_from_File(string input_file, string matrix_name, data_type);*/
	vector<int> Read_Pixel_Values_from_Text_File(string input_file); //Returns size (z, y and x) of dataset from file - thats why it has a special function
	vector<vector<double>> Read_2D_Double_Matrix_from_Text_File(string input_file, string matrix_name);
	vector<vector<int>> Read_2D_Integer_Matrix_from_Text_File(string input_file, string matrix_name);
	void Output_Data_to_File(void);
	void Append_Continuous_Catneoid_Curvature_Output_to_File(int z_max, double alpha, double voxel_size, int gaussian_iterations, int laplacian_iterations, string output_file, int output_faces_frequency);
	void Append_Continuous_Sphere_Curvature_Output_to_File(int input_dataset_size, int gaussian_iterations, int laplacian_iterations, string output_file, int output_faces_frequency);

	//TIFF File Handling Functions
	TIFF* Read_In_TIFF_File(const char* input_file); //libtiff is a C library so can't use string from STL
	void Print_File_Properties_and_Voxel_Values(TIFF* tif_file);
	vector<int> Print_File_Properties_and_Save_Voxel_Values(TIFF* tif_file);  //Returns size (z, y and x) of dataset from file
	void Close_File(TIFF* tif_file);

	//Generate Synthetic Datsets
	vector<int> Generate_Sphere_Pixel_Values(int dataset_size, int voxel_size, int gap_size); //Returns size of dataset as a vector (although this is given as an int argument)
	/* Old System - generates actual surface (not finished)
	vector<vector<double>> Generate_Catenoid_Coordinates(double lower_u, double upper_u, double lower_v, double upper_v, double alpha, double step_size);
	vector<double> Calculate_Point_on_Catenoid(double u, double v, double alpha);
	vector<int> Generate_Catenoid_Pixel_Values(vector<vector<double>> catenoid_coordinates, int decimal_precision, double alpha); //Returns size of dataset (z, y, and x) as a vector*/
	vector<int> Generate_Catenoid_Pixel_Values(int x_size, int y_size, int z_value, double z_step, double alpha); //Returns size of dataset (z, y, and x) as a vector
	vector<int> Generate_Catenoid_Pixel_Values_with_Edges(int x_size, int y_size, int z_value, double z_step, double alpha); //Returns size of dataset (z, y, and x) as a vector, with top and bottom layer
	int Calculate_if_Point_Lies_within_Catenoid(double x, double y, double z, double alpha);
	vector<int> Generate_Catenoid_Pixel_Values_New(int z_max, double alpha, double voxel_size);
	vector<int> Generate_Catenoid_Pixel_Values_New_2(int z_max, double alpha, double voxel_size);
	vector<int> Generate_Catenoid_Pixel_Values_New_3(int z_max, double alpha, double voxel_size);

	//Marching Cubes Algorithm
	void Parse_Voxel_Data_into_Cube(int x, int y, int z, vector<int> dataset_size);
	bool Check_Triangle(vector<double> vertex_1, vector<double> vertex_2, vector<double> vertex_3);
	bool Check_Isosurface_Coordinates(vector<double> coordinates, vector<int> dataset_size);
	int Marching_Cubes_for_Single_Cube(double isolevel, int single_cube_index);
	int Marching_Cubes_for_Single_Cube_Test(double isolevel, int single_cube_index, vector<int> datset_size, bool check_isosurface, bool check_triangles);
	int Calculate_Cube_Index(vector<int> vertices_values, double isolevel);
	vector<double> Linear_Interpolation(double isolevel, double error_margin, vector<double> pos_1, vector<double> pos_2, int val_1, int val_2);
	void Marching_Cubes(vector<int> dataset_size, double isolevel, bool output_cube_index_to_screen, int cube_index_output_frequency, bool check_isosurface, bool check_triangles);
	void Convert_Triangles_to_Faces(string isosurface_file, string triangles_file, bool output_triangle_number_to_screen, int triangle_number_output_frequency);
	void Convert_Isosurface_Coordinates_to_Vertices(void);
	void Convert_Isosurface_Coordinates_to_Vertices(string isosurface_file);
	void Correct_Layering_for_Catenoids_Post_Marching_Cubes(string input_isosurface_file, string input_triangles_file, string output_isosurface_file, string output_triangles_file, int z_value, double z_step);
	void Modify_Pixel_Values(int old_value, int new_value);
	void Swap_Pixel_Values(int pixel_value_1, int pixel_value_2);
	void Find_Common_Interface(string interface_1_faces_file, string interface_1_vertices_file, string interface_2_faces_file, string interface_2_vertices_file);

	//Surface Smoothing
	void Calculate_Face_Centres(void);
	void Calculate_Edge_Centres(bool output_edges_index, int output_frequency);
	vector<double> Calculate_Triangular_Area(vector<double> point, vector<double> edge_centre, vector<double> face_centre); //Returns area with magnitude and direction
	void Calculate_Vertex_Area_Weights_for_Smoothing(void);
	void Calculate_Smoothing_Vertice_Normals(void);
	double Calculate_Weight_Factor(vector<double> adjacent_area, vector<double> original_area);
	vector<double> Calculate_Average_Distance_Vector_for_Movement(vector<double> weight_factors, vector<vector<double>> adjacent_positions, vector<double> original_position);
	vector<double> Laplacian_Smoothing(vector<double> old_position, double beta, vector<double> distance_vector, vector<double> normal_vector); //Overloaded with function that performs Laplacian smoothing outside of Gaussian_Smoothing()
	void Laplacian_Smoothing_Test(vector<double> old_position, double beta, vector<double> distance_vector, vector<double> normal_vector, vector<double> Laplacian_smoothed_position, vector<double> displacement_during_Laplacian_smoothing); //Goes with Gaussian_Smoothing_Test()
	vector<double> Calculate_Average_Displacement_during_Laplacian_Smoothing(vector<double> weight_factors, vector<vector<double>> original_positions, vector<vector<double>> adjacent__laplacian_positions);
	vector<double> Calculate_Average_Displacement_during_Laplacian_Smoothing_Test(vector<double> weight_factors, vector<vector<double>> displacements_during_Laplacian_smoothing); //Goes with Gaussian_Smoothing_Test()
	vector<double> Migrate_Points_after_Laplacian_Smoothing(vector<double> laplacian_position, vector<double> displacement_during_laplacian_smoothing, vector<double> normal_vector);
	void Gaussian_Smoothing(int gaussian_iterations, int iterations_per_step, int output_edges_frequency); //Normal surfaces
	void Gaussian_Smoothing_Test(int gaussian_iterations, int iterations_per_setp, int output_edges_frequency); //Test
	void Gaussian_Smoothing(int gaussian_iterations, int iterations_per_step, int output_edges_frequency, int z_value, double z_step, double tolerance); //Catenoids (with correction)
	void Gaussian_Smoothing(int gaussian_iterations, int iterations_per_step, int output_edges_frequency, int z_value, double alpha, double z_step, double tolerance, string output_curvature_file, int output_faces_frequency, string output_post_smoothing_vertices_filename_prefix); //Catenoids (with correction and continuous output)
	void Gaussian_Smoothing(int gaussian_iterations, int iterations_per_step, int output_edges_frequency, int input_dataset_size, bool continuous_curvature_output, int output_faces_frequency); //Continuous output
	void Skip_Smoothing(void);
	void Laplacian_Smoothing(int laplacian_iterations, int output_edges_frequency); //Normal surfaces - Overloaded with function that goes inside Gaussian_Smoothing()
	void Laplacian_Smoothing(int laplacian_iterations, int output_edges_frequency, bool continuous_curvature_output, int output_faces_frequency); //Normal surfaces - continuous output
	void Laplacian_Smoothing(int laplacian_iterations, int output_edges_frequency, int input_dataset_size, string output_curvature_file, int output_faces_frequency, string output_post_smoothing_vertices_filename_prefix); //Spheres - continuous output
	void Laplacian_Smoothing(int laplacian_iterations, int output_edges_frequency, int z_value, double z_step, double tolerance); //Catenoids (with correction) - Overloaded with function that goes inside Gaussian_Smoothing()
	void Laplacian_Smoothing(int laplacian_iterations, int output_edges_frequency, int z_value, double alpha, double z_step, double tolerance, string output_curvature_file, int output_faces_frequency, string output_post_smoothing_vertices_filename_prefix); //Catenoids (with correction and continuous output) - Overloaded with function that goes inside Gaussian_Smoothing()
	void Correct_Layering_for_Catenoids_Post_Smoothing(int z_value, double z_step, double tolerance);

	//Vector and Matrix Background Calculations
	vector<double> Cross_Product(vector<double> lhs, vector<double> rhs);
	vector<double> Normalise_Vector_Rows(vector<double> input_vector);
	double Vector_Magnitude(vector<double> input_vector); //Aka: Euclidean norm, Euclidean length or 2-norm
	vector<double> Add_3D_Vector(vector<double> lhs, vector<double> rhs);
	vector<double> Subtract_3D_Vector(vector<double> lhs, vector<double> rhs);
	vector<double> Scalar_Multiplication_of_Vector(vector<double> input_vector, double scalar);
	double Elementwise_Multiplication_of_3D_Vectors(vector<double> lhs, vector<double> rhs);
	vector<double> Matrix_Multiplication_2x2_and_Column_Vector(vector<vector<double>> lhs_matrix, double first_rhs_vector_element, double second_rhs_vector_element);
	double Elementwise_Multiplication_of_2D_Vectors(vector<double> lhs_vector, double first_rhs_vector_element, double second_rhs_vector_element);
	vector<vector<double>> Matrix_Addition(vector<vector<double>> lhs_matrix, vector<vector<double>> rhs_matrix);

	//Curvature Calculation
	void Calculate_Face_Normals(void);
	void Calculate_Vertice_Normals(void);
	void Calculate_Curvature(bool output_faces_index, int output_frequency);
	void Get_Principal_Curvatures(void);
	void Calculate_Gaussian_Curvatures(void);
	double Calculate_Gaussian_Curvature_for_Catenoids(int z_value, double z_step);
	void Calculate_Mean_Curvatures(void);
	double Calculate_Mean_Curvature_for_Catenoids(int z_value, double z_step);
	vector<double> Retrieve_Max_Principal_Curvature(int z_value, double z_step); //Used for Catenoid error analysis

	//Background Curvature Calculation Functions
	vector<double> Project_Curvature_Tensor(vector<double> u, vector<double> v, gsl_vector* x, int vector_index, int face_normal_index);
	void Rotate_Coordinate_System(int vector_index, vector<double> normal_vector);

	//Error Analysis
	void Normalise_Gaussian_Curvature_of_Sphere(double radius);
	template <typename data_type>
	void Create_Histogram(vector<data_type> list, double error_bar, string output_file, bool output_histogram_properties);
	vector<vector<double>> Create_Histogram(string input_file, char delimiter, double error_bar, bool output_histogram_properties); //Returns histogram
	template <typename data_type>
	double Calculate_Mean(vector<data_type> list); //Returns mean
	double Calculate_Mean(string input_file, char delimiter);
	double Calculate_Mean_of_Doubles_List_and_Remove_NANs(vector<double> list);
	void Calculate_Max_Principal_Curvature_for_Catenoids(int z_max, double alpha, double z_step, int max_gauss_iterations, int max_laplacian_iterations, string filename_prefix, int output_faces_frequency);
};

extern template
void CCalculatingCurvature::Write_List_to_File(vector<int> output_list, string list_name, string output_file);
extern template
void CCalculatingCurvature::Write_List_to_File(vector<double> output_list, string list_name, string output_file);

extern template
void CCalculatingCurvature::Write_2D_Matrix_to_File(vector<vector<int>> matrix_2D, string matrix_name, string output_file);
extern template
void CCalculatingCurvature::Write_2D_Matrix_to_File(vector<vector<double>> matrix_2D, string matrix_name, string output_file);

extern template
void CCalculatingCurvature::Write_3D_Matrix_to_File(vector<vector<vector<int>>> matrix_3D, string matrix_name, string output_file, bool output_directory_number, bool spaces_between_directories);
extern template
void CCalculatingCurvature::Write_3D_Matrix_to_File(vector<vector<vector<double>>> matrix_3D, string matrix_name, string output_file, bool output_directory_number, bool spaces_between_directories);

extern template
void CCalculatingCurvature::Write_4D_Matrix_to_File(vector<vector<vector<vector<int>>>> matrix_4D, string matrix_name, string output_file);
extern template
void CCalculatingCurvature::Write_4D_Matrix_to_File(vector<vector<vector<vector<double>>>> matrix_4D, string matrix_name, string output_file);

extern template
void CCalculatingCurvature::Write_List_to_Screen(vector<int> list);
extern template
void CCalculatingCurvature::Write_List_to_Screen(vector<double> list);

extern template
void CCalculatingCurvature::Write_2D_Matrix_to_Screen<int>(vector<vector<int>> matrix);
extern template
void CCalculatingCurvature::Write_2D_Matrix_to_Screen<double>(vector<vector<double>> matrix);

extern template
void CCalculatingCurvature::Write_3D_Matrix_to_Screen(vector<vector<vector<int>>> matrix_3D);
extern template
void CCalculatingCurvature::Write_3D_Matrix_to_Screen(vector<vector<vector<double>>> matrix_3D);


extern template
void CCalculatingCurvature::Read_2D_Matrix_from_File<int>(string input_file, vector<vector<int>> matrix_store, string matrix_name);
extern template
void CCalculatingCurvature::Read_2D_Matrix_from_File<double>(string input_file, vector<vector<double>> matrix_store, string matrix_name);

/*
extern template
vector<vector<vector<int>>> CCalculatingCurvature::Read_3D_Matrix_from_File<int>(string input_file, string matrix_name, int);
extern template
vector<vector<vector<double>>> CCalculatingCurvature::Read_3D_Matrix_from_File<double>(string input_file, string matrix_name, double);
*/

extern template
void CCalculatingCurvature::Create_Histogram<int>(vector<int> list, double error_bar, string output_file, bool output_histogram_properties);
extern template
void CCalculatingCurvature::Create_Histogram<double>(vector<double> list, double error_bar, string output_file, bool output_histogram_properties);

extern template
double CCalculatingCurvature::Calculate_Mean<int>(vector<int> list);
extern template
double CCalculatingCurvature::Calculate_Mean<double>(vector<double> list);