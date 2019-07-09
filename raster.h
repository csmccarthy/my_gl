#pragma once
#include "image.h"
#include "model.h"
#include <array>
#include <Eigen/Dense>

void bary_triangle(Eigen::Vector3f v1, Eigen::Vector3f v2, Eigen::Vector3f v3, float* zbuffer, Image& img, Model& head,
	std::array<Eigen::Vector3f, 3> & normals, Eigen::MatrixXf& texture_vertices, Eigen::Vector3f lighting);

void line(int x0, int y0, int x1, int y1, Image& img, Color color);
void hline(int x0, int x1, int y, Image& img, Color color);
void yline(int y0, int y1, int x, Image& img, Color color);
void triangle(int x0, int y0, int x1, int y1, int x2, int y2, Image& img, Color color);
void partial_rref(Eigen::MatrixXf& tbn, Eigen::MatrixXf& inv);
void finish_rref(Eigen::MatrixXf& tbn, Eigen::MatrixXf& inv);

Eigen::Vector3f decompose_to_3d(Eigen::Vector4f v);