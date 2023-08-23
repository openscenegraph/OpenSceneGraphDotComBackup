#include <osg/AutoTransform>

#include <osgText/Text>

#include <osgGA/TrackballManipulator>

#include <osgViewer/ViewerEventHandlers>

#include <osgViewer/CompositeViewer>

class AddViewOperation : public osg::Operation
{
public:
    AddViewOperation(osg::ref_ptr<osgViewer::View> view)
        : osg::Operation("AddView", false)
        , _view(view)
    {}

    void operator() (osg::Object * compositeViewer) override
    {
        OSG_NOTICE << "AddView operator()" << std::endl;
        osgViewer::CompositeViewer * viewer = dynamic_cast<osgViewer::CompositeViewer *>(compositeViewer);
        viewer->stopThreading();
        viewer->addView(_view);
        viewer->startThreading();
    }

protected:
    osg::ref_ptr<osgViewer::View> _view;
};

class RemoveViewOperation : public osg::Operation
{
public:
    RemoveViewOperation(osg::ref_ptr<osgViewer::View> view)
        : osg::Operation("RemoveView", false)
        , _view(view)
    {
        OSG_NOTICE<<"RemoveViewOperation::RemoveViewOperation()"<<std::endl;
    }

    ~RemoveViewOperation()
    {
        OSG_NOTICE<<"RemoveViewOperation::~RemoveViewOperation()"<<std::endl;
    }

    void operator() (osg::Object * compositeViewer) override
    {
        OSG_NOTICE << "RemoveView operator()" << std::endl;
        osgViewer::CompositeViewer * viewer = dynamic_cast<osgViewer::CompositeViewer *>(compositeViewer);
        viewer->stopThreading();
        viewer->removeView(_view);
        viewer->startThreading();
    }

protected:
    osg::ref_ptr<osgViewer::View> _view;
};

class ViewAdder : public osgGA::GUIEventHandler
{
public:
    ViewAdder(osgViewer::CompositeViewer * viewer)
        : _viewer(viewer)
        , _view(nullptr)
    {}

    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
    {
        if (ea.getEventType() == osgGA::GUIEventAdapter::KEYUP && (ea.getKey() == 'v' || ea.getKey() == 'V'))
        {
            if (_view)
            {
                OSG_NOTICE << "Existing view, remove it" << std::endl;
                // parts of the scene get removed before the view gets destroyed.
                // normally this is fine as things get handled by destructors.
                // however, things that are still cached require the cache to be released
                _view->setSceneData(nullptr);
                // We need to remove the view after the event traversal is done to avoid invalidating iterators
                _viewer->addUpdateOperation(new RemoveViewOperation(_view));
                _view = nullptr;
            }
            else
            {
                OSG_NOTICE << "No existing view, create one" << std::endl;
                _view = new osgViewer::View;
                _view->setName("View two");

                _view->setUpViewInWindow(800, 200, 1024, 768);


                osg::ref_ptr<osgText::Text> text2 = new osgText::Text();
                text2->setFont("times.ttf");
                text2->setText("Here's some other text. It appears in the dynamically-added view. It ensures the default font gets used with a context that goes away, and that lives in the cache.");
                osg::ref_ptr<osg::AutoTransform> autoTransform2 = new osg::AutoTransform();
                autoTransform2->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
                autoTransform2->addChild(text2);
                osg::ref_ptr<osg::Node> scene2 = autoTransform2;

                _view->setSceneData(scene2.get());
                _view->setCameraManipulator(new osgGA::TrackballManipulator);

                _viewer->addUpdateOperation(new AddViewOperation(_view));
            }
            return true;
        }
        return false;
    }

protected:
    // raw pointer because it's stack-allocated
    osgViewer::CompositeViewer * _viewer;
    osg::ref_ptr<osgViewer::View> _view;
};

int main(int argc, char **argv)
{

    // use an ArgumentParser object to manage the program arguments.
    osg::ArgumentParser arguments(&argc, argv);

    osg::ref_ptr<osgText::Text> text = new osgText::Text();
    text->setText("Here's some text. It doesn't have to be text, and in fact we don't need anything in the main view as it just exists to keep the application alive.");
    osg::ref_ptr<osg::AutoTransform> autoTransform = new osg::AutoTransform();
    autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
    autoTransform->addChild(text);
    osg::ref_ptr<osg::Node> scene = autoTransform;

    if (!scene)
    {
        OSG_NOTICE << argv[0] << ": requires filename argument." << std::endl;
        return 1;
    }

    // construct the viewer.
    osgViewer::CompositeViewer viewer(arguments);

    // single-threaded to prove a threading bug isn't responsible for the symptoms
    viewer.setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);

    // view one
    {
        osg::ref_ptr<osgViewer::View> view = new osgViewer::View;
        view->setName("View one");
        viewer.addView(view);

        view->setUpViewInWindow(0, 0, 1024, 768);
        view->setSceneData(scene.get());
        view->setCameraManipulator(new osgGA::TrackballManipulator);

        view->addEventHandler(new ViewAdder(&viewer));
    }

    // run the viewer's main frame loop
    return viewer.run();
}
