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

#include "MetaWorldFragment.h"
#include "DataGrid.h"
#include "MetaObject.h"
#include "IsoSurfaceBuilder.h"
#include "IsoSurfaceRenderable.h"

//#define NUM_CELLS 30
//#define WIDTH 4.0
//#define SCALE 4.0/30.0
namespace Ogre
{

Real MetaWorldFragment::mGridScale = 0;
Real MetaWorldFragment::mSize = 0;
std::string MetaWorldFragment::mMaterialName = "";


MetaWorldFragment::MetaWorldFragment(IsoSurfaceRenderable *is, const Vector3 &position, int ylevel)
: 	mSurf(is), mPosition(position), mYLevel(ylevel)
{
}

///Adds MetaObject to mObjs, and to mMoDataGrid
void MetaWorldFragment::addMetaObject(MetaObject *mo)
{
	if(mo->getMetaWorldFragment() != this && mo->getMetaWorldFragment() != 0)
		addToWfList(mo->getMetaWorldFragment());
	mObjs.push_back(mo);
}

///Updates IsoSurface
void MetaWorldFragment::update(IsoSurfaceBuilder *builder)
{
	if(!mSurf)
	{
		mSurf = new IsoSurfaceRenderable();
		mSurf->initialize(builder);
//		mSurf->setMaterial("OverhangTerrain_simple"); //hm... should this be done here?
		if(!mMaterialName.empty())
			mSurf->setMaterial(mMaterialName); //hm... should this be done here?
	}
	/// Zero data grid, then add the fields of objects to it.
	DataGrid * dg = builder->getDataGrid();
	dg->clear();
	for(std::vector<MetaObject*>::iterator it = mObjs.begin(); it != mObjs.end(); ++it)
	{
		(*it)->updateDataGrid(dg);
	}
	builder->update(mSurf);
	mSurf->setBoundingBox(dg->getBoundingBox());
}

void MetaWorldFragment::addToWfList(MetaWorldFragment *wf)
{
	for(WfConstIter it = mAdjacentFragments.begin(); it != mAdjacentFragments.end(); ++it)
	{
		if( *it != this )
			mAdjacentFragments.push_back(*it);
	}
}

}/// namespace Ogre
