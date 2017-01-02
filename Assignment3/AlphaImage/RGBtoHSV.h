
typedef struct hsvColor {
	float h;  //Hue in degrees
	float s;  // Saturation in Degrees
	float v;
}hsvcolor;

class RGBtoHSV {

public:

	static hsvColor rgbtohsvcolor(int ir, int ig, int ib);
};

