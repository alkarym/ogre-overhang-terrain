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

#ifndef META_OBJECT_H
#define META_OBJECT_H

//#include "OverhangTerrainPrerequisites.h"

#include "OgreVector3.h"

/// Abstract class defining the interface for meta objects to be used with MetaObjectDataGrid.
namespace Ogre
{
class MetaWorldFragment;
class DataGrid;

class MetaObject
{
public:
	/// Constructor
	MetaObject(MetaWorldFragment *wf, const Vector3& position = Vector3::ZERO)
		: mPosition(position), mMetaWorldFragment(wf) {;}
	/// Virtual destructor
	virtual ~MetaObject() {;}

	/** Tells the meta object to update the data grid.
		*/
	virtual void updateDataGrid(DataGrid* dataGrid) = 0;
	/// Returns the position of the meta object.
	const Vector3& getPosition() const {return mPosition; }
	/// Sets the position of the meta object.
	void setPosition(const Vector3& position) {mPosition = position; }
	/// Returns the MetaWorldFragment* of the meta object.
	MetaWorldFragment* getMetaWorldFragment() const {return mMetaWorldFragment; }
	/// Checks for overlap with an AABB
//	virtual bool intersects(const AxisAlignedBox aabb) const = 0;
	virtual AxisAlignedBox getAABB() const = 0;

protected:
	Vector3 mPosition;
	MetaWorldFragment* mMetaWorldFragment;
};

}//namespace Ogre
#endif // META_OBJECT_H
