#ifndef IMG_H
#define IMG_H

// Disables strict warnings for header from MSVC compiler
#ifdef _MSC_VER
	#pargma warning(push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG

#include "external/stb_image.h"

#include <cerrno>
#include <cstdlib>
#include <iostream>

class IMG {
	private:
		const int 		bytes_per_pixel = 3;
		float			*fdata = nullptr;		// Linear floating point pixel data
		unsigned char 	*bdata = nullptr; 		// Linear byte pixel data
		int		image_width  = 0;
		int		image_height = 0;
		int		bytes_per_scanline = 0;
		
		static int clamp(int x, int min, int max) {
			// Clamped in [min, max[
			if(x < min)  return min;
			if(x < max) return x;
			return max-1;
		}
		
		static unsigned char float_to_byte(float value) {
			if(value <= 0.) return 0;
			if(1. <= value) return 255;
			return static_cast<unsigned char>(256. * value);
		}
		
		void convert_to_bytes() {
			int total_bytes = image_width * image_height * bytes_per_pixel;
			bdata = new unsigned char[total_bytes];
			
			// converting [0, 1] floats to unsigned [0, 255] bytes
			auto *bptr = bdata;
			auto *fptr = fdata;
			for(int i = 0; i < total_bytes; i++, fptr++, bptr++)
				*bptr = float_to_byte(*fptr);
		}
	
	public:
		IMG() {}
		
		IMG(const char* img_filename) {
			std::string filename = std::string(img_filename);
			
			if(load(filename)) return;
			if(load("images/" + filename)) return;
			if(load("../images/" + filename)) return;
			
			perror("Could not load image file");
		}
		
		~IMG() {
			delete[] bdata;
			STBI_FREE(fdata);
		}
		
		bool load(const std::string& filename) {
			// Pixels are contiguous,
			// going left to right for the width of the image, followed by the next row
			// below, for the full height of the image.
			
			int n = bytes_per_pixel;
			fdata = stbi_loadf(filename.c_str(), &image_width, &image_height, &n, bytes_per_pixel);
			if(fdata == nullptr) return false;
			
			bytes_per_scanline = image_width * bytes_per_pixel;
			convert_to_bytes();
			return true;
		}
		
		int width()  const {return (fdata) ? image_width  : 0;}
		int height() const {return (fdata) ? image_height : 0;}
		
		const unsigned char* pixel_data(int x, int y) const {
			static unsigned char cyan[] = {0, 255, 255};
			if(!bdata) return cyan;
			
			x = clamp(x, 0, image_width);
			y = clamp(y, 0, image_height);
			
			return bdata + y*bytes_per_scanline + x*bytes_per_pixel;
		}
};

// Restore MSVC compiler warnings
#ifdef _MSC_VER
	#pargma warning(pop)
#endif

#endif