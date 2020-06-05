//
//  bvh.cpp
//  Gaia
//
//  Created by Alex Richardson on 15/03/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//
#include "pch.h"

#include "bvh.h"

bool bvh_node::bounding_box(float t0, float t1, aabb& b) const {
    b = box;
    return true;
}

bool bvh_node::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    if (box.hit(r, t_min, t_max)) {
        rec.object_id = object_id;
        hit_record left_rec, right_rec;
        bool hit_left = left->hit(r, t_min, t_max, left_rec);
        bool hit_right = right->hit(r, t_min, t_max, right_rec);
        if (hit_left && hit_right) {
            if (left_rec.t < right_rec.t)
                rec = left_rec;
            else
                rec = right_rec;
            return true;
        }
        else if (hit_left) {
            rec = left_rec;
            return true;
        }
        else if (hit_right) {
            rec = right_rec;
            return true;
        }
        else
            return false;
    }
    else return false;
}

bvh_node::bvh_node(object **l, int n, float time0, float time1) {
    int axis = int(3 * drand48());
    if (axis == 0)
        qsort(l, n, sizeof(object *), box_x_compare);
    else if (axis == 1)
        qsort(l, n, sizeof(object *), box_y_compare);
    else
        qsort(l, n, sizeof(object *), box_z_compare);
    if (n == 1) {
        left = right = l[0];
        left->type = right->type = l[0]->type;
    }
    else if (n == 2) {
        left = l[0];
        left->type = l[0]->type;
        right = l[1];
        right->type = l[1]->type;
    }
    else {
        left = new bvh_node(l, n / 2, time0, time1);
        left->type = "bvh_node";
        right = new bvh_node(l + n / 2, n - n / 2, time0, time1);
        right->type = "bvh_node";
    }
    aabb box_left, box_right;
    if (!left->bounding_box(time0, time1, box_left) || !right->bounding_box(time0, time1, box_right))
        std::cerr << "no bounding box in bvh_node constructor\n";
    box = surrounding_box(box_left, box_right);
}

int box_x_compare(const void * a, const void * b) {
    aabb box_left, box_right;
    object *ah = *(object**)a;
    object *bh = *(object**)b;
    if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
        std::cerr << "no bounding box in bvh_node constructor\n";
    if (box_left.min().x() - box_right.min().x() < 0.0)
        return -1;
    else
        return 1;
}

int box_y_compare(const void * a, const void * b)
{
    aabb box_left, box_right;
    object *ah = *(object**)a;
    object *bh = *(object**)b;
    if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
        std::cerr << "no bounding box in bvh_node constructor\n";
    if (box_left.min().y() - box_right.min().y() < 0.0)
        return -1;
    else
        return 1;
}
int box_z_compare(const void * a, const void * b)
{
    aabb box_left, box_right;
    object *ah = *(object**)a;
    object *bh = *(object**)b;
    if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
        std::cerr << "no bounding box in bvh_node constructor\n";
    if (box_left.min().z() - box_right.min().z() < 0.0)
        return -1;
    else
        return 1;
}
