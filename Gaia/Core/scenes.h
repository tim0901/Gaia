#ifndef SCENES_H
#define SCENES_H

#include "Scene.h"
import sml;
using namespace sml;
#include "../Objects/ObjectList.h"
#include "../Objects/Sphere.h"
#include "../Materials/Material.h"
#include "../Accelerators/BVH.h"
#include "../Maths/Pi.h"

// These functions load scenes to be rendered;

void testSphere(std::shared_ptr<Scene> scene, std::shared_ptr<Flags> flags) {

	Lambertian red(sRGBColour(0.65, 0.05, 0.05), "red");
	scene->InsertMaterial(red);
	Lambertian blue(sRGBColour(0.15, 0.15, 0.85), "blue");
	scene->InsertMaterial(blue);
	Lambertian green(sRGBColour(0.15, 0.65, 0.15), "green");
	scene->InsertMaterial(green);

	scene->xDim = 712;
	scene->yDim = 400;
	scene->samples = 100;
	Vec3d origin = Vec3d(0, 0, 0);
	Vec3d direction = Vec3d(4.0, 0.0, 0.0);
	Vec3d up = Vec3d(0.0, 1.0, 0.0);
	double fov = 20;
	double focalLength = length(direction - origin);
	double aperture = 0.0;// 3.0;
	double aspectRatio = double(scene->xDim) / double(scene->yDim);

	scene->camera = Camera(origin, direction, up, fov, aspectRatio, aperture, focalLength);

	Sphere sphere1(Vec3d(4.0, 0.0, 0.5), 0.5, scene->materialList[0]);
	scene->InsertObject(sphere1);
	Sphere sphere2(Vec3d(4.0, 0.0, -0.5), 0.5, scene->materialList[1]);
	scene->InsertObject(sphere2);
	Sphere sphere3(Vec3d(5.0, 0.0, 0.0), 0.5, scene->materialList[2]);
	scene->InsertObject(sphere3);

	scene->fileName = "test";
}

void bigSmallSheres(std::shared_ptr<Scene> scene, std::shared_ptr<Flags> flags) {

	// +ve X is to the right
	// +ve Y is up
	// -ve Z is into the scene

	Lambertian smallSphere(sRGBColour(0.2, 0.2, 0.2), "smallSphere");
	scene->InsertMaterial(smallSphere);
	Lambertian ground(sRGBColour(0.2, 0.2, 0.2), "ground");
	scene->InsertMaterial(ground);

	Lambertian li(Colour(0.0, 0.0, 0.0), "li");
	scene->InsertMaterial(li);
	Lambertian smallSphere2(sRGBColour(0.2, 0.2, 0.2), "smallSphere2");
	scene->InsertMaterial(smallSphere);

	scene->xDim = 712;
	scene->yDim = 400;
	scene->samples = 1;
	Vec3d origin = Vec3d(0.0, 0.0, 0.0);
	Vec3d direction = Vec3d(0.0, 0.0, -1.0);
	Vec3d up = Vec3d(0.0, 1.0, 0.0);
	double fov = 100;
	double focalLength = length(direction - origin);
	double aperture = 0.0;// 3.0;
	double aspectRatio = double(scene->xDim) / double(scene->yDim);

	scene->camera = Camera(origin, direction, up, fov, aspectRatio, aperture, focalLength);

	// Add spherical area light
	scene->InsertAreaLight(Sphere(Vec3d(0.0, 1.0, 1.0), 0.5, scene->materialList[2]), Vec3d(10.0, 10.0, 10.0));

	Sphere small2(Vec3d(0.0, 0.0, -2.0), 0.25, scene->materialList[3]);
	Sphere small(Vec3d(0.0, 0.0, -1.0), 0.5, scene->materialList[0]);
	Sphere big(Vec3d(0.0, -100.5, -1.0), 100, scene->materialList[1]);
	scene->InsertObject(small2);
	scene->InsertObject(small);
	scene->InsertObject(big);
	scene->fileName = "test";
}

void testTriangle(std::shared_ptr<Scene> scene, std::shared_ptr<Flags> flags) {

	// +ve X is to the right
	// +ve Y is up
	// -ve Z is into the scene

	// Camera!
	scene->xDim = 712;
	scene->yDim = 400;
	scene->samples = 100;
	Vec3d origin = Vec3d(0.0, 0.0, 0.0);
	Vec3d direction = Vec3d(0.0, 0.0, -1.0);
	Vec3d up = Vec3d(0.0, 1.0, 0.0);
	double fov = 100;
	double focalLength = length(direction - origin);
	double aperture = 0.0;// 3.0;
	double aspectRatio = double(scene->xDim) / double(scene->yDim);
	scene->camera = Camera(origin, direction, up, fov, aspectRatio, aperture, focalLength);
	
	// Materials
	Lambertian ground(sRGBColour(0.2, 0.2, 0.2), "ground");
	scene->InsertMaterial(ground);
	Lambertian red(sRGBColour(0.65, 0.05, 0.05), "red");
	scene->InsertMaterial(red);
	Lambertian lightMat(Colour(0.0, 0.0, 0.0), "lightMat");
	scene->InsertMaterial(lightMat);

	// Create a mesh containing a single triangle
	Mesh* mesh = new Mesh(1, 3);
	Vec3f A(0.0, 1.0, -1.0);
	Vec3f B(0.0, 0.0, -1.0);
	Vec3f C(1.0, 0.0, -1.0);
	mesh->vertexPositions[0] = A;
	mesh->vertexPositions[1] = B;
	mesh->vertexPositions[2] = C;
	mesh->vertexIndices.push_back(0);
	mesh->vertexIndices.push_back(1);
	mesh->vertexIndices.push_back(2);
	Triangle tri1(mesh, scene->materialList[1], 0);

	// Add spherical area light
	scene->InsertAreaLight(Sphere(Vec3d(0.0, 1.0, 1.0), 0.5, scene->materialList[2]), Vec3d(10.0, 10.0, 10.0));

	// Add ground sphere
	Sphere groundSphere(Vec3d(0.0, -100.5, -1.0), 100, scene->materialList[0]);
	scene->InsertObject(groundSphere);

	//Add triangle
	scene->InsertObject(tri1);
}

void testMesh(std::shared_ptr<Scene> scene, std::shared_ptr<Flags> flags) {

	// +ve X is to the right
	// +ve Y is up
	// +ve Z is into the scene

	// Camera!
	scene->xDim = 712;
	scene->yDim = 400;
	scene->samples = 10;
	Vec3d origin = Vec3d(1.0,0.75,-1.0);
	Vec3d direction = Vec3d(0.0, -0.25, 1.0);
	Vec3d lookAt = Vec3d(0.0,0.0,0.0);
	direction = lookAt - origin;
	Vec3d up = Vec3d(0.0, 1.0, 0.0);
	double fov = 100;
	double focalLength = length(direction - origin);
	double aperture = 0.0;// 3.0;
	double aspectRatio = double(scene->xDim) / double(scene->yDim);
	scene->camera = Camera(origin, direction, up, fov, aspectRatio, aperture, focalLength);

	// Materials
	Lambertian ground(sRGBColour(0.2, 0.2, 0.2), "ground");
	scene->InsertMaterial(ground);
	Lambertian lightGrey(sRGBColour(0.6,0.6,0.6), "lightGrey");
	scene->InsertMaterial(lightGrey);
	Lambertian lightMat(Colour(0.0, 0.0, 0.0), "lightMat");
	scene->InsertMaterial(lightMat);
	Lambertian red(sRGBColour(0.65, 0.05, 0.05), "red");
	scene->InsertMaterial(red);
	Lambertian green(sRGBColour(0.05, 0.65, 0.05), "green");
	scene->InsertMaterial(green);
	Lambertian blue(sRGBColour(0.05, 0.05, 0.65), "blue");
	scene->InsertMaterial(blue);
	Lambertian purple(sRGBColour(0.65, 0.05, 0.65), "pink");
	scene->InsertMaterial(purple);
	Lambertian yellow(sRGBColour(0.65, 0.65, 0.05), "yellow");
	scene->InsertMaterial(yellow);

	// Add spherical area light
	scene->InsertAreaLight(Sphere(Vec3d(0.0, 1.5, -2.0), 0.5, scene->materialList[2]), Vec3d(100.0, 100.0, 100.0));
	
	std::vector<Material*> meshMaterials;
	meshMaterials.push_back(scene->materialList[0]);

	scene->LoadMesh("Render Assets/cube.obj", Vec3d(-0.5, 0.0, 0.0), meshMaterials, 1);

	// Add ground sphere
	Sphere groundSphere(Vec3d(0.0, -100.5, -1.0), 100, scene->materialList[0]);
	scene->InsertObject(groundSphere);

	flags->savePPM = false;
}

void ShaderBall(std::shared_ptr<Scene> scene, std::shared_ptr<Flags> flags) {

	flags->savePPM = false;
	scene->fileName = "Light sampler test - 2 lights 16 samples";

	// +ve X is to the left
	// +ve Y is up
	// +ve Z is into the scene

	// Camera!
	scene->xDim = 1424;
	scene->yDim = 800;
	scene->samples = 16;
	Vec3d origin = Vec3d(1.0, 1.0, -3.0);
	Vec3d lookAt = Vec3d(0.0, 0.5, 0.0);
	Vec3d direction = lookAt - origin;
	Vec3d up = Vec3d(0.0, 1.0, 0.0);
	double fov = 70;
	double focalLength = length(direction - origin);
	double aperture = 0.0;// 3.0;
	double aspectRatio = double(scene->xDim) / double(scene->yDim);
	scene->camera = Camera(origin, direction, up, fov, aspectRatio, aperture, focalLength);

	// Materials
	Lambertian lightMat(Colour(0.0, 0.0, 0.0), "lightMat");
	scene->InsertMaterial(lightMat);
	Lambertian ground(sRGBColour(0.2, 0.2, 0.2), "ground");
	scene->InsertMaterial(ground);
	Lambertian red(sRGBColour(0.65, 0.05, 0.05), "red");
	scene->InsertMaterial(red);
	Lambertian green(sRGBColour(0.05, 0.65, 0.05), "green");
	scene->InsertMaterial(green);
	Lambertian blue(sRGBColour(0.05, 0.05, 0.65), "blue");
	scene->InsertMaterial(blue);
	Lambertian purple(sRGBColour(0.65, 0.05, 0.65), "pink");
	scene->InsertMaterial(purple);
	Lambertian yellow(sRGBColour(0.65, 0.65, 0.05), "yellow");
	scene->InsertMaterial(yellow);
	Glass glass(1.5, "glass");
	scene->InsertMaterial(glass);


	TrowbridgeReitz trTest(std::make_shared<ConstantTexture>(sRGBColour(0.05, 0.05, 0.65)), 1.5, 0.000001, "trTest"); // 0.394
	scene->InsertMaterial(trTest);

	// Add spherical area light
	scene->InsertAreaLight(Sphere(Vec3d(-1.0, 2.0, -3.0), 0.25, scene->FindMaterial("lightMat")), Vec3d(100.0, 100.0, 100.0));


	//Lambertian earth(std::make_shared<ImageTexture>("Render Assets/earthmap_hd_2.jpg"), "earth");
	AutodeskSurface earth( std::make_shared<ImageTexture>("Render Assets/earthmap_alpha_transparent.jpg"), std::make_shared<ImageTexture>("Render Assets/earthmap_hd_2.jpg"), "earth");
	//AutodeskSurface earth( std::make_shared<ConstantTexture>(sRGBColour(1.0,1.0,1.0)), std::make_shared<ImageTexture>("Render Assets/earthmap_hd_2.jpg"), "earth");
	scene->InsertMaterial(earth);
	scene->InsertObject(Sphere(Vec3d(-2.0, 0.5, 6.0), 4, scene->FindMaterial("earth"))); // Earth Sphere

	std::vector<Material*> meshMaterials;
	meshMaterials.push_back(scene->FindMaterial("glass")); // External material
	meshMaterials.push_back(scene->FindMaterial("ground")); // Internal sphere


	// Load the mesh
	scene->LoadMesh("Render Assets/ShaderBall.obj", Vec3d(0.0, -0.5, 0.0), meshMaterials, 1.0f, 0);
	
	scene->InsertAreaLight(Sphere(Vec3d(2.0, 0.5, 2.0), 0.2, scene->FindMaterial("lightMat")), Vec3d(100.0, 100.0, 100.0));

	// Add spheres
	//scene->InsertObject(Sphere(Vec3d(0.0, 0.5, 0.0), 0.75, scene->FindMaterial("trTest")));
	//scene->InsertObject(Sphere(Vec3d(-1.0, 0.5, -3.0), 0.5, scene->FindMaterial("red")));


	// Add ground sphere
	scene->InsertObject(Sphere(Vec3d(0.0, -100.5, 0.0), 100.0, scene->FindMaterial("ground")));
}

void LightSpiral(std::shared_ptr<Scene> scene, std::shared_ptr<Flags> flags) {

	scene->xDim = 1024;
	scene->yDim = 1024;
	scene->samples = 128;

	scene->fileName = "LightSpiral - " + std::to_string(scene->xDim) + "x" + std::to_string(scene->yDim) + " " + std::to_string(scene->samples) + "s";
	flags->savePPM = false;

	// Camera!
	Vec3d origin = Vec3d(0.0, 1.0, -5.0);
	Vec3d lookAt = Vec3d(0.0, 1.8, 0.0);
	Vec3d direction = lookAt - origin;
	Vec3d up = Vec3d(0.0, 1.0, 0.0);
	double fov = 50;
	double focalLength = length(direction - origin);
	double aperture = 0.0;// 3.0;
	double aspectRatio = double(scene->xDim) / double(scene->yDim);
	scene->camera = Camera(origin, direction, up, fov, aspectRatio, aperture, focalLength);

	// Materials
	Lambertian lightMat(Colour(0.0, 0.0, 0.0), "lightMat");
	scene->InsertMaterial(lightMat);
	Lambertian ground(sRGBColour(0.2, 0.2, 0.2), "ground");
	scene->InsertMaterial(ground);
	Glass glass(1.5, "glass");
	scene->InsertMaterial(glass);

	// Scene

	// Add ground sphere
	scene->InsertObject(Sphere(Vec3d(0.0, -100.5, 0.0), 100.0, scene->FindMaterial("ground")));

	// Add a spiral of lights
	// https://stackoverflow.com/questions/13894715/draw-equidistant-points-on-a-spiral

	int coils = 5;
	double radius = 2.0;
	double rotation = 0.0;
	Vec3d centrePosition(0.0, 0.0, 0.0);
	double maxHeight = 3.0;

	double thetaMax = static_cast<double>(coils) * M_2PI;
	double awayStep = radius / thetaMax;

	// Distance between points
	double chord = 0.2;

	// Size of light
	double lightRadius = 0.05;

	// Luminosity
	Vec3d lightLum(10.0, 10.0, 10.0);

	double away = 0.0;

	for (double theta = chord / awayStep; theta <= thetaMax; theta += chord / away) {

		// How far from centre
		away = awayStep * theta;

		// How far around
		double around = theta + rotation;

		// How far up
		double height = (theta / thetaMax) * maxHeight;

		// Convert to cartesian
		Vec3d location = centrePosition + Vec3d(cos(around) * away, height, sin(around) * away);

		// Add a light at this location
		scene->InsertAreaLight(Sphere(location, lightRadius, scene->FindMaterial("lightMat")), lightLum);
	}
	
	std::cout << scene->lightList.size() << " lights spawned\n";
	
	scene->InsertObject(Sphere(Vec3d(0.0, 2.0, 0.0), 0.5, scene->FindMaterial("glass")));

}


void Lucy(std::shared_ptr<Scene> scene, std::shared_ptr<Flags> flags) {

	flags->savePPM = false;
	scene->fileName = "Lucy";

	// +ve X is to the right
	// +ve Y is up
	// -ve Z is into the scene

	// Camera!
	scene->xDim = 1424;//8640;// 1424;
	scene->yDim = 800; // 15360;// 800;
	scene->samples = 64;// 1;// 64;
	Vec3d origin = Vec3d(0.0, -0.1, 2.0);
	Vec3d lookAt = Vec3d(0.0, 0.0, -1.0);
	Vec3d up = Vec3d(0.0, 1.0, 0.0);
	double fov = 30;
	double focalLength = length(origin - lookAt);
	double aperture = 0.0;// 3.0;
	double aspectRatio = double(scene->xDim) / double(scene->yDim);
	scene->camera = Camera(origin, lookAt, up, fov, aspectRatio, aperture, focalLength);

	// Materials
	Lambertian lightMat(Colour(0.0, 0.0, 0.0), "lightMat");
	scene->InsertMaterial(lightMat);
	Lambertian ground(sRGBColour(0.2, 0.2, 0.2), "ground");
	scene->InsertMaterial(ground);
	Lambertian red(sRGBColour(0.65, 0.05, 0.05), "red");
	scene->InsertMaterial(red);
	Lambertian green(sRGBColour(0.05, 0.65, 0.05), "green");
	scene->InsertMaterial(green);
	Lambertian blue(sRGBColour(0.05, 0.05, 0.65), "blue");
	scene->InsertMaterial(blue);
	Lambertian purple(sRGBColour(0.65, 0.05, 0.65), "pink");
	scene->InsertMaterial(purple);
	Lambertian yellow(sRGBColour(0.65, 0.65, 0.05), "yellow");
	scene->InsertMaterial(yellow);

	// Add spherical area light
	scene->InsertAreaLight(Sphere(Vec3d(0.5, 1.5, 1.5), 0.5, scene->materialList[0]), Vec3d(100.0, 100.0, 100.0));

	std::vector<Material*> meshMaterials;
	meshMaterials.push_back(scene->FindMaterial("ground"));
	meshMaterials.push_back(scene->FindMaterial("ground"));

	// Load the mesh
	//scene->LoadMesh("Render Assets/pbrt-lte-orb.obj", Vec3d(-0.5, 0.0, 0.0), meshMaterials, 3, 3);
	scene->LoadMesh("Render Assets/lucySmall.obj", Vec3d(0.0, -0.5, 0.0), meshMaterials, 0.0005f, 3);

	// Add ground sphere
	scene->InsertObject(Sphere(Vec3d(0.0, -100.5, 0.0), 100.0, scene->materialList[1]));
}

#endif // !SCENES_H
