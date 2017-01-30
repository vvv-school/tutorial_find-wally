#HSLIDE

### Robot vision tutorial with OpenCV
### <span style="color:#e49436">Part One</span>
#### Find Wally

#VSLIDE
### Goals of this Tutorial
 - find <span style="color:#e49436">Wally</span> :-)
 - integrating <span style="color:#e49436">YARP</span> with <span style="color:#e49436">OpenCV</span>
 - yarp::os::<span style="color:#e49436">RFModule</span>
 - <span style="color:#e49436">Thrift</span> services
 - performing simple <span style="color:#e49436">image processing</span> operations

#VSLIDE
### Let's plan what to do...
 - Load image containing the full scene.
 - Display it: stream it through a yarp port to a yarpviewer.
 - Load wally's template and run the template matching algorithm with correct method to figure out where wally is in the scene
 - Modify the streamed image to display the location of wally.
 
#HSLIDE

The End :)
