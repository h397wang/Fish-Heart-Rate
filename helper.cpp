
#include <stdio.h>
#include <assert.h>

#include "helper.h"
#include "cv.h"
#include "highgui.h"

/*
Data dump for image
*/
void printData( IplImage* img ) {
	for( int row = 0; row < img->height; row++ ) {
		uchar* ptr = (uchar*) ( img->imageData + row * img->widthStep );
		for( int col = 0; col < img->width; col++ ) {
			if (ptr[3*col] != 0) {
				printf( "RGB: (%d, %d, %d)\n",
					ptr[ 3 * col],
					ptr[ 3 * col + 1],
					ptr[ 3 * col + 2]
				);
			}
		}
	}
}

/*
Zeros the frequency components that are strictly greater/less than the cutoffs. 
CvMat* signalTimeDom 	- Discrete time signal, time axis runs over columns.
						  Channels run over rows.
CvMat* output 		 	- Preallocated memory for filtered discrete time signal
*/
void bandPassFilter(
	const CvMat* signalTimeDom, 
	CvMat* output, 
	const int lowHarmIndexCutoff,
	const int highHarmIndexCutoff
	)
{
	// Input and output matrices should have the same dimensions
	assert( signalTimeDom->rows == output->rows );
	assert( output->cols == signalTimeDom->cols );

	// Create matrix with padded columns to obtain suitable FFT size
	const int optDftSize = cvGetOptimalDFTSize( signalTimeDom->cols );
	CvMat* signalTimeDomZeroPadded = cvCreateMat( signalTimeDom->rows, optDftSize, signalTimeDom->type );
	CvMat subMat;
	cvGetSubRect(
		signalTimeDomZeroPadded,
		&subMat,
		cvRect(
			0,
			0,
			signalTimeDom->cols,
			signalTimeDom->rows
			) 
		);
	cvCopy( signalTimeDom, &subMat );

	cvGetSubRect(
		signalTimeDomZeroPadded,
		&subMat,
		cvRect(
			signalTimeDom->cols, 
			0, 
			signalTimeDomZeroPadded->cols - signalTimeDom->cols,
			signalTimeDom->rows 
			) 
		);
	cvZero( &subMat );

	// Allocate memory for spectrum matrix
	CvMat* signalFreqDom = cvCreateMat( signalTimeDom->rows, optDftSize, CV_32FC1 ); 

	// DFT
	cvDFT( signalTimeDomZeroPadded, signalFreqDom, CV_DXT_FORWARD);

	// Apply ideal band pass filter, see page 178 for the encoding of the DFT output
	for ( int row = 0; row < signalFreqDom->rows; row++ ) { 
		float* ptr = ( float* )( signalFreqDom->data.ptr + row * signalFreqDom->step );
		for( int col = 0; col < signalFreqDom->cols; col++ ) {
			const int harmonicIndex = (col + 1) / 2; // intrinsic integer floor division 
			if ( harmonicIndex < lowHarmIndexCutoff || harmonicIndex > highHarmIndexCutoff ) {
				*ptr = 0.0f;
			}
			ptr++;
		}
	}

	// IFT
	cvDFT( signalFreqDom, signalTimeDomZeroPadded, CV_DXT_INV_SCALE);
	cvGetSubRect(
		signalTimeDomZeroPadded,
		&subMat,
		cvRect( 0, 0, signalTimeDom->cols, signalTimeDom->rows )
		);
	cvCopy( &subMat, output );

	// Cleanup
	cvReleaseMat( &signalTimeDomZeroPadded );
	cvReleaseMat( &signalFreqDom );
}

/*
Calculate the power for each channel
CvMat* signalTimeDom 	- Discrete time signal, time axis runs over columns.
						  Channels run over rows.
CvMat* output 			- Power of each channel. Channels fun over rows.
*/
void calcPower( 
	const CvMat* signalTimeDom,
	CvMat* output
	)
{
	assert( signalTimeDom->rows == output->rows );
	
	for ( int row = 0; row < signalTimeDom->rows; row++ ) { 
		float* ptr = ( float* ) ( signalTimeDom->data.ptr + row * signalTimeDom->step );
		float power = 0;
		for( int col = 0; col < signalTimeDom->cols; col++ ) {
			power += ( *ptr ) * ( *ptr );
			ptr++;
		}
		float* outputPtr = ( float* ) ( output->data.ptr + row * output->step );
		*outputPtr = power;
	}
}
