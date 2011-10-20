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

#ifndef _ISO_SURFACE_RENDERABLE_H_
#define _ISO_SURFACE_RENDERABLE_H_

#include "DynamicRenderable.h"

namespace Ogre
{
class IsoSurfaceBuilder;

class IsoSurfaceRenderable : public DynamicRenderable
{
public:
	IsoSurfaceRenderable();
	virtual ~IsoSurfaceRenderable() {};
	void createVertexDeclaration();
	void initialize(IsoSurfaceBuilder *builder);
	virtual void fillHardwareBuffers(IsoSurfaceBuilder *surf);
	virtual bool getNormaliseNormals(void) const {return true; }
//	virtual const AxisAlignedBox &getBoundingBox(void) const {return mDataGridPtr->getBoundingBox();}
	virtual const AxisAlignedBox &getBoundingBox(void) const {return mAABB;}

	void deleteGeometry();

protected:
	AxisAlignedBox mAABB;
	/// Flags from IsoSurfaceBuilder used at initialize().
	int mSurfaceFlags;
	/// Pointer to the position element of vertex declaration.
	const VertexElement* mPositionElement;
	/** Pointer to the normal element of vertex declaration.
		@remarks
			The pointer is only valid if GEN_NORMALS is set in IsoSurface::mSurfaceFlags. */
	const VertexElement* mNormalElement;
	/** Pointer to the diffuse colour element of vertex declaration.
		@remarks
			The pointer is only valid if GEN_VERTEX_COLOURS is set in IsoSurface::mSurfaceFlags. */
	const VertexElement* mDiffuseElement;
	/** Pointer to the texture coordinates element of vertex declaration.
		@remarks
			The pointer is only valid if GEN_TEX_COORDS is set in IsoSurface::mSurfaceFlags. */
	const VertexElement* mTexCoordsElement;

};
}/// namespace Ogre
#endif ///_ISO_SURFACE_RENDERABLE_H_