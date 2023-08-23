

#include <osgDB/ReadFile>
#include <osgDB/ReadFile>
#include "EffectCompositor"
#include <osgUtil/Optimizer>
#include <osg/CoordinateSystemNode>

#include <osg/Switch>
#include <osg/Types>
#include <osgText/Text>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>
#include <osg/MatrixTransform>
#include <osgGA/SphericalManipulator>
#include <osgShadow/ShadowTechnique>
#include <osgShadow/ShadowMap>
#include <osgShadow/SoftShadowMap>
#include <osgShadow/StandardShadowMap>
#include <osgShadow/ShadowedScene>

#include <osgGA/Device>

#include <iostream>


int main(int argc, char** argv)
{
  // use an ArgumentParser object to manage the program arguments.
  osg::ArgumentParser arguments(&argc,argv);

  osgViewer::Viewer viewer(arguments);

  unsigned int helpType = 0;
  if ((helpType = arguments.readHelpType()))
  {
    arguments.getApplicationUsage()->write(std::cout, helpType);
    return 1;
  }

  // report any errors if they have occurred when parsing the program arguments.
  if (arguments.errors())
  {
    arguments.writeErrorMessages(std::cout);
    return 1;
  }

  if (arguments.argc()<=1)
  {
    arguments.getApplicationUsage()->write(std::cout,osg::ApplicationUsage::COMMAND_LINE_OPTION);
    return 1;
  }

 

  // set up the camera manipulators.
    {
      osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;
      keyswitchManipulator->addMatrixManipulator( '1', "Trackball", new osgGA::TrackballManipulator() );
      viewer.setCameraManipulator( keyswitchManipulator.get() );
    }

    // add the state manipulator
    viewer.addEventHandler( new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()) );

    // add the thread model handler
    viewer.addEventHandler(new osgViewer::ThreadingHandler);

    // add the window size toggle handler
    viewer.addEventHandler(new osgViewer::WindowSizeHandler);

    // add the stats handler
    viewer.addEventHandler(new osgViewer::StatsHandler);

    // load the data
    osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFiles(arguments);
    if (!loadedModel)
    {
      std::cout << arguments.getApplicationName() << ": No data loaded" << std::endl;
      return 1;
    }

    // any option left unread are converted into errors to write out later.
    arguments.reportRemainingOptionsAsUnrecognized();
    // report any errors if they have occurred when parsing the program arguments.
    if (arguments.errors())
    {
      arguments.writeErrorMessages(std::cout);
      return 1;
    }


    osg::Node *root = loadedModel;
    {
      osgShadow::ShadowedScene *shadowedScene = new osgShadow::ShadowedScene;
      shadowedScene->addChild(loadedModel);
      osgShadow::SoftShadowMap *sm = new osgShadow::SoftShadowMap;
      shadowedScene->setShadowTechnique(sm);
      sm->setTextureUnit(1);
      sm->setTextureSize(osg::Vec2s(1024, 1024));
      root = shadowedScene;
 
    
      osg::ref_ptr<osg::LightSource> ls = new osg::LightSource;
      osg::Vec4 lightpos(-10, -10, 10,1);
      ls->getLight()->setPosition(lightpos);
      shadowedScene->addChild(ls);

      if (false)
      {
        osg::Vec3 center = osg::Vec3(0,0,0);
        osg::Vec3 lightdir = center - osg::Vec3(lightpos.x(), lightpos.y(), lightpos.z());
        lightdir.normalize();
        ls->getLight()->setDirection(lightdir);
        ls->getLight()->setSpotCutoff(25.0f);

        //set the LightSource, only for checking, there is only 1 light in the scene
        osgShadow::ShadowMap* shadowMap = dynamic_cast<osgShadow::ShadowMap*>(shadowedScene->getShadowTechnique());
        if (shadowMap) shadowMap->setLight(ls.get());
      }   
    }

    osg::ref_ptr<osgFX::EffectCompositor> compositor;
    if (true) // if true shadows fails.
    {
      compositor = osgFX::readEffectFile("asd.xml");

      if (!compositor)
      {
        std::cerr << "Effect file \'" << "asd.xml" << "\' can't be loaded!" << std::endl;
        return 0;
      }
    }

    if (compositor) {
      compositor->addChild(root);
      root = compositor;
    }
    viewer.setSceneData(root);

    viewer.realize();
    viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);
    return viewer.run();

}
