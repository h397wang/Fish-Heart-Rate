#include "cv.h"
#include "highgui.h"
#include "test.h"

#define SHOW_FRAMES 1
/* 
1942 frames in 65 seconds = about 30 frames per second
Frame period is then about 1sec / 30 frames = 33 ms/frame
*/
const int MILLIS_PER_FRAME = 33;
const int F_LOW_CUTOFF = 1;

const char* originalWindow = "originalWindow";
const char* filteredWindow = "filteredWindow";
const char* differenceWindow = "differenceWindow";
const char* fileName = "/Users/henrywang/Documents/SideProjects/OpenCVTutorials/Fish/heart_rate.mp4";

int main( int argc, char* argv[] ) {
	IplImage* framePtr;
	IplImage* previousFramePtr;
	IplImage* previousFramePtr_gray;
	IplImage* diffFramePtr;
	IplImage* framePtr_gray;

	FILE *filePtr;

	CvMat* output;

	int numFrames = 0;

	filePtr = fopen("output.txt", "w");
	if ( filePtr == NULL ) return -1;

	CvCapture* capture = 0;
	capture = cvCreateFileCapture(fileName);
	if( capture == NULL ){
		printf("Video file could not be opened\n");
		return -1;
	}

	previousFramePtr = cvQueryFrame(capture);
	if( !previousFramePtr ) return -1;

	framePtr_gray = cvCreateImage(cvGetSize(previousFramePtr), previousFramePtr->depth, 1);
	previousFramePtr_gray = cvCreateImage(cvGetSize(previousFramePtr), previousFramePtr->depth, 1);	
	diffFramePtr = cvCreateImage(cvGetSize(previousFramePtr), previousFramePtr->depth, 1);

	cvCvtColor( previousFramePtr, previousFramePtr_gray, CV_BGR2GRAY);


	output = cvCreateMat ( 1942, 1, CV_32FC1 ); // numFrames hard coded for now
	
	while(1) {
		framePtr = cvQueryFrame(capture);
		if( !framePtr ) break;
		
		cvCvtColor( framePtr, framePtr_gray, CV_BGR2GRAY);
		// Smoothing
		cvSmooth( framePtr_gray, framePtr_gray, CV_BLUR );
		//cvSobel( framePtr_gray, framePtr_gray, 2, 2, 7);

		/*
		Difference between frames was too small and rounded down to 0.
		Implement differential gain.
		Increase the contrast between the two frames before the subtraction.
		*/
		cvAddWeighted(framePtr_gray, 1.0, framePtr_gray, 0.1, 0, framePtr_gray);
		cvAddWeighted(previousFramePtr, 0.9, previousFramePtr, 0, 0, previousFramePtr);

		cvAbsDiff( framePtr_gray, previousFramePtr_gray, diffFramePtr );


		// temporarily hard code the rect params for the ROI
		const int x = 200;
		const int y = 150;
		const int width = 128;
		const int height = 128; 

		// draws the rect
		cvRectangle(diffFramePtr,
					CvPoint(x, y),
					CvPoint(x + width, y + height),
					CvScalar(255,255,255) 
					);

		// ROI for performance
		cvSetImageROI(diffFramePtr, cvRect(x, y, width, height));
		
		// Apply smoothing 
		cvSmooth( diffFramePtr, diffFramePtr, CV_MEDIAN );
		cvSmooth( diffFramePtr, diffFramePtr, CV_BLUR );
		
		CvScalar avgIntensity = cvAvg(diffFramePtr);
		fprintf(filePtr, "%.2f\n", avgIntensity.val[0]);
		cvResetImageROI(diffFramePtr);

#if SHOW_FRAMES

		cvShowImage( originalWindow, framePtr );
		cvShowImage( filteredWindow, framePtr_gray );
		cvShowImage( differenceWindow, diffFramePtr );
		char c = cvWaitKey(MILLIS_PER_FRAME);
		if( c == 27 ) break;
#endif
		previousFramePtr = cvCloneImage(framePtr);

		cvSet1D( output, numFrames, avgIntensity );
		numFrames++;
	}

	// Signal processing


	// Clean up
	fclose(filePtr);

	cvReleaseMat( &output );
	cvReleaseCapture( &capture );
	cvDestroyWindow( originalWindow );
	cvDestroyWindow( filteredWindow );
	cvDestroyWindow( differenceWindow );
	return 0;
}


