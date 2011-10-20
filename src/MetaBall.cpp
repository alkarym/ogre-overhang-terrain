/*
-----------------------------------------------------------------------------
This source file is part of the OverhangTerrainSceneManager
Plugin for OGRE
For the latest info, see http://www.ogre3d.org/phpBB2/viewtopic.php?t=32486

Copyright (c) 2007 Martin Enge. Based on code from DWORD, released into public domain.
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

#include "MetaBall.h"
#include "DataGrid.h"
#include "MetaWorldFragment.h"

namespace Ogre
{
MetaBall::MetaBall(MetaWorldFragment *wf, const Vector3& position, Real radius, bool excavating)
  : MetaObject(wf, position), mRadius(radius), mExcavating(excavating)
{
}

void MetaBall::updateDataGrid(DataGrid* dataGrid)
{
	AxisAlignedBox aabb(
		mPosition - mRadius*Vector3::UNIT_SCALE,
		mPosition + mRadius*Vector3::UNIT_SCALE);

	size_t x0, y0, z0, x1, y1, z1;

	// Find the grid points this meta ball can possibly affect
	if (!dataGrid->mapAABB(aabb, x0, y0, z0, x1, y1, z1))
		return;

	Real* values = dataGrid->getValues();
	const Vector3* vertices = dataGrid->getVertices();
	Vector3* gradient = dataGrid->getGradient();
	ColourValue* colours = dataGrid->getColours();
	std::pair<Real, MetaWorldFragment*>* worldFragments = dataGrid->getMetaWorldFragments();

	for (size_t z = z0; z <= z1; ++z)
	{
		for (size_t y = y0; y <= y1; ++y)
		{
			for (size_t x = x0; x <= x1; ++x)
			{
				size_t index = dataGrid->getGridIndex(x, y, z);
				
				// http://www.geisswerks.com/ryan/BLOBS/blobs.html
/*				Vector3 v = (vertices[index] - mPosition);
				Real r = v.length() * M_SQRT1_2 / mRadius;
				if (r > M_SQRT1_2)
					continue;
				values[index] += r*r*r*r - r*r + 0.25;
*/
				Vector3 v = (vertices[index] - mPosition + dataGrid->getPosition());
				Real r2 = v.squaredLength() / (2.0 * mRadius*mRadius);
				if (r2 > 0.5)
					continue;
				Real currentFieldStrength = r2*r2 - r2 + 0.25;

				if(mExcavating)
					values[index] -= currentFieldStrength;
				else
					values[index] += currentFieldStrength;
				

				if (gradient)
					gradient[index] += v / (mRadius*mRadius);
				
				if(worldFragments)
				{
					if(currentFieldStrength > worldFragments[index].first)
					{
						worldFragments[index].first = currentFieldStrength;
						worldFragments[index].second = mMetaWorldFragment;
					}
				}

				//if (colours)
				//	colours[index] = ColourValue::White;
			}
		}
	}
}



AxisAlignedBox MetaBall::getAABB() const
{
	return AxisAlignedBox(
		  mPosition - mRadius*Vector3::UNIT_SCALE,
		  mPosition + mRadius*Vector3::UNIT_SCALE
		  );
}
}