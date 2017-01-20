#ifndef WINDOWH
#define WINDOWH

#include <stdlib.h>
#include <iterator>
#include <algorithm>
#include <string>
#include <sstream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

/*Rewite this in c++ code*/
template<class T>
const char* toChar(T t) {
    std::ostringstream oss;
    oss << t;
    return oss.str().c_str();
}

class clayWindow {
  public:
  int WIDTH;
  int HEIGHT;
  bool size_dirty;
  Display *display;
  Visual * visual;
  XImage *ximage;
  GC gc;
  char *image32;
  int image_size;
  Window window;
  KeyCode quitKey;
  bool shouldExit;

  void initImage() {
    image32 = new char[WIDTH*HEIGHT*4];
    image_size = WIDTH*HEIGHT*4;
    //ximage = XGetImage(display, window, 0, 0, WIDTH, HEIGHT, 0, ZPixmap);
    //image32 = ximage->data;    
    ximage = XCreateImage(display, visual, 24, ZPixmap, 0, image32, WIDTH, HEIGHT, 32, 0);
  }

  void updateBuffer(char *arr) {
    std::copy(arr, arr+image_size, image32);
  }

  void updateBuffer(float *arr, float n) {
    for (int i=0;i<WIDTH;i++) {
      for (int j=0;j<HEIGHT;j++) {
        int index = i*4+j*WIDTH*4;
        image32[index] = char((arr[index]/n)*255.0f);
        image32[index+1] = char((arr[index+1]/n)*255.0f);
        image32[index+2] = char((arr[index+2]/n)*255.0f);
      }
    }
  }

  //right now this just calls for an update
  //but later this should actually be used for having two buffers
  //and the drawing should happen on another thread
  void swapBuffer() {
    XEvent ev;
    ev.type = Expose;
    XSendEvent(display, window, false, ExposureMask, &ev);
  }

  bool processEvent() {
    XEvent ev;
    XNextEvent(display, &ev);

    switch(ev.type)
    {

    case Expose:
             XPutImage(display, window, gc, ximage, 0, 0, 0, 0, WIDTH, HEIGHT);
             return true;

             break;
    case KeyPress:
             if(ev.xkey.keycode == quitKey) {
               shouldExit = true;
             }
             break;
    case ButtonPress:
             //shouldExit = true;
             break;
    case (ConfigureNotify) :
        // check for window resize
        if (ev.xconfigure.width != WIDTH || ev.xconfigure.height != HEIGHT)
        {
          WIDTH = ev.xconfigure.width;
          HEIGHT = ev.xconfigure.height;
          XDestroyImage(ximage);
          initImage();
          size_dirty = true;
        }
        break;
    }
    return false;
}

  void displayFPS(float fps) {
    const char* st = toChar(fps);
    std::string name = st;
    XStoreName(display, window, name.c_str());
  }

  void displayFT(int passes, float time, float total) {
    const char* p = toChar(passes);
    std::string pn = p;
    const char* t = toChar(time);
    std::string tn = t;
    const char* r = toChar(total);
    std::string tt = r;

    std::string name = "Passes: "+pn+" FTime: "+tn+" Time: "+tt;
    XStoreName(display, window, name.c_str());
  }

  clayWindow() {
    WIDTH = 512; 
    HEIGHT = 512;
    size_dirty = false;
    shouldExit = false;
    display = XOpenDisplay(NULL);
    visual = DefaultVisual(display, 0);
    window = XCreateSimpleWindow(display, RootWindow(display, 0), 0, 0, WIDTH, HEIGHT, 1, 0, (128<<16)+(255<<8)+250);
    gc = DefaultGC(display, 0);  

    quitKey = XKeysymToKeycode(display, XStringToKeysym("Escape"));

    initImage();
    XSelectInput(display, window, ButtonPressMask|ExposureMask|KeyPressMask| StructureNotifyMask);
    XMapWindow(display, window);
    XStoreName(display, window, "xlib window!");
  }
};

#endif
