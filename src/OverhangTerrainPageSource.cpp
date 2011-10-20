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
#include "OverhangTerrainPageSource.h"
#include "OverhangTerrainPage.h"
#include "OverhangTerrainRenderable.h"
#include "OgreSceneNode.h"
#include "OverhangTerrainSceneManager.h"

namespace Ogre {

    //-----------------------------------------------------------------------
    template<> OverhangTerrainPageSourceListenerManager* Singleton<OverhangTerrainPageSourceListenerManager>::ms_Singleton = 0;
    OverhangTerrainPageSourceListenerManager* OverhangTerrainPageSourceListenerManager::getSingletonPtr(void)
    {
        return ms_Singleton;
    }
    OverhangTerrainPageSourceListenerManager& OverhangTerrainPageSourceListenerManager::getSingleton(void)
    {  
        assert( ms_Singleton );  return ( *ms_Singleton );  
    }
    //-------------------------------------------------------------------------
	void OverhangTerrainPageSourceListenerManager::addListener(OverhangTerrainPageSourceListener* pl)
	{
        mPageSourceListeners.push_back(pl);
	}
    //-------------------------------------------------------------------------
	void OverhangTerrainPageSourceListenerManager::removeListener(OverhangTerrainPageSourceListener* pl)
	{
        PageSourceListenerList::iterator i, iend;
        iend = mPageSourceListeners.end();
        for(i = mPageSourceListeners.begin(); i != iend; ++i)
        {
            if (*i == pl)
            {
                mPageSourceListeners.erase(i);
                break;
            }
        }
	}
    //-------------------------------------------------------------------------
	void OverhangTerrainPageSourceListenerManager::firePageConstructed(
		OverhangTerrainSceneManager* sm, size_t pagex, size_t pagez, Real* heightData)
	{
        PageSourceListenerList::iterator i, iend;
        iend = mPageSourceListeners.end();
        for(i = mPageSourceListeners.begin(); i != iend; ++i)
        {
            (*i)->pageConstructed(sm, pagex, pagez, heightData);
        }
	}
	//-------------------------------------------------------------------------
	OverhangTerrainPageSource::OverhangTerrainPageSource() : mSceneManager(0), mAsyncLoading(false) {
	}
	//-------------------------------------------------------------------------
	OverhangTerrainPage* OverhangTerrainPageSource::buildPage(Real* heightData, const MaterialPtr& pMaterial)
    {
        String name;

        // Create a OverhangTerrain Page
        OverhangTerrainPage* page = new OverhangTerrainPage((mPageSize-1) / (mTileSize-1));
        // Create a node for all tiles to be attached to
        // Note we sequentially name since page can be attached at different points
        // so page x/z is not appropriate
		StringUtil::StrStreamType page_str;
		size_t pageIndex = mSceneManager->_getPageCount();
		page_str << pageIndex;
        name = "page[";
        name += page_str.str() + "]";
		if (mSceneManager->hasSceneNode(name))
		{
			page->pageSceneNode = mSceneManager->getSceneNode(name);
		}
		else
		{
			page->pageSceneNode = mSceneManager->createSceneNode(name);
		}
        
        size_t q = 0;
        for ( size_t j = 0; j < mPageSize - 1; j += ( mTileSize - 1 ) )
        {
            size_t p = 0;

            for ( size_t i = 0; i < mPageSize - 1; i += ( mTileSize - 1 ) )
            {
				StringUtil::StrStreamType new_name_str;
				
                // Create scene node for the tile and the TerrainRenderable
                new_name_str << "tile[" << pageIndex << "][" << (int)p << "," << (int)q << "]";
				name = new_name_str.str();

                SceneNode *c;
				if (mSceneManager->hasSceneNode(name))
				{
					c = mSceneManager->getSceneNode( name );
					if (c->getParentSceneNode() != page->pageSceneNode)
						page->pageSceneNode->addChild(c);
				}
				else
				{
					c = page->pageSceneNode->createChildSceneNode( name );
				}

//				OverhangTerrainRenderable *tile = new OverhangTerrainRenderable(name, mSceneManager);
				TerrainTile * tile = new TerrainTile(name, mSceneManager, c);
				// set queue
				tile->setRenderQueueGroup(mSceneManager->getWorldGeometryRenderQueue());
                // Initialise the tile
                tile->setMaterial(pMaterial);
                tile->initialise(i, j, heightData);
                // Attach it to the page
                page->tiles[ p ][ q ] = tile;
                // Attach it to the node
//                c ->attachObject( tile );
                p++;
            }

            q++;

        }

        pageIndex++;

        // calculate neighbours for page
        page->linkNeighbours();

		if(mSceneManager->getOptions().lit)
		{
			q = 0;
			for ( size_t j = 0; j < mPageSize - 1; j += ( mTileSize - 1 ) )
			{
				size_t p = 0;

				for ( size_t i = 0; i < mPageSize - 1; i += ( mTileSize - 1 ) )
				{
					page->tiles[ p ][ q ]->_calculateNormals();
					p++;
				}
				q++;
			}
		}

        return page;
    }
    //-------------------------------------------------------------------------
    void OverhangTerrainPageSource::firePageConstructed(size_t pagex, size_t pagez, Real* heightData)
    {
		OverhangTerrainPageSourceListenerManager::getSingleton().firePageConstructed(
			mSceneManager, pagex, pagez, heightData);
    }
    //-------------------------------------------------------------------------
    void OverhangTerrainPageSource::addListener(OverhangTerrainPageSourceListener* pl)
    {
		OverhangTerrainPageSourceListenerManager::getSingleton().addListener(pl);
    }
    //-------------------------------------------------------------------------
    void OverhangTerrainPageSource::removeListener(OverhangTerrainPageSourceListener* pl)
    {
		OverhangTerrainPageSourceListenerManager::getSingleton().removeListener(pl);

    }
    //-------------------------------------------------------------------------

}
