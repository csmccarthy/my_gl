#pragma once
#include <Eigen/Dense>

Eigen::Vector3f decompose_to_3d(Eigen::Vector4f v);
Eigen::Matrix4f look(Eigen::Vector3f camera_position, Eigen::Vector3f point, Eigen::Vector3f upward);
Eigen::Matrix4f project(float c);
Eigen::Matrix4f view(int x, int y, int w, int h, int d);