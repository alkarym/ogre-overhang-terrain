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

#include "IsoSurfaceBuilder.h"
#include "IsoSurfaceRenderable.h"
#include "IsoSurfaceBuilderTables.h"

namespace Ogre
{

IsoSurfaceBuilder::IsoSurfaceBuilder()
  : mIsoVertexIndices(0), mIsoVertexPositions(0), mIsoVertexNormals(0),
	mIsoVertexColours(0), mIsoVertexTexCoords(0), mNumIsoVertices(0)//, mSurfaceFlags(0)
{
}

IsoSurfaceBuilder::~IsoSurfaceBuilder()
{
	delete[] mIsoVertexIndices;
	delete[] mIsoVertexPositions;
	delete[] mIsoVertexNormals;
	delete[] mIsoVertexColours;
	delete[] mIsoVertexTexCoords;

	destroyGridCells();
}

void IsoSurfaceBuilder::initialize(DataGrid* dataGridPtr, int flags)
{
	// Initialize members
	mDataGrid = dataGridPtr;
	mSurfaceFlags = flags;
	mIsoValue = 0.2;
	mFlipNormals = false;
	mNormalType = NORMAL_WEIGHTED_AVERAGE;

	// Mask out surface options not supported by the supplied data grid.
	// TODO: Inform user of unsupported options?
	if (!mDataGrid->hasGradient())
		mSurfaceFlags &= ~GEN_NORMALS;

	if (!mDataGrid->hasColours())
		mSurfaceFlags &= ~GEN_VERTEX_COLOURS;


	createIsoVertices();
	createGridCells();
}

void IsoSurfaceBuilder::update(IsoSurfaceRenderable *surf)
{
	size_t count = getNumIsoVertices();

	// Clear vertex and triangle vectors before building new iso surface
	mIsoVertices.clear();
	mIsoTriangles.clear();

	// Mark all iso vertices as not being used
	for (size_t i = 0; i < count; ++i)
		mIsoVertexIndices[i] = ~0;

	// Build the iso surface
	buildIsoSurface();

	// Update the render operation
	surf->fillHardwareBuffers(this);
}

size_t IsoSurfaceBuilder::getNumIsoVertices()
{
	if (!mNumIsoVertices)
	{
		size_t x = mDataGrid->getNumCellsX();
		size_t y = mDataGrid->getNumCellsY();
		size_t z = mDataGrid->getNumCellsZ();

		// Initialize iso vertex group offsets
		isoVertexGroupOffsets[0] = 0;
		isoVertexGroupOffsets[1] = isoVertexGroupOffsets[0] + x*(y+1)*(z+1);
		isoVertexGroupOffsets[2] = isoVertexGroupOffsets[1] + (x+1)*y*(z+1);

		// Store the total number of iso vertices
		mNumIsoVertices = isoVertexGroupOffsets[2] + (x+1)*(y+1)*z;
	}

	return mNumIsoVertices;
}

void IsoSurfaceBuilder::createIsoVertices()
{
	size_t size = getNumIsoVertices();

	// Create mandatory hardware vertex buffer index and position arrays
	mIsoVertexIndices = new size_t[size];
	mIsoVertexPositions = new Vector3[size];

	if (mSurfaceFlags & GEN_NORMALS)
	{
		// Create optional normal array
		mIsoVertexNormals = new Vector3[size];
	}

	if (mSurfaceFlags & GEN_VERTEX_COLOURS)
	{
		// Create optional diffuse colour array
		mIsoVertexColours = new ColourValue[size];
	}

	if (mSurfaceFlags & GEN_TEX_COORDS)
	{
		// Create optional texture coordinates array
		mIsoVertexTexCoords = new TexCoords[size];
	}
}


void IsoSurfaceBuilder::createGridCells()
{
	size_t x = mDataGrid->getNumCellsX();
	size_t y = mDataGrid->getNumCellsY();
	size_t z = mDataGrid->getNumCellsZ();

	mGridCells = new GridCell[x*y*z];
	GridCell* gridCell = mGridCells;

	// Initialize the corner index arrays of the grid cells.
	for (size_t k = 0; k < z; ++k)
	{
		for (size_t j = 0; j < y; ++j)
		{
			for (size_t i = 0; i < x; ++i)
			{
				gridCell->cornerIndices[0] = mDataGrid->getGridIndex(i, j, k);
				gridCell->cornerIndices[1] = mDataGrid->getGridIndex(i + 1, j, k);
				gridCell->cornerIndices[2] = mDataGrid->getGridIndex(i + 1, j, k + 1);
				gridCell->cornerIndices[3] = mDataGrid->getGridIndex(i, j, k + 1);
				gridCell->cornerIndices[4] = mDataGrid->getGridIndex(i, j + 1, k);
				gridCell->cornerIndices[5] = mDataGrid->getGridIndex(i + 1, j + 1, k);
				gridCell->cornerIndices[6] = mDataGrid->getGridIndex(i + 1, j + 1, k + 1);
				gridCell->cornerIndices[7] = mDataGrid->getGridIndex(i, j + 1, k + 1);

				++gridCell;
			}
		}
	}

	// Create and initialize the iso vertex index arrays of all grid cells.
	createGridCellIsoVertices();
}

void IsoSurfaceBuilder::destroyGridCells()
{
	size_t count = mDataGrid->getNumCellsX() * mDataGrid->getNumCellsY() * mDataGrid->getNumCellsZ();
	GridCell* gridCell = mGridCells;

	// Delete the iso vertex array of all grid cells
	for (size_t i = 0; i < count; ++i)
	{
		delete[] gridCell->isoVertices;
		++gridCell;
	}

	// Delete the grid cell array
	delete[] mGridCells;
}


void IsoSurfaceBuilder::createGridCellIsoVertices()
{
	size_t x = mDataGrid->getNumCellsX();
	size_t y = mDataGrid->getNumCellsY();
	size_t z = mDataGrid->getNumCellsZ();

	GridCell* gridCell = mGridCells;

	// Create and initialize the iso vertex index arrays of all grid cells.
	for (size_t k = 0; k < z; ++k)
	{
		for (size_t j = 0; j < y; ++j)
		{
			for (size_t i = 0; i < x; ++i)
			{
				gridCell->isoVertices = new size_t[12];

				// The following rather cryptic lines store the indices of the iso vertices
				// that are making up this grid cell.
				gridCell->isoVertices[0] = isoVertexGroupOffsets[0] + k*x*(y+1) + j*x + i;
				gridCell->isoVertices[1] = isoVertexGroupOffsets[2] + k*(x+1)*(y+1) + j*(x+1) + (i+1);
				gridCell->isoVertices[2] = isoVertexGroupOffsets[0] + (k+1)*x*(y+1) + j*x + i;
				gridCell->isoVertices[3] = isoVertexGroupOffsets[2] + k*(x+1)*(y+1) + j*(x+1) + i;

				gridCell->isoVertices[4] = isoVertexGroupOffsets[0] + k*x*(y+1) + (j+1)*x + i;
				gridCell->isoVertices[5] = isoVertexGroupOffsets[2] + k*(x+1)*(y+1) + (j+1)*(x+1) + (i+1);
				gridCell->isoVertices[6] = isoVertexGroupOffsets[0] + (k+1)*x*(y+1) + (j+1)*x + i;
				gridCell->isoVertices[7] = isoVertexGroupOffsets[2] + k*(x+1)*(y+1) + (j+1)*(x+1) + i;

				gridCell->isoVertices[8] = isoVertexGroupOffsets[1] + k*(x+1)*y + j*(x+1) + i;
				gridCell->isoVertices[9] = isoVertexGroupOffsets[1] + k*(x+1)*y + j*(x+1) + (i+1);
				gridCell->isoVertices[10] = isoVertexGroupOffsets[1] + (k+1)*(x+1)*y + j*(x+1) + (i+1);
				gridCell->isoVertices[11] = isoVertexGroupOffsets[1] + (k+1)*(x+1)*y + j*(x+1) + i;

				++gridCell;
			}
		}
	}
}

// Oh my god, I'm using a macro! But it does make this easier to read.
#define USE_ISO_VERTEX(i, a, b) useIsoVertex( \
			gridCell->isoVertices[i], \
			gridCell->cornerIndices[a], \
			gridCell->cornerIndices[b])

void IsoSurfaceBuilder::buildIsoSurface()
{
	size_t count =
		mDataGrid->getNumCellsX() *
		mDataGrid->getNumCellsY() *
		mDataGrid->getNumCellsZ();
	Real* values = mDataGrid->getValues();
	GridCell* gridCell = mGridCells;

	// Loop through all grid cells
	for (size_t i = 0; i < count; ++i)
	{
		IsoTriangle isoTriangle;
		size_t flags = 0;

		// Flag the corners that are outside the iso surface
		if (values[gridCell->cornerIndices[0]] < mIsoValue) flags |= 1;
		if (values[gridCell->cornerIndices[1]] < mIsoValue) flags |= 2;
		if (values[gridCell->cornerIndices[2]] < mIsoValue) flags |= 4;
		if (values[gridCell->cornerIndices[3]] < mIsoValue) flags |= 8;
		if (values[gridCell->cornerIndices[4]] < mIsoValue) flags |= 16;
		if (values[gridCell->cornerIndices[5]] < mIsoValue) flags |= 32;
		if (values[gridCell->cornerIndices[6]] < mIsoValue) flags |= 64;
		if (values[gridCell->cornerIndices[7]] < mIsoValue) flags |= 128;

		// Optionally flip normals
		if (!mFlipNormals)
			flags = 0xFF - flags;

		// Find the vertices where the surface intersects the cube
		if (msEdgeTable[flags] &    1) USE_ISO_VERTEX( 0, 0, 1);
		if (msEdgeTable[flags] &    2) USE_ISO_VERTEX( 1, 1, 2);
		if (msEdgeTable[flags] &    4) USE_ISO_VERTEX( 2, 2, 3);
		if (msEdgeTable[flags] &    8) USE_ISO_VERTEX( 3, 3, 0);
		if (msEdgeTable[flags] &   16) USE_ISO_VERTEX( 4, 4, 5);
		if (msEdgeTable[flags] &   32) USE_ISO_VERTEX( 5, 5, 6);
		if (msEdgeTable[flags] &   64) USE_ISO_VERTEX( 6, 6, 7);
		if (msEdgeTable[flags] &  128) USE_ISO_VERTEX( 7, 7, 4);
		if (msEdgeTable[flags] &  256) USE_ISO_VERTEX( 8, 0, 4);
		if (msEdgeTable[flags] &  512) USE_ISO_VERTEX( 9, 1, 5);
		if (msEdgeTable[flags] & 1024) USE_ISO_VERTEX(10, 2, 6);
		if (msEdgeTable[flags] & 2048) USE_ISO_VERTEX(11, 3, 7);

		// Generate triangles for this cube
		for (size_t i = 0; msTriangleTable[flags][i] != -1; i += 3)
		{
			isoTriangle.vertices[0] = gridCell->isoVertices[msTriangleTable[flags][i]];
			isoTriangle.vertices[1] = gridCell->isoVertices[msTriangleTable[flags][i+1]];
			isoTriangle.vertices[2] = gridCell->isoVertices[msTriangleTable[flags][i+2]];
			addIsoTriangle(isoTriangle);
		}

		++gridCell;
	}
}

}/// namespace Ogre