/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Vadim Tikhanoff
 * email:  vadim.tikhanoff@iit.it
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

#include <cstdlib>

#include <yarp/os/BufferedPort.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Network.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Mutex.h>
#include <yarp/sig/Image.h>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include "findWally_IDL.h"

/********************************************************/
class Finder : public yarp::os::RFModule,
                public findWally_IDL
{
    yarp::os::ResourceFinder *rf;
    yarp::os::RpcServer rpcPort;
    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> >    imageOutPort;

    std::string outImgPortName;

    yarp::os::Mutex mutex;

    cv::Mat inputImage;
    cv::Mat templateImage;
    cv::Mat out_image;

    double x_pos, y_pos;

    bool closing;

    /********************************************************/
    bool attach(yarp::os::RpcServer &source)
    {
        return this->yarp().attachAsServer(source);
    }

    /********************************************************/
    bool load(const std::string &image)
    {
        mutex.lock();
        yarp::os::ResourceFinder rf;
        rf.setVerbose();
        rf.setDefaultContext(this->rf->getContext());

        std::string imageStr = rf.findFile(image);

        yDebug() << "image path is:" << imageStr;

        inputImage = cv::imread(imageStr, CV_LOAD_IMAGE_COLOR);
        if(! inputImage.data )
        {
            yError() <<"Could not open or find the first image " << imageStr;
            mutex.unlock();
            return false;
        }

        x_pos = -1.0;
        y_pos = -1.0;

        mutex.unlock();

        return true;
    }

    /********************************************************/
    yarp::os::Bottle templateMatch (const std::string &image, const int method)
    {
        yarp::os::Bottle pos;

        mutex.lock();
        yarp::os::ResourceFinder rf;
        rf.setVerbose();
        rf.setDefaultContext(this->rf->getContext());
        std::string imageStr = rf.findFile(image);

        yDebug() << "image path is:" << imageStr;

        templateImage = cv::imread(imageStr, CV_LOAD_IMAGE_COLOR);

        if(! templateImage.data || ! inputImage.data || method < 0 || method > 5 )
        {
            yError("Either there is no main image, the template is invalid or the method requested is wrong");
            x_pos = -1.0;
            y_pos = -1.0;
            pos.addDouble(x_pos);
            pos.addDouble(y_pos);
        }
        else
        {
            cv::Mat result;
            int result_cols =  inputImage.cols - templateImage.cols + 1;
            int result_rows = inputImage.rows - templateImage.rows + 1;

            //create the resulting image in grayscale
            result.create( result_rows, result_cols, CV_32FC1 );

            //Check requested template tracking method and call function
            if (method == cv::TM_SQDIFF || method == cv::TM_CCORR_NORMED)
            {
                cv::Mat mask;
                matchTemplate( inputImage, templateImage, result, method, mask);
            }
            else
                matchTemplate( inputImage, templateImage, result, method);

            //Normalizes the norm
            normalize( result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );

            double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
            cv::Point matchLoc;

            //Find minimum value and maximum value and get the location in 2D point
            minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );

            if( method  == cv::TM_SQDIFF || method == cv::TM_SQDIFF_NORMED )
            {
                matchLoc = minLoc;
            }
            else
                matchLoc = maxLoc;

            //Use the previous location to fill in the bottle
            x_pos = matchLoc.x;
            y_pos = matchLoc.y;
            pos.addDouble(x_pos);
            pos.addDouble(y_pos);
        }

        mutex.unlock();

        return pos;
    }

    /********************************************************/
    yarp::os::Bottle getLocation()
    {
        mutex.lock();
        yarp::os::Bottle position;
        position.clear();
        position.addDouble(x_pos);
        position.addDouble(y_pos);
        mutex.unlock();

        return position;
    }

    /********************************************************/
    bool quit()
    {
        closing = true;
        return true;
    }

    public:
    /********************************************************/
    bool configure(yarp::os::ResourceFinder &rf)
    {
        this->rf=&rf;

        std::string moduleName = rf.check("name", yarp::os::Value("find-wally"), "module name (string)").asString();
        setName(moduleName.c_str());

        rpcPort.open("/"+getName("/rpc"));
        imageOutPort.open("/"+getName("/image:o"));

        y_pos = -1.0;
        x_pos = -1.0;

        closing = false;

        attach(rpcPort);
        return true;
    }

    /********************************************************/
    bool close()
    {
        mutex.lock();
        rpcPort.close();
        imageOutPort.close();
        mutex.unlock();

        return true;
    }

    /********************************************************/
    double getPeriod()
    {
        return 1.0;
    }

    /********************************************************/
    bool updateModule()
    {
        mutex.lock();
        yarp::sig::ImageOf<yarp::sig::PixelRgb> &outImg  = imageOutPort.prepare();

        if( inputImage.data)
        {
            out_image = inputImage.clone();

            if (x_pos > 0.0 && y_pos > 0.0)
            {
                //blur the output image
                //The kernel size will determine how many pixels to sample during the convolution.
                //void blur(InputArray src, OutputArray dst, Size ksize)
                cv::blur( out_image, out_image, cv::Size( 10, 10 ) );
                
                //decrease contrast to half
                out_image.convertTo(out_image, CV_8U, 0.5);

                //setup region of interest using the size of the template and location of template matchin result
                cv::Rect roi = cv::Rect(x_pos, y_pos, templateImage.cols, templateImage.rows);

                //fill in the roi with the corresponding image
                cv::Mat input_roi= inputImage(roi);

                //Create a colour image and add the two images together
                cv::Mat foreground(input_roi.size(), CV_8UC3, cv::Scalar(0,0,0));
                input_roi.copyTo(foreground, input_roi);
                foreground.copyTo(out_image(cv::Rect(x_pos, y_pos, foreground.cols, foreground.rows)));

                //Draw a rectangle around the result
                cv::rectangle(out_image, cvPoint(x_pos,y_pos), cvPoint(x_pos + templateImage.cols,y_pos + templateImage.rows), cv::Scalar( 0, 255, 0), 2, 8);
            }

            cvtColor(out_image, out_image, CV_BGR2RGB);

            IplImage yarpImg = out_image;
            outImg.resize(yarpImg.width, yarpImg.height);
            cvCopy( &yarpImg, (IplImage *) outImg.getIplImage());
            imageOutPort.write();
        }
        mutex.unlock();

        return !closing;
    }
};

/********************************************************/
int main(int argc, char *argv[])
{
    yarp::os::Network::init();

    yarp::os::Network yarp;
    if (!yarp.checkNetwork())
    {
        yError()<<"YARP server not available!";
        return EXIT_FAILURE;
    }

    Finder module;
    yarp::os::ResourceFinder rf;

    rf.setVerbose();
    rf.setDefaultConfigFile( "config.ini" );
    rf.setDefaultContext("find-wally");

    rf.configure(argc,argv);

    return module.runModule(rf);
}
