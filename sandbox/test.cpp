
#include <stdio.h>
#include <assert.h>

#include "test.h"
#include "cv.h"
#include "highgui.h"

#include "../helper.h"

IplImage* gImage = NULL;
IplImage* gImageGray = NULL;
IplImage* gImageTmp = NULL;
CvMemStorage* gStorage = NULL;

int gBlobDetection_threshold = 5; 
int gBlobDetection_numErod = 3;
CvMemStorage* gBlobDetection_storage = NULL;

int gHoughCircles_cannyThreshold = 100; // irrelevant once binary thresholded
int gHoughCircles_accumThreshold = 35;
CvMemStorage* gHoughCircles_storage = NULL;

void trackbarCallback_houghCircles(int) {
	if ( gStorage == NULL ) {
		gImageGray = cvCreateImage( cvGetSize( gImage ), 8, 1 );
		gStorage = cvCreateMemStorage(0);
	} else {
		cvClearMemStorage( gStorage );
	}
	CvSeq* contours = NULL;

	cvCvtColor( gImage, gImageGray, CV_BGR2GRAY );

	gImageTmp = cvCreateImage( 
		cvSize( gImage->width, gImage->height ),
		gImage->depth,
		gImage->nChannels
		);
	cvCopy( gImage, gImageTmp );

	cvSmooth( gImageGray, gImageGray, CV_GAUSSIAN, 5, 5 );
	CvSeq* results = cvHoughCircles(
		gImageGray,						// src
		gStorage,						// buffer
		CV_HOUGH_GRADIENT,				// method
		2,								// rsln of accumulator used?
		30,								// min distance between circle centres
		gHoughCircles_cannyThreshold,	// canny threshold
		gHoughCircles_accumThreshold,	// accumulator threshold?
		20,								// min rad
		50								// max rad
		);
	for ( int i = 0; i < results->total; i++ ) {
		float* p = (float*) cvGetSeqElem( results, i );
		CvPoint pt = cvPoint( cvRound( p[0] ), cvRound( p[1] ) );
		cvCircle(
			gImageTmp,
			pt,					//
			cvRound( p[2] ),
			CV_RGB(0xff,0xff,0xff)
		);
	}
	cvShowImage("cvHoughCircles", gImageTmp);

	printf("gHoughCircles_cannyThreshold: %d\n", gHoughCircles_cannyThreshold);
	printf("gHoughCircles_accumThreshold: %d\n", gHoughCircles_accumThreshold);

}


int test_houghCircles(char * imageToLoadPath) {
	const char* windowName = "cvHoughCircles";

	gImage = cvLoadImage( imageToLoadPath );
	if ( gImage == NULL ) {
		printf( "Error: could not open image %s", imageToLoadPath );
		return -1;
	}

	cvNamedWindow( windowName, 1 );
	cvCreateTrackbar(
		"Canny Threshold",
		windowName,
		&gHoughCircles_cannyThreshold,
		255,
		trackbarCallback_houghCircles
	);
	cvCreateTrackbar(
		"Accum Threshold",
		windowName,
		&gHoughCircles_accumThreshold,
		500,
		trackbarCallback_houghCircles
	);

	trackbarCallback_houghCircles(0);

	cvWaitKey(0);
	return 0;
}

void trackbarCallback_BlobDetection(int) {
	if ( gStorage == NULL ) {
		gImageGray = cvCreateImage( cvGetSize( gImage ), 8, 1 );
		gStorage = cvCreateMemStorage(0);
		gHoughCircles_storage = cvCreateMemStorage(0);
	} else {
		cvClearMemStorage( gStorage );
		cvClearMemStorage( gHoughCircles_storage );
	}
	CvSeq* contours = NULL;

	cvCvtColor( gImage, gImageGray, CV_BGR2GRAY );

	gImageTmp = cvCreateImage( 
		cvSize( gImage->width, gImage->height ),
		gImage->depth,
		gImage->nChannels
		);
	cvCopy( gImage, gImageTmp );

	cvErode(
		gImageGray,				// src
		gImageGray,				// dst
		NULL,					// convolution kernel
		gBlobDetection_numErod  // num iterations
		);

	cvThreshold(
		gImageGray,					// src
		gImageGray,					// dst
		gBlobDetection_threshold,	// threshold value
		255,						// max value
		CV_THRESH_BINARY			// threshold type
		);
 
	cvErode(
		gImageGray,				// src
		gImageGray,				// dst
		NULL,					// convolution kernel
		3  						// num iterations
		);

	CvSeq* results = cvHoughCircles(
		gImageGray,						// src
		gHoughCircles_storage,			// buffer
		CV_HOUGH_GRADIENT,				// method
		2,								// rsln of accumulator used?
		20,								// min distance between circle centres
		gHoughCircles_cannyThreshold,	// canny threshold
		gHoughCircles_accumThreshold,	// accumulator threshold?
		40,								// min rad
		60								// max rad
		);

	CvRect vRoi;
	int minRoiAvg = 255;
	for ( int i = 0; i < results->total; i++ ) {
		float* pFloatTup = (float*) cvGetSeqElem( results, i );
		int x = cvRound( pFloatTup[0] );
		int y = cvRound( pFloatTup[1] );
		int rad = cvRound( pFloatTup[2] );

		cvCircle(
			gImageGray,
			cvPoint( x, y),				// center of circle
			rad,						// radius
			CV_RGB( 50, 200, 50 )		// line colour
		);

		const int rectX = (x - rad) > 0 ? (x - rad) : 0;
		const int rectY = (y - rad) > 0 ? (y - rad) : 0;
		const int rectWidth = 2 * rad;

		const CvRect vPotentialRoi = cvRect( rectX , rectY, rectWidth, rectWidth );
		cvSetImageROI( gImageGray, vPotentialRoi );
		CvScalar vScalarRoiAvg = cvAvg( gImageGray );
		if ( vScalarRoiAvg.val[0] < minRoiAvg ) {
			minRoiAvg = vScalarRoiAvg.val[0];
			vRoi = vPotentialRoi;
		}
		cvResetImageROI( gImageGray );
		printf( "Point(%d, %d), Radius: %d\n", x, y, rad );
	}

	cvRectangle(
		gImageGray,
		CvPoint(vRoi.x, vRoi.y),
		CvPoint(vRoi.x + vRoi.width, vRoi.y + vRoi.height),
		CvScalar(150, 50, 100)
		);

	// Show here because the contour function modifies source.
	cvShowImage( "Blob Detection Threshold", gImageGray );

	cvFindContours( gImageGray, gStorage, &contours );

	if( contours ) {
		cvDrawContours(
			gImageTmp,					// Image where the circle is drawn.
			contours,					
			cvScalarAll(0),				// External colour
			cvScalarAll(255),			// Hole colour
			1,							// Max level?
			1, 							// Thickness
			8,							// Line type
			cvPoint(0,0)				// Offset
		);
	}

	cvShowImage( "Blob Detection", gImageTmp );
	
	printf("gHoughCircles_cannyThreshold: %d\n", gHoughCircles_cannyThreshold);
	printf("gHoughCircles_accumThreshold: %d\n", gHoughCircles_accumThreshold);
	printf("gBlobDetection_threshold: %d\n", gBlobDetection_threshold);
	printf("gBlobDetection_numErod: %d\n", gBlobDetection_numErod);
}

int test_BlobDetection(char* imageToLoadPath) {
	const char* windowName = "Blob Detection";
	gImage = cvLoadImage( imageToLoadPath );
	if ( gImage == NULL ) {
		printf( "Error: could not open image %s", imageToLoadPath );
		return -1;
	}
	cvNamedWindow( windowName, 1 );
	cvNamedWindow( "Blob Detection Threshold", 1 );
	cvCreateTrackbar(
		"Threshold",
		windowName,
		&gBlobDetection_threshold,
		255,
		trackbarCallback_BlobDetection
	);
	cvCreateTrackbar(
		"Num Erosions",
		windowName,
		&gBlobDetection_numErod,
		15,
		trackbarCallback_BlobDetection
	);
	cvCreateTrackbar(
		"Accum Threshold",
		windowName,
		&gHoughCircles_accumThreshold,
		300,
		trackbarCallback_BlobDetection
	);
	cvCreateTrackbar(
		"Canny Threshold",
		windowName,
		&gHoughCircles_cannyThreshold,
		255,
		trackbarCallback_BlobDetection
	);

	trackbarCallback_BlobDetection(0);
	cvWaitKey();
	return 0;
}

int test_detectHeartRoi( char * fileName){

	const char* windowName = "test_detectHeartRoi";

	CvCapture* capture = cvCreateFileCapture( fileName );
	if( capture == NULL ){
		printf( "%s could not be opened\n", fileName );
		return -1;
	}

	IplImage* framePtr = cvQueryFrame( capture );
	IplImage* grayFramePtr = cvCreateImage( cvGetSize( framePtr ), framePtr->depth, 1 );

	cvNamedWindow( windowName, 1 );
	const int numFrames = cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_COUNT );
	for (int frameCounter = 0; frameCounter < numFrames; frameCounter++ ) {
		framePtr = cvQueryFrame( capture );
		cvCvtColor( framePtr, grayFramePtr, CV_BGR2GRAY );
		detectHeartRoi( grayFramePtr );
		cvShowImage( windowName, grayFramePtr );
		char c = cvWaitKey( 33 );
		if( c == 27 ) break;
	}

	cvReleaseCapture( &capture );
	cvDestroyWindow( windowName );
	return 0;
}
