#include <iostream>
#include "gfanlib.h"
using namespace gfan;
using namespace std;
int main()
{
    try{
      cout<<mixedVolume(MixedVolumeExamples::cyclic(12),16)<<endl;
      cout<<mixedVolume(MixedVolumeExamples::cyclic(12))<<endl;
      cout<<mixedVolume(MixedVolumeExamples::gaukwa(7),16)<<endl;
    }
    catch (...)
    {
        cerr<<"Error - most likely an integer overflow."<<endl;
    }
    return 0;
}
