#pragma once

#include "cv.h"
#include "highgui.h"

void test();


void printData( IplImage* img );

void printFrameInfo(IplImage* img);

void setROI(IplImage* img);

double getAverageIntensity(IplImage* img);

void bandPassFilter(
					const CvMat* signal_timeDom, // column vector, 
					CvMat* output, // should be initialized?
					int lowHarmIndexCutoff,
					int highHarmIndexCutoff
					);

