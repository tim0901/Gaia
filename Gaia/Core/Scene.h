#ifndef SCENE_H
#define SCENE_H

#include "Camera.h"
#include "../Objects/Object.h"
#include "../Materials/Material.h"
#include "Light.h"
#include "../Accelerators/BVH.h"

// A scene containing a camera, a list of objects and the materials they use
class Scene{
public:
	Scene() {}
	~Scene() {
		// Delete each material stored in the array
		for (int i = 0; i < materialList.size(); i++) {
			delete materialList[i];
		}
		materialList.clear();

		// Delete each light stored in the array
		for (int i = 0; i < lightList.size(); i++) {
			delete lightList[i];
		}
		lightList.clear();

		// Delete each mesh
		for (int i = 0; i < meshList.size(); i++) {
			delete meshList[i];
		}
		meshList.clear();
	}

	bool Intersect(Ray r, IntersectionRecord& rec, double tMin = 0.001, double tMax = DBL_MAX) const {
		return objectList.Intersect(r, rec, tMin, tMax);
	}

	// Return the transmittance between two points - for now this is a binary value based 
	// solely on the geometry of the 
	Vec3d TransmittanceBetween(const Vec3d& p1, const Vec3d& p2) const;

	// Insert a material into the list
	template <typename T>
	void InsertMaterial(const T& mat) {
		materialList.push_back(new T(mat));
	}
	
	Material* FindMaterial(const std::string materialName) const {
		for (int i = 0; i < materialList.size(); i++) {
			if (materialList[i]->name == materialName) {
				return materialList[i];
			}
		}
		std::cout << "No material found with name: " << materialName << "\n";
		return nullptr;
	}

	// Add an object to the scene
	template<typename T>
	void InsertObject(const T& obj) {
		objectList.InsertObject(obj);
	}
	template<typename T>
	void InsertObject(const std::shared_ptr<T>& obj) {
		objectList.InsertObject(obj);
	}

	// Add a light to the scene
	template<typename T>
	void InsertLight(const T& light) {
		lightList.push_back(light);
	}

	// Add an area light to the scene
	// This method ensures the light and geometry are linked properly
	template <typename T>
	void InsertAreaLight(const T& light) {
		lightList.push_back(new T(light));
		lightList.back()->ReturnAttachedObject()->AttachLight(lightList.back());
	}
	template<typename T>
	void InsertAreaLight(const T& obj, const Vec3d& lum) {
		this->InsertObject(obj);
		this->InsertAreaLight(AreaLight(this->objectList.list.back(), lum));
	}
	template<typename T>
	void InsertAreaLight(const std::shared_ptr<T>& obj, const Vec3d& lum) {
		this->InsertObject(obj);
		this->InsertAreaLight(AreaLight(this->objectList.list.back(), lum));
	}

	Light* ChooseLight() {
		return lightList[0];
	}

	// Load a mesh from file, create a BVH and add it to the scene
	void LoadMesh(const std::string file, const Vec3d& location, std::vector<Material*> mats, const float& scale = 1.0f, const int& refinementLevel = 0) {
		Mesh* mesh = new Mesh(file, location, mats, scale, refinementLevel);
		this->InsertObject(std::make_shared<BVH>(mesh->tris));
		meshList.push_back(mesh);
	}

	int xDim = 0;
	int yDim = 0;
	int samples = 0;
	Camera camera;
	ObjectList objectList;

	std::vector<Mesh*> meshList;
	std::vector<Light*> lightList;
	std::vector<Material*> materialList;
	std::string fileName = "UntitledRender";
};



#endif // !SCENE_H
