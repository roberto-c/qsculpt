#pragma once

//
//  Color.h
//  qsculpt
//
//  Created by Juan Roberto Cabral Flores on 8/2/11.
//  Copyright 2011 plastilinaware. All rights reserved.
//

class Color {
    Vector4 _d;
    
public:
    Color() : _d(Vector4(1, 1, 1,1)) {}
    Color(float r, float g, float b, float a=1.0) : _d(Vector4(r, g, b, a)) {}
    
    
    
    float r() const { return _d.x(); }
    void r(float v) { _d.x() = v; }
    
    float g() const { return _d.y(); }
    void g(float v) { _d.y() = v; }
    
    float b() const { return _d.z(); }
    void b(float v) { _d.z() = v; }
    
    float a() const { return _d.w(); }
    void a(float v) { _d.w() = v; }
    
    static Color fromUintRGBA(uint color) {
        Color c = Color(((color & 0xFF000000) >> 24) / 255.0,
                        ((color & 0x00FF0000) >> 16) / 255.0,
                        ((color & 0x0000FF00) >>  8) / 255.0,
                        ((color & 0x000000FF) >>  0) / 255.0);
        return c;
    }
    
    static Color fromUintBGRA(uint color) {
        Color c = Color(((color & 0x0000FF00) >> 8) / 255.0,
                        ((color & 0x00FF0000) >> 16) / 255.0,
                        ((color & 0xFF000000) >> 24) / 255.0,
                        ((color & 0x000000FF) >>  0) / 255.0);
        return c;
    }
    
    static Color fromUintRGB(uint color) {
        Color c = Color(((color & 0xFF000000) >> 24) / 255.0,
                        ((color & 0x00FF0000) >> 16) / 255.0,
                        ((color & 0x0000FF00) >>  8) / 255.0,
                        1.0f);
        return c;
    }
    
    static Color fromUintBGR(uint color) {
        Color c = Color(((color & 0x0000FF00) >> 8) / 255.0,
                        ((color & 0x00FF0000) >> 16) / 255.0,
                        ((color & 0xFF000000) >> 24) / 255.0,
                        1.0f);
        return c;
    }
};