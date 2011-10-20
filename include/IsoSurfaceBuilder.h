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

#ifndef _ISO_SURFACE_BUILDER_H_
#define _ISO_SURFACE_BUILDER_H_

//#include "OgrePrerequisites.h"
#include "DataGrid.h"

class DataGrid;

namespace Ogre
{
class IsoSurfaceRenderable;

class IsoSurfaceBuilder
{
public:
	friend class IsoSurfaceRenderable;
	/// Flags describing what data is generated for rendering the iso surface.
	enum SurfaceFlags
	{
		/// Generate vertex normals by interpolating the gradient stored in the data grid.
		GEN_NORMALS = 0x01,
		/// Generate vertex colours by interpolating the colours stored in the data grid.
		GEN_VERTEX_COLOURS = 0x02,
		/// Generate texture coordinates.
		GEN_TEX_COORDS = 0x04,
	};

	enum NormalType
	{
		/// Normals are calculated as a weighted average of face normals.
		NORMAL_WEIGHTED_AVERAGE,
		/// Normals are calculated as an average of face normals.
		NORMAL_AVERAGE,
		/// Normals are calculated by interpolating the gradient in the data grid.
		NORMAL_GRADIENT
	};

	/// Constructor
	IsoSurfaceBuilder();
	/// Virtual Destructor
	virtual ~IsoSurfaceBuilder();
	/** Initializes the iso surface.
		@remarks
			This function should only be called once.
		@param dataGridPtr Pointer to the data grid to use for iso surface generation.
		@param flags Flags describing what data is generated for rendering the iso surface (see IsoSurface::SurfaceFlags). */
	virtual void initialize(DataGrid* dg, int flags);
	/// Rebuilds the iso surface, and updates the IsoSurfaceRenderable.
	void update(IsoSurfaceRenderable * isr);
	/// Returns the pointer to the data grid.
	DataGrid* getDataGrid() const {return mDataGrid; }
	/// Returns the iso value of the surface.
	Real getIsoValue() const {return mIsoValue; }
	/// Sets the iso value of the surface.
	void setIsoValue(Real isoValue) {mIsoValue = isoValue; }
	/// Returns whether normals are flipped.
	bool getFlipNormals() const {return mFlipNormals; }
	/** Sets whether to flip normals.
		@remarks
			When flip normals is false (the default), the outside of the surface is where the values of the data
			grid are lower than the iso value. */
	void setFlipNormals(bool flipNormals) {mFlipNormals = flipNormals; }
	/// Gets the method used for normal generation.
	NormalType getNormalType() const {return mNormalType; }
	/// Sets the method used for normal generation.
	void setNormalType(NormalType normalType) {mNormalType = normalType; }

	/// Returns the total number of iso vertices to be allocated.
	virtual size_t getNumIsoVertices();
	/// Creates and initializes the iso vertex index arrays of all grid cells.
	virtual void createGridCellIsoVertices();
	/// Builds the iso surface by looping through all grid cells generating triangles.
	virtual void buildIsoSurface();

	inline Vector3* getIsoVertexPositions() {return mIsoVertexPositions;}
	inline size_t* getIsoIndexPositions() {return mIsoVertexIndices;}

protected:
	typedef Real TexCoords[2];

	/// Definition of a triangle in an iso surface.
	struct IsoTriangle
	{
		/// Iso vertex indices defining the triangle.
		size_t vertices[3];
	};

	/// Grid cell.
	struct GridCell
	{
		/** Indices of the eight corners of the cell in the data grid arrays.
		  * <PRE>
		  *       4---------5
		  *      /.        /|
		  *     / .       / |
		  *    7---------6  |
		  *    |  .      |  |
		  *    |  0 . . .|. 1
		  *    | ,       | /
		  *    |,        |/
		  *    3---------2 </PRE>
		  */
		size_t cornerIndices[8];
		/// Iso vertex indices of this grid cell.
		size_t* isoVertices;
	};

	typedef std::vector<size_t> IsoVertexVector;
	typedef std::vector<IsoTriangle> IsoTriangleVector;

	/// The number of iso vertices, calculated on first call of getNumIsoVertices().
    size_t mNumIsoVertices;
	/// Array with offsets to the different iso vertex groups.
	size_t isoVertexGroupOffsets[3];
	/// Reference-counted shared pointer to the data grid associated with this iso surface.
	DataGrid * mDataGrid;
	/// Flags describing what data is generated for rendering the iso surface (see IsoSurface::SurfaceFlags).
	int mSurfaceFlags;
	/// Iso value of the surface, the default is 1.0.
	Real mIsoValue;
	/// Flip normals of the surface, the default is false.
	bool mFlipNormals;
	/// The method used for normal generation.
	NormalType mNormalType;
	/** Hardware vertex buffer indices for all iso vertices.
		@remarks
			A value of ~0 means that the iso vertex is not used. During iso surface generation all
			indices are reset to this value. On the first use of an iso vertex, its parameters are
			calculated, and it is assigned the next index in the hardware vertex buffer. */
	size_t* mIsoVertexIndices;
	/** Positions of all iso vertices.
		@remarks
			Positions are valid only for used iso vertices. */
	Vector3* mIsoVertexPositions;
	/** Normals for all iso vertices.
		@remarks
			This array is only allocated if GEN_NORMALS is set in IsoSurface::mSurfaceFlags,
			and normals are valid only for used iso vertices. */
	Vector3* mIsoVertexNormals;
	/** Vertex colours for all iso vertices.
		@remarks
			This array is only allocated if GEN_VERTEX_COLOURS is set in IsoSurface::mSurfaceFlags,
			and colours are valid only for used iso vertices. */
	ColourValue* mIsoVertexColours;
	/** Texture coordinates for all iso vertices.
		@remarks
			This array is only allocated if GEN_TEX_COORDS is set in IsoSurface::mSurfaceFlags,
			and texture coordinates are valid only for used iso vertices. */
	TexCoords* mIsoVertexTexCoords;
	/// Array of grid cells.
	GridCell* mGridCells;
	/** Vector to which the indices of all used iso vertices are added.
		@remarks
			The iso vertex indices in this vector are iterated when filling the hardware vertex buffer. */
	IsoVertexVector mIsoVertices;
	/** Vector to which all generated iso triangles are added.
		@remarks
			This vector is iterated when filling the hardware index buffer. */
	IsoTriangleVector mIsoTriangles;

	/// ...
	static const size_t msEdgeTable[256];
	/// ...
	static const int msTriangleTable[256][16];
//#include "IsoSurfaceBuilderTables.h"

	/** Creates the iso vertex arrays.
		@remarks
			The function calls the abstract function getNumIsoVertices() to get the length
			of the arrays to be created. Arrays are created according to the flags specified in
			IsoSurface::mSurfaceFlags. */
	void createIsoVertices();
	/** Creates and initializes the grid cells.
		@remarks
			This function initializes the corner index arrays of the grid cells, and calls
			the abstract function createGridCellIsoVertices() to create and initialize iso
			vertex index arrays of all grid cells. */
	void createGridCells();
	/// Destroys the grid cells, including their iso vertex index arrays.
	void destroyGridCells();
	/** Calculates properties of the iso vertex.
		@remarks
			If the properties of the iso vertex has already been calculated, the function
			returns immediately. Otherwise the iso vertex is initialized by calculating
			the necessary properties. Then it is assigned the next index in the hardware
			vertex buffer.
		@param isoVertex Index of the iso vertex to use.
		@param corner0 Index of the first data grid value associated with the iso vertex.
		@param corner1 Index of the second data grid value associated with the iso vertex.
		@returns
			The index passed in the isoVertex parameter. */
	size_t useIsoVertex(size_t isoVertex, size_t corner0, size_t corner1);
	/// ...
	void addIsoTriangle(const IsoTriangle& isoTriangle);
};

//inline functions
inline size_t IsoSurfaceBuilder::useIsoVertex(size_t isoVertex, size_t corner0, size_t corner1)
{
	// Return the assigned hardware vertex buffer index if the iso vertex has already been used
	if (mIsoVertexIndices[isoVertex] != ~0)
		return isoVertex;

	// Calculate the transition of the iso vertex between the two corners
	Real* values = mDataGrid->getValues();
	Real t = (mIsoValue - values[corner0]) / (values[corner1] - values[corner0]);

	// Calculate the iso vertex position by interpolation
	const Vector3* vertices = mDataGrid->getVertices();
	mIsoVertexPositions[isoVertex] = vertices[corner0] + t*(vertices[corner1] - vertices[corner0]);

	if (mSurfaceFlags & GEN_NORMALS)
	{
		// Generate optional normal
		switch (mNormalType)
		{
		case NORMAL_WEIGHTED_AVERAGE:
		case NORMAL_AVERAGE:
			mIsoVertexNormals[isoVertex] = Vector3::ZERO;
			break;

		case NORMAL_GRADIENT:
			{
				Vector3* gradient = mDataGrid->getGradient();
				if (mFlipNormals)
					mIsoVertexNormals[isoVertex] = gradient[corner0] + t*(gradient[corner1] - gradient[corner0]);
				else
					mIsoVertexNormals[isoVertex] = t*(gradient[corner0] - gradient[corner1]) - gradient[corner0];
			}
		}
	}

	if (mSurfaceFlags & GEN_VERTEX_COLOURS)
	{
		// Generate optional vertex colours by interpolation
		ColourValue* colours = mDataGrid->getColours();
		mIsoVertexColours[isoVertex] = colours[corner0] + t*(colours[corner1] - colours[corner0]);
	}

	if (mSurfaceFlags & GEN_TEX_COORDS)
	{
		// Generate optional texture coordinates
		// TODO: Implementation

		mIsoVertexTexCoords[isoVertex][0] = mIsoVertexPositions[isoVertex].x;
		mIsoVertexTexCoords[isoVertex][1] = mIsoVertexPositions[isoVertex].y;
		mIsoVertexTexCoords[isoVertex][2] = mIsoVertexPositions[isoVertex].z;
	}

	// Assign the next index in the hardware vertex buffer to this iso vertex
	mIsoVertexIndices[isoVertex] = mIsoVertices.size(); // TODO: Inefficient?
	mIsoVertices.push_back(isoVertex);

	return isoVertex;
}

inline void IsoSurfaceBuilder::addIsoTriangle(const IsoTriangle& isoTriangle)
{
	if ((mSurfaceFlags & GEN_NORMALS) && (mNormalType != NORMAL_GRADIENT))
	{
		Vector3 normal = 
			(mIsoVertexPositions[isoTriangle.vertices[1]] - mIsoVertexPositions[isoTriangle.vertices[0]]).crossProduct
			(mIsoVertexPositions[isoTriangle.vertices[2]] - mIsoVertexPositions[isoTriangle.vertices[0]]);

		switch (mNormalType)
		{
		case NORMAL_WEIGHTED_AVERAGE:
			normal = normal.normalisedCopy() / normal.length();
			break;
		case NORMAL_AVERAGE:
			normal.normalise();
			break;
		}

		mIsoVertexNormals[isoTriangle.vertices[0]] += normal;
		mIsoVertexNormals[isoTriangle.vertices[1]] += normal;
		mIsoVertexNormals[isoTriangle.vertices[2]] += normal;
	}

	mIsoTriangles.push_back(isoTriangle);
}

}/// namespace Ogre
#endif //_ISO_SURFACE_BUILDER_H_
