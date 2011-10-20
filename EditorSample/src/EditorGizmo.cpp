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

#include "EditorGizmo.h"
#include "OgreCamera.h"
#include "OverhangTerrainSceneManager.h"
//#include "OIS/OIS.h"

#define MAX_RADIUS 100.f;
#define MIN_RADIUS 1.f;

namespace Ogre
{
EditorGizmo::EditorGizmo(Camera *cam, OverhangTerrainSceneManager *sm, Real radius, Real maxDistToCam)
: mCamera(cam), mOTSceneMgr(sm), mMaxDistToCam(maxDistToCam), mDistToCam(40.0f),
  mRadius(radius), mMetaBallPosition(Vector3::ZERO), mSceneNode(0), mInnerEnt(0), mOuterEnt(0)
{
	createSphere("myInnerSphereMesh", mRadius/4.0f, 32, 32);
	createSphere("myOuterSphereMesh", mRadius, 32, 32);
	mInnerEnt = mOTSceneMgr->createEntity("myInnerSphereEntity", "myInnerSphereMesh");
	mOuterEnt = mOTSceneMgr->createEntity("myOuterSphereEntity", "myOuterSphereMesh");
	mSceneNode = mOTSceneMgr->getRootSceneNode()->createChildSceneNode();
	mInnerEnt->setMaterialName("Transparent1");
	mOuterEnt->setMaterialName("Transparent2");
	mSceneNode->attachObject(mOuterEnt);
	mSceneNode->attachObject(mInnerEnt);
}


/// updates position relative to cam.
void EditorGizmo::update()
{
	static Real oldRadius = 0.0;
	Vector3 dir = mCamera->getDirection();
	Vector3 pos = mCamera->getPosition();
	mMetaBallPosition = pos + dir * mDistToCam;
	mSceneNode->setPosition(mMetaBallPosition);
}

bool EditorGizmo::mouseMoved( const OIS::MouseState &ms )
{
	float p = ms.Z.abs / 10000.0f;
	mDistToCam = p * mMaxDistToCam + 40.0f;
	return true;
}

bool EditorGizmo::mousePressed( OIS::MouseButtonID id )
{
	/// do nothing, for now
	return true;
}

bool EditorGizmo::mouseReleased( OIS::MouseButtonID id )
{
	std::cout << "Mouse Button Released.\n";

	if(id == OIS::MB_Left)
		mOTSceneMgr->addMetaBall(mMetaBallPosition, mRadius, true);
	else if(id == OIS::MB_Right)
		mOTSceneMgr->addMetaBall(mMetaBallPosition, mRadius, false);
	return true;
}

void EditorGizmo::setRadius(Real radius) {
	if(radius != mRadius && radius <= 30.f && radius >= 5.f)
	{
		mSceneNode->scale(Vector3(radius/mRadius, radius/mRadius, radius/mRadius));
		mRadius = radius;
	}

}



void EditorGizmo::createSphere(const std::string& strName, const float r, const int nRings, const int nSegments)
{
	MeshPtr pSphere = MeshManager::getSingleton().createManual(strName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	SubMesh *pSphereVertex = pSphere->createSubMesh();

	pSphere->sharedVertexData = new VertexData();
	VertexData* vertexData = pSphere->sharedVertexData;

	// define the vertex format
	VertexDeclaration* vertexDecl = vertexData->vertexDeclaration;
	size_t currOffset = 0;
	// positions
	vertexDecl->addElement(0, currOffset, VET_FLOAT3, VES_POSITION);
	currOffset += VertexElement::getTypeSize(VET_FLOAT3);
	// normals
	vertexDecl->addElement(0, currOffset, VET_FLOAT3, VES_NORMAL);
	currOffset += VertexElement::getTypeSize(VET_FLOAT3);
	// two dimensional texture coordinates
	vertexDecl->addElement(0, currOffset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);
	currOffset += VertexElement::getTypeSize(VET_FLOAT2);

	// allocate the vertex buffer
	vertexData->vertexCount = (nRings + 1) * (nSegments+1);
	HardwareVertexBufferSharedPtr vBuf = HardwareBufferManager::getSingleton().createVertexBuffer(vertexDecl->getVertexSize(0), vertexData->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
	VertexBufferBinding* binding = vertexData->vertexBufferBinding;
	binding->setBinding(0, vBuf);
	float* pVertex = static_cast<float*>(vBuf->lock(HardwareBuffer::HBL_DISCARD));

	// allocate index buffer
	pSphereVertex->indexData->indexCount = 6 * nRings * (nSegments + 1);
	pSphereVertex->indexData->indexBuffer = HardwareBufferManager::getSingleton().createIndexBuffer(HardwareIndexBuffer::IT_16BIT, pSphereVertex->indexData->indexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
	HardwareIndexBufferSharedPtr iBuf = pSphereVertex->indexData->indexBuffer;
	unsigned short* pIndices = static_cast<unsigned short*>(iBuf->lock(HardwareBuffer::HBL_DISCARD));

	float fDeltaRingAngle = (Math::PI / nRings);
	float fDeltaSegAngle = (2 * Math::PI / nSegments);
	unsigned short wVerticeIndex = 0 ;

	// Generate the group of rings for the sphere
	for( int ring = 0; ring <= nRings; ring++ ) {
		float r0 = r * sinf (ring * fDeltaRingAngle);
		float y0 = r * cosf (ring * fDeltaRingAngle);

		// Generate the group of segments for the current ring
		for(int seg = 0; seg <= nSegments; seg++) {
			float x0 = r0 * sinf(seg * fDeltaSegAngle);
			float z0 = r0 * cosf(seg * fDeltaSegAngle);

			// Add one vertex to the strip which makes up the sphere
			*pVertex++ = x0;
			*pVertex++ = y0;
			*pVertex++ = z0;

			Vector3 vNormal = Vector3(x0, y0, z0).normalisedCopy();
			*pVertex++ = vNormal.x;
			*pVertex++ = vNormal.y;
			*pVertex++ = vNormal.z;

			*pVertex++ = (float) seg / (float) nSegments;
			*pVertex++ = (float) ring / (float) nRings;

			if (ring != nRings) {
                               // each vertex (except the last) has six indices pointing to it
				*pIndices++ = wVerticeIndex + nSegments + 1;
				*pIndices++ = wVerticeIndex;               
				*pIndices++ = wVerticeIndex + nSegments;
				*pIndices++ = wVerticeIndex + nSegments + 1;
				*pIndices++ = wVerticeIndex + 1;
				*pIndices++ = wVerticeIndex;
				wVerticeIndex ++;
			}
		}; // end for seg
	} // end for ring

	// Unlock
	vBuf->unlock();
	iBuf->unlock();
	// Generate face list
	pSphereVertex->useSharedVertices = true;

	// the original code was missing this line:
	pSphere->_setBounds( AxisAlignedBox( Vector3(-r, -r, -r), Vector3(r, r, r) ), false );
	pSphere->_setBoundingSphereRadius(r);
        // this line makes clear the mesh is loaded (avoids memory leaks)
        pSphere->load();
 }

}//namespace Ogre
