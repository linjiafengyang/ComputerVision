// Sift.h : Simple wrapper for VLFeat's SIFT implementation
// Hacked by D. Crandall

#ifndef __SIFT_H_
#define __SIFT_H_
#include "CImg.h"
#include "siftpp/sift.hpp"

using namespace std;
using namespace cimg_library;

class SiftDescriptor {
public:
	SiftDescriptor() {}

	SiftDescriptor(float _row, float _col, float _sigma, float _angle, float *_descriptor) : 
		row(_row), col(_col), sigma(_sigma), angle(_angle), descriptor(_descriptor, _descriptor+128) {}

	float row, col, sigma, angle;
	vector<float> descriptor;
};

class Sift {
public:
	// This method takes a luma (grayscale) image plane as input, and returns a vector
	// of SiftDescriptor's.
	static vector<SiftDescriptor> compute_sift(const CImg<float> &luma) {
		bool lowe_compatible = true;
		bool norm = true;
		
		vector<SiftDescriptor> descriptors;

		//typedef vector<pair<VL::Sift::Keypoint,VL::float_t> > Keypoints ;

		//      if(lowe_compatible)
		//	luma /= 255.0;
		
		VL::PgmBuffer buffer;
		buffer.height = luma.height();
		buffer.width = luma.width();
		buffer.data = (VL::pixel_t *)luma.data();
		
		// --------------------------------
		//    Gaussian scale space
		// --------------------------------
		int         O      = octaves;
		int const   S      = levels;
		int const   omin   = first;
		float const sigman = .5;
		float const sigma0 = 1.6 * powf(2.0f, 1.0f / S);
			

		if(O < 1 || lowe_compatible) {
			O = std::max
			(int(std::floor(log2(std::min(luma.height(), luma.width()))) - omin -3), 1);
		}

		// initialize scalespace
		VL::Sift sift(buffer.data, buffer.width, buffer.height, sigman, sigma0, O, S, omin, -1, S+1);
	
		sift.detectKeypoints(threshold, edgeThreshold) ;

		/* set descriptor options */
		sift.setNormalizeDescriptor( norm || lowe_compatible); //! unnormalized ) ;
		sift.setMagnification( magnif ) ;

		// -------------------------------------------------------------
		//     Run detector, compute orientations and descriptors
		// -------------------------------------------------------------
		for(VL::Sift::KeypointsConstIter iter = sift.keypointsBegin(); iter != sift.keypointsEnd(); ++iter) {
			// detect orientations
			VL::float_t angles [4];
			int nangles;
			nangles = sift.computeKeypointOrientations(angles, *iter);
			
			// compute descriptors
			for(int a = 0 ; a < nangles ; ++a) {
				/* compute descriptor */
				VL::float_t descr_pt [128] ;
				sift.computeKeypointDescriptor(descr_pt, *iter, angles[a]);

				if(lowe_compatible)
					for(int j=0; j<128; j++)
						descr_pt[j] = (unsigned char)(descr_pt[j] * 512);
						descriptors.push_back(SiftDescriptor(iter->y, iter->x, iter->sigma, angles[a], descr_pt));
			} // next angle
		} // next keypoint
		return descriptors;
	}	
 protected:
	static const int octaves = 7, levels = 3, first = -1;
	static const float threshold = 0.04f / levels / 2.0f, edgeThreshold = 10.0f, magnif=3.0;
};

#endif
