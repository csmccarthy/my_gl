#pragma once
#include "image.h"
#include <vector>
#include <string>


class Model {
	std::vector<std::array<float, 3>> *wireframe_pts;
	std::vector<std::array<float, 3>> *texture_pts;
	std::vector<std::array<float, 3>> *normal_pts;
	std::vector<std::array<std::string, 3>> *faces;
	Image* texture;
	Image* normal_map;

public:
	Model();
	bool load_model(const char* filename);
	bool load_texture(const char* filename);
	bool load_normal_map(const char* filename);
	int get_size();
	std::array<std::string, 3> face(int i);
	std::array<float, 3> vertex(int i);
	std::array<float, 3> tx_vertex(int i);
	std::array<float, 3> norm_vertex(int i);
	Image* get_texture();
	Image* get_normal_map();
};