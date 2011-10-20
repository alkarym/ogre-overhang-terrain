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

#include "OgreColourValue.h"
#include "DataGrid.h"

namespace Ogre
{
DataGrid::DataGrid()
  : mValues(0), mVertices(0), mGradient(0), mColours(0), mMetaWorldFragments(0),
    mPosition(0,0,0)
{
}

DataGrid::~DataGrid()
{
	delete[] mValues;
	delete[] mVertices;
	delete[] mGradient;
	delete[] mColours;
	delete[] mMetaWorldFragments;
}

void DataGrid::initialize(size_t numCellsX, size_t numCellsY, size_t numCellsZ, Real gridScale, int flags)
{
	// Initialize members
	mNumCellsX = numCellsX;
	mNumCellsY = numCellsY;
	mNumCellsZ = numCellsZ;
	mGridScale = gridScale;
	mGridFlags = flags;

	mNumGridPoints = (mNumCellsX + 1)*(mNumCellsY + 1)*(mNumCellsZ + 1);

	// Create mandatory value and grid vertex arrays
	mValues = new Real[mNumGridPoints];
	mVertices = new Vector3[mNumGridPoints];

	if (hasGradient())
	{
		// Create optional gradient vector array
		mGradient = new Vector3[mNumGridPoints];
	}

	if (hasColours())
	{
		// Create optional colour value array
		mColours = new ColourValue[mNumGridPoints];
	}
	
	if (hasMetaWorldFragments())
	{
		// Create optional MetaWorldFragment array
		mMetaWorldFragments = new std::pair<Real, MetaWorldFragment*>[mNumGridPoints];
	}

	// Initialize the position of grid points and the bounding box
	initializeVertices();
}


void DataGrid::initializeVertices()
{
	Vector3 maximum = Vector3(
		0.5*mGridScale*mNumCellsX,
		0.5*mGridScale*mNumCellsY,
		0.5*mGridScale*mNumCellsZ);

	Vector3* pVertex = mVertices;
	Vector3 position = -maximum;

	// Setup bounding box
	mBoundingBox.setExtents(-maximum, maximum);
	mBoxSize.setExtents(-maximum, maximum);

	// Initialize grid vertices
	for (size_t k = 0; k <= mNumCellsZ; ++k)
	{
		for (size_t j = 0; j <= mNumCellsY; ++j)
		{
			for (size_t i = 0; i <= mNumCellsX; ++i)
			{
				*pVertex++ = position;
				position.x += mGridScale;
			}
			position.x = -maximum.x;
			position.y += mGridScale;
		}
		position.y = -maximum.y;
		position.z += mGridScale;
	}
}

bool DataGrid::mapAABB(const AxisAlignedBox& aabb, size_t &x0, size_t &y0, size_t &z0, size_t &x1, size_t &y1, size_t &z1) const
{
	// x0
	if (aabb.getMinimum().x <= mBoundingBox.getMinimum().x)
		x0 = 0;
	else
	if (aabb.getMinimum().x > mBoundingBox.getMaximum().x)
		return false;
	else
		x0 = Math::Ceil((aabb.getMinimum().x - mBoundingBox.getMinimum().x) / mGridScale);

	// y0
	if (aabb.getMinimum().y <= mBoundingBox.getMinimum().y)
		y0 = 0;
	else
	if (aabb.getMinimum().y > mBoundingBox.getMaximum().y)
		return false;
	else
		y0 = Math::Ceil((aabb.getMinimum().y - mBoundingBox.getMinimum().y) / mGridScale);

	// z0
	if (aabb.getMinimum().z <= mBoundingBox.getMinimum().z)
		z0 = 0;
	else
	if (aabb.getMinimum().z > mBoundingBox.getMaximum().z)
		return false;
	else
		z0 = Math::Ceil((aabb.getMinimum().z - mBoundingBox.getMinimum().z) / mGridScale);

	// x1
	if (aabb.getMaximum().x < mBoundingBox.getMinimum().x)
		return false;
	else
	if (aabb.getMaximum().x >= mBoundingBox.getMaximum().x)
		x1 = mNumCellsX;
	else
		x1 = Math::Floor((aabb.getMaximum().x - mBoundingBox.getMinimum().x) / mGridScale);

	// y1
	if (aabb.getMaximum().y < mBoundingBox.getMinimum().y)
		return false;
	else
	if (aabb.getMaximum().y >= mBoundingBox.getMaximum().y)
		y1 = mNumCellsY;
	else
		y1 = Math::Floor((aabb.getMaximum().y - mBoundingBox.getMinimum().y) / mGridScale);

	// z1
	if (aabb.getMaximum().z < mBoundingBox.getMinimum().z)
		return false;
	else
	if (aabb.getMaximum().z >= mBoundingBox.getMaximum().z)
		z1 = mNumCellsZ;
	else
		z1 = Math::Floor((aabb.getMaximum().z - mBoundingBox.getMinimum().z) / mGridScale);

	return true;
}

void DataGrid::clear()
{
	Real* value = mValues;
	Vector3* gradient = mGradient;
	ColourValue* colour = mColours;
	std::pair<Real, MetaWorldFragment*>* frags = mMetaWorldFragments;

	// Clear data grid
	for (size_t i = 0; i < mNumGridPoints; ++i)
	{
		*value++ = 0.0;

		if (hasGradient())
			*gradient++ = Vector3::ZERO;

		if (hasColours())
			*colour++ = ColourValue(0.0, 0.0, 0.0);

		if (hasMetaWorldFragments())
		{
			frags->first = 0.0;
			frags->second = 0;
			frags++;
		}
	}
}


}