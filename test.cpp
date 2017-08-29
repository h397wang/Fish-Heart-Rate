
#include <stdio.h>
#include <assert.h>

#include "test.h"
#include "cv.h"
#include "highgui.h"

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


// is the optimization really neccesary here?
// may be done in place I suppose
// apparently i cant use column vectors?
void bandPassFilter(
		const CvMat* signal_timeDom, 
		CvMat* output, 
		int lowHarmIndexCutoff,
		int highHarmIndexCutoff ) {

	assert( signal_timeDom->rows == output->rows );
	//assert( output->rows == 1 );
	assert( output->cols == signal_timeDom->cols );

	const int opDFTSize = cvGetOptimalDFTSize( signal_timeDom->cols );
	CvMat* signal_timeDom_padded = cvCreateMat( signal_timeDom->rows, opDFTSize, signal_timeDom->type );
	CvMat subMat;
	cvGetSubRect(
		signal_timeDom_padded,
		&subMat,
		cvRect(
			0,
			0,
			signal_timeDom->cols,
			signal_timeDom->rows ) );
	cvCopy( signal_timeDom, &subMat );

	cvGetSubRect(
		signal_timeDom_padded,
		&subMat,
		cvRect(
			signal_timeDom->cols, 
			0, 
			signal_timeDom_padded->cols - signal_timeDom->cols,
			signal_timeDom->rows ) );
	cvZero( &subMat );

	CvMat* signal_freqDom_padded = cvCreateMat( signal_timeDom->rows, opDFTSize, CV_32FC1 ); 

	cvDFT( signal_timeDom_padded, signal_freqDom_padded, CV_DXT_FORWARD);

	// Iterate the proper way incase the vector is not contiguous
	// See page 178 for the encoding of the DFT output
	// TODO: This method isn't very efficient.
	for (int row = 0; row < signal_freqDom_padded->rows; row++ ) { 
		float* ptr = (float*)(signal_freqDom_padded->data.ptr + row * signal_freqDom_padded->step);
		for(int col = 0; col < signal_freqDom_padded->cols; col++ ) {
			const int harmonicIndex = (col + 1) / 2; // intrinsic floor division 
			if ( harmonicIndex < lowHarmIndexCutoff || harmonicIndex > highHarmIndexCutoff ) {
				*ptr = 0.0f;
			}
			ptr++;
		}
	}

	cvDFT( signal_freqDom_padded, signal_timeDom_padded, CV_DXT_INV_SCALE);
	cvGetSubRect( signal_timeDom_padded, &subMat, cvRect( 0, 0, signal_timeDom->cols, signal_timeDom->rows ) );
	cvCopy( &subMat, output );

	cvReleaseMat( &signal_timeDom_padded );
	cvReleaseMat( &signal_freqDom_padded );
}


void calcPower( const CvMat* signal_timeDom, CvMat* output ) { // 2d array
	//CvMat* signalPowers = cvCreateMat( signal_timeDom->rows, 1, CV_32FC1 ); 
	// assertions
	assert( signal_timeDom->rows == output->rows );
	for (int row = 0; row < signal_timeDom->rows; row++ ) { 
		float* ptr = (float*)(signal_timeDom->data.ptr + row * signal_timeDom->step);
		float power = 0;
		for(int col = 0; col < signal_timeDom->cols; col++ ) {
			power += (*ptr) * (*ptr);
			ptr++;
		}
		float* outputPtr = (float*)(output->data.ptr + row * output->step);
		*outputPtr = power;
		//cvSet2D( output, row, 1, CvScalar( power / signal_timeDom->cols ) );
	}
}
