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

#ifndef _META_HEIGHTMAP_H_
#define _META_HEIGTHMAP_H_

#include "MetaObject.h"
#include "OgreAxisAlignedBox.h"

namespace Ogre
{
class TerrainTile;
/// Class representing a meta heightmap.
class MetaHeightmap : public MetaObject
{
public:
	/// Constructor
	MetaHeightmap(MetaWorldFragment *wf, TerrainTile *t, Real groundThreshold);

	/// Adds this meta heightmap to the data grid.
	virtual void updateDataGrid(DataGrid* dataGrid);
	/// Returns the fallof range of the MetaHeightmap.
	Real getFallofRange() const {return mFallofRange; }
	/// Sets the fallof range. A fallof range less than the dataGrids gridsize 
	/// will make the algorithm fail.
	void setFallofRange(Real fallof) {mFallofRange = fallof; }
	virtual AxisAlignedBox getAABB() const;
	

protected:
	TerrainTile *mTerrainTile;
	Real mFallofRange, mGroundThreshold, mGradient;
};

}/// namespace Ogre
#endif ///_META_HEIGTHMAP_H_
