# use packages
FIND_PACKAGE(OIS REQUIRED)

# settings
INCLUDE_DIRECTORIES(${OIS_INCLUDE_DIRS} include)

# build
REMOVE_DEFINITIONS(-DOGRE_TERRAINPLUGIN_EXPORTS)
FILE(GLOB_RECURSE src "*.cpp" "*.h")
INCLUDE_DIRECTORIES(${OIS_INCLUDE_DIRS})
ADD_EXECUTABLE(EditorSample WIN32 ${src})
TARGET_LINK_LIBRARIES(EditorSample ${OGRE_LIBRARIES} ${OIS_LIBRARIES} OgreOverhangTerrain)

# config files
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/ogre.cfg.in" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ogre.cfg")
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/plugins.cfg.in" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins.cfg")
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/resources.cfg.in" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources.cfg")

