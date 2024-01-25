#include <osg/Group>
#include <osg/Geode>
#include <osg/Texture2D>
#include <osgViewer/Viewer>

osg::ref_ptr< osg::Group > BuildRTTQuad( osg::Vec4 clearColor, float posx = 0.f, float posy = 0.f, float size = 0.3f, int texture_size = 1024)
{
    osg::ref_ptr< osg::Group > group = new osg::Group;

    osg::ref_ptr< osg::Texture2D > texture = new osg::Texture2D;
    texture->setTextureSize(texture_size, texture_size);
    texture->setInternalFormat(GL_RGBA);

    osg::ref_ptr< osg::Camera > camera = new osg::Camera;
    camera->setClearColor(clearColor);
    camera->setViewport(0, 0, texture_size, texture_size);

    // Interesting observation: removing next line "makes" the view correct 
    // probably attching texture to FBO before drawing with it somehow affects the problem
    camera->setRenderOrder(osg::Camera::PRE_RENDER);

    camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
    camera->attach(osg::Camera::COLOR_BUFFER, texture);

    group->addChild(camera);

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable(osg::createTexturedQuadGeometry(osg::Vec3(posx, posy, 0), osg::Vec3(size, 0, 0), osg::Vec3(0, size, 0)));
    geode->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);

    group->addChild(geode);
    return group;
}

// Run without setting OSG_TEXTURE_POOL_SIZE to see how it should look
// ( correct view shows red, green, blue quad on typical osg background )
// 
// To see the bug in action: run with OSG_TEXTURE_POOL_SIZE = 10000000 
// or set the limit in code with this line 
// osg::DisplaySettings::instance()->setMaxTexturePoolSize(10000000);
// ( incorrect view on my machine shows white, red, green quad
//   but I believe your incorrects results may vary because I cannot explain 
//   where white quad comes from. It may be some other undefined color... )

int main(int argc, char** argv)
{
    osgViewer::Viewer viewer( osg::ArgumentParser(&argc, argv) );

    // osg::DisplaySettings::instance()->setMaxTexturePoolSize(10000000);

    osg::ref_ptr< osg::Group > scene = new osg::Group();
    viewer.setSceneData(scene);

    viewer.getCamera()->setProjectionMatrix(osg::Matrixd::ortho2D(0, 1, 0, 1));
    viewer.getCamera()->setViewMatrix(osg::Matrixd::identity());

    for (int frame = 0; !viewer.done(); frame++)
    {
        // Single quad is added per frame in first 3 frames
        if (frame == 0)
            scene->addChild(BuildRTTQuad(osg::Vec4(1, 0, 0, 1), 0.00, 0.33, 0.33));

        if (frame == 1)
            scene->addChild(BuildRTTQuad(osg::Vec4(0, 1, 0, 1), 0.33, 0.33, 0.33));

        if (frame == 2)
            scene->addChild(BuildRTTQuad(osg::Vec4(0, 0, 1, 1), 0.66, 0.33, 0.33));

        viewer.frame();
    }
}