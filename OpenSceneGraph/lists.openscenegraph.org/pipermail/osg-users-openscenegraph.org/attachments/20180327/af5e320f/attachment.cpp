#include <iostream>

#include <osg/Notify>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

int main( int argc, char** argv)
{
    osg::ArgumentParser arguments(&argc, argv);


   // get folder and file to use, default to ../osg/Version header file
   std::string dir = "../include/osg/";
   arguments.read("--dir", dir);

   std::string file = "Version";
   arguments.read("--file", file);

   std::string fullpath = osgDB::findFileInDirectory(file, dir, osgDB::CASE_INSENSITIVE);
   bool result = !fullpath.empty();
   OSG_ALWAYS << "Result with findFileInDirectory(): " << result << " "<<fullpath<<std::endl;

   std::string concatenated = dir + file;
   bool fileExsists = osgDB::fileExists(concatenated);
   OSG_ALWAYS << "Result with fileExists(): " << fileExsists << " concatenated="<<concatenated<<std::endl;

    return 0;
}
