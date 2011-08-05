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
};