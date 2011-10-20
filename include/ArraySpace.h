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



#include <vector>
#include <list>
//#include "OgreAxisAlignedBoundingBox.h"
//#include "OgreVector3.h"

/** Holder class for fast spatial retrieval. Each cell is the same size.
 The arrayspace class is the fastest and most simple type of Space, but is 
 limited to a small world. For bigger worlds, use HashSpace **/

template <class T>
class ArraySpace
{
private:
	std::vector<T*> objs;
	int mNumCells;
public:
	ArraySpace(int nCells);
	std::list<T*> *getIntersecting(Ogre::AxisAlignedBox &aabb);
	std::list<T*> *getIntersectingCreateMissing(Ogre::AxisAlignedBox &aabb);
	void add(T* obj);
};

/// Retrieves a list of the T*'s that intersect with aabb, possibly creating new T's in the process.
template<class T>
std::list<T*>* ArraySpace<T>::getIntersectingCreateMissing(Ogre::AxisAlignedBox &aabb)
{
	assert(T::getSize());
	Real scale = T::getSize();
	Real invScale = 1/scale;
	Real zeroAdjust = T::getSize()*mNumCells*0.5;

	Vector3 min = aabb.getMinimum();
	min += zeroAdjust;
	int minX = floor(min.x * invScale);
	int minY = floor(min.y * invScale);
	int minZ = floor(min.z * invScale);
	assert(minX >= 0 && minY >= 0 && minZ >= 0);

	Vector3 max = aabb.getMaximum();
	max += zeroAdjust;
	int maxX = ceil(max.x * invScale);
	int maxY = ceil(max.y * invScale);
	int maxZ = ceil(max.z * invScale);
	assert(maxX >= 0 && maxY >= 0 && maxZ >= 0);

	std::list<T*> *ret = new std::list<T*>;
	for(int x = minX; x <= maxX; ++x)
	{
		for(int y = minY; y <= maxY; ++y)
		{
			for(int z = minZ; z <= maxZ; ++z)
			{
				int n = x*mNumCells*mNumCells+y*mNumCells+z;
				if(objs[n])
				{
					ret->push_back(objs[n]);
				}
				else
				{
					
					Vector3 pos(x*scale-zeroAdjust,y*scale-zeroAdjust,z*scale-zeroAdjust);
					objs[n] = new T(0,pos);
					ret->push_back(objs[n]);
				}
			}
		}
	}
	return ret;
}
