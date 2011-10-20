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

#ifndef DYNAMIC_RENDERABLE_H
#define DYNAMIC_RENDERABLE_H

#include "OgreSimpleRenderable.h"
#include "OgreRenderOperation.h"
class IsoSurfaceBuilder;

namespace Ogre
{
/// Abstract base class providing mechanisms for dynamically growing hardware buffers.
class DynamicRenderable : public SimpleRenderable
{
public:
	/// Constructor
	DynamicRenderable();
	/// Virtual destructor
	virtual ~DynamicRenderable();

	/** Initializes the dynamic renderable.
		@remarks
			This function should only be called once. It initializes the render operation, and calls
			the abstract function createVertexDeclaration().
		@param operationType The type of render operation to perform.
		@param useIndices Specifies whether to use indices to determine the vertices to use as input. */
	void initialize(RenderOperation::OperationType operationType, bool useIndices);
	/// Implementation of Ogre::SimpleRenderable
	virtual Real getBoundingRadius(void) const;
	/// Implementation of Ogre::SimpleRenderable
	virtual Real getSquaredViewDepth(const Camera* cam) const;

protected:
	/// Maximum capacity of the currently allocated vertex buffer.
	size_t mVertexBufferCapacity;
	/// Maximum capacity of the currently allocated index buffer.
	size_t mIndexBufferCapacity;

	/// Creates the vertex declaration.
	virtual void createVertexDeclaration() = 0;
	/** Prepares the hardware buffers for the requested vertex and index counts.
		@remarks
			This function must be called before locking the buffers in fillHardwareBuffers(). It
			guarantees that the hardware buffers are large enough to hold at least the requested number of
			vertices and indices (if using indices). The buffers are possibly reallocated to achieve this.
		@par
			The vertex and index count in the render operation are set to the values of vertexCount
			and indexCount respectively.
		@param vertexCount The number of vertices the buffer must hold.
		@param indexCount The number of indices the buffer must hold. This parameter is ignored if
			not using indices. */
	void prepareHardwareBuffers(size_t vertexCount, size_t indexCount);
	/** Fills the hardware vertex and index buffers with data.
		@remarks
			This function must call prepareHardwareBuffers() before locking the buffers to ensure the they
			are large enough for the data to be written. Afterwards the vertex and index buffers (if using
			indices) can be locked, and data can be written to them. */
//	virtual void fillHardwareBuffers(IsoSurfaceBuilder * builder) = 0;
};

}/// namespace Ogre
#endif // DYNAMIC_RENDERABLE_H
