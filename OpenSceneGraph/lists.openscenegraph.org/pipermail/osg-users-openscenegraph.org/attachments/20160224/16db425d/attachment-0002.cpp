/* OpenSceneGraph example, osgpackeddepthstencil.
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

#include <osg/GLExtensions>
#include <osg/Depth>
#include <osg/Node>
#include <osg/Geometry>
#include <osg/Notify>
#include <osg/Texture3D>
#include <osg/Texture2D>
#include <osg/Stencil>
#include <osg/ColorMask>
#include <osg/Geode>
#include <osg/FrameBufferObject>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <iostream>
char classic_instancing_vs[] =

    "varying out int instance;\n "
    "\n"\
    "void main(void)\n"\
    "{\n"\
    "instance= gl_InstanceID; \n"
    " gl_Position = gl_ModelViewProjectionMatrix *gl_Vertex; \n"
    "}\n";
char instancing_to3Dgs[] = //"#version 330 core\n"\
    "layout(triangles) in;\n"\
    "layout(triangle_strip, max_vertices =3) out;\n"\
    "\n"\
    "out float slicez;\n"\

    "#extension GL_EXT_geometry_shader4 : enable\n"\
    "\n"\
    "\n"\
    "uniform vec3 rdx;\n"\
    "varying in int instance[3];\n"\
    "\n"\
    "void main(void){\n"\

    "float slicez=(float(instance[0])+0.5)*rdx.z;\n"

    "vec2 minmaxz=vec2(gl_PositionIn[0].z,gl_PositionIn[0].z);\n"
    "minmaxz.x=min(minmaxz.x,gl_PositionIn[1].z);\n"
    "minmaxz.x=min(minmaxz.x,gl_PositionIn[2].z);\n"
    "minmaxz.y=max(minmaxz.y,gl_PositionIn[1].z);\n"
    "minmaxz.y=max(minmaxz.y,gl_PositionIn[2].z);\n"
    "minmaxz*=0.5;minmaxz+=vec2(0.5);\n"
    "if(/*slicez>=minmaxz.x && slicez<minmaxz.y&&*/true){\n"

    "gl_Layer=instance[0];\n"\
    "gl_Position = vec4(gl_PositionIn[0].xyz,1);\n"\
    "EmitVertex();\n"\


    "gl_Layer=instance[0];\n"\
    "gl_Position = vec4(gl_PositionIn[1].xyz,1);\n"\
    "EmitVertex();\n"\


    "gl_Layer=instance[0];\n"\
    "gl_Position =  vec4(gl_PositionIn[2].xyz,1);\n"\
    "EmitVertex();\n"\
    "EndPrimitive();\n"
    "}\n"
    "}\n";


char whitefs[]=
    "void main(){;\n"
    "gl_FragColor=vec4(1,1,1,1);\n"
    "};\n";


osg::Geode* createMask()
{
    osg::Vec3Array *vertices = new osg::Vec3Array;
    vertices->push_back(osg::Vec3(-0.5, -0.5, 0.0));
    vertices->push_back(osg::Vec3(0.5, -0.5, 0.0));
    vertices->push_back(osg::Vec3(0.5, 0.5, 0.0));
    vertices->push_back(osg::Vec3(-0.5, 0.5, 0.0));

    osg::Geometry *geom = new osg::Geometry;

       geom->setUseDisplayList(false);
    geom->setUseVertexBufferObjects(true);

    geom->setVertexArray(vertices);
    geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4,128));



    osg::Geode *geode = new osg::Geode;
    geode->addDrawable(geom);

     osg::Stencil* stencil = new osg::Stencil;
       stencil->setFunction(osg::Stencil::ALWAYS, 1, ~0u);
	    stencil->setOperation(osg::Stencil::KEEP, osg::Stencil::KEEP, osg::Stencil::REPLACE);

    stencil->setFunction(osg::Stencil::ALWAYS, 1, ~0u);
    stencil->setOperation(osg::Stencil::REPLACE, osg::Stencil::REPLACE, osg::Stencil::REPLACE);

    osg::StateSet *ss = geode->getOrCreateStateSet();
    ss->setAttributeAndModes(stencil,                             osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    ss->setAttribute(new osg::ColorMask(false, false, false, false),
                     osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

    osg::ref_ptr<osg::Depth> depth  = new osg::Depth;
    depth->setWriteMask(false);
    depth->setFunction(osg::Depth::ALWAYS);
    ss->setAttributeAndModes(depth, osg::StateAttribute::ON);


    osg::ref_ptr<osg::Program> prog=new osg::Program;
    osg::ref_ptr<osg::Shader> fs=new osg::Shader;
    osg::ref_ptr<osg::Shader> gs=new osg::Shader;
    osg::ref_ptr<osg::Shader> vs=new osg::Shader;

    vs->setType(osg::Shader::VERTEX);
    vs->setShaderSource(classic_instancing_vs);
    prog->addShader(vs);

    gs->setType(osg::Shader::GEOMETRY);
    gs->setShaderSource(instancing_to3Dgs);
    prog->addShader(gs);

    fs->setType(osg::Shader::FRAGMENT);
    fs->setShaderSource(whitefs);
    prog->addShader(fs);

    prog->setParameter(GL_GEOMETRY_VERTICES_OUT_EXT, 3);
    prog->setParameter(GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES);
    prog->setParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP);
    ss->setAttributeAndModes( prog, osg::StateAttribute::ON);

 osg::ref_ptr<osg::Uniform> rdx=new osg::Uniform();

    rdx->setName("rdx");
    rdx->setType(osg::Uniform::FLOAT_VEC3);
    rdx->set(osg::Vec3f(1.0/128.0,1.0/128.0,1.0/128.0));

    ss->addUniform(rdx);

    return geode;
}

osg::Geode* createGeometry()
{
    osg::Vec3Array* vertices = new osg::Vec3Array;
    vertices->push_back(osg::Vec3(-1.0, -1.0, 0.0));
    vertices->push_back(osg::Vec3(1.0, -1.0, 0.0));
    vertices->push_back(osg::Vec3(1.0, 1.0, 0.0));
    vertices->push_back(osg::Vec3(-1.0, 1.0, 0.0));

    osg::Geometry* geom = new osg::Geometry;

    geom->setUseDisplayList(false);
    geom->setUseVertexBufferObjects(true);

    geom->setVertexArray(vertices);
    geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4,128));
    geom->getOrCreateVertexBufferObject();
    osg::Geode* geode = new osg::Geode;
    geode->addDrawable(geom);

    osg::ref_ptr<osg::Stencil> stencil = new osg::Stencil;
   //

    stencil->setFunction(osg::Stencil::NOTEQUAL, 1, ~0u);

  //  stencil->setFunction(osg::Stencil::NOTEQUAL, 0, ~0u);
    //stencil->setFunction(osg::Stencil::NEVER, 1, ~0u);
    stencil->setOperation(osg::Stencil::KEEP, osg::Stencil::KEEP, osg::Stencil::KEEP);

    osg::StateSet *ss = geode->getOrCreateStateSet();
    ss->setAttributeAndModes(stencil,        osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

    osg::ref_ptr<osg::Depth> depth = new osg::Depth;
    depth->setWriteMask(false);
    depth->setFunction(osg::Depth::NEVER);
   // ss->setAttributeAndModes(depth, osg::StateAttribute::ON);



    osg::ref_ptr<osg::Program> prog=new osg::Program;
    osg::ref_ptr<osg::Shader> fs=new osg::Shader;
    osg::ref_ptr<osg::Shader> gs=new osg::Shader;
    osg::ref_ptr<osg::Shader> vs=new osg::Shader;


    vs->setType(osg::Shader::VERTEX);
    vs->setShaderSource(classic_instancing_vs);
    prog->addShader(vs);

    gs->setType(osg::Shader::GEOMETRY);
    gs->setShaderSource(instancing_to3Dgs);
    prog->addShader(gs);

    fs->setType(osg::Shader::FRAGMENT);
    fs->setShaderSource(whitefs);
    prog->addShader(fs);

    prog->setParameter(GL_GEOMETRY_VERTICES_OUT_EXT, 3);
    prog->setParameter(GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES);
    prog->setParameter(GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP);

    ss->setAttributeAndModes( prog, osg::StateAttribute::ON);

    osg::ref_ptr<osg::Uniform> rdx=new osg::Uniform();
    rdx->setName("rdx");
    rdx->setType(osg::Uniform::FLOAT_VEC3);
    rdx->set(osg::Vec3(1.0/128.0,1.0/128.0,1.0/128.0));

    ss->addUniform(rdx);
    return geode;
}

osg::Geode* createTextureQuad(osg::Texture3D *texture)
{
    osg::Vec3Array *vertices = new osg::Vec3Array;
    vertices->push_back(osg::Vec3(-0.8, 0.0, -0.8));
    vertices->push_back(osg::Vec3(0.8, 0.0, -0.8));
    vertices->push_back(osg::Vec3(0.8, 0.0, 0.8));
    vertices->push_back(osg::Vec3(-0.8, 0.0, 0.8));

    osg::Vec2Array *texcoord = new osg::Vec2Array;
    texcoord->push_back(osg::Vec2(0.0, 0.0));
    texcoord->push_back(osg::Vec2(1.0, 0.0));
    texcoord->push_back(osg::Vec2(1.0, 1.0));
    texcoord->push_back(osg::Vec2(0.0, 1.0));

    osg::Geometry *geom = new osg::Geometry;
    geom->setUseDisplayList(false);
    geom->setUseVertexBufferObjects(true);
    geom->setVertexArray(vertices);
    geom->setTexCoordArray(0, texcoord);
    geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));

    osg::Geode *geode = new osg::Geode;
    geode->addDrawable(geom);
    geode->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);

    osg::ref_ptr<osg::Program> prog=new osg::Program;
    osg::ref_ptr<osg::Shader> fs=new osg::Shader;

    fs->setType(osg::Shader::FRAGMENT);

    char source[]="uniform sampler3D stencil;\n"
                  "void main(){;\n"
                  "gl_FragColor=texture3D(stencil,vec3(gl_TexCoord[0].st,0));\n"
                  "};\n";
    fs->setShaderSource(source);

    prog->addShader(fs);

    geode->getOrCreateStateSet()->setAttributeAndModes( prog, osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);

    return geode;
}


int main( int argc, char **argv )
{
    // use an ArgumentParser object to manage the program arguments.
    osg::ArgumentParser arguments(&argc,argv);

    arguments.getApplicationUsage()->addCommandLineOption("--fbo","Use Frame Buffer Object for render to texture, where supported.");
    arguments.getApplicationUsage()->addCommandLineOption("--pbuffer-rtt","Use Pixel Buffer for render to texture, where supported.");
    arguments.getApplicationUsage()->addCommandLineOption("--nopds", "Don't use packed depth stencil.");
    arguments.getApplicationUsage()->addCommandLineOption("--fbo-samples","");
    arguments.getApplicationUsage()->addCommandLineOption("--color-samples", "");

    // construct the viewer.
    osgViewer::Viewer viewer(arguments);

    // add stats
    viewer.addEventHandler( new osgViewer::StatsHandler() );

    // if user request help write it out to cout.
    if (arguments.read("-h") || arguments.read("--help"))
    {
        arguments.getApplicationUsage()->write(std::cout);
        return 1;
    }

    osg::Camera::RenderTargetImplementation renderImplementation = osg::Camera::FRAME_BUFFER_OBJECT;
    int colorSamples = 0, samples = 0;
    bool usePDS = true;

    while (arguments.read("--fbo"))
    {
        renderImplementation = osg::Camera::FRAME_BUFFER_OBJECT;
    }
    while (arguments.read("--pbuffer-rtt"))
    {
        renderImplementation = osg::Camera::PIXEL_BUFFER_RTT;
    }
    while (arguments.read("--nopds"))
    {
        usePDS = false;
    }
    while (arguments.read("--fbo-samples", samples)) {}
    while (arguments.read("--color-samples", colorSamples)) {}


    osg::Group* rootNode = new osg::Group;
    rootNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF|osg::StateAttribute::OVERRIDE);

    // creates texture to be rendered
    osg::Texture3D *texture = new osg::Texture3D;
    osg::ref_ptr<   osg::Texture3D >textureDS;

    texture->setTextureSize(128, 128,128);
    texture->setInternalFormat(GL_RGBA);
    texture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
    texture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
    texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    texture->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
    texture->setBorderColor(osg::Vec4(0, 0, 0, 0));

    // creates rtt camera
    osg::ref_ptr<osg::Camera> rttCamera = new osg::Camera;
    rttCamera->setClearMask(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    rttCamera->setClearColor(osg::Vec4(0.0, 0.4, 0.5, 0.0));
    rttCamera->setClearStencil(0);
    rttCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    rttCamera->setViewMatrix(osg::Matrixd::identity());

    rttCamera->setViewport(0, 0, 128, 128);
    rttCamera->setRenderOrder(osg::Camera::PRE_RENDER);
    rttCamera->setRenderTargetImplementation(renderImplementation);
    rttCamera-> setComputeNearFarMode(osg::Camera::DO_NOT_COMPUTE_NEAR_FAR);

    // set the camera to render before the main camera.
    rttCamera->setRenderOrder(osg::Camera::PRE_RENDER);
    // tell the camera to use OpenGL frame buffer object where supported.
    rttCamera-> setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);


    rttCamera->setProjectionResizePolicy(osg::Camera::FIXED);
    rttCamera-> setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    rttCamera-> setViewMatrix(osg::Matrix::identity());


    rttCamera-> setProjectionMatrix(osg::Matrix::ortho2D(-1, 1, -1, 1));
    if( usePDS)
    {

     //     rttCamera->attach(osg::Camera::PACKED_DEPTH_STENCIL_BUFFER, GL_DEPTH_STENCIL_EXT);

        osg::ref_ptr<   osg::Image > im =new osg::Image();
        //  im->allocateImage(128, 128, 1, GL_DEPTH_STENCIL_EXT, GL_UNSIGNED_INT_24_8_EXT, 1);
        textureDS = new osg::Texture3D( );
        textureDS->setTextureSize(128, 128,128);
        textureDS->setInternalFormat(GL_DEPTH_STENCIL_EXT);

          /* rttCamera->detach( osg::Camera::DEPTH_BUFFER );
           rttCamera->detach( osg::Camera::STENCIL_BUFFER );
           rttCamera->attach(osg::Camera::PACKED_DEPTH_STENCIL_BUFFER, textureDS,0, osg::Camera::FACE_CONTROLLED_BY_GEOMETRY_SHADER, false);*/
    }
    else
    {
        // this doesn't work on NVIDIA/Vista 64bit
        // FBO status = 0x8cd6 (FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT)
        rttCamera->attach(osg::Camera::DEPTH_BUFFER, GL_DEPTH_COMPONENT);
        rttCamera->attach(osg::Camera::STENCIL_BUFFER, GL_STENCIL_INDEX8_EXT);
    }

    rttCamera->attach(osg::Camera::COLOR_BUFFER, texture,0, osg::Camera::FACE_CONTROLLED_BY_GEOMETRY_SHADER, false);// 0, 0, false, samples, colorSamples);
    rttCamera->setCullingMode(osg::Camera::DO_NOT_COMPUTE_NEAR_FAR);
    //rttCamera->setImplicitBufferAttachmentMask(osg::Camera::IMPLICIT_COLOR_BUFFER_ATTACHMENT, osg::Camera::IMPLICIT_COLOR_BUFFER_ATTACHMENT);
    rttCamera->setImplicitBufferAttachmentMask(
        osg::Camera::IMPLICIT_COLOR_BUFFER_ATTACHMENT//| osg::Camera::IMPLICIT_DEPTH_BUFFER_ATTACHMENT| osg::Camera::IMPLICIT_STENCIL_BUFFER_ATTACHMENT
        ,     osg::Camera::USE_DISPLAY_SETTINGS_MASK);//osg::Camera::IMPLICIT_COLOR_BUFFER_ATTACHMENT  | osg::Camera::IMPLICIT_DEPTH_BUFFER_ATTACHMENT);*/
    // creates rtt subtree
    osg::Group* g0 = new osg::Group;
    g0->addChild(createMask());
    g0->addChild(createGeometry());
    rttCamera->addChild(g0);
    rootNode->addChild(rttCamera.get());

    // creates textured quad with result
    rootNode->addChild(createTextureQuad(texture));

    // add model to the viewer.
    viewer.setSceneData( rootNode );

    return viewer.run();
}