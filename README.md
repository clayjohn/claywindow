# claywindow

This is just a very basic library (VERY basic) to allow for drawing cpu generated scenes without using the gpu or awful formats like .ppm

You have the freedom (and responsibility) to write directly into the framebuffer which is copied onto the screen by the library. 
This creates a syntax that is less than ideal but increases the speed of drawing which allows you to see your output much faster.
