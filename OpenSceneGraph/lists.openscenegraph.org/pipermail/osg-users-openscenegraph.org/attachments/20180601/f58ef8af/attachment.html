#include <osg/Geode>
#include <osg/Geometry>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

osg::Node* createTriangle(float x, bool earlyBinding, const osg::Vec4f& color)
{
    osg::Geode* geode = new osg::Geode;
    osg::Geometry* geom = new osg::Geometry;
    geom->setUseVertexArrayObject(true);
    geom->setUseVertexBufferObjects(true);

    osg::Vec3Array* vertices = new osg::Vec3Array();
    vertices->push_back(osg::Vec3(x - 100, 0, -100));
    vertices->push_back(osg::Vec3(x, 0, 100));
    vertices->push_back(osg::Vec3(x, 0, -100));

    osg::Vec3Array* normals = new osg::Vec3Array;
    normals->push_back(osg::Vec3(0, -1, 0));
    normals->push_back(osg::Vec3(0, -1, 0));
    normals->push_back(osg::Vec3(0, -1, 0));
    if (earlyBinding)
        normals->setBinding(osg::Array::BIND_PER_VERTEX);

    osg::Vec4Array* colors = new osg::Vec4Array(osg::Array::BIND_OVERALL);
    colors->push_back(color);

    geom->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES, 0, 3));
    geom->setVertexArray(vertices);
    geom->setNormalArray(normals);
    geom->setColorArray(colors);
    geode->addDrawable(geom);

    // Warning gets generated due to this line not causing (eventually) a VBO creation
    if (!earlyBinding)
        normals->setBinding(osg::Array::BIND_PER_VERTEX);
    return geode;
}

osg::Node* createScene()
{
    osg::Group* group = new osg::Group;

    // Reddish: Generates warning
    group->addChild(createTriangle(-100, false, osg::Vec4f(1, 0.5, 0.5, 1)));

    // Greenish: No warnings
    group->addChild(createTriangle(100, true, osg::Vec4f(0.5,1,0.5,1)));

    return group;
}

int main(int argc, char** argv)
{
    osg::ArgumentParser arguments(&argc, argv);

    // construct the viewer.
    osgViewer::Viewer viewer(arguments);
    viewer.setSceneData(createScene());
    viewer.setUpViewInWindow(100, 100, 800, 600);
    viewer.addEventHandler(new osgViewer::StatsHandler());

    return viewer.run();
}
