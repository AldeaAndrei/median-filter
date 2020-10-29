The code will read Pgm files and will apply a median filter.
The median filter it is used to remove noise.

The filter works by running through the image pixel by pixel and replacing the value with the median of the neighbors. The neighbors are selected if they fit inside a square of size 'filter_size' with the center on the pixel wich, the filter is applied to.