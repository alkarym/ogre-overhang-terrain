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

#ifndef _TERRAIN_TILE_H_
#define _TERRAIN_TILE_H_

#include "OverhangTerrainPrerequisites.h"

namespace Ogre
{

class IsoSurfaceRenderable;
class IsoSurfaceBuilder;
class AxisAlignedBox;

class TerrainTile
{
public:
	TerrainTile(const String& name, OverhangTerrainSceneManager* tsm, SceneNode *c);
	~TerrainTile();

	void deleteGeometry();

	enum Neighbor
	{
		NORTH = 0,
		SOUTH = 1,
		EAST = 2,
		WEST = 3,
		HERE = 4
	};

	/// methods needed because they are called from OgreTerrainPage
	void _setNeighbor(Neighbor n, TerrainTile * t );
	TerrainTile* _getNeighbor( Neighbor n )
	{
		return mNeighbors[n];
	}
	void setRenderQueueGroup(uint8 qid);
	const AxisAlignedBox& getBoundingBox() const;
	const Vector3& getCenter() const;

	// methods needed because they are called from TerrainPageSource
	/** Initializes the TerrainTile.
	@param startx, startz 
	The starting points of the top-left of this tile, in terms of the
	number of vertices.
	@param pageHeightData The source height data for the entire parent page
	*/
    void initialise(int startx, int startz, Real* pageHeightData);
	void setMaterial(const MaterialPtr& m );
	void _calculateNormals();
	/** Intersects the segment witht he terrain tile */
	bool intersectSegment( const Vector3 & start, const Vector3 & end, Vector3 * result );
	/** Returns the terrain height at the given coordinates */
	float getHeightAt( float x, float y );

	OverhangTerrainRenderable* getTerrainRenderable() {return mTerrainRenderable;}
	void addMetaObject(MetaObject *mo, int level, IsoSurfaceBuilder *isb, const Vector3 &pos);
	SceneNode * getSceneNode() {return mSceneNode;}

	inline std::vector<MetaWorldFragment *>& getMetaWorldFragments() {return mMetaWorldFragments;}
	inline std::vector<IsoSurfaceRenderable *>& getMetaWorldRenderables() {return mMetaRenderables;}

protected:
	OverhangTerrainRenderable *mTerrainRenderable;
	/// MetaRenderables from bottom to top (y direction).
	std::vector<IsoSurfaceRenderable *> mMetaRenderables;
	/// WorldFragments from bottom to top (y direction).
	std::vector<MetaWorldFragment *> mMetaWorldFragments;

	TerrainTile *mNeighbors [ 4 ];
	SceneNode * mSceneNode;

};

}//namespace OGRE
#endif// _OGRE_TERRAIN_TILE_H_
