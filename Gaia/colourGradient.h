#pragma once
#ifndef COLOURGRADIENT_H
#define COLOURGRADIENT_H

#include "vec3.h"
#include <vector>

class colourGradient {
public:
	colourGradient() {} //Default constructor
	~colourGradient() {} //Destructor

	// Delete the gradient
	void clearGradient() {
		colours.clear();
	}

	//Add a new colour point to the gradient
	void addColourPoint(vec3 rgb, float pos) {

		for (int i = 0; i < colours.size(); i++) {
			//Find where it falls along the gradient
			if (pos < colours[i].position) {
				colours.insert(colours.begin() + 1, colourPoint(rgb, pos)); //Insert into vector
				return; //End early
			}
		}
		colours.push_back(colourPoint(rgb, pos));// Otherwise insert at the end.
	}

	//A 5-colour heatmap gradient.
	void createDefaultHeatMapGradient() {

		colours.clear(); //Allows us to push these to the back safely - we know that that's the correct order. 
		colours.push_back(colourPoint(vec3(0, 0, 1), 0.00)); //Blue
		colours.push_back(colourPoint(vec3(0, 1, 1), 0.25)); //Cyan 
		colours.push_back(colourPoint(vec3(0, 1, 0), 0.50)); //Green
		colours.push_back(colourPoint(vec3(1, 1, 0), 0.75)); //Yellow
		colours.push_back(colourPoint(vec3(1, 0, 0), 1.00)); //Red
	}

	//For a normalized value, assign the correct colour. 
	void returnColourForValue(float value, vec3& rgb) {

		//Check there is a gradient present
		if (colours.size() == 0) {
			return;
		}

		for (int i = 0; i < colours.size(); i++) {
			colourPoint& currentColour = colours[i];

			if (value < currentColour.position) {//If the value isn't big enough to fall into the next section
				colourPoint& previousColour = colours[std::max(0, i - 1)]; // Retrieve the previous colourpoint

				float valueDifference = previousColour.position - currentColour.position;
				float fractionBetweenPoints = (valueDifference == 0) ? 0 : (value - currentColour.position) / valueDifference; //Check for non-zero difference. Then calculate the percentage colour influence of the two points.

				rgb = (previousColour.colour - currentColour.colour) * fractionBetweenPoints + currentColour.colour;
				return;
			}

			//If value = 1.0
			rgb = colours.back().colour;
		}
	}

private:

	//Internal struct to hold the rgb values of each of the 'points' in our heat map. 
	struct colourPoint {
		colourPoint(vec3(rgb), float pos) :colour(rgb), position(pos) {}

		vec3 colour;
		float position;
	};

	std::vector<colourPoint> colours;

};

#endif // !COLOURGRADIENT_H
