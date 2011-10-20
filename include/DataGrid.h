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

#ifndef DATA_GRID_H
#define DATA_GRID_H

#include "OgreAxisAlignedBox.h"
#include "OverhangTerrainPrerequisites.h"
/// Class providing a grid of data values and methods for accessing and modyfying it.
namespace Ogre
{
class DataGrid
{
public:
	/// Flags describing what data is stored in the data grid.
	enum GridFlags
	{
		/// The data grid stores gradient vectors.
		HAS_GRADIENT = 0x01,
		/// The data grid stores colour values.
		HAS_COLOURS = 0x02,
		/// The data grid stores closest world fragment.
		HAS_WORLD_FRAGMENTS = 0x03
	};

	/// Constructor
	DataGrid();
	/// Virtual destructor
	virtual ~DataGrid();

	/** Initializes the data grid.
		@remarks
			This function should only be called once. The actual data in the grid is left uninitialized.
		@param numCellsX The number of grid cells along the x axis of the grid.
		@param numCellsY The number of grid cells along the y axis of the grid.
		@param numCellsZ The number of grid cells along the z axis of the grid.
		@param gridScale The scale of grid cells; this influences the position of grid vertices.
		@param flags Flags describing what data is stored in the data grid (see DataGrid::GridFlags). */
	void initialize(size_t numCellsX, size_t numCellsY, size_t numCellsZ, Real gridScale, int flags);
	/// Returns the number of grid cells along the x axis.
	size_t getNumCellsX() const {return mNumCellsX; }
	/// Returns the number of grid cells along the y axis.
	size_t getNumCellsY() const {return mNumCellsY; }
	/// Returns the number of grid cells along the z axis.
	size_t getNumCellsZ() const {return mNumCellsZ; }
	/// Returns the grid scale (i.e. the distance along the axes between grid points).
	Real getGridScale() const {return mGridScale; }
	/// Returns the grids position in space.
	Vector3 getPosition() {return mPosition;} const
	/// Sets the grids position in space.
	inline void setPosition(const Vector3 &pos)
	{
		mPosition = pos;
		Real minX, minY, minZ, maxX, maxY, maxZ;
		minX = pos.x - mNumCellsX * mGridScale * 0.5;
		minY = pos.y - mNumCellsY * mGridScale * 0.5;
		minZ = pos.z - mNumCellsZ * mGridScale * 0.5;
		maxX = pos.x + mNumCellsX * mGridScale * 0.5;
		maxY = pos.y + mNumCellsY * mGridScale * 0.5;
		maxZ = pos.z + mNumCellsZ * mGridScale * 0.5;
		mBoundingBox = AxisAlignedBox(minX, minY, minZ, maxX, maxY, maxZ);
	}
	/// Returns true if the grid stores gradient vectors.
	bool hasGradient() const {return (mGridFlags & HAS_GRADIENT) != 0; }
	/// Returns true if the grid stores colour values.
	bool hasColours() const {return (mGridFlags & HAS_COLOURS) != 0; }
	/// Returns true if the grid stores closest world fragment.
	bool hasMetaWorldFragments() const {return (mGridFlags & HAS_WORLD_FRAGMENTS) != 0; }
	/// Returns a pointer to the array of grid values.
	Real* getValues() {return mValues; }
	/// Returns a pointer to the (const) array of grid vertices.
	const Vector3* getVertices() const {return mVertices; }
	/** Returns a pointer to the array of gradient vectors.
		@remarks
			The returned pointer is only valid if HAS_GRADIENT is set in DataGrid::mGridFlags. */
	Vector3* getGradient() {return mGradient; }
	/** Returns a pointer to the array of colour values.
		@remarks
			The returned pointer is only valid if HAS_COLOURS is set in DataGrid::mGridFlags. */
	ColourValue* getColours() {return mColours; }
	/** Returns a pointer to the array of closest world fragments.
		@remarks
			The returned pointer is only valid if HAS_WORLD_FRAGMENT is set in DataGrid::mGridFlags. */
	std::pair<Real, MetaWorldFragment*>* getMetaWorldFragments() {return mMetaWorldFragments; }
	/// Returns the bounding box of the grid.
	const AxisAlignedBox& getBoundingBox() const {return mBoundingBox; }
	const AxisAlignedBox& getBoxSize() const {return mBoxSize;}
	/// Returns the index of the specified grid point.
	size_t getGridIndex(size_t x, size_t y, size_t z) const {return z*(mNumCellsX + 1)*(mNumCellsY + 1) + y*(mNumCellsX + 1) + x; }
	/** Maps an axis aligned box to the grid points inside it.
		@remarks
			This function modifies the values of x0, y0, z0, x1, y1, and z1.
			(x0, y0, z0) will be the 'minimum' grid point <i>inside</i> the axis aligned box, and
			(x1, y1, z1) will be the 'maximum' grid point <i>inside</i> the axis aligned box.
			I.e. the points (x, y, z) for which (x0, y0, z0) <= (x, y, z) <= (x1, y1, z1) lie inside the axis aligned box.
		@returns
			false if the box is completely outside the grid, otherwise it returns true.
		@note
			If the function returns true, it is safe to use the values of (x0, y0, z0) and (x1, y1, z1) in a call to getGridIndex().
		@warning
			If the function returns false, the contents of x0, y0, z0, x1, y1, and z1 are undefined. */
	bool mapAABB(const AxisAlignedBox& aabb, size_t &x0, size_t &y0, size_t &z0, size_t &x1, size_t &y1, size_t &z1) const;
	/// Clears the data grid.
	void clear();

protected:
	/// The number of grid cells along the x axis of the grid.
	size_t mNumCellsX;
	/// The number of grid cells along the y axis of the grid.
	size_t mNumCellsY;
	/// The number of grid cells along the z axis of the grid.
	size_t mNumCellsZ;
	/// Total number of grid points.
	size_t mNumGridPoints;
	/// The scale of grid cells; this influences the position of grid vertices.
	Real mGridScale;
	/// Position of this Grid.
	Vector3 mPosition;
	/// Flags describing what data is stored in the data grid (see DataGrid::GridFlags).
	int mGridFlags;
	/// Data grid values.
	Real* mValues;
	/// Vertex positions of the grid points.
	Vector3* mVertices;
	/** Gradient vectors of the grid.
		@remarks
			This array is only allocated if HAS_GRADIENT is set in DataGrid::mGridFlags. */
	Vector3* mGradient;
	/** Colour values of the grid.
		@remarks
			This array is only allocated if HAS_COLOURS is set in DataGrid::mGridFlags. */
	ColourValue* mColours;
	/** Closest World Fragments of the grid.
		@remarks
			This array is only allocated if HAS_WORLD_FRAGMENTS is set in DataGrid::mGridFlags. */
	std::pair<Real, MetaWorldFragment*>* mMetaWorldFragments;
	/// Bounding box of the grid.
	AxisAlignedBox mBoundingBox, mBoxSize;

	/** Initializes the position of grid points and the bounding box.
		@remarks
			In the default implementation, the grid points form a regular grid centered around (0, 0, 0).
			The distance along the axes between grid points is determined by mGridScale.
		@par
			This function is responsible for initializing mBoundingBox to fit around all grid points. */
	virtual void initializeVertices();

	void * lastHostObject;
};

/// Reference-counted shared pointer to a DataGrid.
typedef SharedPtr<DataGrid> DataGridPtr;
}// namespace Ogre
#endif // DATA_GRID_H
