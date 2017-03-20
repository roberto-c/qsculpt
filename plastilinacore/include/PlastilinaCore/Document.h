/***************************************************************************
 *   Copyright (C) 2006 by Juan Roberto Cabral Flores   *
 *   roberto.cabral@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <PlastilinaCore/IDocument.h>
#include <PlastilinaCore/IIterator.h>

class ISurface;

/**
 * Holds all the document data.
 *
 * @author Juan Roberto Cabral Flores <roberto.cabral@gmail.com>
 */
class DLLEXPORT Document : public IDocument
{
  public:
    typedef std::shared_ptr<Document>       shared_ptr;
    typedef std::shared_ptr<const Document> const_shared_ptr;
    typedef std::weak_ptr<Document>         weak_ptr;
    typedef std::weak_ptr<const Document>   const_weak_ptr;
    typedef std::unique_ptr<Document>       ptr;

    Document();

    virtual ~Document();

    // IDocument interface
    virtual void loadFile(const std::string& fileName);
    virtual void saveFile(const std::string& fileName);
    virtual void selectObject(int iid);
    virtual std::vector<SceneNode::weak_ptr> getSelectedObjects() const;
    virtual Scene::weak_ptr                  scene();
    virtual Scene::weak_ptr                  scene() const;
    virtual Iterator<SceneNode>              sceneIterator();
    virtual Iterator<SceneNode>              constSceneIterator() const;
    Iterator<ISurface>                       surfaceIterator();

    virtual SceneNode::shared_ptr findItem(uint32_t iid);

    virtual SceneNode::shared_ptr index(
        int                          row,
        const SceneNode::shared_ptr& parent = SceneNode::shared_ptr()) const;

    virtual SceneNode::shared_ptr
    parent(const SceneNode::shared_ptr& index) const;

    virtual size_t childrenCount(
        const SceneNode::shared_ptr& parent = SceneNode::shared_ptr()) const;

    virtual void
    addItem(const SceneNode::shared_ptr& node,
            const SceneNode::shared_ptr& parent = SceneNode::shared_ptr());

    CameraNode::shared_ptr getCamera() const;

    // End IDocument interface

  private:
    struct Impl;
    std::unique_ptr<Impl> _d;
};

#endif
