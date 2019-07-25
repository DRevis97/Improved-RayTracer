# Improved-RayTracer
Ray Tracer project with improvements thanks to Peter Shirley and his books: https://github.com/RayTracing
This was made in Visual Studio 2019, so to run this code, just have it build in VS 2019.
The output of the program is a text file in the main directory (raytracing.txt by default).
To view the image, just drop the text file into an image viewer that supports PPM format (I used this site for debugging: http://paulcuth.me.uk/netpbm-viewer/)
Note: The default for the code is 1000 samples per pixels. This results in a cleaner final image with less noise, but it takes quite awhile for the program to compute the image.
For faster completion time, change int ns in main.cpp to a smaller number (10 still gives a decent image with a tolerable amount of noise.)
