#pragma once

//
//  Color.h
//  qsculpt
//
//  Created by Juan Roberto Cabral Flores on 8/2/11.
//  Copyright 2011 plastilinaware. All rights reserved.
//
#include <cstdint>
#include "Eigen/Core"

class Color {
    Eigen::Vector4f _d;
    
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    Color() : _d(Eigen::Vector4f(1, 1, 1,1)) {}
    
    Color(const Eigen::Vector4f & color) : _d(color)
    {
    }
    
    Color(float r, float g, float b, float a=1.0) : _d(Eigen::Vector4f(r, g, b, a)) {}
    
    Eigen::Vector4f & data() { return _d; }
    Eigen::Vector4f data() const {return _d; }
    
    float r() const { return _d.x(); }
    void r(float v) { _d.x() = v; }
    
    float g() const { return _d.y(); }
    void g(float v) { _d.y() = v; }
    
    float b() const { return _d.z(); }
    void b(float v) { _d.z() = v; }
    
    float a() const { return _d.w(); }
    void a(float v) { _d.w() = v; }
    
    static Color fromUintRGBA(uint32_t color) {
        Color c = Color(((color & 0xFF000000) >> 24) / 255.0f,
                        ((color & 0x00FF0000) >> 16) / 255.0f,
                        ((color & 0x0000FF00) >>  8) / 255.0f,
                        ((color & 0x000000FF) >>  0) / 255.0f);
        return c;
    }
    
    static Color fromUintBGRA(uint32_t color) {
        Color c = Color(((color & 0x0000FF00) >> 8) / 255.0f,
                        ((color & 0x00FF0000) >> 16) / 255.0f,
                        ((color & 0xFF000000) >> 24) / 255.0f,
                        ((color & 0x000000FF) >>  0) / 255.0f);
        return c;
    }
    
    static Color fromUintRGB(uint32_t color) {
        Color c = Color(((color & 0xFF000000) >> 24) / 255.0f,
                        ((color & 0x00FF0000) >> 16) / 255.0f,
                        ((color & 0x0000FF00) >>  8) / 255.0f,
                        1.0f);
        return c;
    }
    
    static Color fromUintBGR(uint32_t color) {
        Color c = Color(((color & 0x0000FF00) >> 8) / 255.0f,
                        ((color & 0x00FF0000) >> 16) / 255.0f,
                        ((color & 0xFF000000) >> 24) / 255.0f,
                        1.0f);
        return c;
    }
};