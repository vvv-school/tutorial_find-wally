#HSLIDE

### Robot vision tutorial with OpenCV
### <span style="color:#e49436">Part One</span>
---
#### Find Wally

#HSLIDE
### Goals of this Tutorial
 - find <span style="color:#e49436">Wally</span> :-)
 - integrating <span style="color:#e49436">YARP</span> with <span style="color:#e49436">OpenCV</span>
 - yarp::os::<span style="color:#e49436">RFModule</span>
 - <span style="color:#e49436">Thrift</span> services
 - performing simple <span style="color:#e49436">image processing</span> operations

#VSLIDE
### Let's plan what to do...
 - Change <span style="color:#e49436">CMakeLists.txt</span> to find <span style="color:#e49436">OpenCV</span> correctly
 - <span style="color:#e49436">Load</span> image containing the full scene.
 - Display it: <span style="color:#e49436">stream</span> it through a <span style="color:#e49436">yarp port</span> to a <span style="color:#e49436">yarpviewer</span>.
 - <span style="color:#e49436">Load</span> wally's <span style="color:#e49436">template</span> and run the <span style="color:#e49436">template matching</span> algorithm with correct method to figure out where wally is in the scene
 - Modify the streamed image to <span style="color:#e49436">display</span> the <span style="color:#e49436">location</span> of wally.

#HSLIDE
### CMakeLists modifications

#VSLIDE
### CMakeLists modifications
######<div style="text-align: left;">CMakeLists additions </div>
```CMakeLists
find_package(YARP 3.1.101 REQUIRED)
find_package(ICUBcontrib REQUIRED)
find_package(OpenCV REQUIRED)
```

```CMakeLists
include_directories(${YARP_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})
```

```CMakeLists
target_link_libraries(${PROJECT_NAME} ${YARP_LIBRARIES}
                                      ${OpenCV_LIBRARIES})
```
---
######<div style="text-align: left;">Code headers additions </div>
```c++
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
```

#HSLIDE
### Load an image

#VSLIDE
### Load an image

######<div style="text-align: left;">IDL Services </div>
```c++
/**
 * Load the two required images.
 * @param mainImage name of the image to be loaded.
 * @return true/false on success/failure.
 */
bool load(1:string image);
```
---

######<div style="text-align: left;">Code </div>
```c++
yarp::os::ResourceFinder rf;
rf.setVerbose();
rf.setDefaultContext(this->rf->getContext().c_str());

std::string imageStr = rf.findFile(image.c_str());

cv::Mat inputImage;
inputImage = cv::imread(imageStr, CV_LOAD_IMAGE_COLOR);
```
#HSLIDE
### Stream the image onto a YARP port

#VSLIDE
### Stream the image onto a YARP port

```c++
yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb>> imageOutPort;

```
---
```c++
imageOutPort.open(("/"+getName("/image:o")).c_str());
```
---
```c++
imageOutPort.close();
```
---
```c++
yarp::sig::ImageOf<yarp::sig::PixelRgb> &outImg = imageOutPort.prepare();
outImg = yarp::cv::fromCvMat<yarp::sig::PixelRgb>(out_image);
imageOutPort.write();
```

#HSLIDE
### Run the template tracker algorithm

#VSLIDE
### Run the template tracker algorithm
######<div style="text-align: left;">IDL Services </div>
```c++
/**
 * use template matching on image with desired template and
 * desired method
 * @param template name of the image to be loaded.
 * @param name of method: 0=SQDIFF, 1=SQDIFF NORMED,
 * 2=TM CCORR, 3=TM CCORR NORMED, 4=TM COEFF, 5=TM COEFF NORMED
 * @return Bottle containing the 2D position.
 */
Bottle templateMatch(1:string image, 2:i32 method);
```
######<div style="text-align: left;">template methods </div>
```c++
//Use the OpenCV function matchTemplate to search for matches between an
//image patch and an input image
void matchTemplate(InputArray image, InputArray templ, OutputArray result, int method);
//Use the OpenCV function minMaxLoc to find the maximum and minimum values
//(as well as their positions) in a given array.
void minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );
```

#HSLIDE
The End :)
