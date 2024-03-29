/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2006 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.

You may alternatively use this source under the terms of a specific version of
the OGRE Unrestricted License provided you have obtained such a license from
Torus Knot Software Ltd.
-----------------------------------------------------------------------------
*/
/***************************************************************************
octreenode.cpp  -  description
-------------------
begin                : Fri Sep 27 2002
copyright            : (C) 2002 by Jon Anderson
email                : janders@users.sf.net

Enhancements 2003 - 2004 (C) The OGRE Team

***************************************************************************/

#include <OgreRoot.h>

#include <OgreOctreeNode.h>
#include <OgreOctreeSceneManager.h>

namespace Ogre
{
unsigned long green = 0xFFFFFFFF;

unsigned short OctreeNode::mIndexes[ 24 ] = {0, 1, 1, 2, 2, 3, 3, 0,       //back
        0, 6, 6, 5, 5, 1,             //left
        3, 7, 7, 4, 4, 2,             //right
        6, 7, 5, 4 };          //front
unsigned long OctreeNode::mColors[ 8 ] = {green, green, green, green, green, green, green, green };

OctreeNode::OctreeNode( SceneManager* creator ) : SceneNode( creator )
{
    mOctant = 0;
}

OctreeNode::OctreeNode( SceneManager* creator, const String& name ) : SceneNode( creator, name )
{
    mOctant = 0;
}

OctreeNode::~OctreeNode()
{}
void OctreeNode::_removeNodeAndChildren( )
{
    static_cast< OctreeSceneManager * > ( mCreator ) -> _removeOctreeNode( this ); 
    //remove all the children nodes as well from the octree.
    ChildNodeMap::iterator it = mChildren.begin();
    while( it != mChildren.end() )
    {
        static_cast<OctreeNode *>( it->second ) -> _removeNodeAndChildren();
        ++it;
    }
}
Node * OctreeNode::removeChild( unsigned short index )
{
    OctreeNode *on = static_cast<OctreeNode* >( SceneNode::removeChild( index ) );
    on -> _removeNodeAndChildren(); 
    return on; 
}
Node * OctreeNode::removeChild( Node* child )
{
    OctreeNode *on = static_cast<OctreeNode* >( SceneNode::removeChild( child ) );
    on -> _removeNodeAndChildren(); 
    return on; 
}
void OctreeNode::removeAllChildren()
{
	ChildNodeMap::iterator i, iend;
	iend = mChildren.end();
	for (i = mChildren.begin(); i != iend; ++i)
	{
		OctreeNode* on = static_cast<OctreeNode*>(i->second);
		on->setParent(0);
		on->_removeNodeAndChildren();
	}
	mChildren.clear();
	mChildrenToUpdate.clear();
	
}
    
Node * OctreeNode::removeChild( const String & name )
{
    OctreeNode *on = static_cast< OctreeNode * >( SceneNode::removeChild(  name ) );
    on -> _removeNodeAndChildren( ); 
    return on; 
}

//same as SceneNode, only it doesn't care about children...
void OctreeNode::_updateBounds( void )
{
    mWorldAABB.setNull();
    mLocalAABB.setNull();

    // Update bounds from own attached objects
    ObjectMap::iterator i = mObjectsByName.begin();
    AxisAlignedBox bx;

    while ( i != mObjectsByName.end() )
    {

        // Get local bounds of object
        bx = i->second ->getBoundingBox();

        mLocalAABB.merge( bx );

        mWorldAABB.merge( i->second ->getWorldBoundingBox(true) );
        ++i;
    }


    //update the OctreeSceneManager that things might have moved.
    // if it hasn't been added to the octree, add it, and if has moved
    // enough to leave it's current node, we'll update it.
    if ( ! mWorldAABB.isNull() )
    {
        static_cast < OctreeSceneManager * > ( mCreator ) -> _updateOctreeNode( this );
    }

}

/** Since we are loose, only check the center.
*/
bool OctreeNode::_isIn( AxisAlignedBox &box )
{
	// Always fail if not in the scene graph or box is null
	if (!mIsInSceneGraph || box.isNull()) return false;

	// Always succeed if AABB is infinite
	if (box.isInfinite())
		return true;

    Vector3 center = mWorldAABB.getMaximum().midPoint( mWorldAABB.getMinimum() );

    Vector3 bmin = box.getMinimum();
    Vector3 bmax = box.getMaximum();

    bool centre = ( bmax > center && bmin < center );
	if (!centre)
		return false;

	// Even if covering the centre line, need to make sure this BB is not large
	// enough to require being moved up into parent. When added, bboxes would
	// end up in parent due to cascade but when updating need to deal with
	// bbox growing too large for this child
	Vector3 octreeSize = bmax - bmin;
	Vector3 nodeSize = mWorldAABB.getMaximum() - mWorldAABB.getMinimum();
	return nodeSize < octreeSize;

}

/** Addes the attached objects of this OctreeScene node into the queue. */
void OctreeNode::_addToRenderQueue( Camera* cam, RenderQueue *queue, 
	bool onlyShadowCasters, VisibleObjectsBoundsInfo* visibleBounds )
{
    ObjectMap::iterator mit = mObjectsByName.begin();

    while ( mit != mObjectsByName.end() )
    {
        MovableObject * mo = mit->second;

        mo->_notifyCurrentCamera(cam);
        if ( mo->isVisible() &&
            (!onlyShadowCasters || mo->getCastShadows()))
        {
            mo -> _updateRenderQueue( queue );

			if (visibleBounds)
			{
				visibleBounds->merge(mo->getWorldBoundingBox(true), 
					mo->getWorldBoundingSphere(true), cam);
			}
        }

        ++mit;
    }

}


void OctreeNode::getRenderOperation( RenderOperation& rend )
{

    /* TODO
    rend.useIndexes = true;
    rend.numTextureCoordSets = 0; // no textures
    rend.vertexOptions = LegacyRenderOperation::VO_DIFFUSE_COLOURS;
    rend.operationType = LegacyRenderOperation::OT_LINE_LIST;
    rend.numVertices = 8;
    rend.numIndexes = 24;

    rend.pVertices = mCorners;
    rend.pIndexes = mIndexes;
    rend.pDiffuseColour = mColors;

    const Vector3 * corners = _getLocalAABB().getAllCorners();

    int index = 0;

    for ( int i = 0; i < 8; i++ )
    {
        rend.pVertices[ index ] = corners[ i ].x;
        index++;
        rend.pVertices[ index ] = corners[ i ].y;
        index++;
        rend.pVertices[ index ] = corners[ i ].z;
        index++;
    }
    */


}
}
