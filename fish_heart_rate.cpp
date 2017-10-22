#include "cv.h"
#include "highgui.h"
#include "helper.h"
#include <stdio.h>
#include <stdlib.h>

/* 
Frame period is about 1 sec / 30 frames = 33 ms/frame
Fish heart rate is about (2.6) Hz
Nyquist frequency (max freq we can recontruct is 15 Hz)
*/

// These are determined experimentally.
const float LOW_FREQ_CUTOFF = 2.2; 
const float HIGH_FREQ_CUTOFF = 2.8;

// TODO: Should be your determine dynamically.
const float PEAK_TROUGH_THRESH = 0.25; 

const char* originalWindow = "originalWindow";
const char* filteredWindow = "filteredWindow";
const char* differenceWindow = "differenceWindow";

const char* fileName = "/Users/henrywang/Documents/SideProjects/OpenCVTutorials/Fish/heartRateVids/heart_rate.mp4";
const char* rawOutputFileName = "Output/raw_output.txt";
const char* filteredOutputFileName = "Output/filtered_output.txt";
const char* demoVidName = "/Users/henrywang/Documents/SideProjects/OpenCVTutorials/Fish/DemoVids/heart_rate_demo.mp4";

const int WIDTH_NUM_ROI = 4;
const int HEIGHT_NUM_ROI = 4;
const int SUB_SAMPLE_FRAMES = 256;

/*
IplImage* Grayscale image
*/
void filter( IplImage* imgGray ) {
	for ( int i = 0; i < 5; i++ ) {
		// Dilation reduces black spots and enlarges bright spots.
		cvDilate( imgGray, imgGray );
	}
	for (int i = 0; i < 3; i++) {
		cvSmooth( imgGray, imgGray, CV_BLUR );
	}
}

int main( int argc, char* argv[] ) {

	if (argc != 3) {
		printf("Expected: ./filename <debug> <demo>\n");
		return -1;
	}

	const int debug = atoi(argv[1]);
	const int demo = atoi(argv[2]);

	// Text file for time signal output
	FILE* rawOutputFilePtr = fopen( rawOutputFileName, "w" );
	if ( rawOutputFilePtr == NULL ) {
		printf( "%s could not be opened\n", rawOutputFileName );
		return -1;
	}

	FILE* filteredOutputFilePtr = fopen( filteredOutputFileName, "w" );
	if ( filteredOutputFilePtr == NULL ) {
		printf( "%s could not be opened\n", rawOutputFileName );
		return -1;
	}

	CvCapture* capture = cvCreateFileCapture( fileName );
	if( capture == NULL ){
		printf( "%s could not be opened\n", fileName );
		return -1;
	}

	// Extract video information.
	const int numFrames = cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_COUNT );
	const int vidFps = cvGetCaptureProperty( capture, CV_CAP_PROP_FPS );
	const int milliSecondsPerFrame = 1000 / static_cast<float>( vidFps );
	const float fundamentalFrequency = static_cast<float>( vidFps ) / static_cast<float>( numFrames);
	const int lowHarmIndex = static_cast<int>( LOW_FREQ_CUTOFF / fundamentalFrequency );
	const int highHarmIndex = static_cast<int>( HIGH_FREQ_CUTOFF / fundamentalFrequency );
	const int frameHeight = cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT);
	const int frameWidth = cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH);
	const int roiWidth = frameWidth / WIDTH_NUM_ROI;
	const int roiHeight = frameHeight / HEIGHT_NUM_ROI;

	// Print video information
	if ( debug ) {
		printf( "numFrames: %d\n", numFrames );
		printf( "vidFps: %d\n", vidFps );
		printf( "milliSecondsPerFrame: %d\n", milliSecondsPerFrame );
		printf( "fundamentalFrequency: %f\n", fundamentalFrequency );
		printf( "lowHarmIndex: %d\n", lowHarmIndex );
		printf( "highHarmIndex: %d\n", highHarmIndex );
	}

	CvVideoWriter *writer = cvCreateVideoWriter(
		demoVidName,
		CV_FOURCC('M','J','P','G'),
		vidFps,
		cvSize(frameWidth, frameHeight)
		);

	// Initializations
	IplImage* framePtr 					= cvQueryFrame( capture );
	CvMat* output 						= cvCreateMat( 1, numFrames, CV_32FC1 ); 
	IplImage* grayFramePtr 				= cvCreateImage( cvGetSize( framePtr ), framePtr->depth, 1 );
	IplImage* previousGrayFramePtr 		= cvCreateImage( cvGetSize( framePtr ), framePtr->depth, 1 );
	IplImage* diffFramePtr				= cvCreateImage( cvGetSize( framePtr ), framePtr->depth, 1 );
	cvCvtColor( framePtr, previousGrayFramePtr, CV_BGR2GRAY);
	filter( previousGrayFramePtr );

	// Track the intensities of each ROI over time
	CvMat* roiIntensities = cvCreateMat(HEIGHT_NUM_ROI * WIDTH_NUM_ROI, numFrames, CV_32FC1 ); 
	
	// First iteration to locate ROI
	for ( int frameCounter = 1; frameCounter < SUB_SAMPLE_FRAMES; frameCounter++ ) { 
		framePtr = cvQueryFrame( capture );
		if( !framePtr ) break;
		cvCvtColor( framePtr, grayFramePtr, CV_BGR2GRAY);
		filter( grayFramePtr );
		cvAbsDiff( grayFramePtr, previousGrayFramePtr, diffFramePtr );
		previousGrayFramePtr = cvCloneImage(grayFramePtr);

		// Iterate through each ROI and store its intensity
		for (int row = 0; row < HEIGHT_NUM_ROI; row++ ) {
			for (int col = 0; col < WIDTH_NUM_ROI; col++ ) {
				int x = col * roiWidth;
				int y = row * roiHeight;
				cvSetImageROI( diffFramePtr, cvRect( x, y, roiWidth, roiHeight ) );
				CvScalar intensity = cvAvg( diffFramePtr );
				cvResetImageROI( diffFramePtr );
				int roiIndex =  HEIGHT_NUM_ROI * row + col;
				cvSet2D( roiIntensities, roiIndex, frameCounter, intensity );
			}
		}
	}

	bandPassFilter( roiIntensities, roiIntensities, lowHarmIndex, highHarmIndex );
	CvMat* signalPowers = cvCreateMat( roiIntensities->rows, 1, CV_32FC1 ); 
	calcPower( roiIntensities, signalPowers );

	// Find the roi with the highest power.
	// TODO: This probably isn't the best classification method
	int maxPowerRoiIndex = 0;
	float* ptr = ( float* )( signalPowers->data.ptr );
	float maxPower = *ptr;
	printf( "signalPowers values\n" );
	printf( "%.4f\n", maxPower );
	for (int row = 1; row < signalPowers->rows; row++ ) { 
		printf( "%.4f\n", ptr[row] );
		if ( ptr[row] > maxPower ) { 
			maxPower = ptr[row];
			maxPowerRoiIndex = row;
		}
	}
	
	printf( "maxPower: %.5f\n", maxPower );
	printf( "maxPowerRoiIndex: %d\n", maxPowerRoiIndex );
	
	// Draw a box around it
	const int xRoi = roiWidth * ( maxPowerRoiIndex % WIDTH_NUM_ROI );
	const int yRoi = roiHeight * ( maxPowerRoiIndex / HEIGHT_NUM_ROI );

	// Reiterate through the video once the initial ROI has been located
	cvReleaseCapture( &capture );
	capture = cvCreateFileCapture( fileName );
	if ( capture == NULL ) return -1;

	// Second iteration to get all values of intensity over time
	for (int frameCounter = 1; frameCounter < numFrames; frameCounter++ ) {
		framePtr = cvQueryFrame( capture );
		if( !framePtr ) break;
		cvCvtColor( framePtr, grayFramePtr, CV_BGR2GRAY);
		filter( grayFramePtr );
		cvAbsDiff( grayFramePtr, previousGrayFramePtr, diffFramePtr );
		previousGrayFramePtr = cvCloneImage(grayFramePtr);

		// Draw the ROI rectange
		cvRectangle(
			framePtr,
			CvPoint( xRoi, yRoi ),
			CvPoint( xRoi + roiWidth, yRoi + roiHeight ),
			CvScalar( 255, 255, 255 )
			);

		cvSetImageROI( grayFramePtr, cvRect( xRoi, yRoi, roiWidth, roiHeight ) );
		
		// Filter to reduce the noise in average intensity fluctuations.
		filter( grayFramePtr );
		CvScalar avgIntensity = cvAvg(grayFramePtr);
		cvResetImageROI(grayFramePtr);

		cvSet1D( output, frameCounter, avgIntensity );
		fprintf(rawOutputFilePtr, "%.2f\n", avgIntensity.val[0] );

		if ( debug ) { 
			cvShowImage( originalWindow, framePtr );
			cvShowImage( filteredWindow, grayFramePtr );
			cvShowImage( differenceWindow, diffFramePtr );
			char c = cvWaitKey( milliSecondsPerFrame );
			if( c == 27 ) break;
		}
	}

	bandPassFilter(output, output, lowHarmIndex, highHarmIndex);

	cvReleaseCapture( &capture );
	capture = cvCreateFileCapture( fileName );
	if ( capture == NULL ) return -1;

	// For the third iteration, count the heart beats which should correspond to local maxima. 
	int heartBeatCount = 0;
	float trough = output->data.fl[0]; 
	for( int frame = 1; frame < output->cols - 1; frame++ ) {

		fprintf( filteredOutputFilePtr, "%.2f\n", output->data.fl[frame] );

		if ( output->data.fl[frame] > output->data.fl[frame - 1] && 
				output->data.fl[frame] > output->data.fl[frame + 1] ) {
			if ( output->data.fl[frame] - trough > PEAK_TROUGH_THRESH ) {
				heartBeatCount++;
			}
		} else 
		if ( output->data.fl[frame] < output->data.fl[frame - 1] && 
				output->data.fl[frame] < output->data.fl[frame + 1] ) {
			// TODO: Currently looks for some local min, which may not be ideal.
			trough = output->data.fl[frame];
		}

		if ( demo ) { 
			framePtr = cvQueryFrame(capture);
			if( !framePtr ) break;
			CvFont font;
			cvInitFont( &font, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0, 0, 3, 8);
			char str[3];
			sprintf( str, "%d", heartBeatCount );
			cvRectangle(
				framePtr,
				CvPoint( xRoi, yRoi ),
				CvPoint( xRoi + roiWidth, yRoi + roiHeight ),
				CvScalar( 255, 255, 255 )
				);
			cvPutText( 	
				framePtr,
				str,
				CvPoint( 100, 100 ),
				&font,
				CvScalar( 255, 255, 255 ) 
				);
			//cvWriteFrame( writer, framePtr );
			cvShowImage( originalWindow, framePtr );
			char c = cvWaitKey( milliSecondsPerFrame );
			if ( c == 27 ) break;
		}
	}

	printf("heartBeatCount: %d\n", heartBeatCount);

	// Clean up
	fclose(rawOutputFilePtr);
	fclose(filteredOutputFilePtr);
	cvReleaseMat( &output );
	cvReleaseMat( &roiIntensities );
	cvReleaseMat( &signalPowers );
	cvReleaseVideoWriter( &writer );
	cvReleaseCapture( &capture );
	cvDestroyWindow( originalWindow );
	cvDestroyWindow( filteredWindow );
	return 0;
}