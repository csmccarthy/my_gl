#pragma once
#include <iostream>
#include <Eigen/Dense>
#include "image.h"
#include "lodepng.h"

Image::Image(int w, int h, int chan) : data(NULL), width(w), height(h), channels(chan) {
    unsigned long nbytes = width*height*channels;
    data = new unsigned char[nbytes];
    memset(data, 0, nbytes);
}

Image::Image() : data(NULL), width(NULL), height(NULL), channels(NULL) {
}

int Image::get_width() {
	return width;
}
int Image::get_height() {
	return height;
}
int Image::get_channels() {
	return channels;
}

bool Image::set(int x, int y, Color c) {
    if (!data || x<0 || y<0 || x>=width || y>=height) return false;
    memcpy(data+(x+y*width)*channels, c.raw, channels);
    return true;
}

Color Image::get(int x, int y) {
	if (!data || x < 0 || y < 0 || x >= width || y >= height) {
		return Color();
	}
	unsigned char r = *(data + ((x + y * width) * channels));
	unsigned char g = *(data + ((x + y * width) * channels) + 1);
	unsigned char b = *(data + ((x + y * width) * channels) + 2);
	unsigned char a = *(data + ((x + y * width) * channels) + 3);

	return Color(r, g, b, a);
}

Eigen::Vector3f Image::get_normal(int x, int y) {
	if (!data || x < 0 || y < 0 || x >= width || y >= height) {
		return Eigen::Vector3f();
	}
	unsigned char r = *(data + ((x + y * width) * channels));
	unsigned char g = *(data + ((x + y * width) * channels) + 1);
	unsigned char b = *(data + ((x + y * width) * channels) + 2);
	unsigned char a = *(data + ((x + y * width) * channels) + 3);
	Eigen::Vector3f normal = { (((float)r * 2) / 255) - 1, (((float)g * 2) / 255) - 1, (((float)b * 2) / 255) - 1 };

	return normal.normalized();
}

bool Image::flip_vertically() {
    if (!data) return false;
    unsigned long bytes_per_line = width*channels;
    unsigned char *line = new unsigned char[bytes_per_line];
    int half = height>>1;
    for (int i = 0; i < half; i++) {
        unsigned long line1 = i*bytes_per_line;
        unsigned long line2 = (height-1-i)*bytes_per_line;
        memmove((void *)line, (void *)(data+line1), bytes_per_line);
        memmove((void *)(data+line1), (void *)(data+line2), bytes_per_line);
        memmove((void *)(data+line2), (void *)line, bytes_per_line);
    }
    delete [] line;
    return true;
}

bool Image::write_png_file(const char* filename) {
	unsigned error = 0;
	if (channels == 3) {
		error = lodepng::encode(filename, data, width, height, LCT_RGB);
	}
	else if (channels == 4) {
		error = lodepng::encode(filename, data, width, height, LCT_RGBA);
	}
	if (error) std::cout << "PNG write error " << error << ": " << lodepng_error_text(error) << "\n";
	return true;
}

bool Image::read_png_file(const char* filename) {
	std::vector<unsigned char> img;
	unsigned w, h;
	unsigned error = lodepng::decode(img, w, h, filename);
	if (error) std::cout << "PNG read error " << error << ": " << lodepng_error_text(error) << "\n";

	width = w;
	height = h;
	channels = 4;
	unsigned long new_bytes = width * height * 4;
	if (data) delete[] data;
	data = new unsigned char[new_bytes];
	memset(data, 0, new_bytes);

	// this can be improved, figure out how to parse chunks of it at a time
	for (int i = 0; i < width * height * 4; i+= 4) {
		//int idx = i * 4;
		unsigned char r = img[i];
		unsigned char g = img[i+1];
		unsigned char b = img[i+2];
		unsigned char a = img[i+3];
		int x = (i/4) % width;
		int y = (i/4) / width;
		set(x, y, Color(r, g, b, a));
	}

	return true;
}

bool Image::write_ppm3_file(const char *filename) {
	std::ofstream ppm;
	ppm.open(filename, std::ios::trunc);
	if (!ppm.is_open()) {
		std::cerr << "Can't open file " << filename << "!\n";
		ppm.close();
		return false;
	}
	ppm << "P3\n";
	ppm << width << " " << height << "\n";
	ppm << "255\n";
	for (int i = 0; i < width*height; i++) {
		for (int j = channels-1; j >= 0; j--) {
			ppm << std::to_string(data[(3*i)+j]) << " ";
		}
		ppm << "\n";
	}
	ppm.close();
	return true;
}

bool Image::write_ppm6_file(const char *filename) {
	std::ofstream ppm;
	ppm.open(filename, std::ios::trunc | std::ios::binary);
	if (!ppm.is_open()) {
		std::cerr << "Can't open file " << filename << "!\n";
		ppm.close();
		return false;
	}
	ppm << "P6\n";
	ppm << width << " " << height << "\n";
	ppm << "255\n";
	ppm.write((char *)data, width*height*channels);
	ppm.close();
	return true;
}