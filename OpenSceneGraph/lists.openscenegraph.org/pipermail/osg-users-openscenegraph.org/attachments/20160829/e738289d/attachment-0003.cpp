/* OpenSceneGraph example, osgcallback.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*  THE SOFTWARE.
*/

#include <osgViewer/Viewer>

#include <osg/Notify>
#include <osg/ShapeDrawable>

#include <osgDB/Registry>
#include <osgDB/ReadFile>

#include <osgGA/TrackballManipulator>

class TwiceEventCallback : public osgGA::GUIEventHandler
{
public:
   virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor* nv) 
   { 
      if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH)
      {
         OSG_ALWAYS << "[Twice] Mouse PUSH on frame: " << nv->getFrameStamp()->getFrameNumber() << std::endl;
      }

      return false;
   }
};


class CrashEventCallback : public osg::Callback
{
   virtual bool run(osg::Object* object, osg::Object* data)
   {
      osgGA::EventVisitor* ev = data->asNodeVisitor()->asEventVisitor();

      if (ev)
      {
         for (osgGA::EventQueue::Events::iterator itr = ev->getEvents().begin();
            itr != ev->getEvents().end();
            ++itr)
         {
            osgGA::GUIEventAdapter* ea = (*itr)->asGUIEventAdapter();
            if (ea->getEventType() == osgGA::GUIEventAdapter::PUSH)
            {
               OSG_ALWAYS << "[Crash] Mouse PUSH on frame: " << ev->getFrameStamp()->getFrameNumber() << std::endl;
            }
         }
      }

      return traverse(object, data);
   }
};



class FixEventCallback : public osg::NodeCallback
{
   // implementing this method does NOT work, never called by the event visitor
   // this behaviour is not very intuitive
   virtual bool run(osg::Object* object, osg::Object* data)
   {
      osgGA::EventVisitor* ev = data->asNodeVisitor()->asEventVisitor();

      if (ev)
      {
         for (osgGA::EventQueue::Events::iterator itr = ev->getEvents().begin();
            itr != ev->getEvents().end();
            ++itr)
         {
            osgGA::GUIEventAdapter* ea = (*itr)->asGUIEventAdapter();
            if (ea->getEventType() == osgGA::GUIEventAdapter::PUSH)
            {
               OSG_ALWAYS << "[Fix run()] Mouse PUSH on frame: " << ev->getFrameStamp()->getFrameNumber() << std::endl;
            }
         }
      }

      return traverse(object, data);
   }

   // this one is ok and actually fixes, but method signature returns void
   // so probably the event is not considered handled?? not sure...
   virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
   {
      osgGA::EventVisitor* ev = nv->asEventVisitor();

      if (ev)
      {
         for (osgGA::EventQueue::Events::iterator itr = ev->getEvents().begin();
            itr != ev->getEvents().end();
            ++itr)
         {
            osgGA::GUIEventAdapter* ea = (*itr)->asGUIEventAdapter();
            if (ea->getEventType() == osgGA::GUIEventAdapter::PUSH)
            {
               OSG_ALWAYS << "[Fix operator()] Mouse PUSH on frame: " << ev->getFrameStamp()->getFrameNumber() << std::endl;
            }
         }
      }

      traverse(node, nv);
   }
};


int main( int argc, char **argv )
{
    // use an ArgumentParser object to manage the program arguments.
    osg::ArgumentParser arguments(&argc,argv);

    // initialize the viewer.
    osgViewer::Viewer viewer(arguments);

    osg::Drawable* sphere = new osg::ShapeDrawable(new osg::Sphere());

    if (arguments.read("--crash"))
    {
       sphere->setEventCallback(new CrashEventCallback);
    }
    else if (arguments.read("--fix"))
    {
       sphere->setEventCallback(new FixEventCallback);
    }
    else
    {
       sphere->setEventCallback(new TwiceEventCallback);
    }
    

    // set the scene to render
    viewer.setSceneData(sphere);

    viewer.setCameraManipulator(new osgGA::TrackballManipulator);

    viewer.realize();

    viewer.run();    

    return 0;
}