/***************************************************************************
 *   Copyright (C) 2017 by Juan Roberto Cabral Flores                      *
 *   roberto.cabral@gmail.com                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <PlastilinaCore/Stable.h>

#include <PlastilinaCore/opengl/Context.h>

#define THROW_IF(condition,msg)  if ((condition)) { throw std::runtime_error(msg); }

namespace gl
{

struct Context::Impl
{
    bool              initialized;
    gl::Device        device;
    HGLRC             glrc;
    HWND              nativeWindow;
    HDC               osDevice;
    HINSTANCE         hInstance;

    Impl()
        : initialized(false)
        , glrc(nullptr)
        , nativeWindow(nullptr)
        , osDevice(nullptr)
        , hInstance(nullptr)
    {
    }

    bool initializeGL();
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

bool Context::Impl::initializeGL()
{
    // Create a dummy window to be able to initialize glew
    if (FALSE == GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
        "", &hInstance))
    {
        return false;
    }
    MSG msg = { 0 };
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
    wc.lpszClassName = TEXT("oglversionchecksample");
    wc.style = CS_OWNDC;
    if (!RegisterClass(&wc))
        return false;

    HWND win = CreateWindow(wc.lpszClassName, TEXT("openglversioncheck"), 
        WS_OVERLAPPEDWINDOW , 0, 0, 640, 480, 0, 0, hInstance, 0);

    while (GetMessage(&msg, NULL, 0, 0) > 0)
        DispatchMessage(&msg);

    DestroyWindow(win);
    FreeLibrary(hInstance);
    return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        PIXELFORMATDESCRIPTOR pfd =
        {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
            PFD_TYPE_RGBA,            //The kind of framebuffer. RGBA or palette.
            32,                        //Colordepth of the framebuffer.
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24,                        //Number of bits for the depthbuffer
            8,                        //Number of bits for the stencilbuffer
            0,                        //Number of Aux buffers in the framebuffer.
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
        };

        HDC ourWindowHandleToDeviceContext = GetDC(hWnd);

        int  letWindowsChooseThisPixelFormat;
        letWindowsChooseThisPixelFormat = ChoosePixelFormat(ourWindowHandleToDeviceContext, &pfd);
        SetPixelFormat(ourWindowHandleToDeviceContext, letWindowsChooseThisPixelFormat, &pfd);

        HGLRC ourOpenGLRenderingContext = wglCreateContext(ourWindowHandleToDeviceContext);
        wglMakeCurrent(ourWindowHandleToDeviceContext, ourOpenGLRenderingContext);

        GLenum glewerror = glewInit();
        glewerror = wglewInit();
        
        wglDeleteContext(ourOpenGLRenderingContext);
        PostQuitMessage(0);
    }
    break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

Context::Context(const core::GraphicsContextCreateInfo & createInfo)
    : d(new Impl())
{
    if (!d->initialized)
        d->initialized = d->initializeGL();

    if (createInfo.osContext != 0)
    {
        d->glrc = reinterpret_cast<HGLRC>(createInfo.osContext);
        d->osDevice = wglGetCurrentDC();
        return;
    }

    std::vector<int> attList;
    std::vector<int> pixelFormatAttList;
    d->nativeWindow = reinterpret_cast<HWND>(createInfo.osWindowHandle);
    d->osDevice = reinterpret_cast<HDC>(createInfo.osHandleEx);

    for (auto att : createInfo.pixelFormatAttributes)
    {
        pixelFormatAttList.push_back(att);
    }

    int pixelFormat;
    UINT numFormats;
    BOOL res = wglChoosePixelFormatARB(d->osDevice, pixelFormatAttList.data(),
        nullptr, 1, &pixelFormat, &numFormats);
    THROW_IF((res == FALSE), "Failed to find a suitable pixel format");
    for (auto att : createInfo.attributesList)
    {
        attList.push_back(att);
    }
    THROW_IF((wglCreateContextAttribsARB == nullptr), "wglCreateContextAttribsARB not available");
    d->glrc = wglCreateContextAttribsARB(d->osDevice, nullptr, attList.data());
    if (d->glrc == nullptr)
    {
        DWORD errorCode = GetLastError();
        TRACE(error) << "Errorcode:" << errorCode << " hex: " << std::hex << errorCode << std::dec;
    }
    THROW_IF((d->glrc == nullptr), "Failed to create an OpenGL context");
}

Context::~Context()
{
}

intptr_t Context::windowHandle() const
{
    return reinterpret_cast<intptr_t>(d->nativeWindow);
}

intptr_t Context::nativeDeviceHandle() const
{
    return reinterpret_cast<intptr_t>(d->osDevice);
}

intptr_t Context::nativeGlContext() const
{
    return reinterpret_cast<intptr_t>(d->glrc);
}

bool Context::makeCurrent()
{
    return wglMakeCurrent(d->osDevice, d->glrc) == TRUE;
}

bool Context::swapBuffers()
{
    return SwapBuffers(d->osDevice) == TRUE;
}

core::IDevice* Context::device()
{
    return nullptr;
}

void Context::resize(int w, int h)
{

}

};
