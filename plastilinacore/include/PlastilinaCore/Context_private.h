//
//  Context_private.h
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 1/26/14.
//
//

#ifndef PlastilinaCore_Context_private_h
#define PlastilinaCore_Context_private_h

#include <PlastilinaCore/Context.h>

OBJC_CLASS(NSView);
OBJC_CLASS(NSOpenGLContext);

namespace core {
    struct Context::Impl {
        cl::Context oclctx;
#if defined(__APPLE__)
        
        NSOpenGLContext * glctx;
        NSView			* view;
#endif
        void createGlCtx(const CtxAttributeList & attributes);
        void createClCtx(const CtxAttributeList & attributes);
        bool attribute(CtxAttribute attribute, CtxAttribute * outValue);
        bool setAttribute(CtxAttribute name, CtxAttribute value);
    };
}

#endif
