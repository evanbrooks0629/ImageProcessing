#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

struct Header {
	char idLength;			
	char colorMapType;		
	char dataTypeCode;
	short colorMapOrigin;
	short colorMapLength;
	char colorMapDepth;
	short xOrigin;
	short yOrigin;
	short width;
	short height;
	char bitsPerPixel;
	char imageDescriptor;
};

class TGA {
private:
	Header fileHeader;

	vector<unsigned char> allBytes;

	int numPixels;
public:

	TGA() {}

	vector<unsigned char> getBytes() {
		return allBytes;
	}

	void setBytes(vector<unsigned char> newBytes) {
		allBytes = newBytes;
	}

	void setHeader(Header newHeader) {
		fileHeader = newHeader;
	}

	void loadFile(string fileIn) {
		// reads file and fills class variables with values
		ifstream inFile(fileIn, ios_base::binary);

		// 1. Gather data for the Header object, which describes how to view the file

		inFile.read((char*)&fileHeader.idLength, 1);
		inFile.read((char*)&fileHeader.colorMapType, 1);
		inFile.read((char*)&fileHeader.dataTypeCode, 1);
		inFile.read((char*)&fileHeader.colorMapOrigin, 2);
		inFile.read((char*)&fileHeader.colorMapLength, 2);
		inFile.read((char*)&fileHeader.colorMapDepth, 1);
		inFile.read((char*)&fileHeader.xOrigin, 2);
		inFile.read((char*)&fileHeader.yOrigin, 2);
		inFile.read((char*)&fileHeader.width, 2);
		inFile.read((char*)&fileHeader.height, 2);
		inFile.read((char*)&fileHeader.bitsPerPixel, 1);
		inFile.read((char*)&fileHeader.imageDescriptor, 1);

		// 2. Using data from the Header, gather the image data

		numPixels = fileHeader.width * fileHeader.height;
		// each pixel contains 1 byte for red, 1 byte for green, and 1 byte for blue data

		for (int i = 0; i < numPixels; i++) {
			// store R, G, B in arrays
			// .tga has reverse order --> blue green red NOT red green blue
			unsigned char b;
			inFile.read((char*)&b, 1);
			allBytes.push_back(b);

			unsigned char g;
			inFile.read((char*)&g, 1);
			allBytes.push_back(g);

			unsigned char r;
			inFile.read((char*)&r, 1);
			allBytes.push_back(r);

		}
	}

	void writeFile(string fileOut) {
		ofstream outFile(fileOut, ios_base::binary);

		outFile.write((char*)&fileHeader.idLength, 1);
		outFile.write((char*)&fileHeader.colorMapType, 1);
		outFile.write((char*)&fileHeader.dataTypeCode, 1);
		outFile.write((char*)&fileHeader.colorMapOrigin, 2);
		outFile.write((char*)&fileHeader.colorMapLength, 2);
		outFile.write((char*)&fileHeader.colorMapDepth, 1);
		outFile.write((char*)&fileHeader.xOrigin, 2);
		outFile.write((char*)&fileHeader.yOrigin, 2);
		outFile.write((char*)&fileHeader.width, 2);
		outFile.write((char*)&fileHeader.height, 2);
		outFile.write((char*)&fileHeader.bitsPerPixel, 1);
		outFile.write((char*)&fileHeader.imageDescriptor, 1);

		unsigned char charBytes[numPixels * 3];
		for (int i = 0; i < numPixels * 3; i++) {
			charBytes[i] = allBytes[i];
		}

		outFile.write((char*)&charBytes, sizeof(charBytes));
	}

	int getNumPixels() {
		return numPixels;
	}

	Header getFileHeader() {
		return fileHeader;
	}

};

void write_file(Header newHeader, int numPixels, vector<unsigned char> pixels, string fileOut) {

	ofstream outFile(fileOut, ios_base::binary);

	outFile.write((char*)&newHeader.idLength, 1);
	outFile.write((char*)&newHeader.colorMapType, 1);
	outFile.write((char*)&newHeader.dataTypeCode, 1);
	outFile.write((char*)&newHeader.colorMapOrigin, 2);
	outFile.write((char*)&newHeader.colorMapLength, 2);
	outFile.write((char*)&newHeader.colorMapDepth, 1);
	outFile.write((char*)&newHeader.xOrigin, 2);
	outFile.write((char*)&newHeader.yOrigin, 2);
	outFile.write((char*)&newHeader.width, 2);
	outFile.write((char*)&newHeader.height, 2);
	outFile.write((char*)&newHeader.bitsPerPixel, 1);
	outFile.write((char*)&newHeader.imageDescriptor, 1);

	char charBytes[numPixels * 3];
	for (int i = 0; i < numPixels * 3; i++) {
		charBytes[i] = pixels[i];
	}

	outFile.write((char*)&charBytes, sizeof(charBytes));
}

void multiply(TGA& a, TGA& b, string fileOut) {
	vector<unsigned char> aBytes = a.getBytes();
	vector<unsigned char> bBytes = b.getBytes();
	vector<unsigned char> cBytes;


	if (a.getNumPixels() == b.getNumPixels()) {
		// normalize values to floats
		vector<float> aFloat;
		vector<float> bFloat;
		vector<float> cFloat;

		for (int i = 0; i < a.getNumPixels() * 3; i++) {
			aFloat.push_back((float)((int)aBytes[i]) / (float)255);
			bFloat.push_back((float)((int)bBytes[i]) / (float)255);
			cFloat.push_back(aFloat[i] * bFloat[i]);
		}

		for (int i = 0; i < a.getNumPixels() * 3; i++) {
			cBytes.push_back((char)((cFloat[i] * 255)+0.5f));
		}

		write_file(a.getFileHeader(), a.getNumPixels(), cBytes, fileOut);

	} else {
		cout << "Image dimensions don't match. (Mult)" << endl;
	}
}

void subtract(TGA a, TGA b, string fileOut) {
	// C = A - B

	vector<unsigned char> aBytes = a.getBytes();
	vector<unsigned char> bBytes = b.getBytes();
	vector<unsigned char> cBytes;

	if (a.getNumPixels() == b.getNumPixels()) {
		// normalize values to floats
		vector<int> cInt;

		for (int i = 0; i < a.getNumPixels() * 3; i++) {
			if (((int)((int)aBytes[i]) - (int)bBytes[i]) <= 0) {
				cInt.push_back(0);
			} else {
				cInt.push_back((int)((int)aBytes[i]) - (int)bBytes[i]);
			}
		}

		for (int i = 0; i < a.getNumPixels() * 3; i++) {
			cBytes.push_back((char)(cInt[i]));
		}

		write_file(a.getFileHeader(), a.getNumPixels(), cBytes, fileOut);

	}
	else {
		cout << "Image dimensions don't match. (Sub)" << endl;
	}
}

void screen(TGA a, TGA b, string fileOut) {
	// C = 1 - (1-A)(1-B) --> inverse of multiply

	vector<unsigned char> aBytes = a.getBytes();
	vector<unsigned char> bBytes = b.getBytes();
	vector<unsigned char> cBytes;

	if (a.getNumPixels() == b.getNumPixels()) {
		// normalize values to floats
		vector<float> aFloat;
		vector<float> bFloat;
		vector<float> cFloat;

		for (int i = 0; i < a.getNumPixels() * 3; i++) {
			aFloat.push_back((float)(1-((int)aBytes[i]) / (float)255));
			bFloat.push_back((float)(1-((int)bBytes[i]) / (float)255));
			cFloat.push_back(1-(aFloat[i] * bFloat[i]));
		}

		for (int i = 0; i < a.getNumPixels() * 3; i++) {
			cBytes.push_back((char)((cFloat[i] * 255)+0.5f));
		}

		write_file(a.getFileHeader(), a.getNumPixels(), cBytes, fileOut);

	}
	else {
		cout << "Image dimensions don't match. (Screen)" << endl;
	}
}

void overlay(TGA a, TGA b, string fileOut) {
	// background <= 0.5 --> C = 2*A*B --> multiply but times 2 // >=128
	// background >= 0.5 --> C = 1-2(1-A)(1-B) --> scale but times 2

	vector<unsigned char> aBytes = a.getBytes();
	vector<unsigned char> bBytes = b.getBytes();
	vector<unsigned char> cBytes;

	if (a.getNumPixels() == b.getNumPixels()) {
		// normalize values to floats
		vector<float> aFloat;
		vector<float> bFloat;
		vector<float> cFloat;

		for (int i = 0; i < a.getNumPixels() * 3; i++) {
			if ((int)bBytes[i] <= 127) {
				aFloat.push_back((float)((int)aBytes[i]) / (float)255);
				bFloat.push_back((float)((int)bBytes[i]) / (float)255);
				if ((2 * (aFloat[i] * bFloat[i])) >= 1) {
					cFloat.push_back(1);
				}
				else {
					cFloat.push_back(2 * (aFloat[i] * bFloat[i]));
				}
			} else {
				aFloat.push_back((float)(1 - ((int)aBytes[i]) / (float)255));
				bFloat.push_back((float)(1 - ((int)bBytes[i]) / (float)255));
				if ((1 - (2 * (aFloat[i] * bFloat[i]))) >= 1) {
					cFloat.push_back(1);
				}
				else {
					cFloat.push_back(1 - (2 * (aFloat[i] * bFloat[i])));
				}
				
			}
		}

		for (int i = 0; i < a.getNumPixels() * 3; i++) {
			cBytes.push_back((char)((cFloat[i] * 255)+0.5f));
		}

		write_file(a.getFileHeader(), a.getNumPixels(), cBytes, fileOut);

	}
	else {
		cout << "Image dimensions don't match. (Over)" << endl;
	}
}

void add_color(TGA& a, int red, int green, int blue, string fileOut) {
	// for every group of 3 bytes [b, g, r] add 200 to green --> cap at 255
	vector<unsigned char> aBytes = a.getBytes();
	vector<unsigned char> cBytes;

	int j = 0;
	for (int i = 0; i < a.getNumPixels(); i++) {
		// blue
		if (blue != 0) {
			if ((int)(aBytes[j] + blue) >= 255) {
				cBytes.push_back((char)255);
			}
			else {
				cBytes.push_back((char)((int)(aBytes[j] + blue)));
			}
		}
		else {
			cBytes.push_back(aBytes[j]);
		}
		j++; 

		// green
		if (green != 0) {
			if ((int)(aBytes[j] + green) >= 255) {
				cBytes.push_back((char)255);
			}
			else {
				cBytes.push_back((char)((int)(aBytes[j] + green)));
			}
		}
		else {
			cBytes.push_back(aBytes[j]);
		}
		j++;
		// red
		if (red != 0) {
			if ((int)(aBytes[j] + red) >= 255) {
				cBytes.push_back((char)255);
			}
			else {
				cBytes.push_back((char)((int)(aBytes[j] + red)));
			}
		}
		else {
			cBytes.push_back(aBytes[j]);
		}
		j++;
	}

	write_file(a.getFileHeader(), a.getNumPixels(), cBytes, fileOut);

}

void scale(TGA& a, int red, int green, int blue, string fileOut) {
	vector<unsigned char> aBytes = a.getBytes();
	vector<unsigned char> cBytes;

	int j = 0;
	for (int i = 0; i < a.getNumPixels(); i++) {
		// blue
		if (blue != 0) {
			if (blue != 1) {
				if ((int)(aBytes[j]*blue) >= 255) {
					cBytes.push_back((char)255);
				} else {
					cBytes.push_back((char)((int)(aBytes[j]*blue)));
				}
			} else {
				cBytes.push_back(aBytes[j]);
			}
		}
		else {
			cBytes.push_back(0);
		}
		j++;

		// green
		if (green != 0) {
			if (green != 1) {
				if ((int)(aBytes[j] * green) >= 255) {
					cBytes.push_back((char)255);
				} else {
					cBytes.push_back((char)((int)(aBytes[j] * green)));
				}
			} else {
				cBytes.push_back(aBytes[j]);
			}
		}
		else {
			cBytes.push_back(0);
		}
		j++;
		// red
		if (red != 0) {
			if (red != 1) {
				if ((int)(aBytes[j] * red >= 255)) {
					cBytes.push_back((char)255);
				} else {
					cBytes.push_back((char)((int)(aBytes[j] * red)));
				}
			} else {
			    cBytes.push_back(aBytes[j]);
			}
			
		}
		else {
			cBytes.push_back(0);
		}
		j++;
	}

	write_file(a.getFileHeader(), a.getNumPixels(), cBytes, fileOut);

}

void stream(TGA& a, string fileOutRed, string fileOutGreen, string fileOutBlue) {
	// for every group of 3 bytes [b, g, r] add 200 to green --> cap at 255
	vector<unsigned char> aBytes = a.getBytes();
	vector<unsigned char> redBytes;
	vector<unsigned char> greenBytes;
	vector<unsigned char> blueBytes;

	int j = 0;
	for (int i = 0; i < a.getNumPixels(); i++) {
		// blue
		blueBytes.push_back(aBytes[j]);
		blueBytes.push_back(aBytes[j]);
		blueBytes.push_back(aBytes[j]);
		j+=3;
	}

	int k = 0;
	for (int i = 0; i < a.getNumPixels(); i++) {
		// green
		k++;
		greenBytes.push_back(aBytes[k]);
		greenBytes.push_back(aBytes[k]);
		greenBytes.push_back(aBytes[k]);
		k+=2;
	}

	int l = 0;
	for (int i = 0; i < a.getNumPixels(); i++) {
		// red
		l += 2;
		redBytes.push_back(aBytes[l]);
		redBytes.push_back(aBytes[l]);
		redBytes.push_back(aBytes[l]);
		l++;
	}

	Header newHeader = a.getFileHeader();

	// write blue
	write_file(newHeader, a.getNumPixels(), blueBytes, fileOutBlue);

	// write green
	write_file(newHeader, a.getNumPixels(), greenBytes, fileOutGreen);

	// write red
	write_file(newHeader, a.getNumPixels(), redBytes, fileOutRed);
	
}

void combine(TGA& r, TGA& g, TGA& b, string fileOut) {
	// combine red, green, blue pixels into one file
	vector<unsigned char> rBytes = r.getBytes();
	vector<unsigned char> gBytes = g.getBytes();
	vector<unsigned char> bBytes = b.getBytes();
	vector<unsigned char> cBytes;

	int j = 0;
	for (int i = 0; i < r.getNumPixels(); i++) {
		// blue
		cBytes.push_back(bBytes[j]);
		j++;

		// green
		cBytes.push_back(gBytes[j]);
		j++;

		// red
		cBytes.push_back(rBytes[j]);
		j++;
	}

	write_file(r.getFileHeader(), r.getNumPixels(), cBytes, fileOut);

}

void flip(TGA& a, string fileOut) {
	// rotate 180 degrees
	vector<unsigned char> aBytes = a.getBytes();
	vector<unsigned char> cBytes;

	reverse(aBytes.begin(), aBytes.end());

	// swap [r, g, b] --> [b, g, r]
	int j = 0;
	for (int i = 0; i < a.getNumPixels(); i++) {
		unsigned char r = aBytes[j];
		unsigned char b = aBytes[j + 2];
		cBytes.push_back(b);
		j++;
		cBytes.push_back(aBytes[j]);
		j++;
		cBytes.push_back(r);
		j++;
	}

	write_file(a.getFileHeader(), a.getNumPixels(), cBytes, fileOut);
}

bool compare_header(Header headerA, Header headerB) {
	bool isEqual = true;

	if (headerA.idLength != headerB.idLength) {
		isEqual = false;
	}
	if (headerA.colorMapType != headerB.colorMapType) {
		isEqual = false;
	}
	if (headerA.dataTypeCode != headerB.dataTypeCode) {
		isEqual = false;
	}
	if (headerA.colorMapOrigin != headerB.colorMapOrigin) {
		isEqual = false;
	}
	if (headerA.colorMapLength != headerB.colorMapLength) {
		isEqual = false;
	}
	if (headerA.colorMapDepth != headerB.colorMapDepth) {
		isEqual = false;
	}
	if (headerA.xOrigin != headerB.xOrigin) {
		isEqual = false;
	}
	if (headerA.yOrigin != headerB.yOrigin) {
		isEqual = false;
	}
	if (headerA.width != headerB.width) {
		isEqual = false;
	}
	if (headerA.height != headerB.height) {
		isEqual = false;
	}
	if (headerA.bitsPerPixel != headerB.bitsPerPixel) {
		isEqual = false;
	}
	if (headerA.imageDescriptor != headerB.imageDescriptor) {
		isEqual = false;
	}

	return isEqual;
}

void compare(int i, string file1, string file2) {
	TGA a;
	a.loadFile(file1);

	TGA b;
	b.loadFile(file2);

	// compare header files, then compare byte arrays
	Header headerA = a.getFileHeader();
	Header headerB = b.getFileHeader();

	bool is_header_equal = compare_header(headerA, headerB);

	if (is_header_equal) {
		if (a.getBytes() == b.getBytes()) {
			cout << "Test " << i << " passed!" << endl;
		}
		else {
			cout << "Test " << i << " failed!" << endl;
		}
	}
	else {
		cout << "Test " << i << " failed!" << endl;
	}
	
}

int main() {

	TGA layer1TGA;
	layer1TGA.loadFile("input/layer1.tga");

	TGA layer2TGA;
	layer2TGA.loadFile("input/layer2.tga");

	TGA pattern1TGA;
	pattern1TGA.loadFile("input/pattern1.tga");

	TGA pattern2TGA;
	pattern2TGA.loadFile("input/pattern2.tga");

	TGA carTGA;
	carTGA.loadFile("input/car.tga");

	TGA textTGA;
	textTGA.loadFile("input/text.tga");

	TGA text2TGA;
	text2TGA.loadFile("input/text2.tga");

	TGA circlesTGA;
	circlesTGA.loadFile("input/circles.tga");

	TGA layerRedTGA;
	layerRedTGA.loadFile("input/layer_red.tga");

	TGA layerGreenTGA;
	layerGreenTGA.loadFile("input/layer_green.tga");

	TGA layerBlueTGA;
	layerBlueTGA.loadFile("input/layer_blue.tga");

	// 1) Multiply C = A * B --> A and B are pixels, so each individual byte (3 per pixel) is multiplied by its match
	// "layer1.tga" * "pattern1.tga"
	
	multiply(layer1TGA, pattern1TGA, "output/part1.tga");

	// 2) Subtract C =  A - B --> subtracts the top layer from the bottom layer
	// "car.tga" - "layer1.tga" --> car is bottom, layer is top

	subtract(carTGA, layer2TGA, "output/part2.tga");

	// 3) Multiply "layer1.tga" by "car.tga", store results temporarily, load "text.tga" and, using that as the top layer, combine it with the other image using Screen blending mode

	multiply(layer1TGA, pattern2TGA, "input/test3a.tga");

	TGA test3TGA;
	test3TGA.loadFile("input/test3a.tga");

	screen(test3TGA, textTGA, "output/part3.tga");

	// 4) Multiply "layer2.tga" by "circles.tga", store results temporarily, load "pattern2.tga" and, using that as top layer, subtract the previous result

	multiply(layer2TGA, circlesTGA, "input/test4a.tga");
	
	TGA test4TGA;
	test4TGA.loadFile("input/test4a.tga");

	subtract(test4TGA, pattern2TGA, "output/part4.tga");

	// 5) Combine "layer1.tga" (as top layer) with "pattern1.tga" using the Overlay blending mode

	overlay(layer1TGA, pattern1TGA, "output/part5.tga");

	// 6) load "car.tga" and add 200 to green channel

	add_color(carTGA, 0, 200, 0, "output/part6.tga");

	// 7) load "car.tga" and scale (multiply) red channel by 4 and blue by 0

	scale(carTGA, 4, 1, 0, "output/part7.tga");
	
	// 8) load "car.tga" and write each channel to a separate file
	// stream(tga, fileOut1, fileOut2, fileOut3);

	stream(carTGA, "output/part8_r.tga", "output/part8_g.tga", "output/part8_b.tga");

	// 9) load "layer_red.tga", "layer_green.tga" and "layer_blue.tga" and combine the three files into one file
	// combine(tga, tga, tga, fileOut);

	combine(layerRedTGA, layerGreenTGA, layerBlueTGA, "output/part9.tga");

	// 10) load "text2.tga" and rotate it 180 degrees

	flip(text2TGA, "output/part10.tga");

	// files created, time to test

	compare(1, "examples/EXAMPLE_part1.tga", "output/part1.tga");			// pass
	compare(2, "examples/EXAMPLE_part2.tga", "output/part2.tga");			// pass
	compare(3, "examples/EXAMPLE_part3.tga", "output/part3.tga");			// pass
	compare(4, "examples/EXAMPLE_part4.tga", "output/part4.tga");			// pass
	compare(5, "examples/EXAMPLE_part5.tga", "output/part5.tga");			// pass
	compare(6, "examples/EXAMPLE_part6.tga", "output/part6.tga");			// pass
	compare(7, "examples/EXAMPLE_part7.tga", "output/part7.tga");			// pass
	compare(8, "examples/EXAMPLE_part8_r.tga", "output/part8_r.tga");		// pass
	compare(8, "examples/EXAMPLE_part8_g.tga", "output/part8_g.tga");		// pass
	compare(8, "examples/EXAMPLE_part8_b.tga", "output/part8_b.tga");		// pass
	compare(9, "examples/EXAMPLE_part9.tga", "output/part9.tga");			// pass
	compare(10, "examples/EXAMPLE_part10.tga", "output/part10.tga");		// pass

	return 0;

}
