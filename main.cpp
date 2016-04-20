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

#include <vlc/libvlc.h>
#include <vlc/libvlc_media.h>
#include <vlc/libvlc_media_player.h>
#include <vlc/libvlc_events.h>

struct VideoDataStruct
{
    int param;
};

int done = 0;

libvlc_media_player_t *mp;
unsigned int videoBufferSize = 0;
uint8_t *videoBuffer = 0;


static void handleEvent(const libvlc_event_t* pEvt, void* pUserData)
{
    libvlc_time_t time;
    switch(pEvt->type)
    {
        case libvlc_MediaPlayerTimeChanged:
            time = libvlc_media_player_get_time(mp);
            printf("MediaPlayerTimeChanged %lld ms\n", (long long)time);
            break;
        case libvlc_MediaPlayerEndReached:
            printf ("MediaPlayerEndReached\n");
            done = 1;
            break;
        default:
            printf("%s\n", libvlc_event_type_name(pEvt->type));
    }
}
void cbVideoPrerender(void *p_video_data, uint8_t **pp_pixel_buffer, int size) {
    // Locking
    //HANDLE hMutex = ::OpenMutex(SYNCHRONIZE, FALSE, _T("my_mutex"));
    std::cout << "Pre Render\n";
    if (size > videoBufferSize || !videoBuffer)
    {
        printf("Reallocate raw video buffer\n");
        free(videoBuffer);
        videoBuffer = (uint8_t *) malloc(size);
        videoBufferSize = size;
    }

    // videoBuffer = (uint8_t *)malloc(size);
    *pp_pixel_buffer = videoBuffer;
}
void cbVideoPostrender(void *p_video_data, uint8_t *p_pixel_buffer, int width, int height, int pixel_pitch, int size, int64_t pts) {
    // Unlocking
    //CloseHandle(hMutex);
    std::cout << "Post Render\n";
}



int main(int argc, char* argv[])
 {
    // VLC pointers
    libvlc_instance_t *inst;
    libvlc_media_t *m;
    void *pUserData = 0;

    VideoDataStruct dataStruct;

    // VLC options
    char smem_options[1000];

    // RV24
    sprintf(smem_options
        , "#transcode{vcodec=RV24}:smem{"
         "video-prerender-callback=%lld,"
         "video-postrender-callback=%lld,"
         "video-data=%lld,"
         "no-time-sync},"
        , (long long int)(intptr_t)(void*)&cbVideoPrerender
        , (long long int)(intptr_t)(void*)&cbVideoPostrender
        , (long long int)(intptr_t)(void*)&dataStruct
    );

    const char * const vlc_args[] = {
              /*"-I", "dummy", */           // Don't use any interface
              /*"--ignore-config", */       // Don't use VLC's config
              "--extraintf=logger",     // Log anything
              "--verbose=3"/*1*/,            // Be verbose
              /*"--sout", smem_options*/    // Stream to memory
               };

    // We launch VLC
    inst = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);

    /* Create a new item */
    m = libvlc_media_new_path(inst, "test.avi");

    /* Create a media player playing environement */
    mp = libvlc_media_player_new_from_media (m);

    libvlc_event_manager_t* eventManager = libvlc_media_player_event_manager(mp);
    libvlc_event_attach(eventManager, libvlc_MediaPlayerTimeChanged, handleEvent, pUserData);
    libvlc_event_attach(eventManager, libvlc_MediaPlayerEndReached, handleEvent, pUserData);
    libvlc_event_attach(eventManager, libvlc_MediaPlayerPositionChanged, handleEvent, pUserData);


    libvlc_video_set_format(mp, "RV24", 320, 320, 240 * 3 );

    std::cout << "Gonna Play\n";
    /* play the media_player */
    libvlc_media_player_play (mp);
    std::cout << "Played\n";

    while(1)
    {
        if(videoBuffer)                                             // Check for invalid input
        {
            std::cout<<"Got Buffer\n";
//            // CV_8UC3 = 8 bits, 3 chanels
//            cv::Mat img = cv::Mat(cv::Size(240, 320), CV_8UC3, videoBuffer);
//            // cvtColor(img, img, CV_RGB2BGR);
//            cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);         // Create a window for display.
//            cv::imshow("Display window", img);  // Show our image inside it.
//            cv::waitKey(30);
        }
    }
    sleep (500000); /* Let it play a bit */
    libvlc_release (inst);
}
//int main()
//{
//    CvCapture* capture = /*cvCaptureFromCAM(0)*/
//            cvCaptureFromAVI("rtsp://root:SonDCS123@192.168.1.20/axis-media/media.amp");

//    int fps = 30;
//    int width = 320;
//    int height = 320;
//    CvVideoWriter_FFMPEG writer;
//    writer.init();
//    if( !writer.open("test.avi", CV_FOURCC('D', 'I', 'V', 'X'), fps, width, height, true) )
//    {
//        std::cerr <<  "writer not opened\n";
//        return 1;
//    }

//    CvCapture_FFMPEG cap;
//    cap.open(width, height);

//    while(1)
//    {
//        cvGrabFrame(capture);
//        IplImage* img=cvRetrieveFrame(capture);

//        if(!writer.writeFrame((const uchar*)img->imageData,
//                              img->widthStep, img->width, img->height, img->nChannels, img->origin))
//        {
//            std::cerr <<  "writing frame failed\n";
//            return 1;
//        }
//        else
//        {
////            std::cout << (const uchar*)img->imageData ;
//        }

//        uint8_t* buffer = &writer.outbuf[0];
//        int length = writer.out_size;

//        cap.grabFrame(buffer, length);
//        IplImage* img2 = cap.retrieveFrame();
//        if (img2)
//        {
//            cvShowImage("My Window", img2 );
//            char controlKey = cvWaitKey(10);
//            if(controlKey == 'q')
//                break;
//        }
//        else
//        {
//            std::cerr << "img2 empty\n";
//        }
//    };

//    writer.close();
//    cap.close();

//    return 0;
//}
