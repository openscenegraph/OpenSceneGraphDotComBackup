#include <osgViewer/Viewer>
#include <osg/Texture2DArray>
#include <osgDB/ReadFile>
#include <osg/Geometry>
#include <osg/Uniform>
#include <iostream>

const char* vert =
"#version 330 compatibility\n"
"out vec4 coord;\n"
"void main() {\n"
"    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
"    coord = gl_MultiTexCoord0;\n"
"}\n";

const char* frag =
"#version 330 compatibility\n"
"in vec4 coord;\n"
"out vec4 color;\n"
"uniform sampler2DArray tex;\n"
"void main() {\n"
"    color = texture(tex, vec3(coord.st,0));\n"
"}\n";

int
main(int argc, char** argv)
{
    osg::ArgumentParser arguments(&argc,argv);

    // create a viewer:
    osgViewer::Viewer viewer(arguments);

    if (argc<=1) return 1;

    osg::Image* image = osgDB::readImageFile(argv[1]);

    osg::Texture2DArray* tex = new osg::Texture2DArray();
    tex->setTextureSize(image->s(), image->t(), 1);
    tex->setFilter(tex->MIN_FILTER, tex->LINEAR_MIPMAP_LINEAR);
    tex->setFilter(tex->MAG_FILTER, tex->LINEAR);
    tex->setImage(0, image);

    osg::Geometry* geom = new osg::Geometry();

    osg::Vec3Array* verts = new osg::Vec3Array();
    verts->push_back(osg::Vec3(-1,-1,0));
    verts->push_back(osg::Vec3( 1,-1,0));
    verts->push_back(osg::Vec3( 1, 1,0));
    verts->push_back(osg::Vec3(-1, 1,0));
    geom->setVertexArray(verts);

    osg::Vec2Array* tc = new osg::Vec2Array();
    tc->push_back(osg::Vec2(0,0));
    tc->push_back(osg::Vec2(1,0));
    tc->push_back(osg::Vec2(1,1));
    tc->push_back(osg::Vec2(0,1));
    geom->setTexCoordArray(0, tc);

    geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));

    osg::StateSet* s = geom->getOrCreateStateSet();

    if (image->isImageTranslucent())
    {
        s->setMode(GL_BLEND, osg::StateAttribute::ON);
    }

    s->setTextureAttribute(0, tex);
    s->addUniform(new osg::Uniform("tex",0));




    osg::Shader* VS = new osg::Shader(osg::Shader::VERTEX, vert);
    osg::Shader* FS = new osg::Shader(osg::Shader::FRAGMENT, frag);
    osg::Program* program = new osg::Program();
    program->addShader(VS);
    program->addShader(FS);
    s->setAttribute(program);

    viewer.setSceneData(geom);
    return viewer.run();
}
