#include "GLWidget.h"
#include <QWindow>
#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>


class QtKeyboardMap5
{

public:
    QtKeyboardMap5()
    {
        mKeyMap[Qt::Key_Escape     ] = osgGA::GUIEventAdapter::KEY_Escape;
        mKeyMap[Qt::Key_Delete   ] = osgGA::GUIEventAdapter::KEY_Delete;
        mKeyMap[Qt::Key_Home       ] = osgGA::GUIEventAdapter::KEY_Home;
        mKeyMap[Qt::Key_Enter      ] = osgGA::GUIEventAdapter::KEY_KP_Enter;
        mKeyMap[Qt::Key_End        ] = osgGA::GUIEventAdapter::KEY_End;
        mKeyMap[Qt::Key_Return     ] = osgGA::GUIEventAdapter::KEY_Return;
        mKeyMap[Qt::Key_PageUp     ] = osgGA::GUIEventAdapter::KEY_Page_Up;
        mKeyMap[Qt::Key_PageDown   ] = osgGA::GUIEventAdapter::KEY_Page_Down;
        mKeyMap[Qt::Key_Left       ] = osgGA::GUIEventAdapter::KEY_Left;
        mKeyMap[Qt::Key_Right      ] = osgGA::GUIEventAdapter::KEY_Right;
        mKeyMap[Qt::Key_Up         ] = osgGA::GUIEventAdapter::KEY_Up;
        mKeyMap[Qt::Key_Down       ] = osgGA::GUIEventAdapter::KEY_Down;
        mKeyMap[Qt::Key_Backspace  ] = osgGA::GUIEventAdapter::KEY_BackSpace;
        mKeyMap[Qt::Key_Tab        ] = osgGA::GUIEventAdapter::KEY_Tab;
        mKeyMap[Qt::Key_Space      ] = osgGA::GUIEventAdapter::KEY_Space;
        mKeyMap[Qt::Key_Delete     ] = osgGA::GUIEventAdapter::KEY_Delete;
        mKeyMap[Qt::Key_Alt      ] = osgGA::GUIEventAdapter::KEY_Alt_L;
        mKeyMap[Qt::Key_Shift    ] = osgGA::GUIEventAdapter::KEY_Shift_L;
        mKeyMap[Qt::Key_Control  ] = osgGA::GUIEventAdapter::KEY_Control_L;
        mKeyMap[Qt::Key_Meta     ] = osgGA::GUIEventAdapter::KEY_Meta_L;

        mKeyMap[Qt::Key_F1             ] = osgGA::GUIEventAdapter::KEY_F1;
        mKeyMap[Qt::Key_F2             ] = osgGA::GUIEventAdapter::KEY_F2;
        mKeyMap[Qt::Key_F3             ] = osgGA::GUIEventAdapter::KEY_F3;
        mKeyMap[Qt::Key_F4             ] = osgGA::GUIEventAdapter::KEY_F4;
        mKeyMap[Qt::Key_F5             ] = osgGA::GUIEventAdapter::KEY_F5;
        mKeyMap[Qt::Key_F6             ] = osgGA::GUIEventAdapter::KEY_F6;
        mKeyMap[Qt::Key_F7             ] = osgGA::GUIEventAdapter::KEY_F7;
        mKeyMap[Qt::Key_F8             ] = osgGA::GUIEventAdapter::KEY_F8;
        mKeyMap[Qt::Key_F9             ] = osgGA::GUIEventAdapter::KEY_F9;
        mKeyMap[Qt::Key_F10            ] = osgGA::GUIEventAdapter::KEY_F10;
        mKeyMap[Qt::Key_F11            ] = osgGA::GUIEventAdapter::KEY_F11;
        mKeyMap[Qt::Key_F12            ] = osgGA::GUIEventAdapter::KEY_F12;
        mKeyMap[Qt::Key_F13            ] = osgGA::GUIEventAdapter::KEY_F13;
        mKeyMap[Qt::Key_F14            ] = osgGA::GUIEventAdapter::KEY_F14;
        mKeyMap[Qt::Key_F15            ] = osgGA::GUIEventAdapter::KEY_F15;
        mKeyMap[Qt::Key_F16            ] = osgGA::GUIEventAdapter::KEY_F16;
        mKeyMap[Qt::Key_F17            ] = osgGA::GUIEventAdapter::KEY_F17;
        mKeyMap[Qt::Key_F18            ] = osgGA::GUIEventAdapter::KEY_F18;
        mKeyMap[Qt::Key_F19            ] = osgGA::GUIEventAdapter::KEY_F19;
        mKeyMap[Qt::Key_F20            ] = osgGA::GUIEventAdapter::KEY_F20;

        mKeyMap[Qt::Key_hyphen         ] = '-';
        mKeyMap[Qt::Key_Equal         ] = '=';

        mKeyMap[Qt::Key_division      ] = osgGA::GUIEventAdapter::KEY_KP_Divide;
        mKeyMap[Qt::Key_multiply      ] = osgGA::GUIEventAdapter::KEY_KP_Multiply;
        mKeyMap[Qt::Key_Minus         ] = '-';
        mKeyMap[Qt::Key_Plus          ] = '+';
        //mKeyMap[Qt::Key_H              ] = osgGA::GUIEventAdapter::KEY_KP_Home;
        //mKeyMap[Qt::Key_                    ] = osgGA::GUIEventAdapter::KEY_KP_Up;
        //mKeyMap[92                    ] = osgGA::GUIEventAdapter::KEY_KP_Page_Up;
        //mKeyMap[86                    ] = osgGA::GUIEventAdapter::KEY_KP_Left;
        //mKeyMap[87                    ] = osgGA::GUIEventAdapter::KEY_KP_Begin;
        //mKeyMap[88                    ] = osgGA::GUIEventAdapter::KEY_KP_Right;
        //mKeyMap[83                    ] = osgGA::GUIEventAdapter::KEY_KP_End;
        //mKeyMap[84                    ] = osgGA::GUIEventAdapter::KEY_KP_Down;
        //mKeyMap[85                    ] = osgGA::GUIEventAdapter::KEY_KP_Page_Down;
        mKeyMap[Qt::Key_Insert        ] = osgGA::GUIEventAdapter::KEY_KP_Insert;
        //mKeyMap[Qt::Key_Delete        ] = osgGA::GUIEventAdapter::KEY_KP_Delete;
    }

    virtual ~QtKeyboardMap5()
    {
    }

    int remapKey(QKeyEvent* event)
    {
        KeyMap::iterator itr = mKeyMap.find(event->key());

        if (itr == mKeyMap.end())
            return int(*(event->text().toLocal8Bit().data()));
        else
            return itr.value();
    }

private:
    typedef QHash<unsigned int, int> KeyMap;
    KeyMap mKeyMap;
};

static QtKeyboardMap5 s_QtKeyboardMap;

class FBWindow : public osgViewer::GraphicsWindowEmbedded
{
public:
    explicit FBWindow(GLWidget *renderer)
        : osgViewer::GraphicsWindowEmbedded(0, 0, 0, 0)
        , _renderer(renderer)
    {
        getState()->setUseModelViewAndProjectionUniforms(true);
        getState()->setUseVertexAttributeAliasing(true);
        getState()->setModeValidity(GL_NORMALIZE, false);
        getState()->setModeValidity(GL_RESCALE_NORMAL, false);
        getState()->setModeValidity(GL_TEXTURE_2D, false);
    }

    virtual bool makeCurrentImplementation() Q_DECL_OVERRIDE
    {
        _renderer->makeCurrent();
        return true;
    }

    virtual bool releaseContextImplementation() Q_DECL_OVERRIDE
    {
        _renderer->doneCurrent();
        return true;
    }

private:
    GLWidget *_renderer;
};



class GLWidget::Private
{
public:
    QTimer timer;
    osg::ref_ptr<osgViewer::Viewer> viewer;
    osg::observer_ptr<osgViewer::GraphicsWindow> window;
    osg::ref_ptr<osg::Node> root;
    int dpr;
};



GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , p(new Private())
{
    p->dpr = devicePixelRatio();

    p->viewer = new osgViewer::Viewer;
    p->viewer->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
    p->viewer->setSceneData(p->root);
    p->viewer->setCameraManipulator(new osgGA::OrbitManipulator);
    p->viewer->addEventHandler(new osgViewer::StatsHandler);

    p->window = new FBWindow(this);

    osg::Camera &camera = *p->viewer->getCamera();
    camera.setGraphicsContext(p->window.get());

    connect(&p->timer, SIGNAL(timeout()), this, SLOT(update()));
    p->timer.start(10);
}

void GLWidget::setNode(osg::Node *node)
{
    p->root = node;
    p->viewer->setSceneData(p->root);
    p->viewer->home();
}

void GLWidget::home() const
{
    p->viewer->home();
}

GLWidget::~GLWidget() = default;

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(1,0,0,1);
}

void GLWidget::resizeGL(int w, int h)
{
    p->window->resized(x(), y(), w * p->dpr, h * p->dpr);
    p->viewer->getEventQueue()->windowResize( x(), y(), w * p->dpr, h * p->dpr);

    osg::Camera &camera = *p->viewer->getCamera();
    camera.setViewport(new osg::Viewport(0, 0, w * p->dpr, h*p->dpr));
    const double aspectRatio = static_cast<double>(w*p->dpr)/static_cast<double>(h*p->dpr);
    camera.setProjectionMatrixAsPerspective(30.0, aspectRatio, 1.0, 10000.0);
}

void GLWidget::paintGL()
{
    if (p->viewer.valid())
    {
        p->viewer->frame();
    }
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    int value = s_QtKeyboardMap.remapKey( event );
    p->viewer->getEventQueue()->keyPress( value );
    update();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    int button = 0;
    switch ( event->button() )
    {
        case Qt::LeftButton: button = 1; break;
        case Qt::MidButton: button = 2; break;
        case Qt::RightButton: button = 3; break;
        case Qt::NoButton: button = 0; break;
        default: button = 0; break;
    }
    //setKeyboardModifiers( event );
    p->viewer->getEventQueue()->mouseButtonPress( event->x() * p->dpr, event->y() * p->dpr, button);
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    int button = 0;
    switch ( event->button() )
    {
        case Qt::LeftButton: button = 1; break;
        case Qt::MidButton: button = 2; break;
        case Qt::RightButton: button = 3; break;
        case Qt::NoButton: button = 0; break;
        default: button = 0; break;
    }
    //setKeyboardModifiers( event );
    p->viewer->getEventQueue()->mouseButtonRelease(event->x() * p->dpr, event->y() * p->dpr, button);
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    p->viewer->getEventQueue()->mouseMotion( event->x() * p->dpr, event->y() * p->dpr );
}

