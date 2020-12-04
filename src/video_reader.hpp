#ifndef video_reader_hpp
#define video_reader_hpp
#include "Queue.cpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <inttypes.h>
}



struct VideoReaderState {
     // Public things for other parts of the program to read from
     int width, height;
     AVRational time_base;

     // Private internal state
     AVFormatContext* av_format_ctx;
     AVCodecContext* av_codec_ctx;
     int video_stream_index;
     AVFrame* av_frame;
     AVPacket* av_packet;
     SwsContext* sws_scaler_ctx;
    int64_t nb_frames;
 };

struct FrameInfo{
    AVRational time_base;
    uint8_t* frame_data;
    int64_t pts;
    const int frame_width;
    const int frame_height;
};

static Queue<FrameInfo*> SharedQueue;

class VideoReader{
    
    public:
        
        VideoReader(){}

        static bool video_reader_open(VideoReaderState* state, const char* filename);
        static bool video_reader_read_frame(VideoReaderState* state, uint8_t* frame_buffer, int64_t* pts);
        static bool video_reader_seek_frame(VideoReaderState* state, int64_t ts);
        static void video_reader_close(VideoReaderState* state);
};

#endif

