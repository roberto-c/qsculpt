//
//  Context_private.h
//  PlastilinaCore
//
//  Created by Juan Roberto Cabral Flores on 1/26/14.
//
//
#pragma once

#include <PlastilinaCore/Context.h>
#include <PlastilinaCore/opencl/OpenCL.h>

#ifdef HAS_VULKAN
#include <PlastilinaCore/vulkan/Context.h>
#endif

OBJC_CLASS(NSView);
OBJC_CLASS(NSOpenGLContext);

namespace vulkan
{
class Context;
};

namespace core
{
struct Context::Impl
{
    cl::Context                      oclctx;

#ifdef HAS_VULKAN
    std::unique_ptr<vulkan::Context> vkCtx;
#endif

#if defined(__APPLE__)

    NSOpenGLContext* glctx;
    NSView*          view;
#endif
    void createGlCtx(const CtxAttributeList& attributes);
    void createClCtx(const CtxAttributeList& attributes);
    void createVkCtx(const CtxAttributeList& attributes);
    bool attribute(CtxAttribute attribute, CtxAttribute* outValue);
    bool setAttribute(CtxAttribute name, CtxAttribute value);
};
}
