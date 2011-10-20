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

#include "MetaHeightmap.h"
#include "DataGrid.h"
#include "TerrainTile.h"

namespace Ogre
{
MetaHeightmap::MetaHeightmap(MetaWorldFragment *wf, TerrainTile * t, Real groundThreshold)
: MetaObject(wf, t->getCenter()), mTerrainTile(t), mGroundThreshold(groundThreshold), 
  mFallofRange(0), mGradient(0)
{
}


/// Adds this meta heightmap to the data grid.
void MetaHeightmap::updateDataGrid(DataGrid* dataGrid)
{

	Real* values = dataGrid->getValues();
	const Vector3* vertices = dataGrid->getVertices();
	Vector3* gradient = dataGrid->getGradient();
	ColourValue* colours = dataGrid->getColours();
	std::pair<Real, MetaWorldFragment*>* worldFragments = dataGrid->getMetaWorldFragments();
	if(!mFallofRange)
	{
		mFallofRange = dataGrid->getGridScale();
		mGradient = mGroundThreshold / dataGrid->getGridScale();
	}
	Vector3 gridMin = dataGrid->getBoundingBox().getMinimum();
	Vector3 gridCenter = dataGrid->getPosition();
	for (size_t z = 0; z <= dataGrid->getNumCellsZ(); ++z)
	{
		for (size_t x = 0; x <= dataGrid->getNumCellsX(); ++x)
		{
				
			Vector3 v = vertices[dataGrid->getGridIndex(x, dataGrid->getNumCellsY(), z)] + gridCenter;
			Real h = mTerrainTile->getHeightAt(v.x, v.z);
			for (size_t y = 0; y <= dataGrid->getNumCellsY(); ++y)
			{
				size_t index = dataGrid->getGridIndex(x, y, z);
				Vector3 v2 = vertices[index]+gridCenter;
				Real d = h-v2.y;
				Real fieldStrength = 0;
				if(d <= -mFallofRange)
					continue;
				if(d >= mFallofRange*2.0)
				{
					fieldStrength = 2.0*mGroundThreshold;
				}
				else
				{
					fieldStrength =  (d + mFallofRange) * mGradient;
				}
				values[index] += fieldStrength;
				

				if (gradient)
					gradient[index] += v;/// Not sure about this...
				
				if(worldFragments)
				{
					if(fieldStrength > worldFragments[index].first)
					{
						worldFragments[index].first = fieldStrength;
						worldFragments[index].second = mMetaWorldFragment;
					}
				}

				//if (colours)
				//	colours[index] = ColourValue::White;
			}
		}
	}
}


AxisAlignedBox MetaHeightmap::getAABB() const
{
	return mTerrainTile->getBoundingBox();
}

}/// namespace Ogre