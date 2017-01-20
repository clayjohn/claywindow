#ifndef RENDERERH
#define RENDERERH

#include "window.h"
#include <iostream>
#include <ctime> 
#include <thread>

class Renderer {
  public:
    clayWindow window;
    float * framebuffer;
    int framebuffer_size;
    int framebuffer_dirty;
    int WIDTH, HEIGHT;
    int frame;
    int render_passes;
    clock_t _clock;
    void (Renderer::*_render)(void);
    bool realTime;
    clock_t lastPass;
    bool buffer_drawn;

    Renderer() {

      window = clayWindow();
      //replace with call to resize

      _render = &Renderer::drawDefault;
      resize(false);
      frame = 0;
      render_passes = 0;
      realTime = false;
      //for some reason passes increments three times before anything is drawn
      //probably due to the window resizing!
      _clock = clock();
      lastPass = clock();
      buffer_drawn = true;
    }

    ~Renderer() {
      delete framebuffer;
    }

    void render() {
      if (!framebuffer_dirty&& buffer_drawn)
      {
        (this->*_render)();
        buffer_drawn = false;
      }
    }

    void resize(bool update = true) {
      WIDTH = window.WIDTH;
      HEIGHT = window.HEIGHT;
      if (update) {
        delete framebuffer;
      }
      framebuffer = new float[WIDTH*HEIGHT*4]();
      framebuffer_size = WIDTH*HEIGHT*4;
      framebuffer_dirty = false;

      render_passes = 0;
    }

    // use for updateing the entire frame
    void drawFrame() {
      //not using the framebuffer is kinda hacky but it results in slighter faster speed
      populate(window.image32, 0, HEIGHT, WIDTH, HEIGHT);
      //framebuffer_dirty = true;
      window.swapBuffer();
    }


    void drawDefault() {
      char *buffer = window.image32;
      int width = WIDTH;
      int height = HEIGHT;
      int red, green, blue;
      for (int i = 0;i<width;i++) {
        for (int j = 0;j<height;j++) {
          red = int((float(i)/float(width))*255);
          green = int((float(j)/float(height))*255);
          blue = 128;
          buffer[i*4+j*width*4+0] = blue;
          buffer[i*4+j*width*4+1] = green;
          buffer[i*4+j*width*4+2] = red;
        }
      }
      window.swapBuffer();
    }

    void populate(char *, int, int, int, int);
    void populate(float *, int, int, int, int);
    //used for drawing individual pixel
    void drawPixel();
    void drawPixels() {//loop over all pixels

    } 
    //used for casting a ray from pixel
    void castRayFromPixel(float *, int, int, int, int);
    void castRays() { //cast rays for every pixel

      int red, green, blue;
      for (int i = 0;i<WIDTH;i++) {
        for (int j = 0;j<HEIGHT;j++) {
          castRayFromPixel(framebuffer, i, j, WIDTH, HEIGHT);
        }
      }
      framebuffer_dirty = true;
      render_passes++;
      window.swapBuffer();
    }

    void runloop() {
      if (window.size_dirty) {
        resize();
        window.size_dirty = false;
      }

      if (framebuffer_dirty) {
        window.updateBuffer(framebuffer, float(render_passes));
        framebuffer_dirty = false;
      }
      
      if(!realTime) {
        clock_t current = clock();
        window.displayFT(render_passes, float(current-lastPass)/CLOCKS_PER_SEC, float(current-_clock)/CLOCKS_PER_SEC);
        lastPass = current;
      } else {
        if (frame%60==0) {
          window.displayFPS(1.0f/((float(clock()-_clock)/CLOCKS_PER_SEC)/frame));
        }
      }
      buffer_drawn = window.processEvent();
      frame++;
    }
};

#endif
