//
//  CustomGLContext.mm
//  qsculpt
//
//  Created by Juan Roberto Cabral Flores on 5/26/11.
//  Copyright 2011 plastilinaware. All rights reserved.
//

#include "CustomGLContext.h"
#import <AppKit/NSOpenGL.h>
#import <CGLRenderers.h>

static void * tryFormat(QGLFormat format, bool useSoftwareRenderer = false) {
    static const int Max = 40;
    
    NSOpenGLPixelFormatAttribute attribs[Max];
    
    int cnt = 0;
    //int devType = paintDevice->devType();
    //bool device_is_pixmap = (devType == QInternal::Pixmap);
    int depth = 32; //device_is_pixmap ? static_cast<QPixmap *>(paintDevice)->depth() : 32;
    
    attribs[cnt++] = NSOpenGLPFAColorSize;
    attribs[cnt++] = depth;
    
//    if (device_is_pixmap) {
//        attribs[cnt++] = NSOpenGLPFAOffScreen;
//    } else {
        if (format.doubleBuffer())
            attribs[cnt++] = NSOpenGLPFADoubleBuffer;
//    }
    //if (glFormat.stereo())
    //    attribs[cnt++] = NSOpenGLPFAStereo;
    if (/*device_is_pixmap ||*/ format.alpha()) {
        attribs[cnt++] = NSOpenGLPFAAlphaSize;
        attribs[cnt++] = 8; //def(format.alphaBufferSize(), 8);
    }
    if (format.stencil()) {
        attribs[cnt++] = NSOpenGLPFAStencilSize;
        attribs[cnt++] = 8; //def(format.stencilBufferSize(), 8);
    }
    if (format.depth()) {
        attribs[cnt++] = NSOpenGLPFADepthSize;
        attribs[cnt++] = 32; //def(format.depthBufferSize(), 32);
    }
    if (format.accum()) {
        attribs[cnt++] = NSOpenGLPFAAccumSize;
        attribs[cnt++] = 1; //def(format.accumBufferSize(), 1);
    }
    if (format.sampleBuffers()) {
        attribs[cnt++] = NSOpenGLPFASampleBuffers;
        attribs[cnt++] = 1;
        attribs[cnt++] = NSOpenGLPFASamples;
        attribs[cnt++] = 4; //def(format.samples(), 4);
    }

    if (format.directRendering())
        attribs[cnt++] = NSOpenGLPFAAccelerated;

    if (useSoftwareRenderer) {
        attribs[cnt++] = NSOpenGLPFARendererID;
        attribs[cnt++] = kCGLRendererAppleSWID;
    }
    //if (devType == QInternal::Pbuffer)
    //    attribs[cnt++] = NSOpenGLPFAPixelBuffer;
    attribs[cnt] = 0;

    Q_ASSERT(cnt < Max);

    return [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
}

CustomGLContext::CustomGLContext()
: QGLContext(QGLFormat::defaultFormat()),
_useSoftwareRenderer(false)
{
    
}

CustomGLContext::CustomGLContext(const QGLFormat & format) 
: QGLContext(format),
_useSoftwareRenderer(false)
{
    
}

void * CustomGLContext::chooseMacVisual(GDHandle /*handle*/) {
    return tryFormat(requestedFormat(), _useSoftwareRenderer);
}

void CustomGLContext::setUseSoftwareRenderer(bool use) {
    _useSoftwareRenderer = use;
}

bool CustomGLContext::useSoftwareRenderer() {
    return _useSoftwareRenderer;
}