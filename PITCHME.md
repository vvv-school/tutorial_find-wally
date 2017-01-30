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
### CMakeLists additions

```CMakeLists
find_package(YARP REQUIRED)
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
### Code headers additions
```CMakeLists
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
```

#HSLIDE
### Load an image

#VSLIDE
### Load an image
---
##### IDL service
```CMakeLists
/**
 * Load the two required images.
 * @param mainImage name of the image to be loaded.
 * @return true/false on success/failure.
 */
bool load(1:string image);
```
---
##### Code
```CMakeLists
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

```CMakeLists
yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> >    imageOutPort;

```
---
```CMakeLists
imageOutPort.open(("/"+getName("/image:o")).c_str());
```
---
```CMakeLists
imageOutPort.close();
```
---
```CMakeLists
cvtColor(out_image, out_image, CV_BGR2RGB);

IplImage yarpImg = out_image;
outImg.resize(yarpImg.width, yarpImg.height);
cvCopy( &yarpImg, (IplImage *) outImg.getIplImage());

imageOutPort.write();
```
#HSLIDE
The End :)
