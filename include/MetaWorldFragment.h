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

#ifndef WORLD_FRAGMENT_H
#define WORLD_FRAGMENT_H

/***************************************************************
A MetaWorldFragment is the basic building block of  the world - 
it has its own IsoSurface.

When created an IsoSurface should be created...
The MetaWorldFragment will also contain portals to other
MetaWorldFragments for visibility culling (todo).
****************************************************************/

#include <vector>
#include "DataGrid.h"

namespace Ogre
{
class IsoSurfaceRenderable;
class IsoSurfaceBuilder;

class MetaWorldFragment
{
protected:
	IsoSurfaceRenderable *mSurf;
	Vector3 mPosition;
	AxisAlignedBox mAabb;
	static Real mGridScale;
	static Real mSize;
	//Vector of potentially overlapping MetaObjects
	std::vector<MetaObject*> mObjs;
	std::vector<MetaWorldFragment*> mAdjacentFragments;
	typedef std::vector<MetaWorldFragment*> WfList;
	typedef std::vector<MetaWorldFragment*>::iterator WfIter;
	typedef std::vector<MetaWorldFragment*>::const_iterator WfConstIter;

	/// position in y, counted in tile-sizes.
	size_t mYLevel;
	static std::string mMaterialName;
//	WfList mAdjacentFragments;

public:
	///Creates new MetaWorldFragment, as well as IsoSuface and grid as needed.
	MetaWorldFragment(IsoSurfaceRenderable *is = 0, const Vector3 &position = Vector3::ZERO, int ylevel = 0);
	///Adds MetaObject to mObjs, and to mMoDataGrid
	void addMetaObject(MetaObject *mo);
	///Updates IsoSurface
	void update(IsoSurfaceBuilder *builder);
	int getNumMetaObjects() {return mObjs.size();} const
	AxisAlignedBox getAABB() {return mAabb;} const
	Vector3 getPosition() {return mPosition;} const
	bool empty() {return mObjs.empty();}
	static Real getScale() {return mGridScale;}
	static Real getSize() {return mSize;}
	static void setScale(Real s) {mGridScale = s;}
	static void setSize(Real s) {mSize = s;}

	IsoSurfaceRenderable * getIsoSurface() {return mSurf;}
	size_t getYLevel() { return mYLevel; }

	static void setMaterialName(const std::string &name) {mMaterialName = name;}
	static const std::string &getMaterialName() {return mMaterialName;}
protected:
	void addToWfList(MetaWorldFragment *wf);

};

}/// namespace Ogre
#endif