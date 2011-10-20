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

#include "IsoSurfaceRenderable.h"
#include "IsoSurfaceBuilder.h"

namespace Ogre
{

IsoSurfaceRenderable::IsoSurfaceRenderable()
: mSurfaceFlags(0)
{
}

void IsoSurfaceRenderable::initialize(IsoSurfaceBuilder *builder)
{
	mSurfaceFlags = builder->mSurfaceFlags;
	DynamicRenderable::initialize(RenderOperation::OT_TRIANGLE_LIST, true);
}

void IsoSurfaceRenderable::createVertexDeclaration()
{
	VertexDeclaration* vertexDeclaration = mRenderOp.vertexData->vertexDeclaration;
	size_t offset = 0;

	// Add mandatory position element to vertex declaration
	mPositionElement = &vertexDeclaration->addElement(0, offset, VET_FLOAT3, VES_POSITION);
	offset += VertexElement::getTypeSize(VET_FLOAT3);

	if (mSurfaceFlags & IsoSurfaceBuilder::GEN_NORMALS)
	{
		// Add optional normal element to vertex declaration
		mNormalElement = &vertexDeclaration->addElement(0, offset, VET_FLOAT3, VES_NORMAL);
		offset += VertexElement::getTypeSize(VET_FLOAT3);
	}

	if (mSurfaceFlags & IsoSurfaceBuilder::GEN_VERTEX_COLOURS)
	{
		// Add optional diffuse colour element to vertex declaration
		mDiffuseElement = &vertexDeclaration->addElement(0, offset, VET_COLOUR, VES_DIFFUSE);
		offset += VertexElement::getTypeSize(VET_COLOUR);
	}

	if (mSurfaceFlags & IsoSurfaceBuilder::GEN_TEX_COORDS)
	{
		// Add optional texture coordinates element to vertex declaration
		mTexCoordsElement = &vertexDeclaration->addElement(0, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES);
		offset += VertexElement::getTypeSize(VET_FLOAT2);
	}
}

void IsoSurfaceRenderable::fillHardwareBuffers(IsoSurfaceBuilder *builder)
{
	// Ensure that the hardware buffers are large enough
	prepareHardwareBuffers(builder->mIsoVertices.size(), 3*builder->mIsoTriangles.size());

	// Get the hardware buffers
	HardwareVertexBufferSharedPtr vbuf = mRenderOp.vertexData->vertexBufferBinding->getBuffer(0);
	HardwareIndexBufferSharedPtr ibuf = mRenderOp.indexData->indexBuffer;

	// Fill hardware vertex buffer
	unsigned char* pVert = static_cast<unsigned char*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));
	for (IsoSurfaceBuilder::IsoVertexVector::iterator i = builder->mIsoVertices.begin(); i != builder->mIsoVertices.end(); ++i)
	{
	    // There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
	    //  as second argument. So make it float, to avoid trouble when Ogre::Real will
	    //  be comiled/typedefed as double:
		float* pReal;

		// Write position element
		mPositionElement->baseVertexPointerToElement(pVert, &pReal);
		*pReal++ = builder->mIsoVertexPositions[*i].x;
		*pReal++ = builder->mIsoVertexPositions[*i].y;
		*pReal++ = builder->mIsoVertexPositions[*i].z;

		if (mSurfaceFlags & IsoSurfaceBuilder::GEN_NORMALS)
		{
			// Write normal element
			mNormalElement->baseVertexPointerToElement(pVert, &pReal);
			*pReal++ = builder->mIsoVertexNormals[*i].x;
			*pReal++ = builder->mIsoVertexNormals[*i].y;
			*pReal++ = builder->mIsoVertexNormals[*i].z;
		}

		if (mSurfaceFlags & IsoSurfaceBuilder::GEN_VERTEX_COLOURS)
		{
			uint32* pColour;

			// Write diffuse colour element
			mDiffuseElement->baseVertexPointerToElement(pVert, &pColour);
			Root::getSingleton().convertColourValue(builder->mIsoVertexColours[*i], pColour);
		}

		if (mSurfaceFlags & IsoSurfaceBuilder::GEN_TEX_COORDS)
		{
			// Write texture coordinates element
			mTexCoordsElement->baseVertexPointerToElement(pVert, &pReal);
			*pReal++ = builder->mIsoVertexTexCoords[*i][0];
			*pReal++ = builder->mIsoVertexTexCoords[*i][1];
		}

		pVert += mRenderOp.vertexData->vertexDeclaration->getVertexSize(0);
	}
	vbuf->unlock();

	// Fill hardware index buffer
	unsigned short* pIndex = static_cast<unsigned short*>(
		ibuf->lock(0, mRenderOp.indexData->indexBuffer->getSizeInBytes(), HardwareBuffer::HBL_DISCARD));
	for (IsoSurfaceBuilder::IsoTriangleVector::iterator i = builder->mIsoTriangles.begin(); i != builder->mIsoTriangles.end(); ++i)
	{
		*pIndex++ = static_cast<unsigned short>(builder->mIsoVertexIndices[i->vertices[0]]);
		*pIndex++ = static_cast<unsigned short>(builder->mIsoVertexIndices[i->vertices[1]]);
		*pIndex++ = static_cast<unsigned short>(builder->mIsoVertexIndices[i->vertices[2]]);
		std::cout << *(pIndex - 1) << std::endl;
	}
	ibuf->unlock();
	mAABB = builder->mDataGrid->getBoxSize();
}

void IsoSurfaceRenderable::deleteGeometry()
{
	/// ...and delete geometry.
}

}/// namespace Ogre
