//
//  texture.h
//  Gaia
//
//  Created by Alex Richardson on 18/09/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//
#pragma once

#ifndef TEXTURE_H
#define TEXTURE_H

#include "stb_image.h"

#include "vec2.h"
#include <algorithm>

class texture {
public:
	virtual vec3 value(vec2 uv, const vec3& p) const = 0;
};

class solid_colour : public texture {
public:
	solid_colour() {}
	~solid_colour() {}
	solid_colour(vec3 c) :colour_value(c) {}
	solid_colour(float r, float g, float b) :solid_colour(vec3(r, g, b)) {}
	virtual vec3 value(vec2 uv, const vec3& p) const override {
		return colour_value;
	}

private:
	vec3 colour_value;
};

class image_texture : public texture {
public:
	int bytes_per_pixel = 3;

	image_texture() : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

	image_texture(const char* filename) {
		data = stbi_load(filename, &width, &height, &bytes_per_pixel, bytes_per_pixel);
		if (!data) {
			std::cerr << "Error: Could not load image file: " << filename << "\n";
			width = height = 0;
		}

		bytes_per_scanline = bytes_per_pixel * width;
	}

	~image_texture() { delete data; }

	virtual vec3 value(vec2 uv, const vec3& p) const {
		// If no texture data, return solid cyan for debugging
		if (data == nullptr) {
			return vec3(0, 1, 1);
		}

		uv[0] = std::clamp(uv.u(), 0.0f, 1.0f);
		uv[1] = 1.0 - std::clamp(uv.v(), 0.0f, 1.0f);

		auto i = static_cast<int>(uv.u() * width);
		auto j = static_cast<int>(uv.v() * height);
		//std::cout << i << " " << j << std::endl;
		if (i >= width) {
			i = width - 1;
		}
		if (j >= height) {
			j = height - 1;
		}

		const auto colour_scale = 1.0 / 255.0;
		auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;

		return vec3(colour_scale * pixel[0], colour_scale * pixel[1], colour_scale * pixel[2]);
	}

private:
	unsigned char* data;
	int width, height, bytes_per_scanline;
};

#endif // TEXTURE_H