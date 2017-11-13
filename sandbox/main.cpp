#include "cv.h"
#include "highgui.h"
#include "test.h"
#include <stdio.h>
#include <stdlib.h>



int main( int argc, char* argv[] ) {
	// default
	char* fileToLoadPath = "images/fish0.jpg";
	if (argc < 2) {
		printf("Enter number for specified function:\n"
			"0: test_BlobDetection\n"
			"1: test_houghCircles\n"
			"2: test_detectHeartRoi\n"
			);
		return -1;
	}
	
	if (argc > 2) {
		// no deep copy required
		fileToLoadPath = argv[2];
	}

	int choice = atoi(argv[1]);
	switch(choice){
	case 0:
		test_BlobDetection(fileToLoadPath);
		break;
	case 1:
		test_houghCircles(fileToLoadPath);
		break;
	case 2:
		test_detectHeartRoi(fileToLoadPath);
		break;
	default:
		break;
	}
}

