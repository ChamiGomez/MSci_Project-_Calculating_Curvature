#include "C_Calc_Curvature_Post_Project.h"
#include <time.h>
#include <iostream>
#include <cmath> //Include M_PI for calling Generate_Catenoid_Coordinates()
#include <limits> //Include for setting precision of cout

typedef std::numeric_limits< double > dbl; //Used for setting precision of double passed to cout

/* TIFF File Main
void main(void) {
    clock_t start_time = clock();

    CSurfaceDetection test;
    TIFF* test_tif_file;

    //Read in file
    test_tif_file = test.Read_In_TIFF_File();

    //Output image data to screen
    //Print_File_Properties_and_Voxel_Values(test_tif_file);

    //Output image data to matrix
    cout << "Reading image data into matrix..." << endl;
    test.Print_File_Properties_and_Save_Voxel_Values(test_tif_file);

    //Close File
    test.Close_File(test_tif_file);

    //Write Pixel Values to File
    //cout << "Writing image data to file..." << endl;
    //Write_Pixel_Values_to_File(pixel_values, "Pixel Values", "Pixel_Values_example_a_Segmented - Blurred.txt");

    //Parse Voxel Field Data into cube
    vector<int> temp_v;
    temp_v = test.Parse_Voxel_Data_into_Cube(0, 0, 0);


    clock_t end_time = clock();
    double process_time = (end_time - start_time) / CLOCKS_PER_SEC;
    cout << endl << "Time taken: " << process_time << "s\n";
}*/

//Synthetic Dataset Main
/*
void main(void) {
    clock_t start_time = clock();

    CCalculatingCurvature test;
    vector<int> dataset_sizes;
    int num_triangles = 0;

    //dataset_sizes = test.Read_Pixel_Values_from_Text_File("Sphere_11_1_1_Test.txt");
    //dataset_sizes = test.Read_Pixel_Values_from_Text_File("C:/Users/Chami/OneDrive - Imperial College London/Documents/4th Year Geophysics/MSci Project/C++ Code/MSci - 3D Surface Detection/Test_Dataset.txt");
    dataset_sizes = test.Generate_Sphere_Pixel_Values(5, 1, 1);

    //Single Cube
    //test.Parse_Voxel_Data_into_Cube(3, 1, 2, dataset_sizes);
    //num_triangles = test.Marching_Cubes_for_Single_Cube(1.5, 0);

    //Whole Datasets
    test.Dummy_Function(num_triangles, dataset_sizes);
    test.Marching_Cubes(dataset_sizes, 1.0, false, true, false);
    test.Convert_Triangles_to_Faces("Sphere_5_1_1_Isosurface_Coordinates_Checked.txt", "Sphere_5_1_1_Triangles_Checked.txt");
    test.Output_Data_to_File();

    clock_t end_time = clock();
    double process_time = (end_time - start_time) / CLOCKS_PER_SEC;
    cout << endl << "Time taken: " << process_time << "s\n";
}
*/

/* Curvature Calculation main
void main(void) {
    clock_t start_time = clock();

    Cisosurface test;

    cout << "Begin reading in data..." << endl;
    //test.Read_2D_Matrix_from_File("../../../../UROP/MATLAB Code/Vertices_1_to_12_Tenth_Step_sin(x)+cos(y)=z.txt", 1);
    test.Read_2D_Matrix_from_File("../../MATLAB Code/Vertices_Icosphere_3.txt", 1);
    //test.Read_2D_Matrix_from_File("../../../../UROP/MATLAB Code/Faces_1_to_20_Half_Step.txt", 2);
    test.Read_2D_Matrix_from_File("../../MATLAB Code/Faces_Icosphere_3.txt", 2);
    cout << "Finished reading in data." << endl << endl;

    test.Calculate_Face_Normals();
    test.Calculate_Vertice_Normals();
    test.Calculate_Curvature();
    test.Get_Principal_Curvatures();
    test.Calculate_Gaussian_Curvatures();

    cout << endl;
    //test.Dummy_Function();
    //test.Output_All_Data_to_File();

    double voxel_size;
    vector<double> gaussian_errors;
    voxel_size = test.Calculate_Voxel_Size_for_Synthetic_Sphere(1.0);
    gaussian_errors = test.Gaussian_Curvature_Error_Analysis_for_Spheres(1.0, voxel_size);
    test.Output_Error_Analysis_to_Screen(gaussian_errors, voxel_size, 1.0);

    cout << endl << "Radius errors (no voxel size consideration)" << endl;
    gaussian_errors.clear();
    gaussian_errors = test.Gaussian_Curvature_Error_Analysis_for_Spheres(1.0);
    test.Output_Error_Analysis_to_Screen(gaussian_errors, 1.0);

    clock_t end_time = clock();
    double process_time = (end_time - start_time) / CLOCKS_PER_SEC;
    cout << endl << "Time taken: " << process_time << "s\n";
}
*/

#if 0
//Combined main
void main(void) {
    clock_t start_time = clock();

    CCalculatingCurvature test;
    vector<int> dataset_sizes;
    TIFF* tif_file;

    /*
    //TIFF files
    //tif_file = test.Read_In_TIFF_File("C:/Users/Chami/OneDrive - Imperial College London/Documents/4th Year Geophysics/MSci Project/X-Ray Datasets/Isolated Oil Phase Water Wet Bentheimer Sandstone 49x44x34.tif"); //Read in file
    //dataset_sizes = test.Print_File_Properties_and_Save_Voxel_Values(tif_file); //Print properties and save voxel image data to pixel_values;
    //test.Close_File(tif_file); //Close file
    //test.Swap_Pixel_Values(2, 3);
    //test.Swap_Pixel_Values(1, 3);

    //Catenoids
    //dataset_sizes = test.Generate_Catenoid_Pixel_Values(57, 57, 4, 0.1, alpha);
    //dataset_sizes = test.Generate_Catenoid_Pixel_Values_with_Edges(5, 5, 1, 0.1, alpha);
    dataset_sizes = test.Generate_Catenoid_Pixel_Values_New_3(1, 0.35, 0.1);
    //test.Output_Data_to_File();


    //Spheres
    //dataset_sizes = test.Generate_Sphere_Pixel_Values(5, 1, 1);

    //Surface Detection
    //Note: Marching Cubes faster when (false, true) than (true, false) - check if can run when on (false, false)?
    //test.Marching_Cubes(dataset_sizes, 3, true, 10000, false, true); //Real segmented images
    test.Marching_Cubes(dataset_sizes, 1.0, true, 10000, false, true); //Sythetic images with binary pixel values
    clock_t marching_cubes_end_time = clock();
    int marching_cubes_run_time = (marching_cubes_end_time - start_time) / CLOCKS_PER_SEC;
    int marching_cubes_end_mins = marching_cubes_run_time / 60;
    int march_cubes_end_secs = marching_cubes_run_time % 60;
    if (marching_cubes_end_mins == 0)
        cout << "\nTime taken to reach end of marching cubes: " << march_cubes_end_secs << "secs" << endl;
    else
        cout << "\nTime taken to reach end of marching cubes: " << marching_cubes_end_mins << "mins " << march_cubes_end_secs << "secs" << endl;
    test.Output_Data_to_File();


    test.Correct_Layering_for_Catenoids_Post_Marching_Cubes("New Catenoids 3/Catenoid_1_0.35_0.1_Isosurface_Coordinates.txt", "New Catenoids 3/Catenoid_1_0.35_0.1_Triangles.txt",
        "New Catenoids 3/Catenoid_1_0.35_0.1_Corrected_Isosurface_Coordinates.txt", "New Catenoids 3/Catenoid_1_0.35_0.1_Corrected_Triangles.txt", 1, 0.1);

    test.Convert_Triangles_to_Faces("New Catenoids 3/Catenoid_1_0.35_0.1_Corrected_Isosurface_Coordinates.txt", "New Catenoids 3/Catenoid_1_0.35_0.1_Corrected_Triangles.txt", true, 1000);
    test.Convert_Isosurface_Coordinates_to_Vertices("New Catenoids 3/Catenoid_1_0.35_0.1_Corrected_Isosurface_Coordinates.txt");

    test.Find_Common_Interface("Real Datasets/Water-Wet/Water Wet Bentheimer Sandstone 49x44x34 - Water_Phase_Faces.txt", "Real Datasets/Water-Wet/Water Wet Bentheimer Sandstone 49x44x34 - Water_Phase_Vertices.txt",
        "Real Datasets/Water-Wet/Isolated Oil Phase Water Wet Bentheimer Sandstone 49x44x34_Faces.txt", "Real Datasets/Water-Wet/Isolated Oil Phase Water Wet Bentheimer Sandstone 49x44x34_Vertices.txt");
    */


    //Gaussian Smoothing
    //int input_dataset_size = 5;
    int z_max = 1;
    double alpha = 0.35;
    double z_step = 0.1;

    /*
    //test.Calculate_Max_Principal_Curvature_for_Catenoids(z_max, alpha, z_step, 8, 8, "New Catenoids 3/Catenoid_", 1000);
    test.vertices = test.Read_2D_Double_Matrix_from_Text_File("New Catenoids 3/Catenoid_1_0.35_0.1_Vertices.txt", "Post Smoothing Vertices");
    test.faces = test.Read_2D_Integer_Matrix_from_Text_File("New Catenoids 3/Catenoid_1_0.35_0.1_Faces.txt", "Faces");

    /*
    //Calculate curvature
    vector<double> max_principal_curvatures;
    double mean_max_principal_curvature;
    test.Calculate_Face_Normals();
    test.Calculate_Vertice_Normals();
    test.Calculate_Curvature(true, 1000);
    test.Get_Principal_Curvatures();
    max_principal_curvatures = test.Retrieve_Max_Principal_Curvature(z_max, z_step);
    cout << "Number of vertices (whole dataset): " << test.vertices.size() << "\tNumber of vertices (excluding top and bottom layers): " << max_principal_curvatures.size() << endl;
    mean_max_principal_curvature = test.Calculate_Mean<double>(max_principal_curvatures);

    //Output result to screen before writing to file
    cout << "Mean of Maximum Principal Curvatures: " << mean_max_principal_curvature << endl;
    */

    test.pre_smoothing_vertices = test.Read_2D_Double_Matrix_from_Text_File("Real Datasets/Water-Wet/Bentheimer 49x44x34 Oil-Water Interface/Oil-Water_Updated_Vertices.txt", "Vertices");
    test.faces = test.Read_2D_Integer_Matrix_from_Text_File("Real Datasets/Water-Wet/Bentheimer 49x44x34 Oil-Water Interface/Oil-Water_Updated_Faces.txt", "Faces");

    //test.Gaussian_Smoothing(10, 1, 10000, z_max, z_step, 0.25);
    //test.Gaussian_Smoothing(8, 3, 10000, z_max, alpha, z_step, 0.25, "New Catenoids 3/Catenoid Curvature Overview 3 - z_max = 1.txt", 1000, "New Catenoids 3/Catenoid_");
    //test.Gaussian_Smoothing(2, 2, 10000);
    //test.Gaussian_Smoothing_Test(1, 1, 10000);
    //test.Gaussian_Smoothing(8, 2, 10000, 43, true, 1000);


    test.Laplacian_Smoothing(8, 10000);
    //test.Laplacian_Smoothing(8, 10000, true, 1000);
    //test.Laplacian_Smoothing(8, 10000, input_dataset_size, "New Sphere Data - Updated Smoothing/New Sphere Curvature Overview - Updated Smoothing.txt", 1000, "New Sphere Data - Updated Smoothing/Sphere_");
    //test.Laplacian_Smoothing(5, 10000, z_max, z_step, 0.25);
    //test.Laplacian_Smoothing(8, 10000, z_max, alpha, z_step, 0.25, "New Catenoids 3/Catenoid Curvature Overview 3 - z_max = 1.txt", 1000, "New Catenoids 3/Catenoid_");

    test.Write_2D_Matrix_to_File(test.vertices, "Vertices Post Smoothing", "Real Datasets/Water-Wet/Bentheimer 49x44x34 Oil-Water Interface/Oil-Water_0_8_Smoothing.txt");
    //test.Skip_Smoothing();
    //cout << test.vertices.size() << endl;
    //test.Output_Data_to_File();


    //Curvature Calculation
    double mean_of_mean_curvature = 0.0, mean_gaussian_curvature = 0.0;
    test.Calculate_Face_Normals();
    test.Calculate_Vertice_Normals();
    test.Calculate_Curvature(true, 1000);
    test.Get_Principal_Curvatures();
    test.Calculate_Gaussian_Curvatures();
    //mean_gaussian_curvature = test.Calculate_Gaussian_Curvature_for_Catenoids(z_max, z_step);
    test.Calculate_Mean_Curvatures();
    //mean_of_mean_curvature = test.Calculate_Mean_Curvature_for_Catenoids(z_max, z_step);
    //test.Write_2D_Matrix_to_File(test.principal_curvatures, "Principal Curvatures", "Catenoid Data with Edges/Catenoid_with_Edges_57_57_4_0.1_1_Principal_Curvatures.txt");
    //test.Write_List_to_File<double>(test.mean_curvature, "Mean Curvature", "Catenoid Data with Edges/Catenoid_with_Edges_23_23_3_0.1_1_Mean_Curvature.txt");
    //test.Output_Data_to_File();


    //Error Analysis
    //vector<vector<double>> temp_hist;
    //test.Output_Data_to_File();
    //test.Create_Histogram(test.normalised_gaussian_curvature, 0.00001, "Sphere_21_1_1_Normalised_Gaussian_Curvature_Histogram.txt", true);
    //test.Create_Histogram(test.gaussian_curvature, 0.00001, "Sphere_21_1_1_Gaussian_Curvature_Histogram.txt", true);


    //Catenoids and General
    //cout.precision(dbl::max_digits10);
    //cout << "Mean of Mean Curvature (Excluding Top and Bottom Layers): " << mean_of_mean_curvature << endl;
    mean_of_mean_curvature = test.Calculate_Mean<double>(test.mean_curvature);
    cout << "Mean of Mean Curvature (Whole Dataset): " << mean_of_mean_curvature << endl;
    mean_of_mean_curvature = test.Calculate_Mean_of_Doubles_List_and_Remove_NANs(test.mean_curvature);
    cout << "Mean of Mean Curvature (Whole Dataset, Removed NANs): " << mean_of_mean_curvature << endl;
    //cout << "Mean Gaussian Curvature (Excluding Top and Bottom Layers): " << mean_gaussian_curvature << endl;
    mean_gaussian_curvature = test.Calculate_Mean<double>(test.gaussian_curvature);
    cout << "Mean Gaussian Curvature (Whole Dataset): " << mean_gaussian_curvature << endl;
    mean_gaussian_curvature = test.Calculate_Mean_of_Doubles_List_and_Remove_NANs(test.gaussian_curvature);
    cout << "Mean Gaussian Curvature (Whole Dataset, Removed NANs): " << mean_gaussian_curvature << endl;

    /*
    //Spheres Only
    double radius, centre_coordinate, mean;
    centre_coordinate = input_dataset_size / 2; //Integer maths
    //Original radius (with gap/voxel size both of 1) + 1 - top vertice's height post smoothing
    radius = (input_dataset_size - centre_coordinate - 2) + 1 - test.vertices[0][2];
    cout << "Radius: " << radius << endl;
    test.Normalise_Gaussian_Curvature_of_Sphere(radius);
    mean = test.Calculate_Mean<double>(test.normalised_gaussian_curvature);
    cout << "Mean Normalised Gaussian Curvature: " << mean << endl;
    //test.Output_Data_to_File();
    */

    clock_t end_time = clock();
    int process_time = (end_time - start_time) / CLOCKS_PER_SEC;
    int process_mins = process_time / 60;
    int process_secs = process_time % 60;
    if (process_mins != 0)
        cout << "\nTime taken: " << process_mins << "mins " << process_secs << "secs" << endl;
    else
        cout << "\nTime taken: " << process_time << "s" << endl;
}
#endif

void main(void) {
    clock_t start_time = clock();

    CCalculatingCurvature test;
    vector<int> dataset_sizes;
    TIFF* tif_file;

    /*
    //Spheres
    dataset_sizes = test.Generate_Sphere_Pixel_Values(23, 1, 1);

    //Surface Detection
    //Note: Marching Cubes faster when (false, true) than (true, false) - check if can run when on (false, false)?
    //test.Marching_Cubes(dataset_sizes, 3, true, 10000, false, true); //Real segmented images
    test.Marching_Cubes(dataset_sizes, 1.0, true, 10000, false, true); //Sythetic images with binary pixel values
    clock_t marching_cubes_end_time = clock();
    int marching_cubes_run_time = (marching_cubes_end_time - start_time) / CLOCKS_PER_SEC;
    int marching_cubes_end_mins = marching_cubes_run_time / 60;
    int march_cubes_end_secs = marching_cubes_run_time % 60;
    if (marching_cubes_end_mins == 0)
        cout << "\nTime taken to reach end of marching cubes: " << march_cubes_end_secs << "secs" << endl;
    else
        cout << "\nTime taken to reach end of marching cubes: " << marching_cubes_end_mins << "mins " << march_cubes_end_secs << "secs" << endl;
    test.Output_Data_to_File();


    test.Correct_Layering_for_Catenoids_Post_Marching_Cubes("New Catenoids 3/Catenoid_1_0.35_0.1_Isosurface_Coordinates.txt", "New Catenoids 3/Catenoid_1_0.35_0.1_Triangles.txt",
        "New Catenoids 3/Catenoid_1_0.35_0.1_Corrected_Isosurface_Coordinates.txt", "New Catenoids 3/Catenoid_1_0.35_0.1_Corrected_Triangles.txt", 1, 0.1);

    test.Convert_Triangles_to_Faces("New Catenoids 3/Catenoid_1_0.35_0.1_Corrected_Isosurface_Coordinates.txt", "New Catenoids 3/Catenoid_1_0.35_0.1_Corrected_Triangles.txt", true, 1000);
    test.Convert_Isosurface_Coordinates_to_Vertices("New Catenoids 3/Catenoid_1_0.35_0.1_Corrected_Isosurface_Coordinates.txt");

    test.Find_Common_Interface("Real Datasets/Water-Wet/Water Wet Bentheimer Sandstone 49x44x34 - Water_Phase_Faces.txt", "Real Datasets/Water-Wet/Water Wet Bentheimer Sandstone 49x44x34 - Water_Phase_Vertices.txt",
        "Real Datasets/Water-Wet/Isolated Oil Phase Water Wet Bentheimer Sandstone 49x44x34_Faces.txt", "Real Datasets/Water-Wet/Isolated Oil Phase Water Wet Bentheimer Sandstone 49x44x34_Vertices.txt");
    */


    //Gaussian Smoothing
    //int input_dataset_size = 5;
    int z_max = 1;
    double alpha = 0.35;
    double z_step = 0.1;

    /*
    //test.Calculate_Max_Principal_Curvature_for_Catenoids(z_max, alpha, z_step, 8, 8, "New Catenoids 3/Catenoid_", 1000);
    test.vertices = test.Read_2D_Double_Matrix_from_Text_File("New Catenoids 3/Catenoid_1_0.35_0.1_Vertices.txt", "Post Smoothing Vertices");
    test.faces = test.Read_2D_Integer_Matrix_from_Text_File("New Catenoids 3/Catenoid_1_0.35_0.1_Faces.txt", "Faces");

    /*
    //Calculate curvature
    vector<double> max_principal_curvatures;
    double mean_max_principal_curvature;
    test.Calculate_Face_Normals();
    test.Calculate_Vertice_Normals();
    test.Calculate_Curvature(true, 1000);
    test.Get_Principal_Curvatures();
    max_principal_curvatures = test.Retrieve_Max_Principal_Curvature(z_max, z_step);
    cout << "Number of vertices (whole dataset): " << test.vertices.size() << "\tNumber of vertices (excluding top and bottom layers): " << max_principal_curvatures.size() << endl;
    mean_max_principal_curvature = test.Calculate_Mean<double>(max_principal_curvatures);

    //Output result to screen before writing to file
    cout << "Mean of Maximum Principal Curvatures: " << mean_max_principal_curvature << endl;
    */

    test.pre_smoothing_vertices = test.Read_2D_Double_Matrix_from_Text_File("Real Datasets/Water-Wet/Bentheimer 49x44x34 Oil-Water Interface/Oil-Water_Updated_Vertices.txt", "Vertices");
    test.faces = test.Read_2D_Integer_Matrix_from_Text_File("Real Datasets/Water-Wet/Bentheimer 49x44x34 Oil-Water Interface/Oil-Water_Updated_Faces.txt", "Faces");

    //test.Gaussian_Smoothing(10, 1, 10000, z_max, z_step, 0.25);
    //test.Gaussian_Smoothing(8, 3, 10000, z_max, alpha, z_step, 0.25, "New Catenoids 3/Catenoid Curvature Overview 3 - z_max = 1.txt", 1000, "New Catenoids 3/Catenoid_");
    //test.Gaussian_Smoothing(2, 2, 10000);
    //test.Gaussian_Smoothing_Test(1, 1, 10000);
    //test.Gaussian_Smoothing(8, 2, 10000, 43, true, 1000);

    //test.Laplacian_Smoothing(8, 10000);
    //test.Laplacian_Smoothing(8, 10000, true, 1000);
    //test.Laplacian_Smoothing(8, 10000, input_dataset_size, "New Sphere Data - Updated Smoothing/New Sphere Curvature Overview - Updated Smoothing.txt", 1000, "New Sphere Data - Updated Smoothing/Sphere_");
    //test.Laplacian_Smoothing(5, 10000, z_max, z_step, 0.25);
    //test.Laplacian_Smoothing(8, 10000, z_max, alpha, z_step, 0.25, "New Catenoids 3/Catenoid Curvature Overview 3 - z_max = 1.txt", 1000, "New Catenoids 3/Catenoid_");

    test.Write_2D_Matrix_to_File(test.vertices, "Vertices Post Smoothing", "Real Datasets/Water-Wet/Bentheimer 49x44x34 Oil-Water Interface/Oil-Water_0_8_Smoothing.txt");
    //test.Skip_Smoothing();
    //cout << test.vertices.size() << endl;
    //test.Output_Data_to_File();


    //Curvature Calculation
    double mean_of_mean_curvature = 0.0, mean_gaussian_curvature = 0.0;
    test.Calculate_Face_Normals();
    test.Calculate_Vertice_Normals();
    test.Calculate_Curvature(true, 1000);
    test.Get_Principal_Curvatures();
    test.Calculate_Gaussian_Curvatures();
    //mean_gaussian_curvature = test.Calculate_Gaussian_Curvature_for_Catenoids(z_max, z_step);
    test.Calculate_Mean_Curvatures();
    //mean_of_mean_curvature = test.Calculate_Mean_Curvature_for_Catenoids(z_max, z_step);
    //test.Write_2D_Matrix_to_File(test.principal_curvatures, "Principal Curvatures", "Catenoid Data with Edges/Catenoid_with_Edges_57_57_4_0.1_1_Principal_Curvatures.txt");
    //test.Write_List_to_File<double>(test.mean_curvature, "Mean Curvature", "Catenoid Data with Edges/Catenoid_with_Edges_23_23_3_0.1_1_Mean_Curvature.txt");
    //test.Output_Data_to_File();


    //Error Analysis
    //vector<vector<double>> temp_hist;
    //test.Output_Data_to_File();
    //test.Create_Histogram(test.normalised_gaussian_curvature, 0.00001, "Sphere_21_1_1_Normalised_Gaussian_Curvature_Histogram.txt", true);
    //test.Create_Histogram(test.gaussian_curvature, 0.00001, "Sphere_21_1_1_Gaussian_Curvature_Histogram.txt", true);


    //Catenoids and General
    //cout.precision(dbl::max_digits10);
    //cout << "Mean of Mean Curvature (Excluding Top and Bottom Layers): " << mean_of_mean_curvature << endl;
    mean_of_mean_curvature = test.Calculate_Mean<double>(test.mean_curvature);
    cout << "Mean of Mean Curvature (Whole Dataset): " << mean_of_mean_curvature << endl;
    mean_of_mean_curvature = test.Calculate_Mean_of_Doubles_List_and_Remove_NANs(test.mean_curvature);
    cout << "Mean of Mean Curvature (Whole Dataset, Removed NANs): " << mean_of_mean_curvature << endl;
    //cout << "Mean Gaussian Curvature (Excluding Top and Bottom Layers): " << mean_gaussian_curvature << endl;
    mean_gaussian_curvature = test.Calculate_Mean<double>(test.gaussian_curvature);
    cout << "Mean Gaussian Curvature (Whole Dataset): " << mean_gaussian_curvature << endl;
    mean_gaussian_curvature = test.Calculate_Mean_of_Doubles_List_and_Remove_NANs(test.gaussian_curvature);
    cout << "Mean Gaussian Curvature (Whole Dataset, Removed NANs): " << mean_gaussian_curvature << endl;

    /*
    //Spheres Only
    double radius, centre_coordinate, mean;
    centre_coordinate = input_dataset_size / 2; //Integer maths
    //Original radius (with gap/voxel size both of 1) + 1 - top vertice's height post smoothing
    radius = (input_dataset_size - centre_coordinate - 2) + 1 - test.vertices[0][2];
    cout << "Radius: " << radius << endl;
    test.Normalise_Gaussian_Curvature_of_Sphere(radius);
    mean = test.Calculate_Mean<double>(test.normalised_gaussian_curvature);
    cout << "Mean Normalised Gaussian Curvature: " << mean << endl;
    //test.Output_Data_to_File();
    */

    clock_t end_time = clock();
    int process_time = (end_time - start_time) / CLOCKS_PER_SEC;
    int process_mins = process_time / 60;
    int process_secs = process_time % 60;
    if (process_mins != 0)
        cout << "\nTime taken: " << process_mins << "mins " << process_secs << "secs" << endl;
    else
        cout << "\nTime taken: " << process_time << "s" << endl;
}