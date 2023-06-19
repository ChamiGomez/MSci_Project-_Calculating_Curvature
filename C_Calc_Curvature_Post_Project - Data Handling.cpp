#include "C_Calc_Curvature_Post_Project.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <conio.h> //Includes _getch()
#include <locale> //Includes toupper()
#include <algorithm> //Inclues transform()
//#include <typeinfo>

fstream CCalculatingCurvature::Preliminary_Output_File_Checks(string filename) {
    fstream file;
    bool create_new_file = false;
    file.open(filename, fstream::out | fstream::in);  //Won't create file
    if (file.is_open()) { //If file already exists
        char option = 'a'; //Dummy assignment to initialise local variable for while check below - is overwritten
        cout << "Warning: " << filename << " already exists. Would you like to overwrite the file or exit?\n";
        cout << "(Y) - Proceed and overwrite file, (N) - Exit. Enter option: ";
        do {
            option = _getch();
            if (toupper(option) == 'N')
                exit(0);
            else if (toupper(option) == 'Y')
                create_new_file = true;
            else
                cout << "\nInvalid option entered. Please enter (Y/N): ";
        } while (toupper(option) != 'Y' && toupper(option) != 'N');
    }
    else { // If file doesn't already exist
        file.clear();
        file.open(filename, fstream::out);
        if (file.fail()) {
            cout << "Error opening file for writing!" << endl << endl;
            exit(0);
        }
        else {
            cout << "File opened for writing successfully..." << endl;
            return file;
        }
    }
    if (create_new_file) { //If file already exists but user wants to overwrite file
        file.close();
        file.open(filename, fstream::out);
        if (file.fail()) {
            cout << "\nError opening file for writing!" << endl << endl;
            exit(0);
        }
        else {
            cout << "\nFile opened for writing successfully..." << endl;
            return file;
        }
    }
}

//New version using terminal input
fstream CCalculatingCurvature::Preliminary_Output_File_Checks(void) {
    fstream file;
    string filename;
    bool create_new_file = false;
    do {
        cout << "Please enter output filename: ";
        getline(cin, filename);
        file.open(filename, fstream::out | fstream::in);  //Won't create file
        if (file.is_open()) { //If file already exists
            char option = 'a'; //Dummy assignment to initialise local variable for while check below - is overwritten
            cout << "Warning: " << filename << " already exists. Would you like to overwrite the file, type a new filename or exit?\n";
            cout << "(Y) - Proceed and overwrite file, (N) - Type a new filename or (E) - Exit. Enter option: ";
            do {
                option = _getch();
                if (toupper(option) == 'E')
                    exit(1);
                else if (toupper(option) == 'Y')
                    create_new_file = true;
                else if (toupper(option) == 'N') { //Will cause a repeat in the outer do...while loop
                    continue;
                }
                else
                    cout << "\nInvalid option entered. Please enter (Y/N/E): ";
            } while (toupper(option) != 'Y' && toupper(option) != 'N' && toupper(option) != 'E');
        }
        else { // If file doesn't already exist
            file.clear();
            file.open(filename, fstream::out);
            if (file.fail())
                cout << "Error opening file for writing!" << endl;
            else {
                cout << "File opened for writing successfully..." << endl;
                return file;
            }
        }
        if (create_new_file) { //If file already exists but user wants to overwrite file
            file.close();
            file.open(filename, fstream::out);
            if (file.fail()) {
                cout << "\nError opening file for writing!" << endl;
                create_new_file = false;
            }
            else {
                cout << "\nFile opened for writing successfully..." << endl;
                return file;
            }
        }
    } while (!create_new_file);
}

template
void CCalculatingCurvature::Write_List_to_File(vector<int> output_list, string list_name, string output_file);
template
void CCalculatingCurvature::Write_List_to_File(vector<double> output_list, string list_name, string output_file);
template<typename data_type>
void CCalculatingCurvature::Write_List_to_File(vector<data_type> output_list, string list_name, string output_file) {
    fstream Data_Out;
    Data_Out = Preliminary_Output_File_Checks(output_file);

    for (int i = 0; i < output_list.size(); i++) {
        if (i < output_list.size() - 1)
            Data_Out << output_list[i] << "\n";
        else if (i == output_list.size() - 1)
            Data_Out << output_list[i];
    }

    //Data_Out.close();
    cout << list_name << " written to file successfully!" << endl;
}

template
void CCalculatingCurvature::Write_2D_Matrix_to_File(vector<vector<int>> matrix_2D, string matrix_name, string output_file);
template
void CCalculatingCurvature::Write_2D_Matrix_to_File(vector<vector<double>> matrix_2D, string matrix_name, string output_file);
template<typename data_type>
void CCalculatingCurvature::Write_2D_Matrix_to_File(vector<vector<data_type>> matrix_2D, string matrix_name, string output_file) {
    fstream Data_Out;
    Data_Out = Preliminary_Output_File_Checks(output_file);

    for (int i = 0; i < matrix_2D.size(); i++) { //For each row
        if (matrix_2D[i].empty()) //Skip empty rows
            continue;
        else {
            for (int j = 0; j < matrix_2D[i].size(); j++) { //For each column
                if (j < matrix_2D[i].size() - 1) //If this isn't the last column
                    Data_Out << matrix_2D[i][j] << "\t"; //Output the value and a tab
                else if (j == matrix_2D[i].size() - 1) //If this is the last column
                    Data_Out << matrix_2D[i][j]; //Output the value only
            }
            if (i < matrix_2D.size() - 1) //If this isn't the last row
                Data_Out << "\n"; //Output a new line to start the new row
        }
    }

    //Data_Out.close();
    cout << matrix_name << " written to file successfully!" << endl;
}

template
void CCalculatingCurvature::Write_3D_Matrix_to_File(vector<vector<vector<int>>> matrix_3D, string matrix_name, string output_file, bool output_directory_number, bool spaces_between_directories);
template
void CCalculatingCurvature::Write_3D_Matrix_to_File(vector<vector<vector<double>>> matrix_3D, string matrix_name, string output_file, bool output_directory_number, bool spaces_between_directories);
template<typename data_type>
void CCalculatingCurvature::Write_3D_Matrix_to_File(vector<vector<vector<data_type>>> Matrix_3D, string matrix_name, string output_file, bool output_directory_number, bool spaces_between_directories) {
    fstream Data_Out;
    Data_Out = Preliminary_Output_File_Checks(output_file);

    for (int i = 0; i < Matrix_3D.size(); i++) { //For each 2D Matrix
        if (output_directory_number)
            Data_Out << i << "\n"; //Output 2D Matrix number
        if (Matrix_3D[i].empty()) //Skip empty directories
            continue;
        for (int j = 0; j < Matrix_3D[i].size(); j++) { //For each row
            if (Matrix_3D[i][j].empty()) //Skip empty rows
                continue;
            else {
                for (int k = 0; k < Matrix_3D[i][j].size(); k++) { //For each column
                    if (k < Matrix_3D[i][j].size() - 1) //If this isn't the last column
                        Data_Out << Matrix_3D[i][j][k] << "\t"; //Output the value and a tab
                    else if (k == Matrix_3D[i][j].size() - 1) //If this is the last column
                        Data_Out << Matrix_3D[i][j][k]; //Output the value only
                }
                Data_Out << "\n"; //Output a new line to start the new row
            }
        }
        if (spaces_between_directories)
            if (i < Matrix_3D.size() - 1) //If this isn't the last directory
                Data_Out << "\n"; //Output two new lines at the end of each directory
    }

    //Data_Out.close();
    cout << matrix_name << " written to file successfully!" << endl;
}

template
void CCalculatingCurvature::Write_4D_Matrix_to_File(vector<vector<vector<vector<int>>>> matrix_4D, string matrix_name, string output_file);
template
void CCalculatingCurvature::Write_4D_Matrix_to_File(vector<vector<vector<vector<double>>>> matrix_4D, string matrix_name, string output_file);
template<typename data_type>
void CCalculatingCurvature::Write_4D_Matrix_to_File(vector<vector<vector<vector<data_type>>>> Matrix_4D, string matrix_name, string output_file) {
    fstream Data_Out;
    Data_Out = Preliminary_Output_File_Checks(output_file);

    for (int i = 0; i < Matrix_4D.size(); i++) { //For each single cube
        if (Matrix_4D[i].empty()) //Skip empty cubes
            continue;
        for (int j = 0; j < Matrix_4D[i].size(); j++) { //For each triangle
            for (int k = 0; k < Matrix_4D[i][j].size(); k++) { //For each vertex of the triangle
                for (int l = 0; l < Matrix_4D[i][j][k].size(); l++) { //For each coordinate of a vertex
                    if (l < Matrix_4D[i][j][k].size() - 1) //If this isn't the last column
                        Data_Out << Matrix_4D[i][j][k][l] << "\t"; //Output the value and a tab
                    else if (l == Matrix_4D[i][j][k].size() - 1) //If this is the last column
                        Data_Out << Matrix_4D[i][j][k][l]; //OUtput the value only
                }
                if (k < Matrix_4D[i][j].size() - 1) //If this isn't the last vertex
                    Data_Out << "\n"; //Output a new line to start the new row
            }
            if (j < Matrix_4D[i].size() - 1) //If this isn't the last triangle
                Data_Out << "\n\n" << endl; //Output two new lines at the end of each triangle
        }
        if (i < Matrix_4D.size() - 1) //If this isn't the last cube
            Data_Out << "\n\n\n"; //Output three new lines at the end of each cube
    }

    //Data_Out.close();;
    cout << matrix_name << " written to file successfully!" << endl;
}

template
void CCalculatingCurvature::Write_List_to_Screen(vector<int> list);
template
void CCalculatingCurvature::Write_List_to_Screen(vector<double> list);
template<typename data_type>
void CCalculatingCurvature::Write_List_to_Screen(vector<data_type> list) {
    if (list.size() <= 1) {
        cout << "List size is too small!\nSize is: " << list.size() << endl;
        exit(1);
    }

    cout << "[";
    for (int i = 0; i < list.size(); i++) {
        if (i != list.size() - 1)
            cout << list[i] << ",\t";
        if (i == list.size() - 1)
            cout << list[i] << "]" << endl << endl;
    }
}

template
void CCalculatingCurvature::Write_2D_Matrix_to_Screen<int>(vector<vector<int>> matrix);
template
void CCalculatingCurvature::Write_2D_Matrix_to_Screen<double>(vector<vector<double>> matrix);
template<typename data_type>
void CCalculatingCurvature::Write_2D_Matrix_to_Screen(vector<vector<data_type>> matrix) {
    cout << "[\t";											//Output '[' to start matrix off, and then tab to line up elements

    int rows = matrix.size();
    for (int i = 0; i < rows; i++)							//For each row
    {
        cout << "Row " << i << ":\t";						//Output row number
        int columns = matrix[i].size();
        if (i + 1 == rows)									//For the last row (i + 1 because matrix indexing starts from 0 but Cmatrix::rows starts from 1)
        {
            for (int j = 0; j < columns; j++)				//For each element
            {
                if (j + 1 != columns)						//If the element is not the last element output is normal (the number followed by a comma and a tab)
                    cout << matrix[i][j] << ",\t";
                else if (j + 1 == columns)					//If the element is the last element output the number (with no comma) and a tab
                    cout << matrix[i][j] << "\t";
            }
        }
        else												//For every other row
        {
            for (int j = 0; j < columns; j++)				//For each element
            {
                cout << matrix[i][j] << ",\t";				//Output is normal (the number followed by a comma and a tab)
            }
        }

        if (i + 1 < rows) cout << "\n\t";					//If its not the final row, start a new line (and tab to line up first element)		
    }

    cout << "]" << endl << endl;							//Output ']' to end matrix
}

template
void CCalculatingCurvature::Write_3D_Matrix_to_Screen(vector<vector<vector<int>>> matrix_3D);
template
void CCalculatingCurvature::Write_3D_Matrix_to_Screen(vector<vector<vector<double>>> matrix_3D);
template<typename data_type>
void CCalculatingCurvature::Write_3D_Matrix_to_Screen(vector<vector<vector<data_type>>> matrix_3D) {
    for (int i = 0; i < matrix_3D.size(); i++) { //For each layer
        cout << "Layer " << i << ":" << endl << "["; //Output layer number and [ to start layer
        for (int j = 0; j < matrix_3D[i].size(); j++) { //For each row
            cout << "\tRow " << j << ":\t";   //Output row number
            for (int k = 0; k < matrix_3D[i][j].size(); k++) { //For each column
                if (k < matrix_3D[i][j].size() - 1) //If this isn't the last column
                    cout << matrix_3D[i][j][k] << ",\t"; //Output the value and a tab
                else if (k == matrix_3D[i][j].size() - 1) //If this is the last column
                    cout << matrix_3D[i][j][k]; //Output the value only
            }
            if (j != matrix_3D[i].size() - 1) //If this isn't the last row
                cout << "\n"; //Output a new line to start the new row
        }
        cout << "]"; //Output square bracket to end layer
        if (i < matrix_3D.size() - 1) //If this isn't the last layer
            cout << "\n\n"; //Output two new lines at the end of each layer
    }
}


//Not working - fails to store properly in matrix_store
template
void CCalculatingCurvature::Read_2D_Matrix_from_File<int>(string input_file, vector<vector<int>> matrix_store, string matrix_name);
template
void CCalculatingCurvature::Read_2D_Matrix_from_File<double>(string input_file, vector<vector<double>> matrix_store, string matrix_name);
template<typename data_type>
void CCalculatingCurvature::Read_2D_Matrix_from_File(string input_file, vector<vector<data_type>> matrix_store, string matrix_name) {
    cout << "Begin reading in " << matrix_name << " from file..." << endl;

    //Declare initial local vairables
    ifstream Data_In;						//Declare the stream that will input the data
    string LineIn;							//Declare string for that will contain the line being read in from the file

    Data_In.open(input_file, fstream::in);

    if (Data_In.fail()) {
        cout << "Error opening file" << endl << endl;
        exit(0);
    }

    //Declare subsequent local variables
    vector<vector<data_type>> TempMatrix;					//Used for reading in data

    //Next step - store elements in TempMatrix[i][j]
    while (getline(Data_In, LineIn)) {						//While there are lines to read in from the file, store them in Line_In
        istringstream Input_String_Stream(LineIn);			//Copy LineIn to an Input String Stream
        string Word;										//Create a string that stores each word
        data_type ElementValue;								//Create a temporary variable to store the words (which are strings) as doubles
        vector<data_type>TempVector;						//Create a temporary vector to store the doubles/ints for each value for each line (as opposed to the strings for each word for each line)
        while (getline(Input_String_Stream, Word, '\t')) {	//Split LineIn (which is the Input_String_Stream) into Words (seperated by a tab delimiter)
            ElementValue = stod(Word);                      //Should work for ints as well, as ElementValue is an integer
            TempVector.push_back(ElementValue);				//Store the double/int in TempVector
        }
        TempMatrix.push_back(TempVector);					//Add TempVector onto TempMatrix
    }

    matrix_store.clear();
    matrix_store.resize(TempMatrix.size());
    for (int i = 0; i < matrix_store.size(); i++) {
        matrix_store[i].resize(TempMatrix[i].size());
        for (int j = 0; j < matrix_store[i].size(); j++)
            matrix_store[i][j] = TempMatrix[i][j];
    }

    Write_2D_Matrix_to_Screen(matrix_store);

    cout << matrix_name << " read in from file successfully, with size of [" << matrix_store[0].size() << ", " << matrix_store.size() << "] (x, y)." << endl;
}

/*
template
vector<vector<vector<int>>> CCalculatingCurvature::Read_3D_Matrix_from_File<int>(string input_file, string matrix_name, int);
template
vector<vector<vector<double>>> CCalculatingCurvature::Read_3D_Matrix_from_File<double>(string input_file, string matrix_name, double);
template<typename data_type>
vector<vector<vector<data_type>>> CCalculatingCurvature::Read_3D_Matrix_from_File(string input_file, string matrix_name, data_type) {
    cout << "Begin reading in " << matrix_name << " from file..." << endl;
    fstream Data_In;
    string Line_In;

    Data_In.open(input_file, fstream::in);
    if (Data_In.fail()) {
        cout << "Error opening file for reading" << endl << endl;
        exit(0);
    }
    else
        cout << "File opened for reading successfully" << endl;

    vector<vector<vector<data_type>>> Temp_3D_Dataset;
    vector<vector<data_type>> Temp_2D_Layer;

    //Read in each directory
    while (getline(Data_In, Line_In)) {					    	//While there are lines to read in from the file, store them in Line_In
        if (Line_In.empty()) {
            Temp_3D_Dataset.push_back(Temp_2D_Layer);
            Temp_2D_Layer.clear();
        }
        else {
            istringstream Input_String_Stream(Line_In);			//Copy Line_In to an Input String Stream
            string Word;										//Create a string that stores each word
            vector<data_type> Temp_Vector;                      //Create a temporary vector to store the doubles/ints for each value for each line (as opposed to the strings for each word for each line)
            while (getline(Input_String_Stream, Word, '\t')) {  //Split LineIn (which is the Input_String_Stream) into Words (seperated by a tab delimiter)
                data_type ElementValue;	    					//Create a temporary variable to store the words (which are strings) as doubles/ints
                if (data_type == int)
                    ElementValue = stoi(Word);				    //Convert the word into an int
                if (data_type == double)
                    ElementValue = stod(Word);				    //Convert the word into a double
                Temp_Vector.push_back(ElementValue);		    //Store the doubles/ints in Temp_Vector
            }
            Temp_2D_Layer.push_back(Temp_Vector);               //Store the Temp_Vector in Temp_2D_Layer
        }
    }
    //Flush last directory to
    Temp_3D_Dataset.push_back(Temp_2D_Layer);
    Temp_2D_Layer.clear();

    cout << matrix_name << " read in from file successfully, with size of [" << Temp_3D_Dataset[0][0].size() << ", " << Temp_3D_Dataset[0].size() << ", " << Temp_3D_Dataset.size() << "] (x, y, z)." << endl;
    return Temp_3D_Dataset;
}*/

vector<int> CCalculatingCurvature::Read_Pixel_Values_from_Text_File(string input_file) {
    cout << "Begin reading pixel values from file..." << endl;
    fstream Data_In;
    string Line_In;

    Data_In.open(input_file, fstream::in);
    if (Data_In.fail()) {
        cout << "Error opening file for reading" << endl << endl;
        exit(0);
    }
    else
        cout << "File opened for reading successfully" << endl;

    vector<vector<vector<int>>> Temp_3D_Dataset;
    vector<vector<int>> Temp_Directory;

    //Read in each directory
    while (getline(Data_In, Line_In)) {						//While there are lines to read in from the file, store them in Line_In
        if (Line_In.empty()) {
            Temp_3D_Dataset.push_back(Temp_Directory);
            Temp_Directory.clear();
        }
        else {
            istringstream Input_String_Stream(Line_In);			//Copy Line_In to an Input String Stream
            string Word;										//Create a string that stores each word
            vector<int> Temp_Vector;                            //Create a temporary vector to store the doubles for each value for each line (as opposed to the strings for each word for each line)
            while (getline(Input_String_Stream, Word, '\t')) {  //Split LineIn (which is the Input_String_Stream) into Words (seperated by a tab delimiter)
                int ElementValue;	    						//Create a temporary variable to store the words (which are strings) as doubles
                ElementValue = stoi(Word);				       	//Convert the word into an int
                Temp_Vector.push_back(ElementValue);		    //Store the int in Temp_Vector
            }
            Temp_Directory.push_back(Temp_Vector);
        }
    }
    //Flush last directory to 
    Temp_3D_Dataset.push_back(Temp_Directory);
    Temp_Directory.clear();
    cout << "Data read in from file successfully" << endl;

    pixel_values.resize(Temp_3D_Dataset.size());
    for (int i = 0; i < pixel_values.size(); i++) {
        pixel_values[i].resize(Temp_3D_Dataset[i].size());
        for (int j = 0; j < pixel_values[i].size(); j++)
            pixel_values[i][j].insert(begin(pixel_values[i][j]), begin(Temp_3D_Dataset[i][j]), end(Temp_3D_Dataset[i][j]));
    }

    vector<int> dataset_dimensions;
    dataset_dimensions.push_back(Temp_3D_Dataset.size());
    dataset_dimensions.push_back(Temp_3D_Dataset[0].size());
    dataset_dimensions.push_back(Temp_3D_Dataset[0][0].size());

    cout << "Finished reading pixel values from file." << endl << endl;
    return dataset_dimensions;
}

vector<vector<double>> CCalculatingCurvature::Read_2D_Double_Matrix_from_Text_File(string input_file, string matrix_name) {
    cout << "Begin reading in " << matrix_name << " from file..." << endl;

    //Declare initial local vairables
    ifstream Data_In;						//Declare the stream that will input the data
    string LineIn;							//Declare string for that will contain the line being read in from the file

    Data_In.open(input_file, fstream::in);
    if (Data_In.fail()) {
        cout << "Error opening file" << endl << endl;
        exit(0);
    }

    //Declare subsequent local variables
    vector<vector<double>> TempMatrix;					    //Used for reading in data

    //Next step - store elements in TempMatrix[i][j]
    while (getline(Data_In, LineIn)) {						//While there are lines to read in from the file, store them in Line_In
        istringstream Input_String_Stream(LineIn);			//Copy LineIn to an Input String Stream
        string Word;										//Create a string that stores each word
        double ElementValue;							    //Create a temporary variable to store the words (which are strings) as doubles
        vector<double>TempVector;					        //Create a temporary vector to store the doubles for each value for each line (as opposed to the strings for each word for each line)
        while (getline(Input_String_Stream, Word, '\t'))	//Split LineIn (which is the Input_String_Stream) into Words (seperated by a tab delimiter)
        {
            ElementValue = stod(Word);
            TempVector.push_back(ElementValue);				//Store the double in TempVector
        }
        TempMatrix.push_back(TempVector);					//Add TempVector onto TempMatrix
    }

    cout << matrix_name << " read in from file successfully, with size of [" << TempMatrix[0].size() << ", " << TempMatrix.size() << "] (x, y)." << endl;
    return TempMatrix;
}

vector<vector<int>> CCalculatingCurvature::Read_2D_Integer_Matrix_from_Text_File(string input_file, string matrix_name) {
    cout << "Begin reading in " << matrix_name << " from file..." << endl;

    //Declare initial local vairables
    ifstream Data_In;						//Declare the stream that will input the data
    string LineIn;							//Declare string for that will contain the line being read in from the file

    Data_In.open(input_file, fstream::in);
    if (Data_In.fail()) {
        cout << "Error opening file" << endl << endl;
        exit(0);
    }

    //Declare subsequent local variables
    vector<vector<int>> TempMatrix;					         //Used for reading in data

    //Next step - store elements in TempMatrix[i][j]
    while (getline(Data_In, LineIn)) {						//While there are lines to read in from the file, store them in Line_In
        istringstream Input_String_Stream(LineIn);			//Copy LineIn to an Input String Stream
        string Word;										//Create a string that stores each word
        int ElementValue;						    	    //Create a temporary variable to store the words (which are strings) as integers
        vector<int> TempVector;                              //Create a temporary vector to store the integers for each value for each line (as opposed to the strings for each word for each line)
        while (getline(Input_String_Stream, Word, '\t')) {	//Split LineIn (which is the Input_String_Stream) into Words (seperated by a tab delimiter)
            ElementValue = stoi(Word);
            TempVector.push_back(ElementValue);				//Store the integer in TempVector
        }
        TempMatrix.push_back(TempVector);					//Add TempVector onto TempMatrix
    }

    cout << matrix_name << " read in from file successfully, with size of [" << TempMatrix[0].size() << ", " << TempMatrix.size() << "] (x, y)." << endl;
    return TempMatrix;
}

void CCalculatingCurvature::Output_Data_to_File(void) {
    //Write_3D_Matrix_to_File<int>(pixel_values, "Pixel Values", "New Catenoids 3/Catenoid_1_0.35_0.1_Pixel_Values.txt", false, true);
    //Write_3D_Matrix_to_File<double>(isosurface_coordinates, "Isosurface Coordinates", "New Catenoids 3/Catenoid_1_0.35_0.1_Isosurface_Coordinates.txt", false, false);
    //Write_4D_Matrix_to_File<double>(triangles, "Triangles", "New Catenoids 3/Catenoid_1_0.35_0.1_Triangles.txt");
    Write_2D_Matrix_to_File<int>(faces, "Faces", "Real Datasets/Water-Wet/Bentheimer 49x44x34 Oil-Water Interface/Oil-Water_Updated_Faces.txt");
    Write_2D_Matrix_to_File<double>(vertices, "Vertices", "Real Datasets/Water-Wet/Bentheimer 49x44x34 Oil-Water Interface/Oil-Water_Updated_Vertices.txt");
    //Write_List_to_File<double>(gaussian_curvature, "Gaussian Curvature", "New Sphere Data/Sphere_83_Gaussian_Curvature.txt");
    //Write_List_to_File<double>(normalised_gaussian_curvature, "Normalised_Gaussian_Curvature", "New Sphere Data/Sphere_83_Normalised_Gaussian_Curvature.txt");    
}

void CCalculatingCurvature::Append_Continuous_Catneoid_Curvature_Output_to_File(int z_max, double alpha, double voxel_size, int gaussian_iterations, int laplacian_iterations, string output_file, int output_faces_frequency) {
    fstream file;
    file.open(output_file, fstream::out | fstream::in);  //Won't create file
    if (file.is_open()) { //If file already exists
        //file.clear();
        //file.open(output_file, fstream::app | fstream::out); //Open in append mode
        file.close();
        ofstream out_file;
        out_file.open(output_file, ofstream::app);
        if (out_file.fail()) {
            cout << "Error opening file for writing!" << endl << endl;
            exit(0);
        }
        else {
            cout << "File opened for writing (in append mode) successfully..." << endl;
            //Output catenoid details
            out_file << z_max << "\t" << alpha << "\t" << voxel_size << "\t" << gaussian_iterations << "\t" << laplacian_iterations << "\t";
            //Calculate overall curvature results
            double mean_of_mean_curvature = 0.0, mean_of_mean_curvature_excluding_top_and_bottom_layers = 0.0, mean_gaussian_curvature = 0.0, mean_gaussian_curvature_excluding_top_and_bottom_layers = 0.0;
            Calculate_Face_Normals();
            Calculate_Vertice_Normals();
            Calculate_Curvature(true, output_faces_frequency);
            Get_Principal_Curvatures();

            //Calcualte specific curvature results
            mean_of_mean_curvature_excluding_top_and_bottom_layers = Calculate_Mean_Curvature_for_Catenoids(z_max, voxel_size);
            mean_of_mean_curvature = Calculate_Mean_of_Doubles_List_and_Remove_NANs(mean_curvature);
            mean_gaussian_curvature_excluding_top_and_bottom_layers = Calculate_Gaussian_Curvature_for_Catenoids(z_max, voxel_size);
            mean_gaussian_curvature = Calculate_Mean_of_Doubles_List_and_Remove_NANs(gaussian_curvature);

            //Output specific results
            out_file << mean_of_mean_curvature << "\t" << mean_of_mean_curvature_excluding_top_and_bottom_layers << "\t" << mean_gaussian_curvature << "\t" << mean_gaussian_curvature_excluding_top_and_bottom_layers << "\n";
        }
    }
    else { // If file doesn't already exist
        cout << "File doesn't exist. Enter valid name of file to append to or create file manually to ensure correct placement of data output." << endl;
        exit(0);
    }

    cout << "Results written (in append mode) to file successfully." << endl;
}

void CCalculatingCurvature::Append_Continuous_Sphere_Curvature_Output_to_File(int input_dataset_size, int gaussian_iterations, int laplacian_iterations, string output_file, int output_faces_frequency) {
    fstream file;
    file.open(output_file, fstream::out | fstream::in);  //Won't create file
    if (file.is_open()) { //If file already exists
        //file.clear();
        //file.open(output_file, fstream::app | fstream::out); //Open in append mode
        file.close();
        ofstream out_file;
        out_file.open(output_file, ofstream::app);
        if (out_file.fail()) {
            cout << "Error opening file for writing!" << endl << endl;
            exit(0);
        }
        else {
            cout << "File opened for writing (in append mode) successfully..." << endl;
            //Calculate sphere radius
            double centre_coordinate, radius;
            centre_coordinate = input_dataset_size / 2; //Integer maths
            radius = (input_dataset_size - centre_coordinate - 2) + 1 - vertices[0][2]; //Original radius (with gap/voxel size both of 1) + 1 - top vertice's height post smoothing
            //Output catenoid details
            out_file << input_dataset_size << "\t" << radius << "\t" << gaussian_iterations << "\t" << laplacian_iterations << "\t";

            //Curvature Calculation
            double mean_of_mean_curvature = 0.0, mean_gaussian_curvature = 0.0, mean_normalised_gaussian_curvature = 0.0;
            Calculate_Face_Normals();
            Calculate_Vertice_Normals();
            Calculate_Curvature(true, 1000);
            Get_Principal_Curvatures();
            Calculate_Gaussian_Curvatures();
            Calculate_Mean_Curvatures();

            //Calcualte specific curvature results
            mean_of_mean_curvature = Calculate_Mean_of_Doubles_List_and_Remove_NANs(mean_curvature);
            mean_gaussian_curvature = Calculate_Mean_of_Doubles_List_and_Remove_NANs(gaussian_curvature);
            Normalise_Gaussian_Curvature_of_Sphere(radius);
            mean_normalised_gaussian_curvature = Calculate_Mean_of_Doubles_List_and_Remove_NANs(normalised_gaussian_curvature);

            //Output specific results
            out_file << mean_of_mean_curvature << "\t" << mean_gaussian_curvature << "\t" << mean_normalised_gaussian_curvature << "\n";
        }
    }
    else { // If file doesn't already exist
        cout << "File doesn't exist. Enter valid name of file to append to or create file manually to ensure correct placement of data output." << endl;
        exit(0);
    }

    cout << "Results written (in append mode) to file successfully." << endl;
}