#include "WireCellNav/SliceDataSource.h"
#include "WireCellSst/FrameDataSource.h"
#include "WireCellSst/GeomDataSource.h"
#include "WireCellTiling/TileMaker.h"

#include "TFile.h"
#include "TTree.h"

#include <ctime>
#include <fstream>
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
    if (argc < 2) {
	cerr << "usage: wce-example-loop sst-geometry.txt sst-celltree.root" << endl;
	exit (1);
    }

    time_t now=0, start_time = 0;

    // Get wire geometry
    start_time = time(0);
    ifstream geotext(argv[1]);
    WireCellSst::GeomDataSource gds;
    gds.load(geotext);
    now = time(0);
    cerr << "Loaded geometry in " << now - start_time << endl;

    
    // One of the basic cell tilings
    start_time = time(0);
    WireCell::TileMaker tiling(gds);
    now = time(0);
    cerr << "Loaded tiling in " << now - start_time << endl;    

    // open data file to make frame data source
    TFile* tfile = TFile::Open(argv[2]);
    TTree* tree = dynamic_cast<TTree*>(tfile->Get("/Event/Sim"));
    WireCellSst::FrameDataSource fds(*tree);
    
    start_time = time(0);
    WireCell::SliceDataSource sds(fds);
    now = time(0);
    cerr << "Loaded slice data source in " << now - start_time << endl;    
    
    // Loop over frames (aka "events")
    size_t nframes = fds.size();
    cout << "FDS: " << nframes << " frames" << endl;

    start_time = time(0);
    for (size_t iframe = 0; iframe < nframes; ++iframe) {

	cout << "FDS: jumping to frame #" << iframe << endl;

	int iframe_got = fds.jump(iframe);
	if (iframe_got < 0) {
	    cerr << "Failed to get frame " << iframe << endl;
	    exit(1); // real code may want to do something less drastic
	}

	// loop over slices of the frame 

	time_t start_frame_time = time(0);

	size_t nslices = sds.size();
	cout << "SDS: " << nslices << " slices in frame " << iframe_got << endl;
	for (size_t islice = 0; islice < nslices; ++islice) {
	    int islice_got = sds.jump(islice);
	    if (islice_got < 0) {
		cerr << "Failed to get slice " << islice << " from frame " << iframe << endl;
		exit(1); // real code may want to do something less drastic
	    }

	    const WireCell::Slice& slice = sds.get();
	    if (islice%1000 == 1) {
		time_t now = time(0);
		cerr << "slice #" << islice
		     << " time elapsed in frame: " << now-start_frame_time
		     << " slice tbin:" << slice.tbin() 
		     << " with " << slice.group().size() << " wires"
		     << endl;
	    }

	}

	time_t now = time(0);
	cerr << "frame #" << iframe
	     << " time elapsed for frame: " << now-start_frame_time
	     << " total time: " << now-start_time
	     << endl;
    }

    return 0;

} // main()
