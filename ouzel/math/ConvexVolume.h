// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "math/Box3.h"
#include "math/Plane.h"
#include "math/Vector4.h"

namespace ouzel
{
    class ConvexVolume
    {
    public:
        ConvexVolume() {}
        ConvexVolume(const std::vector<Plane>& aPlanes):
            planes(aPlanes)
        {
        }

        bool isPointInside(const Vector4& position)
        {
            for (const Plane& plane : planes)
            {
                if (plane.dot(position) < 0.0f)
                {
                    return false;
                }
            }

            return true;
        }

        bool isSphereInside(const Vector4& position, float radius)
        {
            for (const Plane& plane : planes)
            {
                if (plane.dot(position) < -radius)
                {
                    return false;
                }
            }

            return true;
        }

        bool isBoxInside(const Box3& box)
        {
            for (const Plane& plane : planes)
            {
                if (plane.dot(Vector4(box.min.v[0], box.min.v[1], box.min.v[2], 1.0f)) >= 0.0f)
                {
                    continue;
                }

                if (plane.dot(Vector4(box.max.v[0], box.min.v[1], box.min.v[2], 1.0f)) >= 0.0f)
                {
                    continue;
                }

                if (plane.dot(Vector4(box.min.v[0], box.max.v[1], box.min.v[2], 1.0f)) >= 0.0f)
                {
                    continue;
                }

                if (plane.dot(Vector4(box.min.v[0], box.min.v[1], box.max.v[2], 1.0f)) >= 0.0f)
                {
                    continue;
                }

                if (plane.dot(Vector4(box.max.v[0], box.max.v[1], box.min.v[2], 1.0f)) >= 0.0f)
                {
                    continue;
                }

                if (plane.dot(Vector4(box.max.v[0], box.min.v[1], box.max.v[2], 1.0f)) >= 0.0f)
                {
                    continue;
                }

                if (plane.dot(Vector4(box.min.v[0], box.max.v[1], box.max.v[2], 1.0f)) >= 0.0f)
                {
                    continue;
                }

                if (plane.dot(Vector4(box.max.v[0], box.max.v[1], box.max.v[2], 1.0f)) >= 0.0f)
                {
                    continue;
                }

                return false;
            }

            return true;
        }

        std::vector<Plane> planes;
    };
}
