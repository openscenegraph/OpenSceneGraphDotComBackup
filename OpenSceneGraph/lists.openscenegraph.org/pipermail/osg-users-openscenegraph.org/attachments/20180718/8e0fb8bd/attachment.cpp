#include <osg/ArgumentParser>
#include <osg/Array>
#include <osg/Group>
#include <osg/Geometry>
#include <osg/PrimitiveSet>
#include <osg/Vec3>
#include <osg/LineWidth>
#include <osg/Program>
#include <osg/Shader>
#include <osgViewer/Viewer>
#include <osgText/Text>

static const char * vLineShader = {
    "#version 330 core\n"
    "in vec4 osg_Vertex;\n"
    "in vec4 osg_Color;\n"
    "uniform mat4 osg_ModelViewProjectionMatrix;\n"
    "out vec4 vertexColor;\n"
    "void main(void)\n"
    "{\n"
    "    gl_Position = osg_ModelViewProjectionMatrix * osg_Vertex;\n"
    "    vertexColor = osg_Color; \n"
    "}\n"
};

static const char * fLineShader = {
    "#version 330 core\n"
    "in vec4 vertexColor;\n"
    "void main(void)\n"
    "{\n"
    "    gl_FragColor = vertexColor;\n"
    "}\n"
};

#define USE_WRONG_VALUES 1

osg::ref_ptr<osg::Geometry> buildLine()
{
    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
    geometry->setUseDisplayList(false);
    
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
#if defined( USE_WRONG_VALUES ) && USE_WRONG_VALUES
    vertices->push_back( osg::Vec3( 7300.0, 1300.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 7400.0, 1400.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 7500.0, 1400.0, 0.0 ) );
#else
    vertices->push_back( osg::Vec3(  0.0,  0.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 10.0, 10.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 20.0, 10.0, 0.0 ) );
#endif
    
    geometry->setVertexArray( vertices );
    
    
    osg::ref_ptr<osg::DrawElementsUInt> drawElements = new osg::DrawElementsUInt( osg::PrimitiveSet::LINE_STRIP, 0 );
    drawElements->push_back( 0 );
    drawElements->push_back( 1 );
    
    geometry->addPrimitiveSet( drawElements );
    
    
    osg::ref_ptr<osg::Vec4Array> color( new osg::Vec4Array );
    color->push_back( osg::Vec4( 1.0, 0.0, 0.0, 1.0 ) );

    geometry->setColorArray( color, osg::Array::BIND_OVERALL );

    
    geometry->getOrCreateStateSet()->setAttributeAndModes( new osg::LineWidth( 10 ), osg::StateAttribute::ON );
    
    
    osg::ref_ptr<osg::Program> program = new osg::Program;
    program->addShader(new osg::Shader(osg::Shader::VERTEX, vLineShader));
    program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fLineShader));
    geometry->getOrCreateStateSet()->setAttributeAndModes(program, osg::StateAttribute::ON);
    
    return geometry;
}

osg::ref_ptr<osg::Node> buildText()
{
    osg::ref_ptr<osgText::Text> textItem = new osgText::Text;
    
    textItem->setFont( "/home/chris/SOURCEN/OpenSceneGraph-TrainingMaterials-CAE/Data/OpenSceneGraph-Data/fonts/times.ttf" );
    textItem->setColor( osg::Vec4( 0.0, 1.0, 0.0, 1.0 ) );
    textItem->setText( "FOO" );
    textItem->setAxisAlignment( osgText::Text::SCREEN );
    textItem->setFontResolution( 40, 40 );
    textItem->setCharacterSize( 32 );
    textItem->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
    textItem->setDrawMode( osgText::Text::TEXT | osgText::Text::BOUNDINGBOX );
    textItem->setAlignment( osgText::TextBase::CENTER_CENTER );
#if defined( USE_WRONG_VALUES ) && USE_WRONG_VALUES
    textItem->setPosition( osg::Vec3( 7300.0, -1500.0, 0.0 ) );
#else
    textItem->setPosition( osg::Vec3( -5.0, -5.0, 0.0 ) );
#endif
    return textItem;
}

osg::ref_ptr<osg::Group> buildIt()
{
    osg::Group * group = new osg::Group;
    
    group->addChild( buildLine() );
    
    group->addChild( buildText() );
    
    return group;
}

int main ( int argc, char **argv )
{
    // constrcut the viewer.
    // use an ArgumentParser object to manage the program arguments.
    osg::ArgumentParser arguments( &argc, argv );
    osgViewer::Viewer viewer( arguments );

    viewer.setSceneData( buildIt() );

    viewer.realize();

    osg::DisplaySettings::instance()->setTextShaderTechnique( "NONE" );


    osg::State * state = viewer.getCamera()->getGraphicsContext()->getState();
    if ( 0 == state )
    {
        state = new osg::State;
    }
    
    state->setUseModelViewAndProjectionUniforms( true );
    state->setUseVertexAttributeAliasing( true ) ;
    viewer.getCamera()->getGraphicsContext()->setState( state );

    viewer.getCamera()->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    viewer.getCamera()->getOrCreateStateSet()->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
    viewer.getCamera()->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    // run the viewers main loop.
    return viewer.run();
}
