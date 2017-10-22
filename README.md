### Automate Detection of Fish Heart Rate

### Preface
Tedious chore for researchers. 

### Assumptions
- Fish larvae is static throughout the video, such that the region of interest doesn't change. The fish should be sedated so we don't expect movement elsewhere. This is beneficial as it reduced noise in our calulations.
- We know the upper and lower limits of the fish heart rate, this is required for bandpass filtering.

### General Algorithm

### Region of Interest (ROI) Identification
This can be done programmatically, by letting the user make the selection, or by simply hardcoding the rectangle coordinates into the program. Here's a sample video illustrating the kind of movement we're looking for.

<a href="https://github.com/h397wang/Fish-Heart-Rate/blob/master/DemoVids/fish_heart_rate_trimmed.gif"><img src="https://github.com/h397wang/Fish-Heart-Rate/blob/master/DemoVids/fish_heart_rate_trimmed.gif" align="center" ></a><a 

After applying several filters, it would appear that we are looking for a bright pulse that occurs at some frequency.

<a href="https://github.com/h397wang/Fish-Heart-Rate/blob/master/DemoVids/fish_heart_rate_filtered.gif"><img src="https://github.com/h397wang/Fish-Heart-Rate/blob/master/DemoVids/fish_heart_rate_filtered.gif" align="center" ></a><a 

A hacky way to do it is to treat the video display as a recangular grid. For each rectangular ROI, evaluate the average intensities over time. Apply a bandpass filter to each discrete time signal. Now find the ROI that has the highest signal power. This should work because since the other potential ROI should have relatively low signal power, after the filtering, because those areas should be fairly static. 

### Filtering Process
The same filtering process is used in the ROI identification, as well as the counting of heart beats. 

Dilation and smoothing serves to enhance the bright spots that represent the fish heart beat. This helps to reduce the SNR because doing raw frame subtractions yields a very small relative difference.

Bandpass filtering is the essential algorithm used here.
1. The average intensity of the ROI is calculated for each frame. This gives a discrete time signal.
2. Applying the Fourier Transform allows filtering to be easily done in the frequency domain.
3. Frequencies outside of the upper and lower frequency limits are zeroed.
4. Apply the Inverse Fourier transform to obtained the filtered discrete time signal.

This signal processing looks like this.

href="https://github.com/h397wang/Fish-Heart-Rate/blob/master/Output/raw_signal.png"><img src="https://github.com/h397wang/Fish-Heart-Rate/blob/master/Output/raw_signal.png" align="top" width="300" ></a>  

href="https://github.com/h397wang/Fish-Heart-Rate/blob/master/Output/filtered_signal.png"><img src="https://github.com/h397wang/Fish-Heart-Rate/blob/master/Output/filtered_signal.png" align="top" width="300" ></a>  

From visual inspection, it becomes very obvious where the heart beats appear.

### Demonstration
Here's an overly lengthy gif of the end product.

<a href="https://github.com/h397wang/Fish-Heart-Rate/blob/master/DemoVids/fish_heart_rate_demo_trimmed.gif"><img src="https://github.com/h397wang/Fish-Heart-Rate/blob/master/DemoVids/fish_heart_rate_demo_trimmed.gif" align="center" ></a><a 
