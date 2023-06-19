#include "C_Calc_Curvature_Post_Project.h"
#include <iostream>

//Read in file
TIFF* CCalculatingCurvature::Read_In_TIFF_File(const char* input_file) {
    TIFF* tif_file;

    //Open file
    tif_file = TIFFOpen(input_file, "r");
    //Checks file has opened successfully
    if (tif_file)
        cout << "File opened successfully..." << endl;
    else {
        cout << "File failed to open!" << endl;
        exit(0);
    }

    return tif_file;
}

//Read pixel values into raster and print results
void CCalculatingCurvature::Print_File_Properties_and_Voxel_Values(TIFF* tif_file) {
    tdata_t buf;
    tstrip_t strip;
    uint32_t length, width;
    int dir_iterator = 0, total_directories = 0;

    TIFFGetField(tif_file, TIFFTAG_IMAGELENGTH, &length);
    TIFFGetField(tif_file, TIFFTAG_IMAGEWIDTH, &width);
    total_directories = TIFFNumberOfDirectories(tif_file);
    cout << "Number of directories: " << total_directories << endl;
    cout << "Number of strips: " << TIFFNumberOfStrips(tif_file) << endl;
    cout << "Strip size: " << TIFFStripSize(tif_file) << endl;
    cout << "Length: " << length << endl;
    cout << "Width: " << width << endl;


    buf = _TIFFmalloc(TIFFStripSize(tif_file));
    do {
        for (strip = 0; strip < TIFFNumberOfStrips(tif_file); strip++) {
            TIFFReadEncodedStrip(tif_file, strip, buf, (tsize_t)-1);
            uint16_t* p16;
            p16 = (uint16_t*)buf;
            //printf("Strip: %d\n", strip);
            if (dir_iterator == 18) {
                for (int i = 0; i < length; i++) {      // the strip buffer is *length* elements long
                    for (int j = 0; j < width; j++)
                        printf("%d: %u\t", i + 1, p16[(i * width) + j]);
                    cout << endl;
                }
            }
        }
        dir_iterator++;
    } while (TIFFReadDirectory(tif_file));

    _TIFFfree(buf);
}

//Close file
void CCalculatingCurvature::Close_File(TIFF* tif_file) {
    TIFFClose(tif_file);
    cout << "File closed successfully." << endl;
}

//Read pixel values into raster and print results
vector<int> CCalculatingCurvature::Print_File_Properties_and_Save_Voxel_Values(TIFF* tif_file) {
    cout << "Saving voxel image data and printing file properties..." << endl;

    tdata_t buf;
    tstrip_t strip;
    uint32_t length, width;
    int dir_iterator = 0, total_directories = 0;

    TIFFGetField(tif_file, TIFFTAG_IMAGELENGTH, &length);
    TIFFGetField(tif_file, TIFFTAG_IMAGEWIDTH, &width);
    total_directories = TIFFNumberOfDirectories(tif_file);
    cout << "Number of directories: " << total_directories << endl;
    cout << "Number of strips: " << TIFFNumberOfStrips(tif_file) << endl;
    cout << "Strip size: " << TIFFStripSize(tif_file) << endl;
    cout << "Length: " << length << endl;
    cout << "Width: " << width << endl;


    buf = _TIFFmalloc(TIFFStripSize(tif_file));
    pixel_values.resize(total_directories);
    do {
        pixel_values[dir_iterator].resize(length);
        for (strip = 0; strip < TIFFNumberOfStrips(tif_file); strip++) {
            TIFFReadEncodedStrip(tif_file, strip, buf, (tsize_t)-1);
            uint16_t* p16;
            p16 = (uint16_t*)buf;
            //printf("Strip: %d\n", strip);
            for (int i = 0; i < length; i++) {      // the strip buffer is *length* elements long
                pixel_values[dir_iterator][i].resize(width);
                for (int j = 0; j < width; j++)
                    pixel_values[dir_iterator][i][j] = p16[(i * width) + j];
            }
        }
        dir_iterator++;
    } while (TIFFReadDirectory(tif_file));
    _TIFFfree(buf);

    vector<int> dataset_size;
    dataset_size.push_back(total_directories);
    dataset_size.push_back(length);
    dataset_size.push_back(width);

    cout << "Voxel image data retrieved successfully!" << endl;
    return dataset_size;
}