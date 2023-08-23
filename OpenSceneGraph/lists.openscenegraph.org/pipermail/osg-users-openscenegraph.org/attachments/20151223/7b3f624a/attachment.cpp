/**
 * 
 * Example of osgUI 
 *
 */

#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>

#include <iostream>
#include <sstream>

osg::ref_ptr<osg::Camera> createHUDCamera(){
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	camera->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	camera->setRenderOrder(osg::Camera::POST_RENDER);
	camera->setAllowEventFocus(true);
	camera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1400, 0, 760));
	camera->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	camera->setName("HUD Camera");
	return camera;
}

//
// MAIN
//
int main( int argc, char **argv )
{   
	osg::ref_ptr<osg::Node> tabWidget1 = osgDB::readNodeFile("ui/TabWidget.lua");
	osg::ref_ptr<osg::Node> tabWidget2 = osgDB::readNodeFile("ui/TabWidget.lua");

	osg::ref_ptr<osg::MatrixTransform> transform1 = new osg::MatrixTransform;
	transform1->setMatrix(osg::Matrix::rotate(osg::PI_2, 1, 0, 0)*osg::Matrix::rotate(osg::PI_4, 0, 0, 1)*osg::Matrix::translate(-120.0, 0.0f, -50.0f));
	transform1->addChild(tabWidget1);

	osg::ref_ptr<osg::MatrixTransform> transform2 = new osg::MatrixTransform;
	transform2->setMatrix(osg::Matrix::scale(3.0f, 3.0f, 3.0f)*osg::Matrix::translate(800.0f, 380.0f - 150.0f, 0.0f));
	transform2->addChild(tabWidget2);

	osg::ref_ptr<osg::Group> root = new osg::Group;
	auto hud = createHUDCamera();
	hud->addChild(transform2.get());
	root->addChild(hud.get());
	root->addChild(transform1.get());

    osgViewer::Viewer viewer;	// Construct the viewer.   
    viewer.setSceneData( root.get() );	// Set the final SceneData to show
	viewer.addEventHandler(new osgViewer::StatsHandler);
	viewer.setCameraManipulator(new osgGA::TrackballManipulator);
	viewer.setUpViewInWindow(50, 50, 1400, 760);

	osg::Vec3d eye, center, up;
	eye = osg::Vec3d(0.0,-400.0, 0.0f);
	center = osg::Vec3d(0.0, 0.0f, 0.0f);
	up = osg::Vec3d(0.0f, 0.0f, 1.0f);
	auto cm = viewer.getCameraManipulator();
	cm->setHomePosition(eye, center, up, false);

	viewer.home();
	while (!viewer.done()){
		viewer.frame();
	}
	return 0;
}