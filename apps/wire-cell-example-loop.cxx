#include "WireCellNav/SliceDataSource.h"
#include "WireCellSst/FrameDataSource.h"
#include "WireCellSst/GeomDataSource.h"

#include "TFile.h"
#include "TTree.h"

#include <fstream>
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
    if (argc < 2) {
	cerr << "usage: wce-example-loop sst-geometry.txt sst-celltree.root" << endl;
	exit (1);
    }

    // Get wire geometry
    ifstream geotext(argv[1]);
    WireCellSst::GeomDataSource gds(geotext);

    // open data file to make frame data source
    TFile* tfile = TFile::Open(argv[2]);
    TTree* tree = dynamic_cast<TTree*>(tfile->Get("/Event/Sim"));
    WireCellSst::FrameDataSource fds(*tree);
    
    WireCell::SliceDataSource sds(fds);
    
    // Loop over frames (aka "events")
    size_t nframes = fds.size();
    cout << "FDS: " << nframes << " frames" << endl;
    for (size_t iframe = 0; iframe < nframes; ++iframe) {

	int iframe_got = fds.jump(iframe);
	if (iframe_got < 0) {
	    cerr << "Failed to get frame " << iframe << endl;
	    exit(1); // real code may want to do something less drastic
	}

	// loop over slices of the frame 

	size_t nslices = sds.size();
	cout << "SDS: " << nslices << " slices in frame " << iframe_got << endl;
	for (size_t islice = 0; islice < nslices; ++islice) {
	    int islice_got = sds.jump(islice);
	    if (islice_got < 0) {
		cerr << "Failed to get slice " << islice << " from frame " << iframe << endl;
		exit(1); // real code may want to do something less drastic
	    }

	    const WireCell::Slice& slice = sds.get();

	    cout << "SDS:\t tbin=" << slice.tbin << " #bins:" << slice.charge.size() << endl;
	}
    }

    return 0;

} // main()
