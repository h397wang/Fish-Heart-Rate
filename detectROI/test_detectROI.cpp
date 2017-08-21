
#include <cv.h>
#include <highgui.h>
#include <math.h>

#include "detectROI.h"

const float PI = 3.1415f;
const float PI_OVER_2 = 1.57075f;

const char* originalWindow = "originalWindow";
const char* filterWindow = "filterWindow";
const char* cannyWindow = "cannyWindow";
const char* straightLinesWindow = "straightLinesWindow";


const char* fileName = "/Users/henrywang/Documents/SideProjects/OpenCVTutorials/Fish/heartRateVids/heart_rate_4116.mp4";
const char* linesImg = "/Users/henrywang/Documents/SideProjects/OpenCVTutorials/Fish/detectROI/straightlines.jpg";

// new idea
// detect the ROI based on movement or change in pixels rather than feature detection...


// First expand the darkness,??
// tresholding...

void detectBlob( IplImage* grayFramePtr ) { // filtered actually
	cvThreshold(
		grayFramePtr,
		grayFramePtr,
		100, // threshold
		255, // max value
		CV_THRESH_BINARY );
}

// draws on the framePtr
void detectLines(IplImage* cannyFramePtr, IplImage* framePtr ) {
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* houghLines = cvHoughLines2( 	cannyFramePtr,
										storage,
										CV_HOUGH_STANDARD,
										15, // rho
										0.10, // theta in radians
										1800) ; // weight required
	printf("number of lines found %d\n", houghLines->total);
	for( int i = 0; i < houghLines->total; i++ ) {
		float* p = (float*) cvGetSeqElem( houghLines, i );
		float rho = p[0];
		float theta = p[1];

		CvPoint start = cvPoint(0, 0);
		CvPoint end = cvPoint(0, 0);
		float yInter;
		
		if ( rho > 0 && theta > PI / 2 ) {
			yInter = cvRound( rho / cos(theta - PI / 2) ); // positive	
		} else
		if (rho < 0 && theta > PI / 2) {
			yInter = cvRound( rho / cos(theta - PI / 2) ); // negative
		} else
		if (rho > 0 && theta <= PI / 2) {
			yInter = cvRound( rho / sin( theta ) );				
		}

		float slope = -1.0f / tan( theta );
		float xInter = cvRound( -yInter / slope );
		float xInterBottom = cvRound( (framePtr->height - yInter) / slope );
		float yInterRight = cvRound( slope * framePtr->width + yInter );

		if ( yInter > 0 && yInter < framePtr->height ) {
			start.x = 0;
			start.y = yInter;

			if ( xInterBottom > 0 && xInterBottom < framePtr->width) {
				end.x = xInterBottom;
				end.y = framePtr->height;
			} else {
				end.x = framePtr->width;
				end.y = yInterRight;
			}
		} else 
		if ( slope > 0 ) {
			start.x = xInter;
			start.y = 0;

			if ( xInterBottom > 0 && xInterBottom < framePtr->width) {
				end.x = xInterBottom;
				end.y = framePtr->height;
			} else {
				end.x = framePtr->width;
				end.y = yInterRight;
			}
		} else {
			start.x = xInterBottom;
			start.y = framePtr->height; // minus 1?

			if ( xInter > 0 && xInter < framePtr->width) {
				end.x = xInter;
				end.y = 0;
			} else {
				end.x = framePtr->width;
				end.y = yInterRight;
			}
		}
		cvLine( framePtr, start, end, CV_RGB( 255, 255, 255 ), 3 );
	}
}



int main(int argc, char** argv) {
	CvCapture* capture = cvCreateFileCapture( fileName );
	if ( capture == NULL ){
		printf( "%s could not be opened\n", fileName );
		return -1;
	}

	IplImage* framePtr = cvQueryFrame( capture );
	IplImage* grayFramePtr = cvCreateImage( cvGetSize( framePtr ), framePtr->depth, 1 );
	IplImage* cannyFramePtr = cvCreateImage( cvGetSize( framePtr ), framePtr->depth, 1 );
	IplImage* straightLinesFramePtr = cvCreateImage( cvGetSize( framePtr ), framePtr->depth, 1 );

	cvCvtColor( framePtr, grayFramePtr, CV_BGR2GRAY );

	IplConvKernel* kern = cvCreateStructuringElementEx( 3, 3, 1, 1, CV_SHAPE_RECT );

	// expands the darkness
	cvMorphologyEx( 
		grayFramePtr, 
		grayFramePtr,
		NULL,
		kern,
		CV_MOP_OPEN,
		5 );

	cvReleaseStructuringElement( &kern );
	cvSmooth( grayFramePtr, grayFramePtr, CV_GAUSSIAN );
	cvCanny( grayFramePtr, cannyFramePtr, 50, 150 );

	// expand the whites
	cvDilate( cannyFramePtr, cannyFramePtr, NULL, 1 );

	// detectLines(cannyFramePtr, framePtr );

	cvNamedWindow( originalWindow, 1 );
	cvNamedWindow( filterWindow, 1 );
	cvNamedWindow( cannyWindow, 1 );

	detectBlob(grayFramePtr);

	cvShowImage( originalWindow, framePtr );
	cvShowImage( filterWindow, grayFramePtr );
	cvShowImage( cannyWindow, cannyFramePtr );
	cvWaitKey(0);

	cvReleaseCapture( &capture );
	cvDestroyWindow( originalWindow );
	cvDestroyWindow( filterWindow );
	cvDestroyWindow( cannyWindow );
	return 0;
}