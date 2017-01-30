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

#HSLIDE
The End :)
