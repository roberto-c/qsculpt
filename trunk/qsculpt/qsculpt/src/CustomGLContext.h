//
//  CustomGLContext.h
//  qsculpt
//
//  Created by Juan Roberto Cabral Flores on 5/26/11.
//  Copyright 2011 plastilinaware. All rights reserved.
//
#ifndef CUSTOMGLCONTEXT_H_
#define CUSTOMGLCONTEXT_H_

#include <QtOpenGL/QGLContext>

class CustomGLContext : public QGLContext {
    bool _useSoftwareRenderer;
    
public:
    CustomGLContext();
    
    CustomGLContext(const QGLFormat & format);
    
    void * chooseMacVisual(GDHandle /*handle*/);
    
    void setUseSoftwareRenderer(bool use);
    
    bool useSoftwareRenderer();
};

#endif
