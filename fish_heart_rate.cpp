#include "cv.h"
#include "highgui.h"
#include "test.h"

#define DEMO 0
#define DEBUG 1
/* 
Frame period is about 1 sec / 30 frames = 33 ms/frame
Fish heart rate is about (2.6) Hz
Nyquist frequency (max freq we can recontruct is 15 Hz)
*/

// These are determined experimentally.
const float LOW_FREQ_CUTOFF = 2.0; 
const float HIGH_FREQ_CUTOFF = 3.0;

// TODO: Should be your determine dynamically.
const float PEAK_TROUGH_THRESH = 0.25; 

const char* originalWindow = "originalWindow";
const char* filteredWindow = "filteredWindow";
const char* differenceWindow = "differenceWindow";
const char* fileName = "/Users/henrywang/Documents/SideProjects/OpenCVTutorials/Fish/heartRateVids/heart_rate.mp4";

const int WIDTH_NUM_ROI = 4;
const int HEIGHT_NUM_ROI = 4;
const int SUB_SAMPLE_FRAMES = 128;

/*
Input: Grayscale image
*/
void filter( IplImage* img_gray ) {
	for ( int i = 0; i < 5; i++ ) {
		// Dilation reduces black spots and enlarges bright spots.
		cvDilate( img_gray, img_gray );
	}
	for (int i = 0; i < 3; i++) {
		cvSmooth( img_gray, img_gray, CV_BLUR );
	}
}

int main( int argc, char* argv[] ) {

	FILE* filePtr = fopen( "output.txt", "w" );
	if ( filePtr == NULL ) return -1;

	CvCapture* capture = cvCreateFileCapture( fileName );
	if( capture == NULL ){
		printf( "%s could not be opened\n", fileName );
		return -1;
	}

	// Extract video information.
	const int numFrames = cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_COUNT );
	const int vidFPS = cvGetCaptureProperty( capture, CV_CAP_PROP_FPS );
	const int milliSecondsPerFrame = 1000 / static_cast<float>( vidFPS );
	const float fundamentalFrequency = static_cast<float>( vidFPS ) / static_cast<float>( numFrames);
	const int lowHarmIndex = static_cast<int>( LOW_FREQ_CUTOFF / fundamentalFrequency );
	const int highHarmIndex = static_cast<int>( HIGH_FREQ_CUTOFF / fundamentalFrequency );
	const int frameHeight = cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT);
	const int frameWidth = cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH);
	
	const int roiWidth = frameWidth / WIDTH_NUM_ROI;
	const int roiHeight = frameHeight / HEIGHT_NUM_ROI;

	// Print video information
	printf( "numFrames: %d\n", numFrames );
	printf( "vidFPS: %d\n", vidFPS );
	printf( "milliSecondsPerFrame: %d\n", milliSecondsPerFrame );
	printf( "fundamentalFrequency: %f\n", fundamentalFrequency );
	printf( "lowHarmIndex: %d\n", lowHarmIndex );
	printf( "highHarmIndex: %d\n", highHarmIndex );

	// Initializations
	IplImage* framePtr 					= cvQueryFrame( capture );
	CvMat* output 						= cvCreateMat( 1, numFrames, CV_32FC1 ); 
	IplImage* framePtr_gray 			= cvCreateImage( cvGetSize( framePtr ), framePtr->depth, 1 );
	IplImage* previousFramePtr_gray 	= cvCreateImage( cvGetSize( framePtr ), framePtr->depth, 1 );
	IplImage* diffFramePtr				= cvCreateImage( cvGetSize( framePtr ), framePtr->depth, 1 );

	cvCvtColor( framePtr, previousFramePtr_gray, CV_BGR2GRAY);
	filter( previousFramePtr_gray );

	// TODO: this part needs to be extracted into a function.
	CvMat* ROIIntensities = cvCreateMat(HEIGHT_NUM_ROI * WIDTH_NUM_ROI, numFrames, CV_32FC1 ); 
	for ( int frameCounter = 1; frameCounter < SUB_SAMPLE_FRAMES; frameCounter++ ) { 
		framePtr = cvQueryFrame( capture );
		if( !framePtr ) break;
		cvCvtColor( framePtr, framePtr_gray, CV_BGR2GRAY);
		filter( framePtr_gray );

		cvAbsDiff( framePtr_gray, previousFramePtr_gray, diffFramePtr );

		previousFramePtr_gray = cvCloneImage(framePtr_gray);

		// Iterate through each ROI and collect the intensity over time
		for (int row = 0; row < HEIGHT_NUM_ROI; row++ ) {
			for (int col = 0; col < WIDTH_NUM_ROI; col++ ) {
				int x = col * roiWidth;
				int y = row * roiHeight;

				cvSetImageROI( diffFramePtr, cvRect( x, y, roiWidth, roiHeight ) );
				CvScalar intensity = cvAvg(diffFramePtr);
				cvResetImageROI(diffFramePtr);
				int roiIndex =  HEIGHT_NUM_ROI * row + col;
				cvSet2D(
					ROIIntensities,
					roiIndex,
					frameCounter,
					intensity );
			}
		}
	}

	bandPassFilter(ROIIntensities, ROIIntensities, lowHarmIndex, highHarmIndex);
	CvMat* signalPowers = cvCreateMat( ROIIntensities->rows, 1, CV_32FC1 ); 
	calcPower( ROIIntensities, signalPowers );

	// should be a column matrix
	// get max
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

	// to 0
	for (int frameCounter = 1; frameCounter < numFrames; frameCounter++ ) {
		framePtr = cvQueryFrame( capture );
		if( !framePtr ) break;
		cvCvtColor( framePtr, framePtr_gray, CV_BGR2GRAY);

		filter( framePtr_gray );

		cvAbsDiff( framePtr_gray, previousFramePtr_gray, diffFramePtr );

		previousFramePtr_gray = cvCloneImage(framePtr_gray);

		cvRectangle(
			framePtr,
			CvPoint( xRoi, yRoi ),
			CvPoint( xRoi + roiWidth, yRoi + roiHeight ),
			CvScalar( 255, 255, 255 ) );
#if 0		
		const int x 		= 200;
		const int y 		= 150;
		const int width 	= 128;
		const int height 	= 128;
		cvRectangle(
			framePtr,
			CvPoint( x, y ),
			CvPoint( x + width, y + height ),
			CvScalar( 255, 255, 255 ) );
#endif
		// Filter to reduce the noise in average intensity fluctuations.
		filter( framePtr_gray );

		// Ideally, other CV tasks should be enclosed in this scope for performance.
		cvSetImageROI( framePtr_gray, cvRect( xRoi, yRoi, roiWidth, roiHeight ) );
		CvScalar avgIntensity = cvAvg(framePtr_gray);
		cvResetImageROI(framePtr_gray);
		cvSet1D( output, frameCounter, avgIntensity );

#if DEBUG
		cvShowImage( originalWindow, framePtr );
		cvShowImage( filteredWindow, framePtr_gray );
		cvShowImage( differenceWindow, diffFramePtr );
		char c = cvWaitKey( milliSecondsPerFrame );
		if( c == 27 ) break;
#endif	
	}

	bandPassFilter(output, output, lowHarmIndex, highHarmIndex);

#if 0				
	for( int col = 0; col < output->cols; col++ ) {
		fprintf( filePtr, "%.2f\n", output->data.fl[col] );
	}
	printf("Wrote %d points of the bandpass filtered discrete time signal text file\n", output->cols);
#endif

	cvReleaseCapture( &capture );
	capture = cvCreateFileCapture( fileName );
	if ( capture == NULL ) return -1;

	// Count the heart beats which should correspond to local maxima. 
	int heartBeatCount = 0;
	float trough = output->data.fl[0]; 
	for( int frame = 1; frame < output->cols - 1; frame++ ) {
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

#if DEMO
		framePtr = cvQueryFrame(capture);
		if( !framePtr ) break;
		CvFont font;
		cvInitFont( &font, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0);
		char str[3];
		sprintf( str, "%d", heartBeatCount );
		cvPutText( 	
			framePtr,
			str,
			CvPoint( 100, 100 ),
			&font,
			CvScalar( 255, 255, 255 ) );
		cvShowImage( originalWindow, framePtr );
		char c = cvWaitKey( milliSecondsPerFrame );
		if ( c == 27 ) break;
#endif // End DEMO
	}

	printf("heartBeatCount: %d\n", heartBeatCount);

	// Clean up
	fclose(filePtr);
	cvReleaseMat( &output );
	cvReleaseMat( &ROIIntensities );
	cvReleaseMat( &signalPowers );
	cvReleaseCapture( &capture );
	cvDestroyWindow( originalWindow );
	cvDestroyWindow( filteredWindow );
	return 0;
}