#pragma once
#include <fstream>
#include <Eigen/Dense>


struct Color {
    union {
        struct {
            unsigned char r, g, b, a;
        };
        unsigned char raw[4];
        unsigned int val;
    };
    int channels;

    Color(unsigned char R, unsigned char G, unsigned char B, unsigned char A): r(R), g(G), b(B), a(A), channels(4) {
    }

    Color(int v, int chan): val(v), channels(chan){
    }

	Color() : val(0), channels(1) {
	}

	Color operator* (const float scale) {
		return Color(r * scale, g * scale, b * scale, a);
	}
};



class Image {
    int width;
    int height;
    int channels;
    unsigned char* data;

public:
    enum Format {
        GRAYSCALE=1, RGB=3, RGBA=4
    };
    Image(int w, int h, int chan);
	Image();
    bool set(int x, int y, Color c);
	Color get(int x, int y);
	Eigen::Vector3f get_normal(int x, int y);
    bool write_ppm3_file(const char *filename);
    bool write_ppm6_file(const char *filename);
	bool write_png_file(const char* filename);
	bool read_png_file(const char* filename);
    bool flip_vertically();
    int get_width();
    int get_height();
    int get_channels();
};