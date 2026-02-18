/* -*- mode: c; c-file-style: "openbsd" -*- */
/*
 * Copyright (c) 2025 Vincent Bernat <bernat@luffy.cx>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "term.h"

#include <math.h>

/* CIE LAB color for palette interpolation */
typedef struct {
	double L;
	double a;
	double b;
} LABColor;

/* The following is converted from Python.
 * See: https://github.com/jake-stewart/color256/blob/main/color256.py
 */

static double
srgb_to_linear(double c)
{
	return c <= 0.04045 ? c / 12.92 : pow((c + 0.055) / 1.055, 2.4);
}

static double
linear_to_srgb(double c)
{
	if (c <= 0.0) return 0.0;
	if (c >= 1.0) return 1.0;
	return c <= 0.0031308 ? c * 12.92 : 1.055 * pow(c, 1.0 / 2.4) - 0.055;
}

static LABColor
rgb_to_lab(const GdkRGBA *c)
{
	double r = srgb_to_linear(c->red);
	double g = srgb_to_linear(c->green);
	double b = srgb_to_linear(c->blue);

	double x = (0.4124 * r + 0.3576 * g + 0.1805 * b) / 0.95047;
	double y = (0.2126 * r + 0.7152 * g + 0.0722 * b) / 1.0;
	double z = (0.0193 * r + 0.1192 * g + 0.9505 * b) / 1.08883;

	double fx = x > 0.008856 ? cbrt(x) : 7.787 * x + 16.0 / 116.0;
	double fy = y > 0.008856 ? cbrt(y) : 7.787 * y + 16.0 / 116.0;
	double fz = z > 0.008856 ? cbrt(z) : 7.787 * z + 16.0 / 116.0;

	return (LABColor){ 116.0 * fy - 16.0, 500.0 * (fx - fy), 200.0 * (fy - fz) };
}

static GdkRGBA
lab_to_rgb(LABColor lab)
{
	double fy = (lab.L + 16.0) / 116.0;
	double fx = lab.a / 500.0 + fy;
	double fz = fy - lab.b / 200.0;

	double x = (fx > 6.0/29.0 ? fx * fx * fx : (fx - 16.0/116.0) / 7.787) * 0.95047;
	double y = (fy > 6.0/29.0 ? fy * fy * fy : (fy - 16.0/116.0) / 7.787) * 1.0;
	double z = (fz > 6.0/29.0 ? fz * fz * fz : (fz - 16.0/116.0) / 7.787) * 1.08883;

	double r =  3.2406 * x - 1.5372 * y - 0.4986 * z;
	double g = -0.9689 * x + 1.8758 * y + 0.0415 * z;
	double b =  0.0557 * x - 0.2040 * y + 1.0570 * z;

	return (GdkRGBA){ linear_to_srgb(r), linear_to_srgb(g), linear_to_srgb(b), 0 };
}

static LABColor
lerp_lab(double t, LABColor a, LABColor b)
{
	return (LABColor){
		a.L + t * (b.L - a.L),
		a.a + t * (b.a - a.a),
		a.b + t * (b.b - a.b),
	};
}

/* Generate 240 extended colors (indices 16-255) from the base 8 colors
 * using trilinear interpolation in LAB colorspace.
 */
void
generate_palette(GdkRGBA *palette, const GdkRGBA *bg, const GdkRGBA *fg)
{
	LABColor base8_lab[8];
	for (int i = 0; i < 8; i++)
		base8_lab[i] = rgb_to_lab(&palette[i]);
	LABColor bg_lab = rgb_to_lab(bg);
	LABColor fg_lab = rgb_to_lab(fg);

	/* 6x6x6 color cube (216 colors) */
	int idx = 16;
	for (int r = 0; r < 6; r++) {
		LABColor c0 = lerp_lab(r / 5.0, bg_lab, base8_lab[1]);
		LABColor c1 = lerp_lab(r / 5.0, base8_lab[2], base8_lab[3]);
		LABColor c2 = lerp_lab(r / 5.0, base8_lab[4], base8_lab[5]);
		LABColor c3 = lerp_lab(r / 5.0, base8_lab[6], fg_lab);
		for (int g = 0; g < 6; g++) {
			LABColor c4 = lerp_lab(g / 5.0, c0, c1);
			LABColor c5 = lerp_lab(g / 5.0, c2, c3);
			for (int b = 0; b < 6; b++) {
				LABColor c6 = lerp_lab(b / 5.0, c4, c5);
				palette[idx++] = lab_to_rgb(c6);
			}
		}
	}

	/* Grayscale ramp (24 colors) */
	for (int i = 0; i < 24; i++) {
		double t = (i + 1) / 25.0;
		palette[idx++] = lab_to_rgb(lerp_lab(t, bg_lab, fg_lab));
	}
}
