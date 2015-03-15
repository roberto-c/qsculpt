//
//  Context.h
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 1/26/14.
//
//

#ifndef PlastilinaCore_Context_h
#define PlastilinaCore_Context_h

#include <memory>
#include "cl.hpp"


namespace core {
    typedef intptr_t CtxAttribute;
    typedef std::vector<CtxAttribute> CtxAttributeList;
    
    enum {
        CTX_ATR_NULL = 0,
        CTX_ATR_GL_NSVIEW,
        CTX_ATR_GL_QTWIDGET,
        CTX_ATR_GL_DOUBLE_BUFFER,
        CTX_ATR_GL_COLOR_SIZE,
        CTX_ATR_CL_ALPHA_SIZE,
        CTX_ATR_GL_DEPTH_SIZE,
        CTX_ATR_GL_STENCIL_SIZE,
        CTX_ATR_GL_ACCUM_SIZE,
        CTX_ATR_CL_SHARE_GL
    };

    class Context {
        struct Impl;
        std::unique_ptr<Impl> d;
        
    public:
        Context(const CtxAttributeList & oclctx) ;
        
        Context(core::Context & oclctx) ;
        
        ~Context() ;
        
        CtxAttribute attribute(CtxAttribute attribute);
        
        void setAttribute(CtxAttribute attributeName, CtxAttribute value);
    };
}

#endif
