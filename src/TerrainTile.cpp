/*
-----------------------------------------------------------------------------
This source file is part of the OverhangTerrainSceneManager
Plugin for OGRE
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2007 Martin Enge
martin.enge@gmail.com

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

-----------------------------------------------------------------------------
*/

#include "TerrainTile.h"
#include "OgreSceneNode.h"
#include "OverhangTerrainRenderable.h"
#include "IsoSurfaceRenderable.h"
#include "MetaWorldFragment.h"
#include "MetaHeightmap.h"


namespace Ogre
{

TerrainTile::TerrainTile(const String& name, OverhangTerrainSceneManager* tsm, SceneNode *c)
: mTerrainRenderable(0), mSceneNode(c)
{
	mTerrainRenderable = new OverhangTerrainRenderable(name, tsm);
	for ( int i = 0; i < 4; i++ )
	{
		mNeighbors[ i ] = 0;
	}

}

TerrainTile::~TerrainTile()
{
	deleteGeometry();
}
void TerrainTile::_setNeighbor(Neighbor n, TerrainTile * t )
{
	mNeighbors[n] = t;
	if(mTerrainRenderable)
		mTerrainRenderable->_setNeighbor(OverhangTerrainRenderable::Neighbor(n), t->getTerrainRenderable());
}

void TerrainTile::deleteGeometry()
{
	if(!mMetaRenderables.empty())
	{
		for(std::vector<IsoSurfaceRenderable*>::iterator it = mMetaRenderables.begin(); it != mMetaRenderables.end(); ++it)
			delete (*it);
		for(std::vector<MetaWorldFragment*>::iterator it = mMetaWorldFragments.begin(); it != mMetaWorldFragments.end(); ++it)
			delete (*it);
		mMetaRenderables.clear();
		mMetaWorldFragments.clear();
	}
	if(mTerrainRenderable)
		mTerrainRenderable->deleteGeometry();
}

void TerrainTile::setRenderQueueGroup(uint8 qid)
{
	if(!mMetaRenderables.empty())
	{
		for(std::vector<IsoSurfaceRenderable*>::iterator it = mMetaRenderables.begin(); it != mMetaRenderables.end(); ++it)
			(*it)->setRenderQueueGroup(qid);
	}
	if(mTerrainRenderable)
	{
		mTerrainRenderable->setRenderQueueGroup(qid);
	}
}

void TerrainTile::initialise(int startx, int startz, Real* pageHeightData)
{
	mTerrainRenderable->initialise(startx, startz, pageHeightData);
	mSceneNode->attachObject(mTerrainRenderable);
}

void TerrainTile::setMaterial(const MaterialPtr& m )
{
	assert( mTerrainRenderable || !mMetaRenderables.empty() );
	if(!mMetaRenderables.empty())
	{
		for(std::vector<IsoSurfaceRenderable*>::iterator it = mMetaRenderables.begin(); it != mMetaRenderables.end(); ++it)
			(*it)->setMaterial(m->getName());
	}
	if(mTerrainRenderable)
	{
		mTerrainRenderable->setMaterial(m);
	}
}

void TerrainTile::_calculateNormals()
{
	assert( mTerrainRenderable );
//	if(mMetaRenderable)
//		mMetaRenderable->_calculateNormals();
	if(mTerrainRenderable)
		mTerrainRenderable->_calculateNormals();
}

const AxisAlignedBox& TerrainTile::getBoundingBox() const
{
	assert( mTerrainRenderable);
/*	if(!mMetaWorldFragments.empty())
	{
		std::vector<MetaWorldFragment*>::const_iterator it = mMetaWorldFragments.begin();
		AxisAlignedBox aabb = (*it)->getAABB();
		while(it != mMetaWorldFragments.end())
		{
			aabb.merge((*it)->getAABB());
			++it;
		}
		return aabb;
	}
	else
*/	{
		return mTerrainRenderable->getBoundingBox();
	}
}

const Vector3& TerrainTile::getCenter() const
{
	assert(mTerrainRenderable);
	return mTerrainRenderable->getCenter();
}

bool TerrainTile::intersectSegment( const Vector3 & start, const Vector3 & end, Vector3 * result )
{
	assert(mTerrainRenderable);
	return mTerrainRenderable->intersectSegment( start, end, result);
}
float TerrainTile::getHeightAt( float x, float y )
{
	assert(mTerrainRenderable);
	return mTerrainRenderable->getHeightAt(x,y);
}

void TerrainTile::addMetaObject(MetaObject *mo, int level, IsoSurfaceBuilder *isb, const Vector3 &pos)
{
	// check if level already exists.
	for(std::vector<MetaWorldFragment*>::iterator it = mMetaWorldFragments.begin(); it != mMetaWorldFragments.end(); ++it)
	{
		if(level == (*it)->getYLevel())
		{
			(*it)->addMetaObject(mo);
			(*it)->update(isb);
			return;
		}
	}
	//this y-level didn't exist - we have to create it!
	MetaWorldFragment *wf = new MetaWorldFragment(0, mTerrainRenderable->getWorldPosition()+Vector3(0,level*MetaWorldFragment::getScale(),0), level);
	MetaHeightmap *mhm = new MetaHeightmap(0, this, 0.2);
	wf->addMetaObject(mhm);
	wf->addMetaObject(mo);
	mMetaWorldFragments.push_back(wf);
	wf->update(isb);

	mMetaRenderables.push_back(wf->getIsoSurface());
	SceneNode *child = mSceneNode->createChildSceneNode(pos);
	child->attachObject(wf->getIsoSurface());
	//child->showBoundingBox(true);

	// if meta object intersects terrain renderable
	if(mTerrainRenderable->getBoundingBox().intersects(mo->getAABB())) {

		// make terrain renderable invisible
		mTerrainRenderable->setVisible(false);

		//finally, a cheap hack to fix stitching. Simply force renderlevel to highest detail on surrounding tiles.
		//mTerrainRenderable->setForcedRenderLevel(0);
		for (int i = 0; i < 4; ++i) {
			OverhangTerrainRenderable::Neighbor n =
					OverhangTerrainRenderable::Neighbor(i);
			OverhangTerrainRenderable *t = mTerrainRenderable->_getNeighbor(n);
			if (t)
				t->setForcedRenderLevel(0);
		}
	}
}



}///namespace Ogre
