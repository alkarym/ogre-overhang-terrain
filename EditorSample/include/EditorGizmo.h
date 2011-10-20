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

#ifndef _EDITOR_GIZMO_H_
#define _EDITOR_GIZMO_H_

#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>
//#include "OIS/oismouse.h"
#include "OgreVector3.h"
namespace Ogre
{
	class Camera;
	class OverhangTerrainSceneManager;
	
class EditorGizmo
{
public:
	EditorGizmo(Camera *cam, OverhangTerrainSceneManager *sm, Real radius = 10.0, Real mMaxDistToCam = 160.0);
	virtual ~EditorGizmo() {};

	/// updates position relative to cam. To be called by framelistener _after_ moving cam.
	void update();

	/// changes distance of metaball to cam depending on z-axis (mouse wheel)
	bool mouseMoved( const OIS::MouseState &ms );
	/// changes color of sphere or something(?)
	bool mousePressed( OIS::MouseButtonID id );
	/// adds meta ball to scene
	bool mouseReleased( OIS::MouseButtonID id );
	Real getRadius() {return mRadius;}
	void setRadius(Real radius);


protected :
	Real mDistToCam;
	Real mMaxDistToCam;
	Real mRadius;
	Vector3 mMetaBallPosition;
	Camera *mCamera;
	OverhangTerrainSceneManager *mOTSceneMgr;
	SceneNode *mSceneNode;
	Entity *mInnerEnt, *mOuterEnt;
	/// whether the metaball digs (true), or constructs overhangs (false).
	bool excavator;


	void createSphere(const std::string& strName, const float r, const int nRings = 16, const int nSegments = 16);
};
}// namespace Ogre
#endif //_EDITOR_GIZMO_H_
