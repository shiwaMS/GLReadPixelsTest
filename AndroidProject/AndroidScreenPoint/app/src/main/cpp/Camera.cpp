#include <string>
#include <thread>
#include <stdlib.h>
#include <opencv2/opencv.hpp>

// Note 10/26/2017
// Lacking of the definition of GL_RGB8 and GL_BGR, copy it from gl.h which is found the following url
// https://grey.colorado.edu/coin3d/gl_8h_source.html
#ifndef GL_RGB8
#define GL_RGB8 0x8051
#endif /* GL_RGB8 */

#ifndef GL_BGR
#define GL_BGR 0x80E0
#endif /* GL_BGR */

//For Debugging
//#include "DebugCPP.h"
//http://stackoverflow.com/questions/43732825/use-debug-log-from-c/43735531#43735531

//Unity Headers
#include "UnityPluginHeaders/IUnityInterface.h"
#include "UnityPluginHeaders/IUnityGraphics.h"


//Headers for Windows
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(WINAPI_FAMILY)
#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <stdlib.h>
#include "glext.h"
#pragma comment(lib, "opengl32.lib")

//--------------------------------------------------

//Headers for Android
#elif defined(ANDROID) || defined(__ANDROID__)

#include <jni.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android/log.h>
//Link lGLESv2 in the CMakeList.txt file
//LOCAL_LDLIBS += -lGLESv2

//--------------------------------------------------

//Headers for MAC and iOS
//http://nadeausoftware.com/articles/2012/01/c_c_tip_how_use_compiler_predefined_macros_detect_operating_system
#elif defined(__APPLE__) && defined(__MACH__)
//Apple OSX and iOS (Darwin)
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR == 1
//iOS in Xcode simulator
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#elif TARGET_OS_IPHONE == 1
//iOS on iPhone, iPad, etc.
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#elif TARGET_OS_MAC == 1
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#endif

//--------------------------------------------------

//Headers for Linux
#elif defined(__linux__)
#include <GL/gl.h>
#include <GL/glu.h>
#endif


class Camera {
public:
    explicit Camera(int device)
            : camera_(device) {
        startCapture();
    }

    ~Camera() {
        stopCapture();
        camera_.release();
    }

private:
    void startCapture() {
        thread_ = std::thread([this] {
            isRunning_ = true;
            while (isRunning_) {
                camera_ >> image_;
            }
        });
    }

    void stopCapture() {
        isRunning_ = false;
        if (thread_.joinable()) {
            thread_.join();
        }
    }

public:
    void update() {
        glBindTexture(GL_TEXTURE_2D, texture_);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        // カメラ画は 3 byte 毎に色が並ぶが 1, 2, 4 しか境界は指定できないので 1 を指定
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGB8, // RGB それぞれ 8 bit で計 24 bit
                getWidth(),
                getHeight(),
                0,
                GL_BGR, // OpenCV では色は BGR の順に並ぶ
                GL_UNSIGNED_BYTE,
                image_.data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    bool isOpened() const {
        return camera_.isOpened();
    }

    int getWidth() const {
        return image_.cols;
    }

    int getHeight() const {
        return image_.rows;
    }

    int getTexturePtr() const {
        return texture_;
    }

    void setTexturePtr(void *ptr) {
        texture_ = (GLuint) (size_t) ptr;
    }

    void createTexture() {
        glGenTextures(1, &texture_);
    }

private:
    cv::VideoCapture camera_;
    cv::Mat image_;
    GLuint texture_ = 0;

    std::thread thread_;
    bool isRunning_ = false;
};


extern "C"
{
using DebugFuncPtr = void (*)(const char *) ;
using UnityRenderEvent = void (*)(int) ;

namespace {
    DebugFuncPtr debug;
    Camera *g_pCamera;
    GLuint g_pTexture;
}

void *getCamera(int device) {
    g_pCamera = new Camera(device);
    return g_pCamera;
}

void releaseCamera(void *ptr) {
    auto camera = reinterpret_cast<Camera *>(ptr);
    delete camera;
}

bool isCameraOpened(void *ptr) {
    auto camera = reinterpret_cast<Camera *>(ptr);
    return camera->isOpened();
}

int getCameraWidth(void *ptr) {
    auto camera = reinterpret_cast<Camera *>(ptr);
    return camera->getWidth();
}

int getCameraHeight(void *ptr) {
    auto camera = reinterpret_cast<Camera *>(ptr);
    return camera->getHeight();
}

int getCameraTexturePtr(void *ptr) {
    auto camera = reinterpret_cast<Camera *>(ptr);
    return camera->getTexturePtr();
}

void updateCamera(void *ptr) {
    auto camera = reinterpret_cast<Camera *>(ptr);
    camera->update();
}

void setCameraTexturePtr(void *ptr, void *pTexture) {
    auto camera = reinterpret_cast<Camera *>(ptr);
    camera->setTexturePtr(pTexture);
}

void createTexture(void *ptr) {
    auto camera = reinterpret_cast<Camera *>(ptr);
    camera->createTexture();
}

void onRenderEvent(int eventId) {
    g_pCamera->update();
}

UnityRenderEvent getRenderEventFunc() {
    return onRenderEvent;
}
}
