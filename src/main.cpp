#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include "video_reader.hpp"

class MyMain{

public:

    static void gui() {
        GLFWwindow* window;
        
        if (!glfwInit()) {
            printf("Couldn't init GLFW\n");
            return;
        }

        window = glfwCreateWindow(800, 480, "Hello World", NULL, NULL);
        if (!window) {
            printf("Couldn't open window\n");
           return;
        }

        glfwMakeContextCurrent(window);

        // Generate texture
        GLuint tex_handle;
        glGenTextures(1, &tex_handle);
        glBindTexture(GL_TEXTURE_2D, tex_handle);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        while (!glfwWindowShouldClose(window)) {
            
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                // Set up orphographic projection
                int window_width, window_height;
                glfwGetFramebufferSize(window, &window_width, &window_height);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(0, window_width, window_height, 0, -1, 1);
                glMatrixMode(GL_MODELVIEW);
               
                FrameInfo* fr=(FrameInfo*)SharedQueue.pop();
                
                static bool first_frame = true;
                if (first_frame) {
                    glfwSetTime(0.0);
                    first_frame = false;
                }

                double pt_in_seconds = fr->pts * (double)fr->time_base.num / (double)fr->time_base.den;
                while (pt_in_seconds > glfwGetTime()) {
                    glfwWaitEventsTimeout(pt_in_seconds - glfwGetTime());
                }

                glBindTexture(GL_TEXTURE_2D, tex_handle);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fr->frame_width, fr->frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, fr->frame_data);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, tex_handle);
                glBegin(GL_QUADS);
                    glTexCoord2d(0,0); glVertex2i(200, 200);
                    glTexCoord2d(1,0); glVertex2i(200 + fr->frame_width, 200);
                    glTexCoord2d(1,1); glVertex2i(200 + fr->frame_width, 200 + fr->frame_height);
                    glTexCoord2d(0,1); glVertex2i(200, 200 + fr->frame_height);
                glEnd();
                glDisable(GL_TEXTURE_2D);
            
                glfwSwapBuffers(window);
                glfwPollEvents();
        }
    }
};

int FrameExtract(){
    
    VideoReaderState vr_state{0};
    
    if (!VideoReader::video_reader_open(&vr_state, "/Users/manojpattnayak/Downloads/videos/file_example_AVI_480_750kB.avi")) {
        printf("Couldn't open video file (make sure you set a video file that exists)\n");
        return 1;
    }
    
    // Allocate frame buffer
    constexpr int ALIGNMENT = 128;
    const int frame_width = vr_state.width;
    const int frame_height = vr_state.height;
    uint8_t* frame_data;
    if (posix_memalign((void**)&frame_data, ALIGNMENT, frame_width * frame_height * 4) != 0) {
        printf("Couldn't allocate frame buffer\n");
        return 1;
    }
    int64_t temp=vr_state.nb_frames;
    while (temp > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        // Read a new frame and load it into texture
        int64_t pts;
        if (!VideoReader::video_reader_read_frame(&vr_state, frame_data, &pts)) {
            printf("Couldn't load video frame\n");
            return 1;
        }
        FrameInfo info={vr_state.time_base,frame_data,pts,vr_state.width,vr_state.height};
        SharedQueue.push(&info);
        temp--;
    }
    printf("Show Ends !");
    VideoReader::video_reader_close(&vr_state);
    return 0;
}





int main(int argc, const char** argv) {
       std::thread thread_1(&FrameExtract);
       MyMain::gui();
       thread_1.join();
};
