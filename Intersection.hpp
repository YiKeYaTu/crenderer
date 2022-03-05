//
// Created by lc on 2022/2/22.
//

#ifndef CRENDERER_INTERSECTION_HPP
#define CRENDERER_INTERSECTION_HPP

extern class Object;

class Intersection {
public:
    Intersection() = default;
    explicit Intersection(bool happened): happened_(happened) {}
    Intersection(double t, double tMax, double alpha, double beta, double gamma, double distance, const Vec3f& hitPoint, const Object *intersectedObject)
    : t_(t),
    tMax_(tMax),
    alpha_(alpha),
    beta_(beta),
    gamma_(gamma),
    happened_(true),
    distance_(distance),
    hitPoint_(hitPoint),
    intersectedObject_(intersectedObject) {}

    bool happened() const { return happened_; }
    double t() const { return t_; }
    double tMax() const { return tMax_; }
    double alpha() const { return alpha_; }
    double beta() const { return beta_; }
    double gamma() const { return gamma_; }
    double distance() const { return distance_; }
    Vec3f hitPoint() const { return hitPoint_; }
    const Object* intersectedObject() const { return intersectedObject_; }
private:
    double t_ = std::numeric_limits<double>::max();
    double tMax_ = std::numeric_limits<double>::max();
    double alpha_;
    double beta_;
    double gamma_;
    double distance_;
    Vec3f hitPoint_;
    bool happened_ = false;
    const Object* intersectedObject_;
};

#endif //CRENDERER_INTERSECTION_HPP
