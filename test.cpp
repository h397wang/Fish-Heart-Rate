
#include <stdio.h>
#include "test.h"
#include "cv.h"
#include "highgui.h"

void test() {
    printf("test\n");
}

// Assumes img is a 3 channel type
void printData( IplImage* img ) {
	for( int row = 0; row < img->height; row++ ) {
		uchar* ptr = (uchar*) (
			img->imageData + row * img->widthStep
			);
		for( int col = 0; col < img->width; col++ ) {

			if (ptr[3*col] != 0) {
				printf("RGB: (%d, %d, %d)\n",
					ptr[3*col],
					ptr[3*col+1],
					ptr[3*col+2]
				);
			}
		}
	}
}


void printFrameInfo(IplImage* img) {
	printf("nChannels: %d\n", img->nChannels);
	printf("alphaChannel: %d\n", img->alphaChannel);
	printf("depth: %d\n", img->depth);
	printf("width: %d\n", img->width);
	printf("height: %d\n", img->height);
}

// This function should calculate the ROI somehow too.
void setROI(IplImage* img) {
	// hard code these values until they can be determined dynamically.
	const int x = 100;
	const int y = 100;
	const int width = 100;
	const int height = 100; 
	cvSetImageROI(img, cvRect(x, y, width, height));
}

/*
img may be 3 channels for now, until I figure out how to convert to grayscale
*/
double getAverageIntensity(IplImage* img) {
	double ret = 0;
	double sum = 0;
	if (img->nChannels == 3) {
		for( int row = 0; row < img->height; row++ ) {
			uchar* ptr = (uchar*) (img->imageData + row * img->widthStep);
			for( int col = 0; col < img->width; col++ ) {
				// Just use the average method
				sum += ptr[3*col];
				sum += ptr[3*col+1];
				sum += ptr[3*col+2];
			}
		}

		ret = ( sum / (double) (img->height * img->width * 3) );

	}
	return ret;
}