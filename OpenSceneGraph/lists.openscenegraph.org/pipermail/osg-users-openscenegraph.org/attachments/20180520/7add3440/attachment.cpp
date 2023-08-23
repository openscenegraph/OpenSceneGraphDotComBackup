#include <osgViewer/Viewer>
#include <osgText/Text>

int main(int argc, char** argv)
{
    osg::ArgumentParser arguments(&argc,argv);
    osgViewer::Viewer viewer(arguments);

    osgText::Text* text = new osgText::Text();
    text->setText("Hello, world.");
    text->setCharacterSizeMode(osgText::Text::SCREEN_COORDS);
    text->setAlignment(osgText::Text::CENTER_CENTER);
    //text->setAxisAlignment(osgText::Text::SCREEN);

    if      (arguments.read("--xy")) text->setAxisAlignment(osgText::Text::XY_PLANE);
    else if (arguments.read("--rxy")) text->setAxisAlignment(osgText::Text::REVERSED_XY_PLANE);
    else if (arguments.read("--xz")) text->setAxisAlignment(osgText::Text::XZ_PLANE);
    else if (arguments.read("--rxz")) text->setAxisAlignment(osgText::Text::REVERSED_XZ_PLANE);
    else if (arguments.read("--yz")) text->setAxisAlignment(osgText::Text::YZ_PLANE);
    else if (arguments.read("--ryz")) text->setAxisAlignment(osgText::Text::REVERSED_YZ_PLANE);
    else if (arguments.read("--screen")) text->setAxisAlignment(osgText::Text::SCREEN);
    else
    {
        double angle;
        osg::Vec3d axis;
        if (arguments.read("--rot", angle, axis.x(), axis.y(), axis.z()))
        {
            text->setAxisAlignment(osgText::Text::USER_DEFINED_ROTATION);
            text->setRotation(osg::Quat(osg::DegreesToRadians(angle), axis));
        }
    }

    viewer.setSceneData(text);
    return viewer.run();
}
