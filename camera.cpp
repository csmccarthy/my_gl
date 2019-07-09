#pragma once
#include <Eigen/Dense>

Eigen::Vector3f decompose_to_3d(Eigen::Vector4f v) {
	v = v / v(3);
	Eigen::Vector3f new_v = { v(0), v(1), v(2) };
	return new_v;
}


Eigen::Matrix4f look(Eigen::Vector3f camera_position, Eigen::Vector3f point, Eigen::Vector3f upward) {
	Eigen::Vector3f z = camera_position - point;
	z.normalize();
	Eigen::Vector3f x = upward.cross(z);
	x.normalize();
	Eigen::Vector3f y = z.cross(x);
	y.normalize();
	Eigen::Matrix4f inverse = Eigen::Matrix4f::Identity(4, 4);
	Eigen::Matrix4f translate = Eigen::Matrix4f::Identity(4, 4);
	for (int i = 0; i < 3; i++) {
		inverse(0, i) = x(i);
		inverse(1, i) = y(i);
		inverse(2, i) = z(i);
		translate(i, 3) = -point(i);
	}
	Eigen::Matrix4f ModelView = inverse * translate;
	return ModelView;
}

Eigen::Matrix4f project(float c) {
	Eigen::Matrix4f proj = Eigen::Matrix4f::Identity(4, 4);
	proj(3, 2) = -1.f / c;
	return proj;
}

Eigen::Matrix4f view(int x, int y, int w, int h, int d) {
	Eigen::Matrix4f scale;
	scale << w / 2.f, 0, 0, 0,
			0, h / 2.f, 0, 0,
			0, 0, d / 2.f, 0,
			0, 0, 0, 1;
	Eigen::Matrix4f translate;
	translate << 1, 0, 0, x + w / 2.f,
				0, 1, 0, y + h / 2.f,
				0, 0, 1, d / 2.f,
				0, 0, 0, 1;
	return translate * scale;
}