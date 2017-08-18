#include "cv.h"
#include "highgui.h"
#include "test.h"

#define SHOW_FRAMES 0
/* 
1942 frames in 65 seconds = about 30 frames per second
Frame period is then about 1sec / 30 frames = 33 ms/frame
Fish heart rate is about (2.6) Hz
Nyquist frequency (max freq we can recontruct is 15 Hz)
*/

// These need to be determined experimentally?
const float LOW_FREQ_CUTOFF = 1.0; 
const float HIGH_FREQ_CUTOFF = 4.0;


const int LOW_THRESH = 100;
const int HIGH_THRESH = 170;

const char* originalWindow = "originalWindow";
const char* filteredWindow = "filteredWindow";
const char* differenceWindow = "differenceWindow";
const char* fileName = "/Users/henrywang/Documents/SideProjects/OpenCVTutorials/Fish/heart_rate.mp4";

// done in place
void filter(IplImage* img_gray) {
	for (int i = 0; i < 5; i++) {
		cvDilate(img_gray, img_gray);
	}
	for (int i = 0; i < 3; i++) {
		cvSmooth( img_gray, img_gray, CV_BLUR );
	}
}

int main( int argc, char* argv[] ) {
	IplImage* framePtr;
	IplImage* previousFramePtr;
	IplImage* previousFramePtr_gray;
	IplImage* diffFramePtr;
	IplImage* framePtr_gray;

	FILE *filePtr;
	FILE *filePtr_DFT;

	CvMat* output;
	CvMat* outputDFT;
	CvMat* bandFilter;

	int frameCounter = 0;

	filePtr = fopen("output.txt", "w");
	if ( filePtr == NULL ) return -1;

	filePtr_DFT = fopen("output_DFT.txt", "w");
	if ( filePtr_DFT == NULL ) return -1;

	CvCapture* capture = 0;
	capture = cvCreateFileCapture(fileName);
	if( capture == NULL ){
		printf("Video file could not be opened\n");
		return -1;
	}

	const int numFrames = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
	const int vidFPS = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
	const int milliSecondsPerFrame = 1000 / static_cast<float>(vidFPS);
	const float fundamentalFrequency = static_cast<float>(vidFPS) / static_cast<float>(numFrames);
	const int lowHarmIndex = static_cast<int>(LOW_FREQ_CUTOFF / fundamentalFrequency);
	const int highHarmIndex = static_cast<int>(HIGH_FREQ_CUTOFF / fundamentalFrequency);

	printf("numFrames: %d\n", numFrames);
	printf("vidFPS: %d\n", vidFPS);
	printf("milliSecondsPerFrame: %d\n", milliSecondsPerFrame);
	printf("fundamentalFrequency: %f\n", fundamentalFrequency);
	printf("lowHarmIndex: %d\n", lowHarmIndex);
	printf("highHarmIndex: %d\n", highHarmIndex);

	previousFramePtr = cvQueryFrame(capture);
	if( !previousFramePtr ) return -1;

	framePtr_gray = cvCreateImage(cvGetSize(previousFramePtr), previousFramePtr->depth, 1);
	previousFramePtr_gray = cvCreateImage(cvGetSize(previousFramePtr), previousFramePtr->depth, 1);	
	diffFramePtr = cvCreateImage(cvGetSize(previousFramePtr), previousFramePtr->depth, 1);

	cvCvtColor( previousFramePtr, previousFramePtr_gray, CV_BGR2GRAY);
	filter(framePtr_gray);

	output = cvCreateMat( 1, numFrames, CV_32FC1 ); 
	outputDFT = cvCreateMat( 1, numFrames, CV_32FC1 ); 

	while(1) {
		framePtr = cvQueryFrame(capture);
		if( !framePtr ) break;
		
		cvCvtColor( framePtr, framePtr_gray, CV_BGR2GRAY);
		// Smoothing and Filters
		filter(framePtr_gray);

		// temporarily hard code the rect params for the ROI
		const int x = 200;
		const int y = 150;
		const int width = 128;
		const int height = 128; 

		// draws the rect
		cvRectangle(framePtr_gray,
					CvPoint(x, y),
					CvPoint(x + width, y + height),
					CvScalar(255,255,255) 
					);


		// ROI for performance
		cvSetImageROI(framePtr_gray, cvRect(x, y, width, height));
		CvScalar avgIntensity = cvAvg(framePtr_gray);
		//fprintf(filePtr, "%.2f\n", avgIntensity.val[0]);
		cvResetImageROI(framePtr_gray);

#if SHOW_FRAMES
		cvShowImage( originalWindow, framePtr );
		cvShowImage( filteredWindow, framePtr_gray );

		char c = cvWaitKey(milliSecondsPerFrame);
		if( c == 27 ) break;
#endif
		previousFramePtr = cvCloneImage(framePtr_gray);
		cvSet1D( output, frameCounter, avgIntensity );
		frameCounter++;
	}

	// Signal processing
	bandPassFilter(output, output, lowHarmIndex, highHarmIndex);
				
	for( int col = 0; col < output->cols; col++ ) {
		fprintf( filePtr, "%.2f\n", output->data.fl[col] );
	}
	printf("Wrote %d points of the bandpass filtered discrete time signal text file\n", output->cols);
	
	// Clean up
	fclose(filePtr);
	fclose(filePtr_DFT);

	cvReleaseMat( &output );
	cvReleaseCapture( &capture );
	cvDestroyWindow( originalWindow );
	cvDestroyWindow( filteredWindow );
	//cvDestroyWindow( differenceWindow );
	return 0;
}