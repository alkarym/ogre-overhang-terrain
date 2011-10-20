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

#include "DynamicRenderable.h"
#include "OgreHardwareBufferManager.h"
namespace Ogre
{
DynamicRenderable::DynamicRenderable()
{
}

DynamicRenderable::~DynamicRenderable()
{
	delete mRenderOp.vertexData;
	delete mRenderOp.indexData;
}

void DynamicRenderable::initialize(RenderOperation::OperationType operationType, bool useIndices)
{
	// Initialize render operation
	mRenderOp.operationType = operationType;
	mRenderOp.useIndexes = useIndices;
	mRenderOp.vertexData = new VertexData;
	if (mRenderOp.useIndexes)
		mRenderOp.indexData = new IndexData;

	// Reset buffer capacities
	mVertexBufferCapacity = 0;
	mIndexBufferCapacity = 0;

	// Create vertex declaration
	createVertexDeclaration();
}

void DynamicRenderable::prepareHardwareBuffers(size_t vertexCount, size_t indexCount)
{
	// Prepare vertex buffer
	if ((vertexCount > mVertexBufferCapacity) ||
		(!mVertexBufferCapacity))
	{
		// vertexCount exceeds current capacity!
		// It is necessary to reallocate the buffer.

		// Check if this is the first call
		if (!mVertexBufferCapacity)
			mVertexBufferCapacity = 1;

		// Make capacity the next power of two
		while (mVertexBufferCapacity < vertexCount)
			mVertexBufferCapacity <<= 1;

		// Create new vertex buffer
		HardwareVertexBufferSharedPtr vbuf =
			HardwareBufferManager::getSingleton().createVertexBuffer(
				mRenderOp.vertexData->vertexDeclaration->getVertexSize(0),
				mVertexBufferCapacity,
				HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY,true); // TODO: Custom HBU_?

		// Bind buffer
		mRenderOp.vertexData->vertexBufferBinding->setBinding(0, vbuf);
	}

	// Update vertex count in the render operation
	mRenderOp.vertexData->vertexCount = vertexCount;

	if (mRenderOp.useIndexes)
	{
		OgreAssert(indexCount <= std::numeric_limits<unsigned short>::max(), "indexCount exceeds 16 bit");

		// Prepare index buffer
		if ((indexCount > mIndexBufferCapacity) ||
			(!mIndexBufferCapacity))
		{
			// indexCount exceeds current capacity!
			// It is necessary to reallocate the buffer.

			// Check if this is the first call
			if (!mIndexBufferCapacity)
				mIndexBufferCapacity = 1;

			// Make capacity the next power of two
			while (mIndexBufferCapacity < indexCount)
				mIndexBufferCapacity <<= 1;

			// Create new index buffer
			mRenderOp.indexData->indexBuffer =
				HardwareBufferManager::getSingleton().createIndexBuffer(
					HardwareIndexBuffer::IT_16BIT,
					mIndexBufferCapacity,
					HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY,true); // TODO: Custom HBU_?
		}

		// Update index count in the render operation
		mRenderOp.indexData->indexCount = indexCount;
	}
}

Real DynamicRenderable::getBoundingRadius(void) const
{
	// TODO: Implementation
	return 0.0;
}

Real DynamicRenderable::getSquaredViewDepth(const Camera* cam) const
{
	// TODO: Implementation
	return 0.0;
}

}///namespace Ogre
