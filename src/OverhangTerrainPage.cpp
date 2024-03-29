/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2006 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

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

You may alternatively use this source under the terms of a specific version of
the OGRE Unrestricted License provided you have obtained such a license from
Torus Knot Software Ltd.

Hacked by Martin Enge (martin.enge@gmail.com) 2007 to fit into the OverhangTerrain Scene Manager

-----------------------------------------------------------------------------
*/
#include "OverhangTerrainPage.h"
#include "TerrainTile.h"
#include "OgreAxisAlignedBox.h"

namespace Ogre {

    //-------------------------------------------------------------------------
    OverhangTerrainPage::OverhangTerrainPage(unsigned short numTiles)
    {
        tilesPerPage = numTiles;
        // Set up an empty array of TerrainTile pointers
        int i, j;
        for ( i = 0; i < tilesPerPage; i++ )
        {
            tiles.push_back( TerrainRow() );

            for ( j = 0; j < tilesPerPage; j++ )
            {
                tiles[ i ].push_back( 0 );
            }
        }

        pageSceneNode = 0;

    }
    //-------------------------------------------------------------------------
    OverhangTerrainPage::~OverhangTerrainPage()
    {
        Terrain2D::iterator i, iend;
        iend = tiles.end();
        for (i = tiles.begin(); i != iend; ++i)
        {
            TerrainRow::iterator j, jend;
            jend = i->end();
            for (j = i->begin(); j != jend; ++j)
            {
                delete *j;
                *j = 0;
            }
        }

    }
    //-------------------------------------------------------------------------
    void OverhangTerrainPage::linkNeighbours(void)
    {
        //setup the neighbor links.

        for ( size_t j = 0; j < tilesPerPage; j++ )
        {
            for ( size_t i = 0; i < tilesPerPage; i++ )
            {
                if ( j != tilesPerPage - 1 )
                {
                    tiles[ i ][ j ] -> _setNeighbor( TerrainTile::SOUTH, tiles[ i ][ j + 1 ] );
                    tiles[ i ][ j + 1 ] -> _setNeighbor( TerrainTile::NORTH, tiles[ i ][ j ] );
                }

                if ( i != tilesPerPage - 1 )
                {
                    tiles[ i ][ j ] -> _setNeighbor( TerrainTile::EAST, tiles[ i + 1 ][ j ] );
                    tiles[ i + 1 ][ j ] -> _setNeighbor( TerrainTile::WEST, tiles[ i ][ j ] );
                }

            }
        }
    }
    //-------------------------------------------------------------------------
    TerrainTile * OverhangTerrainPage::getTerrainTile( const Vector3 & pt )
    {
        /* Since we don't know if the terrain is square, or has holes, we use a line trace
        to find the containing tile...
        */

        TerrainTile * tile = tiles[ 0 ][ 0 ];

        while ( tile != 0 )
        {
            AxisAlignedBox b = tile -> getBoundingBox();

            if ( pt.x < b.getMinimum().x )
                tile = tile -> _getNeighbor( TerrainTile::WEST );
            else if ( pt.x > b.getMaximum().x )
                tile = tile -> _getNeighbor( TerrainTile::EAST );
            else if ( pt.z < b.getMinimum().z )
                tile = tile -> _getNeighbor( TerrainTile::NORTH );
            else if ( pt.z > b.getMaximum().z )
                tile = tile -> _getNeighbor( TerrainTile::SOUTH );
            else
                return tile;
        }

        return 0;
    }
	//-------------------------------------------------------------------------
	void OverhangTerrainPage::setRenderQueue(uint8 qid)
	{
		for ( size_t j = 0; j < tilesPerPage; j++ )
		{
			for ( size_t i = 0; i < tilesPerPage; i++ )
			{
				if ( j != tilesPerPage - 1 )
				{
					tiles[ i ][ j ]->setRenderQueueGroup(qid);
				}
			}
		}
	}

}

