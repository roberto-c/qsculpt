//
//  RenderState.h
//  QSculpt
//
//  Created by Juan Roberto Cabral Flores on 4/12/15.
//
//

#ifndef QSculpt_RenderState_h
#define QSculpt_RenderState_h

enum class RenderMode;

class SceneNode;
class CameraNode;
class Material;

struct DLLEXPORT RenderState {
    const SceneNode 	*root;
    const SceneNode 	*currentNode;
    RenderMode  		renderMode;
    CameraNode			*camera;
    Material			*material;
    
    RenderState 		merge(const RenderState & state) const;
    
    bool                isValid() const;
};

#endif
