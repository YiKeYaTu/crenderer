//
// Created by lc on 2022/2/22.
//

#ifndef CRENDERER_INTERSECTION_HPP
#define CRENDERER_INTERSECTION_HPP

class Intersection {
public:
    bool happened() { return happened_; }
    double t() { return t_; }
private:
    double t_ = std::numeric_limits<double>::max();
    bool happened_ = false;
};

#endif //CRENDERER_INTERSECTION_HPP
