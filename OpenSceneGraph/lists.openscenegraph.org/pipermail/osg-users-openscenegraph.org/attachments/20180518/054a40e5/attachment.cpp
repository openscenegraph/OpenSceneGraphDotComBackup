#include <osg/Texture2D>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileNameUtils>

#include <osgViewer/GraphicsWindow>

USE_GRAPHICSWINDOW()

int main(int argc, char** argv)
{
    osg::ArgumentParser arguments(&argc, argv);

    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;

    traits->x = 0;
    traits->y = 0;
    traits->width = 640;
    traits->height = 480;
    traits->pbuffer = true;
    traits->sharedContext = 0;

    //osgViewer::Viewer viewer;

    // we have to make the graphics context current to do any OpenGL calls
    osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
    if (!gc)
    {
        OSG_NOTICE<<"Warning: unable to create graphics context"<<std::endl;
        return 1;
    }

    // realize and make the OpenGL context current so we can do OpenGL work
    gc->realize();
    gc->makeCurrent();

    std::string readFileName = "Images/lz.rgb";

    while(arguments.read("-i", readFileName)) {}

    osg::ref_ptr<osg::Image> image = osgDB::readImageFile(readFileName);
    if (!image)
    {
        OSG_NOTICE<<"Could not load image : "<<readFileName<<std::endl;
        return 1;
    }

    std::string writeFileName = osgDB::getStrippedName(readFileName)+".dds";
    while(arguments.read("-o", writeFileName)) {}

    OSG_NOTICE<<"Read image "<<readFileName<<std::endl;

    // if valid
    osg::ref_ptr <osg::State> state = gc->getState();

    osg::ref_ptr <osg::Texture2D> texture = new osg::Texture2D;

    texture->setInternalFormatMode (osg::Texture::USE_S3TC_DXT5_COMPRESSION);

    texture->setImage(image.get());

    texture->apply(*state.get());

    image->readImageFromCurrentTexture(0, true);

    OSG_NOTICE<<"Compressed image, writing out to : "<<writeFileName<<std::endl;

    osgDB::writeImageFile(*image, writeFileName);

    return 0;
}
