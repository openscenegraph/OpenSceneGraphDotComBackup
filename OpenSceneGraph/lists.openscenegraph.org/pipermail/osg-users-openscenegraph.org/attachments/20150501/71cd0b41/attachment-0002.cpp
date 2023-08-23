#include <QTimer>
#include <QApplication>
#include <QGridLayout>

#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgGA/MultiTouchTrackballManipulator>

#include <osgDB/ReadFile>

#include <osgQt/GraphicsWindowQt>


#include <osgUtil/Optimizer>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/Registry>
#include <osgGA/StateSetManipulator>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Geode>
#include <osg/Camera>
#include <osg/ShapeDrawable>
#include <osg/Sequence>
#include <osg/PolygonMode>
#include <osg/io_utils>
#include <osgText/Font>
#include <osgText/Text>


#include <iostream>

#include <osgcamera.h>


osg::Group* createHUDText()
{

    osg::Group* rootNode = new osg::Group;

    osgText::Font* font = osgText::readFontFile("fonts/arial.ttf");

    //osg::setNotifyLevel(osg::INFO);

    osg::Geode* geode  = new osg::Geode;
    rootNode->addChild(geode);

    bool useVBOs = false;
#if defined(OSG_GL3_AVAILABLE) && !defined(OSG_GL2_AVAILABLE) && !defined(OSG_GL1_AVAILABLE)
    useVBOs = true;
    osg::Program* program = new osg::Program;
    program->addShader(new osg::Shader(osg::Shader::VERTEX, gl3TextVertexShader));
    program->addShader(new osg::Shader(osg::Shader::FRAGMENT, gl3TextFragmentShader));
    geode->getOrCreateStateSet()->setAttributeAndModes(program, osg::StateAttribute::ON);
#endif

    float windowHeight = 1024.0f;
    float windowWidth = 1280.0f;
    float margin = 50.0f;


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Examples of how to set up different text layout
//

    osg::Vec4 layoutColor(1.0f,1.0f,0.0f,1.0f);
    float layoutCharacterSize = 20.0f;

    {
        osgText::Text* text = new osgText::Text;
        text->setUseVertexBufferObjects(useVBOs);
        text->setFont(font);
        text->setColor(layoutColor);
        text->setCharacterSize(layoutCharacterSize);
        text->setPosition(osg::Vec3(margin,windowHeight-margin,0.0f));

        // the default layout is left to right, typically used in languages
        // originating from europe such as English, French, German, Spanish etc..
        text->setLayout(osgText::Text::LEFT_TO_RIGHT);

        text->setText("text->setLayout(osgText::Text::LEFT_TO_RIGHT);");
        geode->addDrawable(text);
    }

    {
        osgText::Text* text = new osgText::Text;
        text->setUseVertexBufferObjects(useVBOs);
        text->setFont(font);
        text->setColor(layoutColor);
        text->setCharacterSize(layoutCharacterSize);
        text->setPosition(osg::Vec3(windowWidth-margin,windowHeight-margin,0.0f));

        // right to left layouts would be used for hebrew or arabic fonts.
        text->setLayout(osgText::Text::RIGHT_TO_LEFT);
        text->setAlignment(osgText::Text::RIGHT_BASE_LINE);

        text->setText("text->setLayout(osgText::Text::RIGHT_TO_LEFT);");
        geode->addDrawable(text);
    }

    {
        osgText::Text* text = new osgText::Text;
        text->setUseVertexBufferObjects(useVBOs);
        text->setFont(font);
        text->setColor(layoutColor);
        text->setPosition(osg::Vec3(margin,windowHeight-margin,0.0f));
        text->setCharacterSize(layoutCharacterSize);

        // vertical font layout would be used for asian fonts.
        text->setLayout(osgText::Text::VERTICAL);

        text->setText("text->setLayout(osgText::Text::VERTICAL);");
        geode->addDrawable(text);
    }


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Examples of how to set up different font resolution
//

    osg::Vec4 fontSizeColor(0.0f,1.0f,1.0f,1.0f);
    float fontSizeCharacterSize = 30;

    osg::Vec3 cursor = osg::Vec3(margin*2,windowHeight-margin*2,0.0f);

    {
        osgText::Text* text = new osgText::Text;
        text->setUseVertexBufferObjects(useVBOs);
        text->setFont(font);
        text->setColor(fontSizeColor);
        text->setCharacterSize(fontSizeCharacterSize);
        text->setPosition(cursor);

        // use text that uses 10 by 10 texels as a target resolution for fonts.
        text->setFontResolution(10,10); // blocky but small texture memory usage

        text->setText("text->setFontResolution(10,10); // blocky but small texture memory usage");
        geode->addDrawable(text);
    }

    cursor.y() -= fontSizeCharacterSize;
    {
        osgText::Text* text = new osgText::Text;
        text->setUseVertexBufferObjects(useVBOs);
        text->setFont(font);
        text->setColor(fontSizeColor);
        text->setCharacterSize(fontSizeCharacterSize);
        text->setPosition(cursor);

        // use text that uses 20 by 20 texels as a target resolution for fonts.
        text->setFontResolution(20,20); // smoother but higher texture memory usage (but still quite low).

        text->setText("text->setFontResolution(20,20); // smoother but higher texture memory usage (but still quite low).");
        geode->addDrawable(text);
    }

    cursor.y() -= fontSizeCharacterSize;
    {
        osgText::Text* text = new osgText::Text;
        text->setUseVertexBufferObjects(useVBOs);
        text->setFont(font);
        text->setColor(fontSizeColor);
        text->setCharacterSize(fontSizeCharacterSize);
        text->setPosition(cursor);

        // use text that uses 40 by 40 texels as a target resolution for fonts.
        text->setFontResolution(40,40); // even smoother but again higher texture memory usage.

        text->setText("text->setFontResolution(40,40); // even smoother but again higher texture memory usage.");
        geode->addDrawable(text);
    }


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Examples of how to set up different sized text
//

    osg::Vec4 characterSizeColor(1.0f,0.0f,1.0f,1.0f);

    cursor.y() -= fontSizeCharacterSize*2.0f;

    {
        osgText::Text* text = new osgText::Text;
        text->setUseVertexBufferObjects(useVBOs);
        text->setFont(font);
        text->setColor(characterSizeColor);
        text->setFontResolution(20,20);
        text->setPosition(cursor);

        // use text that is 20 units high.
        text->setCharacterSize(20); // small

        text->setText("text->setCharacterSize(20.0f); // small");
        geode->addDrawable(text);
    }

    cursor.y() -= 30.0f;
    {
        osgText::Text* text = new osgText::Text;
        text->setUseVertexBufferObjects(useVBOs);
        text->setFont(font);
        text->setColor(characterSizeColor);
        text->setFontResolution(30,30);
        text->setPosition(cursor);

        // use text that is 30 units high.
        text->setCharacterSize(30.0f); // medium

        text->setText("text->setCharacterSize(30.0f); // medium");
        geode->addDrawable(text);
    }

    cursor.y() -= 50.0f;
    {
        osgText::Text* text = new osgText::Text;
        text->setUseVertexBufferObjects(useVBOs);
        text->setFont(font);
        text->setColor(characterSizeColor);
        text->setFontResolution(40,40);
        text->setPosition(cursor);

        // use text that is 60 units high.
        text->setCharacterSize(60.0f); // large

        text->setText("text->setCharacterSize(60.0f); // large");
        geode->addDrawable(text);
    }


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Examples of how to set up different alignments
//

    osg::Vec4 alignmentSizeColor(0.0f,1.0f,0.0f,1.0f);
    float alignmentCharacterSize = 25.0f;
    cursor.x() = 640;
    cursor.y() = margin*4.0f;

    typedef std::pair<osgText::Text::AlignmentType,std::string> AlignmentPair;
    typedef std::vector<AlignmentPair> AlignmentList;
    AlignmentList alignmentList;
    alignmentList.push_back(AlignmentPair(osgText::Text::LEFT_TOP,"text->setAlignment(\nosgText::Text::LEFT_TOP);"));
    alignmentList.push_back(AlignmentPair(osgText::Text::LEFT_CENTER,"text->setAlignment(\nosgText::Text::LEFT_CENTER);"));
    alignmentList.push_back(AlignmentPair(osgText::Text::LEFT_BOTTOM,"text->setAlignment(\nosgText::Text::LEFT_BOTTOM);"));
    alignmentList.push_back(AlignmentPair(osgText::Text::CENTER_TOP,"text->setAlignment(\nosgText::Text::CENTER_TOP);"));
    alignmentList.push_back(AlignmentPair(osgText::Text::CENTER_CENTER,"text->setAlignment(\nosgText::Text::CENTER_CENTER);"));
    alignmentList.push_back(AlignmentPair(osgText::Text::CENTER_BOTTOM,"text->setAlignment(\nosgText::Text::CENTER_BOTTOM);"));
    alignmentList.push_back(AlignmentPair(osgText::Text::RIGHT_TOP,"text->setAlignment(\nosgText::Text::RIGHT_TOP);"));
    alignmentList.push_back(AlignmentPair(osgText::Text::RIGHT_CENTER,"text->setAlignment(\nosgText::Text::RIGHT_CENTER);"));
    alignmentList.push_back(AlignmentPair(osgText::Text::RIGHT_BOTTOM,"text->setAlignment(\nosgText::Text::RIGHT_BOTTOM);"));
    alignmentList.push_back(AlignmentPair(osgText::Text::LEFT_BASE_LINE,"text->setAlignment(\nosgText::Text::LEFT_BASE_LINE);"));
    alignmentList.push_back(AlignmentPair(osgText::Text::CENTER_BASE_LINE,"text->setAlignment(\nosgText::Text::CENTER_BASE_LINE);"));
    alignmentList.push_back(AlignmentPair(osgText::Text::RIGHT_BASE_LINE,"text->setAlignment(\nosgText::Text::RIGHT_BASE_LINE);"));
    alignmentList.push_back(AlignmentPair(osgText::Text::LEFT_BOTTOM_BASE_LINE,"text->setAlignment(\nosgText::Text::LEFT_BOTTOM_BASE_LINE);"));
    alignmentList.push_back(AlignmentPair(osgText::Text::CENTER_BOTTOM_BASE_LINE,"text->setAlignment(\nosgText::Text::CENTER_BOTTOM_BASE_LINE);"));
    alignmentList.push_back(AlignmentPair(osgText::Text::RIGHT_BOTTOM_BASE_LINE,"text->setAlignment(\nosgText::Text::RIGHT_BOTTOM_BASE_LINE);"));


    osg::Sequence* sequence = new osg::Sequence;
    {
        for(AlignmentList::iterator itr=alignmentList.begin();
            itr!=alignmentList.end();
            ++itr)
        {
            osg::Geode* alignmentGeode = new osg::Geode;
            sequence->addChild(alignmentGeode);
            sequence->setTime(sequence->getNumChildren(), 1.0f);

            osgText::Text* text = new osgText::Text;
            text->setFont(font);
            text->setColor(alignmentSizeColor);
            text->setCharacterSize(alignmentCharacterSize);
            text->setPosition(cursor);
            text->setDrawMode(osgText::Text::TEXT|osgText::Text::ALIGNMENT|osgText::Text::BOUNDINGBOX);

            text->setAlignment(itr->first);
            text->setText(itr->second);

            alignmentGeode->addDrawable(text);


        }

    }

    sequence->setMode(osg::Sequence::START);
    sequence->setInterval(osg::Sequence::LOOP, 0, -1);
    sequence->setDuration(1.0f, -1);

    rootNode->addChild(sequence);


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Examples of how to set up different fonts...
//

    cursor.x() = margin*2.0f;
    cursor.y() = margin*2.0f;

    osg::Vec4 fontColor(1.0f,0.5f,0.0f,1.0f);
    float fontCharacterSize = 20.0f;
    float spacing = 40.0f;

    {
        osgText::Text* text = new osgText::Text;
        text->setUseVertexBufferObjects(useVBOs);
        text->setColor(fontColor);
        text->setPosition(cursor);
        text->setCharacterSize(fontCharacterSize);

        text->setFont(0);
        text->setText("text->setFont(0); // inbuilt font.");
        geode->addDrawable(text);

        cursor.x() = text->getBoundingBox().xMax() + spacing ;
    }

    {
        osgText::Font* arial = osgText::readFontFile("fonts/arial.ttf");

        osgText::Text* text = new osgText::Text;
        text->setUseVertexBufferObjects(useVBOs);
        text->setColor(fontColor);
        text->setPosition(cursor);
        text->setCharacterSize(fontCharacterSize);

        text->setFont(arial);
        text->setText(arial!=0?
                      "text->setFont(\"fonts/arial.ttf\");":
                      "unable to load \"fonts/arial.ttf\"");
        geode->addDrawable(text);

        cursor.x() = text->getBoundingBox().xMax() + spacing ;
    }

    {
        osgText::Font* times = osgText::readFontFile("fonts/times.ttf");

        osgText::Text* text = new osgText::Text;
        text->setUseVertexBufferObjects(useVBOs);
        text->setColor(fontColor);
        text->setPosition(cursor);
        text->setCharacterSize(fontCharacterSize);

        geode->addDrawable(text);
        text->setFont(times);
        text->setText(times!=0?
                      "text->setFont(\"fonts/times.ttf\");":
                      "unable to load \"fonts/times.ttf\"");

        cursor.x() = text->getBoundingBox().xMax() + spacing ;
    }

    cursor.x() = margin*2.0f;
    cursor.y() = margin;

    {
        osgText::Font* dirtydoz = osgText::readFontFile("fonts/dirtydoz.ttf");

        osgText::Text* text = new osgText::Text;
        text->setUseVertexBufferObjects(useVBOs);
        text->setColor(fontColor);
        text->setPosition(cursor);
        text->setCharacterSize(fontCharacterSize);

        text->setFont(dirtydoz);
        text->setText(dirtydoz!=0?
                      "text->setFont(\"fonts/dirtydoz.ttf\");":
                      "unable to load \"fonts/dirtydoz.ttf\"");
        geode->addDrawable(text);

        cursor.x() = text->getBoundingBox().xMax() + spacing ;
    }

    {
        osgText::Font* fudd = osgText::readFontFile("fonts/fudd.ttf");

        osgText::Text* text = new osgText::Text;
        text->setUseVertexBufferObjects(useVBOs);
        text->setColor(fontColor);
        text->setPosition(cursor);
        text->setCharacterSize(fontCharacterSize);

        text->setFont(fudd);
        text->setText(fudd!=0?
                      "text->setFont(\"fonts/fudd.ttf\");":
                      "unable to load \"fonts/fudd.ttf\"");
        geode->addDrawable(text);

        cursor.x() = text->getBoundingBox().xMax() + spacing ;
    }

    return rootNode;
}

class ViewerWidget : public QWidget, public osgViewer::CompositeViewer
{
public:

    osg::Camera* camera;

    ViewerWidget(osg::Group* group, QWidget* parent = 0, Qt::WindowFlags f = 0, osgViewer::ViewerBase::ThreadingModel threadingModel=osgViewer::CompositeViewer::SingleThreaded) : QWidget(parent, f)
    {
        setThreadingModel(threadingModel);

        // disable the default setting of viewer.done() by pressing Escape.
        setKeyEventSetsDone(0);

        QWidget* widget1 = addViewWidget( createGraphicsWindow(0,0,100,100), group, osgDB::readNodeFile("cow.osgt") );
        QWidget* widget2 = addViewWidget( createGraphicsWindow(0,0,100,100), group, osgDB::readNodeFile("cessna.osgt") );

        QGridLayout* gridlayout = new QGridLayout;
        gridlayout->addWidget(widget1, 0, 0);
        gridlayout->addWidget(widget2, 0, 1);
        setLayout(gridlayout);

        connect( &_timer, SIGNAL(timeout()), this, SLOT(update()) );
        _timer.start( 16 );
    }

    QWidget* addViewWidget( osgQt::GraphicsWindowQt* gw, osg::Group* group, osg::Node* scene )
    {
        osgViewer::View* view = new osgViewer::View;
        addView( view );

        group->addChild(scene);

        // create the hud.
        osg::Camera* camera = new osg::Camera;
        camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
        camera->setProjectionMatrixAsOrtho2D(0,1280,0,1024);
        camera->setViewMatrix(osg::Matrix::identity());
        camera->setClearMask(GL_DEPTH_BUFFER_BIT);
        camera->addChild(createHUDText());
        camera->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

        group->addChild(camera);

        osg::Camera* vcamera = view->getCamera();
        vcamera->setGraphicsContext( gw );

        const osg::GraphicsContext::Traits* traits = gw->getTraits();

        vcamera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
        vcamera->setViewport( new osg::Viewport(0, 0, traits->width, traits->height) );
        vcamera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0f, 10000.0f );

        view->setSceneData( group );
        view->addEventHandler( new osgViewer::StatsHandler );
        view->setCameraManipulator( new osgGA::MultiTouchTrackballManipulator );
        gw->setTouchEventsEnabled( true );

        return gw->getGLWidget();
    }

    osgQt::GraphicsWindowQt* createGraphicsWindow( int x, int y, int w, int h, const std::string& name="", bool windowDecoration=false )
    {
        osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
        osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
        traits->windowName = name;
        traits->windowDecoration = windowDecoration;
        traits->x = x;
        traits->y = y;
        traits->width = w;
        traits->height = h;
        traits->doubleBuffer = true;
        traits->alpha = ds->getMinimumNumAlphaBits();
        traits->stencil = ds->getMinimumNumStencilBits();
        traits->sampleBuffers = ds->getMultiSamples();
        traits->samples = ds->getNumMultiSamples();
        return new osgQt::GraphicsWindowQt(traits.get());
    }

    virtual void paintEvent( QPaintEvent* )
    { frame(); }
protected:
    QTimer _timer;
};

int main( int argc, char** argv )
{
    osg::ArgumentParser arguments(&argc, argv);

    osg::Group* group = new osg::Group;

#if QT_VERSION >= 0x050000
    // Qt5 is currently crashing and reporting "Cannot make QOpenGLContext current in a different thread" when the viewer is run multi-threaded, this is regression from Qt4
    osgViewer::ViewerBase::ThreadingModel threadingModel = osgViewer::ViewerBase::SingleThreaded;
#else
    osgViewer::ViewerBase::ThreadingModel threadingModel = osgViewer::ViewerBase::CullDrawThreadPerContext;
#endif

    while (arguments.read("--SingleThreaded")) threadingModel = osgViewer::ViewerBase::SingleThreaded;
    while (arguments.read("--CullDrawThreadPerContext")) threadingModel = osgViewer::ViewerBase::CullDrawThreadPerContext;
    while (arguments.read("--DrawThreadPerContext")) threadingModel = osgViewer::ViewerBase::DrawThreadPerContext;
    while (arguments.read("--CullThreadPerCameraDrawThreadPerContext")) threadingModel = osgViewer::ViewerBase::CullThreadPerCameraDrawThreadPerContext;

#if QT_VERSION >= 0x040800
    // Required for multithreaded QGLWidget on Linux/X11, see http://blog.qt.io/blog/2011/06/03/threaded-opengl-in-4-8/
    if (threadingModel != osgViewer::ViewerBase::SingleThreaded)
        QApplication::setAttribute(Qt::AA_X11InitThreads);
#endif
    
    QApplication app(argc, argv);
    ViewerWidget* viewWidget = new ViewerWidget(group, 0, Qt::Widget, threadingModel);
    viewWidget->setGeometry( 100, 100, 1024, 768 );
    viewWidget->resize(1280, 1024);
    viewWidget->show();
    return app.exec();
}