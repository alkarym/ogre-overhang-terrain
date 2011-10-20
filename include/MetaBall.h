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

#ifndef META_BALL_H
#define META_BALL_H

#include "MetaObject.h"

// Define math constants
#ifndef M_SQRT1_2
#define M_SQRT1_2 0.707106781186547524401
#endif

#include "OgrePrerequisites.h"
/// Class representing a meta ball.
namespace Ogre
{
class MetaBall : public MetaObject
{
public:
	/// Constructor
	MetaBall(MetaWorldFragment *wf, const Vector3& position = Vector3::ZERO, Real radius = 30.0, bool excavating = true);

	/// Adds this meta ball to the data grid.
	virtual void updateDataGrid(DataGrid* dataGrid);
	/// Returns the radius of the meta ball.
	Real getRadius() const {return mRadius; }
	/// Sets the radius of the meta ball.
	void setRadius(Real radius) {mRadius = radius; }
	void setExcavating(bool e) {mExcavating = true;}
	virtual AxisAlignedBox getAABB() const;
	

protected:
	Real mRadius;
	bool mExcavating;

};

} ///namespace Ogre
#endif // META_BALL_H
