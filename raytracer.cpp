#pragma once
#include "raster.h"
#include "image.h"
#include "model.h"
#include "camera.h"
#include <string>
#include <vector>
#include <array>
#include <boost/algorithm/string.hpp>
#include <Eigen/Dense>
#include <cmath>
#include <iostream>

const Color white = Color(255, 255, 255, 255);
const Color red = Color(255, 0, 0, 255);
const Color green = Color(0, 255, 0, 255);
const Color blue = Color(0, 0, 255, 255);
const Color magenta = Color(255, 0, 255, 255);

Eigen::Vector3f lighting(1, 1, 1);
Eigen::Vector3f eye(1, 1, 3);
Eigen::Vector3f center(0, 0, 0);
Eigen::Vector3f up(0, 1, 0);

void get_face_info(Model& head, int index, std::array<Eigen::Vector3f, 3>& verts, std::array<Eigen::Vector3f, 3>& normals, Eigen::MatrixXf& texture_matrix, Eigen::Matrix4f transform) {

	std::vector<std::string> vsplit;
	for (int j = 0; j < 3; j++) {

		boost::split(vsplit, head.face(index)[j], [](char c) {return c == '/'; });
		int model_idx = std::stoi(vsplit[0]) - 1;
		int texture_idx = std::stoi(vsplit[1]) - 1;
		int normal_idx = std::stoi(vsplit[2]) - 1;

		std::array<float, 3> pts = head.vertex(model_idx);
		Eigen::Vector4f vert = { pts[0], pts[1], pts[2], 1.f };

		pts = head.norm_vertex(normal_idx);
		Eigen::Vector3f normal = { pts[0], pts[1], pts[2] };

		texture_matrix(0, j) = head.tx_vertex(texture_idx)[0];
		texture_matrix(1, j) = head.tx_vertex(texture_idx)[1];

		verts[j] = decompose_to_3d(transform * vert);
		normals[j] = normal;
	}

}

void render(Model& head, float* zbuffer, Image img) {

	int w = img.get_width();
	int h = img.get_height();
	int d = 255;

	Eigen::Matrix4f proj = project((eye-center).norm());
	Eigen::Matrix4f viewport = view(w/8, h/8, 3*w/4, 3*h/4, d);
	Eigen::Matrix4f modelview = look(eye, center, up);

	Eigen::Matrix4f full_transform = viewport * proj * modelview;
	//Eigen::Matrix4f norm_transform = (full_transform.transpose()).inverse();
	
	for (int i = 0; i < head.get_size(); i++) {
		
		std::array<Eigen::Vector3f, 3> verts;
		std::array<Eigen::Vector3f, 3> normals;
		Eigen::MatrixXf texture_matrix(2, 3);

		get_face_info(head, i, verts, normals, texture_matrix, full_transform);

		bary_triangle(verts[0], verts[1], verts[2], zbuffer, img, head, normals, texture_matrix, lighting.normalized());

		if (i % 100 == 0) {
			img.flip_vertically();
			img.write_png_file("output.png");
			img.flip_vertically();
		}
	}
}

int main(int argc, char**argv) {

	Eigen::MatrixXf tbn(3, 3);
	tbn.row(0) = Eigen::Vector3f(0, 0, 0);

	std::cout << tbn;

	tbn << 1, 2, 3,
		   4, 9, 6,
		   4, 8, 10;

	Eigen::MatrixXf partial_inv(3, 3);

	partial_inv << 1, 0, 0,
				   0, 1, 0,
				   0, 0, 1;

	partial_rref(tbn, partial_inv);

	finish_rref(tbn, partial_inv);


	std::cout << tbn;
	std::cout << "\n";
	std::cout << partial_inv;

 //   Image img(800, 800, Image::RGB);

	//float* zbuffer = new float[800 * 800];
	//for (int i = 0; i < 800 * 800; i++) {
	//	zbuffer[i] = -std::numeric_limits<float>::infinity();
	//}

	//Model *head = new Model();
	//head->load_model("african_head.txt");
	//head->load_texture("african_head_diffuse.png");
	//head->load_normal_map("african_head_nm_tangent.png");

	//render(*head, zbuffer, img);

	//img.flip_vertically();
	//img.write_png_file("output.png");
    return 0;
}