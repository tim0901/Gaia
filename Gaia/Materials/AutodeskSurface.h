#ifndef AUTODESKSURFACE_H
#define AUTODESKSURFACE_H

#include "Material.h"

enum AutodeskNode {
    transparency = 1,
    col = 2
};

// An implementation of the Autodesk Standard Surface
// Note: the Autodesk Standard Surface is a non-reciprocal BSDF and as such cannot be used with 
// integrators that rely on this property, such as a bidirectional path integrator
class AutodeskSurface : public Material {
public:
    AutodeskSurface(std::shared_ptr<Texture> trans, std::shared_ptr<Texture> col, std::string n) : colour(col) {
        this->AddAlphaMap(trans);
        this->name = n;
    }
    
    Vec3d EmittedRadiance(const Ray& r, IntersectionRecord& iRec) const { return Vec3d(0.0, 0.0, 0.0); }

    Vec3d SampleAndEvaluate(const Ray& r, IntersectionRecord& iRec, ScatteringRecord& sRec) const {

        return Vec3d(0.0, 0.0, 0.0);
    }

    double EvaluatePDFFromDirections(const Vec3d& in, const Vec3d& scattered, const Vec3d& surfaceNormal) const {
        // Evaluate the PDF here
        if (dot(in, scattered) > 0) {
            return rng.RandomCosineHemispherePDF(scattered, surfaceNormal);
        }
        return 0.0;
    }

    // Create a new ray direction based on the reflective properties of the surface
    void SampleDirection(const Ray& r, const IntersectionRecord& iRec, ScatteringRecord& sRec) const {

        // 1. Find which node was used to sample the material - we need to scatter
        // accordingly - we don't want a cosine sample when we have just sampled
        // a specular or microfacet node

        if (iRec.autodeskNodeType == AutodeskNode::transparency) {
            sRec.scatteredRay = Ray(iRec.p, -r.direction);
            sRec.scatteringPDF = 0.0;
            return;
        }

        // A newly sampled direction in local coordinates
        Vec3d newDirection = rng.RandomCosineHemisphere();

        if (dot(iRec.normal, newDirection) < 0.0) {
            // Wrong hemisphere
            newDirection *= -1;
        }

        sRec.scatteredRay = Ray(iRec.p, unit_vector(newDirection));
        sRec.scatteringPDF = EvaluatePDFFromDirections(r.direction, sRec.scatteredRay.direction, iRec.normal);
        
    }

    Vec3d ScatteredRadiance(const Vec3d& inDirection, const Vec3d& outDirection, IntersectionRecord& iRec, ScatteringRecord& sRec) const {

        Vec3d opacity = this->EvaluateAlphaMap(iRec);
        Vec3d colourValue = colour->SampleTexture(iRec);

        if (opacity == Vec3d(0.0,0.0,0.0)) {
            // transmission = 100%. We should not be transmitting this ray
            iRec.autodeskNodeType = AutodeskNode::transparency;
        }

        // Opacity is dealt with during the intersection test

        return colourValue;
    }
    
    std::shared_ptr<Texture> colour;
};

#endif // !AUTODESKSURFACE_H
