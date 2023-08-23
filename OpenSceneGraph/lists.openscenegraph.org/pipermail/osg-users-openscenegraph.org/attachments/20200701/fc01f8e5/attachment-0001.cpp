// MFC_OSG.cpp : implementation of the cOSG class
//
#include "stdafx.h"
#include "MFC_OSG.h"


#include <osgGA/TerrainManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>


class PickHandler : public osgGA::GUIEventHandler {
public:

	PickHandler(osgText::Text* updateText) :
		_updateText(updateText) {}

	~PickHandler() {}

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	virtual void pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);

	void setLabel(const std::string& name)
	{
		if (_updateText.get()) _updateText->setText(name);
	}

protected:

	osg::ref_ptr<osgText::Text>  _updateText;
};

bool PickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	switch (ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::PUSH) :
	{
		osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
		if (view) pick(view, ea);
		return false;
	}
	case(osgGA::GUIEventAdapter::KEYDOWN) :
	{
		if (ea.getKey() == 'c')
		{
			osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
			osg::ref_ptr<osgGA::GUIEventAdapter> event = new osgGA::GUIEventAdapter(ea);
			event->setX((ea.getXmin() + ea.getXmax())*0.5);
			event->setY((ea.getYmin() + ea.getYmax())*0.5);
			if (view) pick(view, *event);
		}
		return false;
	}
	default:
		return false;
	}
}

void PickHandler::pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea)
{
	osgUtil::LineSegmentIntersector::Intersections intersections;

	std::string gdlist = "";

	if (view->computeIntersections(ea, intersections))
	{
		for (osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
			hitr != intersections.end();
			++hitr)
		{
			std::ostringstream os;
			if (!hitr->nodePath.empty() && !(hitr->nodePath.back()->getName().empty()))
			{
				// the geodes are identified by name.
				os << "Object \"" << hitr->nodePath.back()->getName() << "\"" << std::endl;
			}
			else if (hitr->drawable.valid())
			{
				os << "Object \"" << hitr->drawable->className() << "\"" << std::endl;
			}

			osg::Vec3d v1 = hitr->getLocalIntersectPoint();
			os << "        local coords vertex(";
			os << v1.x() << "  ";
			os << v1.y() << "  ";
			os << v1.z() << "  ";
			os << ")";
			os << std::endl;

			const osgUtil::LineSegmentIntersector::Intersection::IndexList& vil = hitr->indexList;
			for (unsigned int i = 0; i < vil.size(); ++i)
			{
				os << "        vertex indices [" << i << "] = " << vil[i] << std::endl;
			}

			gdlist += os.str();
		}
	}
	setLabel(gdlist);
}


cOSG::cOSG(HWND hWnd) :
   m_hWnd(hWnd)
{
}

cOSG::~cOSG()
{
    mViewer->setDone(true);
    Sleep(1000);
    mViewer->stopThreading();

    delete mViewer;
}

void cOSG::InitOSG(std::string modelname)
{
    // Store the name of the model to load
    m_ModelName = modelname;

    // Init different parts of OSG
    InitManipulators();
    InitSceneGraph();
    InitCameraConfig();
}

void cOSG::InitManipulators(void)
{
    // Create a trackball manipulator
    trackball = new osgGA::TrackballManipulator();

    // Create a Manipulator Switcher
    keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;

    // Add our trackball manipulator to the switcher
    keyswitchManipulator->addMatrixManipulator( '1', "Trackball", trackball.get());

    // Init the switcher to the first manipulator (in this case the only manipulator)
    keyswitchManipulator->selectMatrixManipulator(0);  // Zero based index Value
}


void cOSG::InitSceneGraph(void)
{
    // Init the main Root Node/Group
    mRoot  = new osg::Group;

    // Load the Model from the model name
    mModel = osgDB::readNodeFile(m_ModelName);
    if (!mModel) return;

    // Optimize the model
    osgUtil::Optimizer optimizer;
    optimizer.optimize(mModel.get());
    optimizer.reset();

    // Add the model to the scene
    mRoot->addChild(mModel.get());
}

void cOSG::InitCameraConfig(void)
{
    // Local Variable to hold window size data
    RECT rect;

    // Create the viewer for this window
    mViewer = new osgViewer::Viewer();

    // Add a Stats Handler to the viewer
    mViewer->addEventHandler(new osgViewer::StatsHandler);
	// add the state manipulator
	mViewer->addEventHandler(new osgGA::StateSetManipulator(mViewer->getCamera()->getOrCreateStateSet()));//对于L,T，W键
    // Get the current window size
    ::GetWindowRect(m_hWnd, &rect);

    // Init the GraphicsContext Traits
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;

    // Init the Windata Variable that holds the handle for the Window to display OSG in.
    osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(m_hWnd);

    // Setup the traits parameters
    traits->x = 0;
    traits->y = 0;
    traits->width = rect.right - rect.left;
    traits->height = rect.bottom - rect.top;
    traits->windowDecoration = false;
    traits->doubleBuffer = true;
    traits->sharedContext = 0;
    traits->setInheritedWindowPixelFormat = true;
    traits->inheritedWindowData = windata;

    // Create the Graphics Context
    osg::GraphicsContext* gc = osg::GraphicsContext::createGraphicsContext(traits.get());//函数内部修改了traits的x,y值！！！

    // Init Master Camera for this View
    osg::ref_ptr<osg::Camera> camera = mViewer->getCamera();

    // Assign Graphics Context to the Camera
    camera->setGraphicsContext(gc);

    // Set the viewport for the Camera
	//camera->setViewport(new osg::Viewport(traits->x, traits->y, traits->width, traits->height));
	camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));

    // Set projection matrix and camera attribtues
    camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    camera->setClearColor(osg::Vec4f(0.2f, 0.2f, 0.4f, 1.0f));
    camera->setProjectionMatrixAsPerspective(
        30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0, 1000.0);

    // Add the Camera to the Viewer
    //mViewer->addSlave(camera.get());
    mViewer->setCamera(camera.get());

	osg::ref_ptr<osgText::Text> updateText = new osgText::Text;
	mRoot->addChild(createHUD(updateText.get()));

    // Add the Camera Manipulator to the Viewer
    mViewer->setCameraManipulator(keyswitchManipulator.get());

	

	mViewer->addEventHandler(new PickHandler(updateText));
    // Set the Scene Data
    mViewer->setSceneData(mRoot.get());

    // Realize the Viewer
    mViewer->realize();

    // Correct aspect ratio
    /*double fovy,aspectRatio,z1,z2;
    mViewer->getCamera()->getProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);
    aspectRatio=double(traits->width)/double(traits->height);
    mViewer->getCamera()->setProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);*/
}

osg::Node* cOSG::createHUD(osgText::Text* updateText)
{

	// create the hud. derived from osgHud.cpp
	// adds a set of quads, each in a separate Geode - which can be picked individually
	// eg to be used as a menuing/help system!
	// Can pick texts too!

	osg::Camera* hudCamera = new osg::Camera;
	hudCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	hudCamera->setProjectionMatrixAsOrtho2D(0, 1280, 0, 1024);
	hudCamera->setViewMatrix(osg::Matrix::identity());
	hudCamera->setRenderOrder(osg::Camera::POST_RENDER);
	hudCamera->setClearMask(GL_DEPTH_BUFFER_BIT);

	//std::string timesFont("fonts/times.ttf");
	std::string timesFont("fonts/arial.ttf");
	

	// turn lighting off for the text and disable depth test to ensure its always ontop.
	osg::Vec3 position(150.0f, 800.0f, 0.0f);
	osg::Vec3 delta(0.0f, -60.0f, 0.0f);

	{
		osg::Geode* geode = new osg::Geode();
		osg::StateSet* stateset = geode->getOrCreateStateSet();
		stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
		geode->setName("simple");
		hudCamera->addChild(geode);

		osgText::Text* text = new  osgText::Text;
		geode->addDrawable(text);

		text->setFont(timesFont);
		text->setText(std::string("Picking in MFC!"));
		text->setPosition(position);

		position += delta;
	}


	for (int i = 0; i < 5; i++) {
		osg::Vec3 dy(0.0f, -30.0f, 0.0f);
		osg::Vec3 dx(120.0f, 0.0f, 0.0f);
		osg::Geode* geode = new osg::Geode();
		osg::StateSet* stateset = geode->getOrCreateStateSet();
		const char *opts[] = { "One", "Two", "Three", "January", "Feb", "2003" };
		osg::Geometry *quad = new osg::Geometry;
		stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
		std::string name = "subOption";
		name += " ";
		name += std::string(opts[i]);
		geode->setName(name);
		osg::Vec3Array* vertices = new osg::Vec3Array(4); // 1 quad
		osg::Vec4Array* colors = new osg::Vec4Array;
		colors = new osg::Vec4Array;
		colors->push_back(osg::Vec4(0.8 - 0.1*i, 0.1*i, 0.2*i, 1.0));
		quad->setColorArray(colors, osg::Array::BIND_OVERALL);
		(*vertices)[0] = position;
		(*vertices)[1] = position + dx;
		(*vertices)[2] = position + dx + dy;
		(*vertices)[3] = position + dy;
		quad->setVertexArray(vertices);
		quad->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
		geode->addDrawable(quad);
		hudCamera->addChild(geode);

		position += delta;
	}



	{ // this displays what has been selected
		osg::Geode* geode = new osg::Geode();
		osg::StateSet* stateset = geode->getOrCreateStateSet();
		stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
		geode->setName("The text label");
		geode->addDrawable(updateText);
		hudCamera->addChild(geode);

		updateText->setCharacterSize(20.0f);
		updateText->setFont(timesFont);
		updateText->setColor(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
		updateText->setText("");
		updateText->setPosition(position);
		updateText->setDataVariance(osg::Object::DYNAMIC);

		position += delta;
	}

	return hudCamera;

}


void cOSG::PreFrameUpdate()
{
    // Due any preframe updates in this routine
}

void cOSG::PostFrameUpdate()
{
    // Due any postframe updates in this routine
}

/*void cOSG::Render(void* ptr)
{
    cOSG* osg = (cOSG*)ptr;

    osgViewer::Viewer* viewer = osg->getViewer();

    // You have two options for the main viewer loop
    //      viewer->run()   or
    //      while(!viewer->done()) { viewer->frame(); }

    //viewer->run();
    while(!viewer->done())
    {
        osg->PreFrameUpdate();
        viewer->frame();
        osg->PostFrameUpdate();
        //Sleep(10);         // Use this command if you need to allow other processes to have cpu time
    }

    // For some reason this has to be here to avoid issue:
    // if you have multiple OSG windows up
    // and you exit one then all stop rendering
    AfxMessageBox("Exit Rendering Thread");

    _endthread();
}*/

CRenderingThread::CRenderingThread( cOSG* ptr )
:   OpenThreads::Thread(), _ptr(ptr), _done(false)
{
}

CRenderingThread::~CRenderingThread()
{
    _done = true;
    if (isRunning())
    {
        cancel();
        join();
    }
}

void CRenderingThread::run()
{
    if ( !_ptr )
    {
        _done = true;
        return;
    }

    osgViewer::Viewer* viewer = _ptr->getViewer();
    do
    {
        _ptr->PreFrameUpdate();
        viewer->frame();
        _ptr->PostFrameUpdate();
    } while ( !testCancel() && !viewer->done() && !_done );
}
