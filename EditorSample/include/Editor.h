/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2006 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
LGPL like the rest of the engine.

Modified by Martin Enge (martin.enge@gmail.com) 2007 to fit into the OverhangTerrain Scene Manager

-----------------------------------------------------------------------------
*/

/**
    \file 
        Terrain.h
    \brief
        Specialisation of OGRE's framework application to show the
        OverhangTerrain plugin 
*/

#include "ExampleApplication.h"
#include "OverhangTerrainSceneManager.h"
#include "EditorGizmo.h"

RaySceneQuery* raySceneQuery = 0;

// Event handler to add ability to alter curvature
class TerrainFrameListener : public ExampleFrameListener
{
public:
    TerrainFrameListener(RenderWindow* win, Camera* cam, OverhangTerrainSceneManager *otsm)
        : ExampleFrameListener(win, cam), mOverhangSM(otsm), mGizmo(cam, otsm, 10)
    {
    	mMoveSpeed = 80;
		cam->setPosition(750,100,750);

	}
	virtual ~TerrainFrameListener() {};

    bool frameStarted(const FrameEvent& evt)
    {
        if( ExampleFrameListener::frameStarted(evt) == false )
		return false;

		static Real lastTime = 0.0;
		lastTime += evt.timeSinceLastFrame;
		if( mKeyboard->isKeyDown(OIS::KC_P) )
		{
			if(lastTime > 0.05)
			{
				mGizmo.setRadius(mGizmo.getRadius() * 1.05);
				lastTime = 0.0;
			}
		}
		else if( mKeyboard->isKeyDown(OIS::KC_L) )
		{
			if(lastTime > 0.05)
			{
				mGizmo.setRadius(mGizmo.getRadius() * 0.95238095238);
				lastTime = 0.0;
			}
		}

		mGizmo.update();

		static OIS::MouseState oldMouseState;
		const OIS::MouseState& mouseState = mMouse->getMouseState();
		mGizmo.mouseMoved(mouseState);

		if(!oldMouseState.buttonDown(OIS::MB_Left) && mouseState.buttonDown(OIS::MB_Left))
			mGizmo.mousePressed(OIS::MB_Left);
		if(oldMouseState.buttonDown(OIS::MB_Left) && !mouseState.buttonDown(OIS::MB_Left))
			mGizmo.mouseReleased(OIS::MB_Left);

		if(!oldMouseState.buttonDown(OIS::MB_Right) && mouseState.buttonDown(OIS::MB_Right))
			mGizmo.mousePressed(OIS::MB_Right);
		if(oldMouseState.buttonDown(OIS::MB_Right) && !mouseState.buttonDown(OIS::MB_Right))
			mGizmo.mouseReleased(OIS::MB_Right);

		oldMouseState = mouseState;

        return true;

    }

protected:
	OverhangTerrainSceneManager *mOverhangSM;
	EditorGizmo mGizmo;

};

#include <DataGrid.h>
#include <IsoSurfaceRenderable.h>
#include <MetaWorldFragment.h>

class TerrainApplication : public ExampleApplication
{
public:
    TerrainApplication() {}

    ~TerrainApplication()
    {
        delete raySceneQuery;
    }

    inline Root* getRoot() {return mRoot;}

protected:


    virtual void chooseSceneManager(void)
    {
        mSceneMgr = mRoot->createSceneManager("OverhangTerrainSceneManager");
    }

    virtual void createCamera(void)
    {
        // Create the camera
        mCamera = mSceneMgr->createCamera("PlayerCam");

        // Position it at 500 in Z direction
        mCamera->setPosition(Vector3(128,100,128));
        // Look back along -Z
        mCamera->lookAt(Vector3(0,0,-300));
        mCamera->setNearClipDistance( 1 );
        mCamera->setFarClipDistance( 1000 );

    }

    // Just override the mandatory create scene method
    void createScene(void)
    {
        Plane waterPlane;

        // Set ambient light
        mSceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));

        // Create a light
        Light* l = mSceneMgr->createLight("MainLight");
        // Accept default settings: point light, white diffuse, just set position
        // NB I could attach the light to a SceneNode if I wanted it to move automatically with
        //  other objects, but I don't
        l->setPosition(20,80,50);

        // Fog
        // NB it's VERY important to set this before calling setWorldGeometry 
        // because the vertex program picked will be different
        ColourValue fadeColour(0.93, 0.86, 0.76);
        mSceneMgr->setFog( FOG_LINEAR, fadeColour, .001, 500, 1000);
        mWindow->getViewport(0)->setBackgroundColour(fadeColour);

        std::string terrain_cfg("terrain.cfg");
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
        terrain_cfg = mResourcePath + terrain_cfg;
#endif

		std::cout << "blub\n";
        mSceneMgr -> setWorldGeometry( terrain_cfg );
		std::cout << "blub\n";

        // Infinite far plane?
        if (mRoot->getRenderSystem()->getCapabilities()->hasCapability(RSC_INFINITE_FAR_PLANE))
        {
            mCamera->setFarClipDistance(0);
        }

        raySceneQuery = mSceneMgr->createRayQuery(
            Ray(mCamera->getPosition(), Vector3::NEGATIVE_UNIT_Y));


    }
    // Create new frame listener
    void createFrameListener(void)
    {
		OverhangTerrainSceneManager *otsm = static_cast<OverhangTerrainSceneManager*> (mSceneMgr);
        mFrameListener= new TerrainFrameListener(mWindow, mCamera, otsm);
        mRoot->addFrameListener(mFrameListener);
    }

};
