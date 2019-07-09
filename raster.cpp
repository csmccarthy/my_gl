#pragma once
#include "image.h"
#include "model.h"
#include <array>
#include <Eigen/Dense>

void line(int x0, int y0, int x1, int y1, Image& img, Color color) {
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int dy = abs(y1 - y0);
	int dx = x1 - x0;

	bool decrease = false;
	if (y0 > y1) {
		decrease = true;
	}

	bool swapped = false;
	if (dy > dx) {
		std::swap(dy, dx);
		std::swap(x0, y0);
		std::swap(x1, y1);
		swapped = true;
	}

	int error = 0;
	if (swapped && decrease) {
		int y = y1;
		for (int i = x1; i <= x0; i++) {
			img.set(y, i, color);
			error += dy;
			if (error >= dx) {
				error -= dx;
				y--;
			}
		}
	}
	else if (swapped && !decrease) {
		int y = y0;
		for (int i = x0; i <= x1; i++) {
			img.set(y, i, color);
			error += dy;
			if (error >= dx) {
				error -= dx;
				y++;
			}
		}
	}
	else if (!swapped && decrease) {
		int y = y0;
		for (int i = x0; i <= x1; i++) {
			img.set(i, y, color);
			error += dy;
			if (error >= dx) {
				error -= dx;
				y--;
			}
		}
	}
	else {
		int y = y0;
		for (int i = x0; i <= x1; i++) {
			img.set(i, y, color);
			error += dy;
			if (error >= dx) {
				error -= dx;
				y++;
			}
		}
	}
}

void hline(int x0, int x1, int y, Image& img, Color color) {
	if (x1 < x0) {
		std::swap(x0, x1);
	}
	for (int i = x0; i <= x1; i++) {
		img.set(i, y, color);
	}
}

void yline(int y0, int y1, int x, Image& img, Color color) {
	if (y1 < y0) {
		std::swap(y0, y1);
	}
	for (int i = y0; i <= y0; i++) {
		img.set(x, i, color);
	}
}

void triangle(int x0, int y0, int x1, int y1, int x2, int y2, Image& img, Color color) {
	if (y0 > y1) {
		std::swap(y0, y1);
		std::swap(x0, x1);
	}
	if (y0 > y2) {
		std::swap(y0, y2);
		std::swap(x0, x2);
	}
	if (y1 > y2) {
		std::swap(y1, y2);
		std::swap(x1, x2);
	}

	double step1 = (double)(x1 - x0) / (double)(y1 - y0);
	double step2 = (double)(x2 - x0) / (double)(y2 - y0);
	double step3 = (double)(x2 - x1) / (double)(y2 - y1);

	double side1 = x0;
	double side2 = x0;

	if (y0 != y1) {
		for (int i = y0; i <= y1; i++) {
			side1 += step1;
			side2 += step2;
			hline(side1, side2, i, img, color);
		}
	}
	else {
		side1 = x1;
	}

	if (y1 != y2) {
		for (int i = y1; i <= y2; i++) {
			side1 += step3;
			side2 += step2;
			hline(side1, side2, i, img, color);
		}
	}
}

Eigen::MatrixXf partial_rref(Eigen::MatrixXf& tbn, Eigen::MatrixXf& inv) {
	if (tbn(0, 1) == 0) {
		tbn.row(2) = tbn.row(1);
		tbn.row(1) = tbn.row(0);
		tbn.row(0) = tbn.row(2);
	}

	float factor = 1.f/tbn(0, 0);
	tbn.row(0) *= factor;
	inv.row(0) *= factor;

	factor = tbn(1, 0) / tbn(0, 0);
	tbn.row(1) -= tbn.row(0) * factor;
	inv.row(1) -= inv.row(0) * factor;

	factor = 1.f / tbn(1, 1);
	tbn.row(1) *= factor;
	inv.row(1) *= factor;

	factor = tbn(0, 1) / tbn(1, 1);
	tbn.row(0) -= tbn.row(1) * factor;
	inv.row(0) -= inv.row(1) * factor;

	return inv;
}

Eigen::MatrixXf finish_rref(Eigen::MatrixXf& tbn, Eigen::MatrixXf& inv) {
	
	float factor;

	for (int i = 0; i < 2; i++) {
		factor = tbn(2, i) / tbn(i, i);
		tbn.row(2) -= tbn.row(i) * factor;
		inv.row(2) -= inv.row(i) * factor;
	}

	factor = 1.f / tbn(2, 2);
	tbn(2, 2) = 1.f;
	inv.row(2) *= factor;

	for (int i = 1; i > -1; i--) {
		factor = tbn(i, 2) / tbn(2, 2);
		tbn.row(i) -= tbn.row(2) * factor;
		inv.row(i) -= inv.row(2) * factor;
	}

	return inv;
}

void bary_triangle(Eigen::Vector3f v1, Eigen::Vector3f v2, Eigen::Vector3f v3, float* zbuffer, Image& img, Model& head,
	std::array<Eigen::Vector3f, 3>& normals, Eigen::MatrixXf& texture_vertices, Eigen::Vector3f lighting) {

	Eigen::Matrix2f T(2, 2);
	T << v1[0] - v3[0], v2[0] - v3[0],
		v1[1] - v3[1], v2[1] - v3[1];
	Eigen::Matrix2f T_inv = T.inverse();
	Eigen::Vector2f screen_v3 = { v3[0], v3[1] };

	Eigen::Vector3f intensities;
	for (int i = 0; i < 3; i++) {
		intensities(i) = std::max(normals[i].dot(lighting), 0.f);
	}

	Eigen::Vector3f u_vector = { texture_vertices(0, 1) - texture_vertices(0, 0), texture_vertices(0, 2) - texture_vertices(0, 0), 0.f };
	Eigen::Vector3f v_vector = { texture_vertices(1, 1) - texture_vertices(1, 0), texture_vertices(1, 2) - texture_vertices(1, 0), 0.f };

	Eigen::MatrixXf tbn(3, 3);

	tbn.row(0) = (v2 - v1).normalized();
	tbn.row(1) = (v3 - v1).normalized();
	tbn.row(2) = Eigen::Vector3f(0, 0, 0);

	Image* texture = head.get_texture();
	int tx_w = texture->get_width();
	int tx_h = texture->get_height();
	Image* normal_map = head.get_normal_map();

	int xmin = std::max((int)std::min(v1[0], std::min(v2[0], v3[0])), 0);
	int xmax = std::min((int)std::max(v1[0], std::max(v2[0], v3[0])), img.get_width());
	int ymin = std::max((int)std::min(v1[1], std::min(v2[1], v3[1])), 0);
	int ymax = std::min((int)std::max(v1[1], std::max(v2[1], v3[1])), img.get_height());

	std::vector<std::array<float, 2>> y_params;

	for (int i = xmin; i <= xmax; i++) {
		std::array<float, 2> x_params = { 0, 0 };
		x_params[0] = T_inv(0, 0) * (i - screen_v3[0]);
		x_params[1] = T_inv(1, 0) * (i - screen_v3[0]);

		for (int j = ymin; j <= ymax; j++) {

			if (i == xmin) {
				std::array<float, 2> y_comp = { 0, 0 };
				y_comp[0] = T_inv(0, 1) * (j - screen_v3[1]);
				y_comp[1] = T_inv(1, 1) * (j - screen_v3[1]);
				y_params.push_back(y_comp);
			}

			Eigen::Vector2f params = { x_params[0] + y_params[j - ymin][0], x_params[1] + y_params[j - ymin][1] };
			Eigen::Vector3f bary = { params(0), params(1), 1.f - params(0) - params(1) };

			float zval = (bary(0) * v1[2]) + (bary(1) * v2[2]) + (bary(2) * v3[2]);
			int z_idx = (j * img.get_width() + i);

			if ((bary(0) >= 0 && bary(1) >= 0 && bary(2) >= 0) && (zbuffer[z_idx] < zval)) {
				zbuffer[z_idx] = zval;
				Eigen::Vector2f pos_ratios = texture_vertices * bary;
				float light_ratio = intensities.dot(bary);
				if (light_ratio <= 0) {
					return;
				}

				Eigen::Vector3f tri_normal = { 0, 0, 0 };
				for (int k = 0; k < 3; k++) {
					tri_normal = tri_normal + (normals[k] * bary(k)); // turn this into matrix mult
				}

				int texture_x = pos_ratios(0) * tx_w;
				int texture_y = pos_ratios(1) * tx_h;

				Color texture_color = texture->get(texture_x, texture_y);
				Eigen::Vector3f tan_normal = normal_map->get_normal(texture_x, texture_y);

				tbn.row(2) = tri_normal.normalized();
				Eigen::MatrixXf tbn_inv = tbn.inverse();
				Eigen::MatrixXf basis(3, 3);
				basis.col(0) = (tbn_inv * u_vector).normalized();
				basis.col(1) = (tbn_inv * v_vector).normalized();
				basis.col(2) = tri_normal.normalized();

				Eigen::Vector3f n = (basis * tan_normal).normalized();

				light_ratio = std::max(n.dot(lighting), 0.f);
				Color lighted_color = texture_color * light_ratio;

				//Color lighted_color = Color(255, 255, 255, 255) * light_ratio;
				img.set(i, j, lighted_color);
			}
		}
	}
}

