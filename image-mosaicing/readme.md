This assignment is about creating panorama from a set of image. We create panorama by using homography maps.
User inputs some corresponding point via a ui and accordingly a homography mapping is generated. 
Using perspective transform both the images are mapped to a new image which stitches two image. We repeat this process for all the images to generate a bigger and better panorama.

Some samples images and results are below : 
![image_0](data2/m0607_0.jpg)
![image_1](data2/m0607_1.jpg)
![image_2](data2/m0607_2.jpg)
![image_3](data2/m0607_3.jpg)
![image_4](data2/m0607_4.jpg)
![image_5](data2/m0607_5.jpg)

Resulting image : 
![result](results_data2/final_output.jpg)

Another example : 
![image_0](data3/m0405_0.jpg)
![image_1](data3/m0405_1.jpg)
![image_2](data3/m0405_2.jpg)
![image_3](data3/m0405_3.jpg)
![image_4](data3/m0405_4.jpg)
![image_6](data3/m0405_6.jpg)
![image_7](data3/m0405_7.jpg)

Resulting image : 

![result](results_data3/output.jpg)

Clearly selecting point by hand is not good as it may lead to error. Hence we can use feature matching to generate corresponding points. This will generate much better homograph and hence better panorama. 
This is done with web.cpp