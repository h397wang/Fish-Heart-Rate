
#include <stdio.h>
#include <assert.h>

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
kind of redundant
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

// Implement a fourier transform here, either 1024 points or 2048?

// Use DFT to accelerate the convolution of array A by kernel B.
// Place the result in array V.
//
void convolution(	
				const CvMat* mat, // Size: M1xN1
				const CvMat* filter, // Size: M2xN2, frequency domain
				CvMat* output // Size:(mat->rows+kernel->rows-1)x(mat->cols+kernel->cols-1)
				) {

}

// is the optimization really neccesary here?
// may be done in place I suppose
// apparently i cant use column vectors?
void bandPassFilter(
					const CvMat* signal_timeDom, // row vector 
					CvMat* output, 
					int lowHarmIndexCutoff,
					int highHarmIndexCutoff
					) {

	assert( signal_timeDom->rows == 1 );
	assert( output->rows == 1 );
	assert( output->cols == signal_timeDom->cols );

	int opDFTSize = cvGetOptimalDFTSize( signal_timeDom->cols );

	CvMat* signal_timeDom_padded = cvCreateMat( 1, opDFTSize, signal_timeDom->type );
	CvMat subMat;
	cvGetSubRect(
				signal_timeDom_padded,
				&subMat,
				cvRect(
					0,
					0,
					signal_timeDom->cols, // width
					signal_timeDom->rows // height
					)
				);
	cvCopy( signal_timeDom, &subMat );

	cvGetSubRect(
				signal_timeDom_padded,
				&subMat,
				cvRect(
					signal_timeDom->cols, // x is horizontal coordinate component
					0, // y is vertical coordinate component
					signal_timeDom_padded->cols - signal_timeDom->cols,
					signal_timeDom->rows) // should be 1
				);
	cvZero( &subMat );


	CvMat* signal_freqDom_padded = cvCreateMat( 1, opDFTSize, CV_32FC1 ); 

	cvDFT( signal_timeDom_padded, signal_freqDom_padded, CV_DXT_FORWARD);

	// Iterate the proper way incase the vector is not contiguous
	// See page 178 for the encoding of the DFT output
	float* ptr = (float*)(signal_freqDom_padded->data.fl);
	for(int col = 0; col < signal_freqDom_padded->cols; col++ ) {
		const int harmonicIndex = (col + 1) / 2; // intrinsic floor division 
		if ( harmonicIndex < lowHarmIndexCutoff || harmonicIndex > highHarmIndexCutoff ) {
			*ptr = 0.0f;
		}
		ptr++;
	}

	cvDFT( signal_freqDom_padded, signal_timeDom_padded, CV_DXT_INV_SCALE);
	cvGetSubRect( signal_timeDom_padded, &subMat, cvRect( 0, 0, signal_timeDom->cols, signal_timeDom->rows ) );
	cvCopy( &subMat, output );

	cvReleaseMat( &signal_timeDom_padded );
	cvReleaseMat( &signal_freqDom_padded );
}



