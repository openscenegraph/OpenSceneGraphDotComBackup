#include <future>
#include <iostream>
#include <vector>

#include <osgViewer/Viewer>

class ComputeThread : public osg::Referenced, public OpenThreads::Thread
{
public:

    int _numOperations;

    ComputeThread(int n):_numOperations(n) {}

    virtual void run()
    {
        for(volatile int n = 0; n != _numOperations; ++n)
        {
            ++n;
            --n;
        }
        std::cout << "Thread " << this << " done\n";
    }
};


int main(int argc, char** argv)
{
    // use an ArgumentParser object to manage the program arguments.
    osg::ArgumentParser arguments(&argc,argv);

    int numThreads = 16;
    int numOperations = 1000000000;

    osgViewer::Viewer viewer;

    while (arguments.read("--num-threads", numThreads)) {}
    while (arguments.read("--num-operatins", numOperations)) {}

    if (arguments.read("--SingleThreaded"))
    {
        viewer.setThreadingModel(osgViewer::ViewerBase::ThreadingModel::SingleThreaded);
    }

    viewer.realize();

    if (arguments.read("--ot"))
    {
        typedef std::list<osg::ref_ptr<ComputeThread> > Threads;
        Threads threads;
        for(int i = 0; i != numThreads; ++i)
        {
            threads.push_back(new ComputeThread(numOperations));
        }

        for(Threads::iterator itr = threads.begin();
            itr != threads.end();
            ++itr)
        {
            (*itr)->startThread();
        }

        for(Threads::iterator itr = threads.begin();
            itr != threads.end();
            ++itr)
        {
            (*itr)->join();
        }

    }
    else
    {
        if (arguments.read("--reset"))
        {
            OpenThreads::SetProcessorAffinityOfCurrentThread(OpenThreads::Affinity());
        }

        unsigned int cpuNumber, cpuCount;
        if (arguments.read("--cpus", cpuNumber, cpuCount))
        {
            OpenThreads::SetProcessorAffinityOfCurrentThread(OpenThreads::Affinity(cpuNumber, cpuCount));
        }

        if (arguments.read("--cpu", cpuNumber))
        {
            OpenThreads::SetProcessorAffinityOfCurrentThread(cpuNumber);
        }

        const auto compute = [] (const int i, const int numOps)
        {
            for(volatile int n = 0; n != numOps; ++n)
            {
                ++n;
                --n;
            }
            std::cout << "Thread " << i << " done\n";
        };

        std::vector<std::future<void>> futures;
        for(int i = 0; i != numThreads; ++i)
        {
            futures.push_back(std::async(std::launch::async, compute, i, numOperations));
        }

        for(auto & future: futures)
        {
            future.wait();
        }
    }


    return 0;
}