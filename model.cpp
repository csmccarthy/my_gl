#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "model.h"
#include <array>
#include <boost/algorithm/string.hpp>

Model::Model() {
	wireframe_pts = new std::vector<std::array<float, 3>>;
	texture_pts = new std::vector<std::array<float, 3>>;
	normal_pts = new std::vector<std::array<float, 3>>;
	faces = new std::vector<std::array<std::string, 3>>;
	texture = new Image(1, 1, Image::RGBA);
	normal_map = new Image(1, 1, Image::RGBA);
}

bool Model::load_model(const char* filename) {

	float coord;
	std::string line;
	std::vector<std::string> split;
	std::array<float, 3> vertex;
	std::array<std::string, 3> face;
	std::ifstream file(filename);

	if (file.is_open()) {
		while (std::getline(file, line)) {
			boost::split(split, line, [](char c) {return c == ' '; });
			if (split[0] == "v") {
				for (unsigned int i = 1; i < split.size(); i++) {
					coord = std::stof(split[i]);
					vertex[i - 1] = coord;
				}
				(*wireframe_pts).push_back(vertex);
			}
			else if (split[0] == "vt") {
				for (unsigned int i = 2; i < split.size(); i++) {
					coord = std::stof(split[i]);
					vertex[i - 2] = coord;
				}
				(*texture_pts).push_back(vertex);
			}
			else if (split[0] == "vn") {
				for (unsigned int i = 2; i < split.size(); i++) {
					coord = std::stof(split[i]);
					vertex[i - 2] = coord;
				}
				(*normal_pts).push_back(vertex);
			}
			else if (split[0] == "f") {
				for (unsigned int i = 1; i < split.size(); i++) {
					face[i - 1] = split[i];
				}
				(*faces).push_back(face);
			}
		}
		file.close();
	}
	return true;
}

bool Model::load_texture(const char* filename) {
	texture->read_png_file(filename);
	texture->flip_vertically();
	return true;
}

bool Model::load_normal_map(const char* filename) {
	normal_map->read_png_file(filename);
	normal_map->flip_vertically();
	return true;
}

int Model::get_size() {
	return faces->size();
}

std::array<std::string, 3> Model::face(int i) {
	return (*faces)[i];
}

std::array<float, 3> Model::vertex(int i) {
	return (*wireframe_pts)[i];
}

std::array<float, 3> Model::tx_vertex(int i) {
	return (*texture_pts)[i];
}

std::array<float, 3> Model::norm_vertex(int i) {
	return (*normal_pts)[i];
}

Image* Model::get_texture() {
	return texture;
}

Image* Model::get_normal_map() {
	return normal_map;
}