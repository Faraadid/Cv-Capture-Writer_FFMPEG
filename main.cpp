#define _CRT_SECURE_NO_WARNINGS
#define __STDC_CONSTANT_MACROS


//Include all the FFMPEG header files.
extern "C" {
    //Library containing decoders and encoders for audio/video codecs.
    #include <libavcodec/avcodec.h>
    //Library containing demuxers and muxers for multimedia container formats.
    #include <libavformat/avformat.h>
    //Library performing highly optimized image scaling and color space/pixel format conversion operations.
    #include <libswscale/swscale.h>

    #include <libavutil/imgutils.h>
    #include <libavutil/opt.h>
    #include <libavutil/mathematics.h>
    #include <libavutil/samplefmt.h>
}

//Include headers for OpenCV Image processing.
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>

#include <iostream>
#include "cap_ffmpeg_impl.hpp"


int main()
{
    CvCapture* capture = cvCaptureFromCAM(0);

    int fps = 30;
    int width = 320;
    int height = 320;
    CvVideoWriter_FFMPEG writer;
    writer.init();
    if( !writer.open("test.avi", CV_FOURCC('D', 'I', 'V', 'X'), fps, width, height, true) )
    {
        std::cerr <<  "writer not opened\n";
        return 1;
    }

    CvCapture_FFMPEG cap;
    cap.open(width, height);

    while(1)
    {
        cvGrabFrame(capture);
        IplImage* img=cvRetrieveFrame(capture);

        if(!writer.writeFrame((const uchar*)img->imageData,
                              img->widthStep, img->width, img->height, img->nChannels, img->origin))
        {
            std::cerr <<  "writing frame failed\n";
            return 1;
        }
        else
        {
//            std::cout << (const uchar*)img->imageData ;
        }

        uint8_t* buffer = &writer.outbuf[0];
        int length = writer.out_size;

        cap.grabFrame(buffer, length);
        IplImage* img2 = cap.retrieveFrame();
        if (img2)
        {
            cvShowImage("My Window", img2 );
            char controlKey = cvWaitKey(10);
            if(controlKey == 'q')
                break;
        }
        else
        {
            std::cerr << "img2 empty\n";
        }
    };

    writer.close();
    cap.close();

    return 0;
}
