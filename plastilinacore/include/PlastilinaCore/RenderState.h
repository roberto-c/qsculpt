//
//  RenderState.h
//  QSculpt
//
//  Created by Juan Roberto Cabral Flores on 4/12/15.
//
//

#ifndef QSculpt_RenderState_h
#define QSculpt_RenderState_h

#include <memory>

enum class RenderMode;

class SceneNode;
class CameraNode;
class Material;

struct DLLEXPORT RenderState {
    std::shared_ptr<const SceneNode> root;
    std::shared_ptr<const SceneNode> currentNode;
    RenderMode  		             renderMode;
    std::shared_ptr<CameraNode>      camera;
    std::shared_ptr<Material>	     material;
    
    RenderState 		merge(const RenderState & state) const;
    
    bool                isValid() const;
};

#endif
