
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include "C:\Users\Pc\Downloads\stb_image.h"
using namespace std;

bool pixelMatch(unsigned char* source, unsigned char* target, int sourceIdx, int targetIdx) {
    for (int k = 0; k < 4; k++) {
        if (source[sourceIdx + k] != target[targetIdx + k])
            return false;
    }
    return true;
}

void rotates(unsigned char* T_data, int& T_width, int& T_height) {

    unsigned char* temp = new unsigned char[T_width * T_height * 4];
    for (int i = 0; i < T_width * T_height * 4; i++) {
        temp[i] = T_data[i];
    }

    int originalWidth = T_width;
    int originalHeight = T_height;

    // swapping dimensions for rotation
    T_width = originalHeight;
    T_height = originalWidth;

    for (int y = 0; y < originalHeight; y++) {
        for (int x = 0; x < originalWidth; x++) {

            int original_index = (y * originalWidth + x) * 4;

            int new_x = y;
            int new_y = originalWidth - 1 - x;
            int new_index = (new_y * T_width + new_x) * 4;

            for (int channel = 0; channel < 4; channel++) {
                T_data[new_index + channel] = temp[original_index + channel];
            }
        }
    }

    delete[] temp;
}
int locateImage(unsigned char* source, int sourceWidth, int sourceHeight,
    unsigned char* target, int targetWidth, int targetHeight) {
    for (int y = 0; y <= sourceHeight - targetHeight; y++) {
        for (int x = 0; x <= sourceWidth - targetWidth; x++) {
            bool found = true;
            for (int ty = 0; ty < targetHeight && found; ty++) {
                for (int tx = 0; tx < targetWidth && found; tx++) {
                    int sourceIdx = ((y + ty) * sourceWidth + (x + tx)) * 4;
                    int targetIdx = (ty * targetWidth + tx) * 4;
                    if (!pixelMatch(source, target, sourceIdx, targetIdx)) found = false;
                }
            }
            if (found) return (y * sourceWidth + x) * 4;
        }
    }
}

void flipImage(unsigned char* T_data, int width, int height) {
    for (int y = 0; y < height; y++) {
        unsigned char* row = T_data + y * width * 4; //start of current row

        //swap pixels within the row from left to right
        for (int x = 0; x < width / 2; x++) {
            int oppositeX = width - 1 - x;
            int leftPixel = x * 4;         // left pixel position in row
            int rightPixel = oppositeX * 4; // right pixel position in row

            // Swap all 4 channels 
            for (int channel = 0; channel < 4; channel++) {
                unsigned char temp = row[leftPixel + channel];
                row[leftPixel + channel] = row[rightPixel + channel];
                row[rightPixel + channel] = temp;
            }
        }
    }
}
void flipImageV(unsigned char* T_data, int width, int height) {
    int rowSize = width * 4;
    unsigned char* tempRow = new unsigned char[rowSize];

    for (int y = 0; y < height / 2; y++) {
        int oppositeY = height - 1 - y;
        unsigned char* row = T_data + y * rowSize;
        unsigned char* oppositeRow = T_data + oppositeY * rowSize;


        for (int i = 0; i < rowSize; i++) {
            tempRow[i] = row[i];
            row[i] = oppositeRow[i];
            oppositeRow[i] = tempRow[i];
        }
    }
    delete[] tempRow;
}
void input(int& width, int& height, int& channels, int& T_width, int& T_height, int& T_channels, unsigned char*& dataIn, unsigned char*& T_dataIn) {

    cout << " enter width height and channels of original image" << endl;
    cin >> width >> height >> channels;
    cout << " enter width height and channels of target image" << endl;
    cin >> T_width >> T_height >> T_channels;
	dataIn = new unsigned char[width * height * channels];
	T_dataIn = new unsigned char[T_width * T_height * T_channels];
	cout << " enter pixel values of original image" << endl;
    for (int i = 0; i < width * height * channels; i++) {
        int pixelValue;
        cin >> pixelValue;
        dataIn[i] = static_cast<unsigned char>(pixelValue);
	}
	cout << " enter pixel values of target image" << endl;
    for (int i = 0; i < T_width * T_height * T_channels; i++) {
        int pixelValue;
        cin >> pixelValue;
        T_dataIn[i] = static_cast<unsigned char>(pixelValue);
	}
}

int main()
{
    const char* filename = "download.png";
    const char* target = "sword.png";
    /*const char *filename = "Document.png";
    const char* target = "subDocumentRot.png";
	const char* target = "subDocumentRev.png";*/

    int width, height, channels;
    int T_width, T_height, T_channels;

 //   unsigned char* dataIn;
	//unsigned char* T_dataIn;
 //   input(width, height, channels, T_width, T_height, T_channels, dataIn, T_dataIn);

    unsigned char *data = stbi_load(filename, &width, &height, &channels, 4);
    unsigned char *T_data = stbi_load(target, &T_width, &T_height, &T_channels, 4);

    if (!data || !T_data)
    {
        cerr << "Failed to load image\n";
        return 1;
    }
     //for printing sizes
    cout << "Image size: " << width << "x" << height << "\n";
    cout << "target size: " << T_width << "x" << T_height << endl;
    /*//for printing arrays (pixel values)
        for (int i = 0; i < width * height * 4; i++) {
            cout << (int)data[i] << " ";
		}
	cout << endl;
    for (int i = 0; i < T_width * T_height * 4; i++) {
		cout << (int)T_data[i] << " ";
    */
    int total_pixels = width * height;
    int targetPixels = T_width * T_height;

    //cout << " " << total_pixels << endl;

    int targetIndex = -1;
    targetIndex = locateImage(data, width, height, T_data, T_width, T_height);

    // test  rotations 90, 180, 270
    int rotateCount = 0;
    while (targetIndex == -1 && rotateCount < 4) {
        rotates(T_data, T_width, T_height);
        targetIndex = locateImage(data, width, height, T_data, T_width, T_height);
        rotateCount++;
    }

    //try horizontal flip + rotations
    if (targetIndex == -1) {
        flipImage(T_data, T_width, T_height); //horizontal flip
        targetIndex = locateImage(data, width, height, T_data, T_width, T_height);

        //test rotations after horizontal flip
        rotateCount = 0;
        while (targetIndex == -1 && rotateCount < 4) {
            rotates(T_data, T_width, T_height);
            targetIndex = locateImage(data, width, height, T_data, T_width, T_height);
            rotateCount++;
        }
    }

    //if still not found, try vertical flip
    if (targetIndex == -1) {
        flipImageV(T_data, T_width, T_height); 
        targetIndex = locateImage(data, width, height, T_data, T_width, T_height);

        rotateCount = 0;
        while (targetIndex == -1 && rotateCount < 4) {
            rotates(T_data, T_width, T_height);
            targetIndex = locateImage(data, width, height, T_data, T_width, T_height);
            rotateCount++;
        }
    }

    if (targetIndex == -1) {
        cout << "Target image not detected in the source";
        return 0;
    }
    else {

        // if its rotated 90 degrees then we r
        int x_coordinate = (targetIndex / 4) % width;
        int y_coordinate = (targetIndex / 4) / width;
        cout << "The target image is found at " << x_coordinate << "x and " << y_coordinate << "y, ("<<x_coordinate<<","<<y_coordinate<<") of the source image."<<endl;
		cout << " (" << x_coordinate + T_width << ", " << y_coordinate << ")  , (" << x_coordinate << ", " << y_coordinate + T_height << ") , (" << x_coordinate + T_width << ", " << y_coordinate + T_height << ") are the other corners of the target image." << endl;
       

    }
}
