OverhangTerrain Scene Manager - test implementation.

Copyright (c) 2007 Martin Enge
martin.enge@gmail.com

This is a test implementation of some ideas I had for creating terrain with 
overhangs/tunnels, released in the hope that someone will extend it to be 
truly useful for the OGRE community. It builds heavily upon OGRE's terrain 
Scene Manager, as well as code by DWORD showing off his DynamicRenderable 
class (released to public domain, available here: 
http://www.ogre3d.org/phpBB2/viewtopic.php?p=68286.)
To build out of the box you should place it in the PlugIns folder of the OGRE
source distribution, and change your resources.cfg to point to the materials 
folder.

The plugin renders terrain tiles using two different approaches. For 
non-overhang terrain tiles it uses the method from Ogres terrain SM, and for
terrain with overhangs it uses MetaObjects made into a surface with the 
marching cubes algorithm. The second approach combines a "Metaheightmap" with
MetaBalls (which can be both excavating or overhang-creating).

Although the plugin more-or-less works as a scene manager, it was written as
a proof of principle rather than a ready solution. I would love to help out 
anyone who wants to make a proper application from this (with the little time
I have), but I will not implement and maintain one on my own.


...oh, and the structure is somewhat weird - it inherits some stuff from my 
main project that don't make much sense in this framework. For example the 
MetaWorldFragment class plays an important role in my SM - it is  supposed 
to hold information related to culling, among other things. In the context of
this project, however, the class is just confusing, and should probably be 
merged with IsoSurfaceRenderable.