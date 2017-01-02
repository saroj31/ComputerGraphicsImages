#include "RGBtoHSV.h"

#define maximum(x, y, z) ((x) > (y)? ((x) > (z)? (x) : (z)) : ((y) > (z)? (y) : (z))) 
#define minimum(x, y, z) ((x) < (y)? ((x) < (z)? (x) : (z)) : ((y) < (z)? (y) : (z)))


hsvColor RGBtoHSV::rgbtohsvcolor(int ir, int ig, int ib)
{
	hsvColor hsvValue = { 0 };

	double red, green, blue;
	double max, min, delta;
	float h, s, v;

	red = ir; green = ig; blue = ib;  /* r, g, b to 0 - 1 scale */

	max = maximum(red, green, blue);
	min = minimum(red, green, blue);

	v = max;        /* value is maximum of r, g, b */

	if (max == 0) {    /* saturation and hue 0 if value is 0 */
		s = 0;
		h = 0;
	}
	else {
		s = (max - min) / max;           /* saturation is color purity on scale 0 - 1 */

		delta = max - min;
		if (delta == 0)                         /* hue doesn't matter if saturation is 0 */
			h = 0;
		else {
			if (red == max)                    /* otherwise, determine hue on scale 0 - 360 */
				h = (green - blue) / delta;
			else if (green == max)
				h = 2.0 + (blue - red) / delta;
			else /* (green == max) */
				h = 4.0 + (red - green) / delta;

			h = h * 60.0;                       /* change hue to degrees */
			if (h < 0)
				h = h + 360.0;                /* negative hue rotated to equivalent positive around color wheel */
		}
	}

	hsvValue.h = h;
	hsvValue.s = s;
	hsvValue.v = v;
	
	return hsvValue;
}