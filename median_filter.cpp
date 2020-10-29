//The program can open P2 and P5 PGM files but only output P2 format

/*

#include <fstream>
#include <string>
#include <stdlib.h>
#include <cstdio>
#include <math.h>
#include <stdio.h>
*/

#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

ifstream in_file;
ofstream out_file;

//Parameters
char sort_type;
char* infile = (char*)calloc(25, sizeof(char));
char* outfile = (char*)calloc(25, sizeof(char));
//Filter size
int filter_size;
int filter_half;
//Image parameters
char* img_type = (char*)calloc(2, sizeof(char));
int img_w;
int img_h;
int big_img_w;
int big_img_h;
int img_maxV;
//Image
int **image;
int **out_image;

void ReadImage()
{
    //Reading file type
    in_file >> img_type[0] >> img_type[1];

    //Reading and ignoring comments
    char dz = ' ';
    string com;

    in_file >> dz;
    if(dz == '#')
    {
        getline(in_file, com);
        in_file >> img_w >> img_h >> img_maxV;
    }
    else
    {
        in_file >> img_w >> img_h >> img_maxV;

        int l = 0;
        int w_ = img_w;
        while(w_ != 0)
        {
            l++;
            w_ /= 10;
        }
        img_w += ((int)(dz) - (int)('0'))*100;
    }

    cout << "\n\n"
    << "Image info:\n"
    << "Image type: " << img_type[0]<<img_type[1]<<"\n"
    << "Image size (width height): "<<img_w<<" "<<img_h<<"\n"
    << "The maximum gray value: "<<img_maxV<<"\n\n";


    big_img_w = img_w + filter_size - 1;
    big_img_h = img_h + filter_size - 1;

    //Allocating memory
    image = (int**)calloc(img_h + filter_size - 1, sizeof(int*));
    for(int i = 0; i < img_h + filter_size - 1; i++)
        image[i] = (int*)calloc(img_w + filter_size - 1, sizeof(int));

    out_image = (int**)calloc(img_h, sizeof(int*));
    for(int i = 0; i < img_h + 1; i++)
        out_image[i] = (int*)calloc(img_w, sizeof(int));

    if(img_type[1] == '5')
    {
        img_type[1] = '2';

        for(int i = filter_half; i < img_w + filter_half ; i++){
            for(int j = filter_half; j < img_h + filter_half ; j++){
                unsigned char lo;
                unsigned char hi;
                int val;
                // assuming the file stores them "big endian"

                if(img_maxV > 256)
                {
                    in_file.read(reinterpret_cast<char*>(&hi), 1);
                    in_file.read(reinterpret_cast<char*>(&lo), 1);
                    val = (hi << 8) | lo;
                }
                else
                {
                    in_file.read(reinterpret_cast<char*>(&hi), 1);
                    val = (int)hi;
                }

                image[i][j] = val;
            }
        }
    }
    else
    {
        for(int i = filter_half; i < img_w + filter_half ; i++)
            for(int j = filter_half; j < img_h + filter_half ; j++)
                in_file >> image[i][j];

    }

}

void Border()
{
//Copying the values from the edges to expand the image
//Left
    for(int i = filter_half; i < big_img_h; i++)
        for(int j = 0; j < filter_half; j++)
            image[i][j] = image[i][filter_half];

//Right
    for(int i = filter_half; i < big_img_h; i++)
        for(int j = img_w + filter_half; j < big_img_w; j++)
            image[i][j] = image[i][img_w + filter_half - 1];

//Up
    for(int i = 0; i < filter_half; i++)
        for(int j = 0; j < big_img_w; j++)
            image[i][j] = image[filter_half][j];

//Down
    for(int i = img_h; i < big_img_h; i++)
        for(int j = 0; j < big_img_w; j++)
            image[i][j] = image[filter_half][j];
}

int Filter(int x, int y)
{
    int pix_val[filter_size * filter_size];
    int pix_index = 0;

    //Getting the pixels values from the (x, y) point and around it
    for(int i = -filter_half; i <= filter_half; i++)
    {
        for(int j = -filter_half; j <= filter_half; j++)
        {
            pix_val[pix_index] = image[filter_half + x + i][filter_half + y + j];
            pix_index++;
        }
    }


    //The sort is applied to select the center value
    //Merge sort
    if(sort_type == 'm')
    {
        sort(pix_val, pix_val + (filter_size * filter_size));
    }
    //Bubble sort
    else if(sort_type == 'b')
    {
        for(int i = 0; i < filter_size * filter_size; i++)
        {
            for(int j = i; j < filter_size * filter_size; j++)
            {
                if(pix_val[j] > pix_val[i])
                {
                    swap(pix_val[j], pix_val[i]);
                }
            }
        }
    }

    //Returning the average value for the (x, y) point
    return(pix_val[(filter_size * filter_size)/2 + 1]);
}

void WriteImage()
{
    //Writing the file header
    out_file<<img_type[0]<<img_type[1]<<"\n"
    <<img_w<<" "<<img_h<<"\n"
    <<img_maxV<<"\n";

    //Writing the pixels values
    for(int i = 0; i <= img_w - 1; i++)
    {
        for(int j = 0; j <= img_h - 1; j++)
        {
            out_image[i][j] = Filter(i,j);
            out_file<<out_image[i][j]<<" ";
        }
        out_file<<"\n";
    }


}

void FreeMemory()
{
    for(int i = 0; i < img_w + filter_size - 1; i++)
        free(image[i]);
    free(image);

    for(int i = 0; i < img_w; i++)
        free(out_image[i]);
    free(out_image);

    free(infile);
    free(outfile);
    free(img_type);

    in_file.close();
    out_file.close();
}

int main()
{
    //Code info
    cout << "The median filter is a non-linear digital filtering technique, often used to remove noise from an image.\n\n";
    cout << "Enter the type of sort: m - merge sort, b - bubble sort;\n" <<
    "the size of the filter (must be an odd number bigger than 3),\n" <<
    "and the names of input and output files (must be a .pgm file).\n\n" <<
    "<sort type> <filter size> <input file> <output file>\n\n";

    int tempval = 0;
    //Reading parameters
    scanf("%c %d %s %s", &sort_type, &tempval, infile, outfile);

    if(sort_type != 'm' || sort_type != 'b')
    {
        cout << "The sort type must be merge or bubble. It was set to merge.\n";
        sort_type = 'm';
    }
    if(tempval < 3)
    {
        cout << "The filter size cannot be lower than 3. It was set to 3.\n";
        tempval = 3;
    }
    else if(tempval%2 == 0)
    {
        tempval++;

        cout << "The filter size cannot be even. It was set to " <<
        tempval << ".\n";
    }
    //Setting filter dimension
    filter_size = tempval;
    filter_half = (filter_size - 1) / 2;
    //Opening and checking files
    cout << "\n\nOpening file...\n";
    in_file.open(infile, ios::in | ios::binary);
    out_file.open(outfile);
    if(in_file.is_open())cout << "Input file is open.\n";
    else {cout << "ERROR while opening the input file.\n"; return 0;}
    if(in_file.is_open())cout << "Output file is open.\n";
    else {cout << "ERROR while opening the output file.\n"; return 0;}
    //Reading the image
    ReadImage();
    //Setting the border values
    Border();
    //Applying the filter and writing the output image
    WriteImage();
    FreeMemory();
    return 0;
}
