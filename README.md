### Automated Detection of Fish Heart Beats Using Computer Vision

### Preface
Counting the number of heart beats is a tedious and menial chore for researchers, and generally not a good use of their time. Imagine dozens of video samples, even if they were only 1 minute in length, that could add up to several hours per week. The issue is compounded with the fact that the researcher has to slow down the video if the heart rate is greater than 2 Hz. The following program prototype seeks to determine the feasibility of automating this process using basic computer vision algorithms. 

### Assumptions
I assume the fish larvae is static throughout the video, such that the region of interest (ROI) containing the heart doesn't change over time. Realistically, the fish should be sedated so we don't expect movement outside of the ROI.

We know the upper and lower limits of the fish heart rate (2 to 3 Hz), these parameters are required for bandpass filtering.

### General Algorithm

### Region of Interest Identification
This initial step is beneficial as it reduces noise and computation time, for some overhead. This can be done programmatically, by letting the user make the selection, or by simply hardcoding the rectangle coordinates into the program. The current program attempts to determine the ROI using basic spectrum analysis. Here's a sample video illustrating the kind of movement we're looking for. 

<a href="https://github.com/h397wang/Fish-Heart-Rate/blob/master/DemoVids/fish_heart_rate_trimmed.gif"><img src="https://github.com/h397wang/Fish-Heart-Rate/blob/master/DemoVids/fish_heart_rate_trimmed.gif" align="center" ></a> 

Looking for "movement" between frames is fairly straight forward. We convert the images to grayscale, and take the difference between consecutive frames. After applying several dilation and smoothing filters, it would appear that we are looking for a white spot on a black background that pulses at some frequency (TODO: insert gif). 

<a href="https://github.com/h397wang/Fish-Heart-Rate/blob/master/DemoVids/fish_heart_rate_filtered.gif"><img src="https://github.com/h397wang/Fish-Heart-Rate/blob/master/DemoVids/fish_heart_rate_filtered.gif" align="center" ></a> 

A hacky way to do it is to treat the video display as a recangular grid. For each rectangular ROI, evaluate the average intensities over time. Apply a bandpass filter to each discrete time signal. Now within this set of signals, find the signal that has the highest signal power, its corresponding rectangle should be our ROI. This should work because since the other potential ROI should have relatively low signal power, after the filtering, because those areas should be fairly static. 

### Filtering Process
The same filtering process is used in the counting of heart beats. 

Dilation and smoothing serves to enhance the bright spot pulses that correspond to heart beats. This helps to reduce the SNR because doing raw frame subtractions yields a very small relative difference, as we notice the "movement" is quite small.

Bandpass filtering is the essential algorithm used here.
1. The average intensity of the ROI is calculated for each frame. This gives a discrete time signal.
2. Applying the Fourier Transform allows filtering to be easily done in the frequency domain.
3. Frequencies outside of the upper and lower frequency limits are zeroed.
4. Apply the Inverse Fourier transform to obtain the filtered discrete time signal.

This signal processing looks like this.

<a href="https://github.com/h397wang/Fish-Heart-Rate/blob/master/Output/raw_signal.png"><img src="https://github.com/h397wang/Fish-Heart-Rate/blob/master/Output/raw_signal.png" align="top" width="300" ></a>

<a href="https://github.com/h397wang/Fish-Heart-Rate/blob/master/Output/filtered_signal.png"><img src="https://github.com/h397wang/Fish-Heart-Rate/blob/master/Output/filtered_signal.png" align="top" width="300" ></a>  

From visual inspection, it becomes very obvious where the heart beats appear. We can confirm this assumption too. We know the video's FPS, and therefore the frame period (33 ms). A period on the graph appears to take 10 frames which implies that the heart beat period is about 330 ms. So a frequency range of 2.5 to 3.5 Hz should be appropriate.  

### Demonstration
Here's an overly lengthy gif of the end product (it might take awhile to load).

<a href="https://github.com/h397wang/Fish-Heart-Rate/blob/master/DemoVids/fish_heart_rate_demo_10sec.gif"><img src="https://github.com/h397wang/Fish-Heart-Rate/blob/master/DemoVids/fish_heart_rate_demo_10sec.gif" align="center" ></a>
