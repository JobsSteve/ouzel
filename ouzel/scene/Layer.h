// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <vector>
#include "scene/NodeContainer.h"
#include "math/Vector2.h"

namespace ouzel
{
    namespace scene
    {
        class Scene;
        class Node;
        class Camera;

        class Layer: public NodeContainer
        {
            friend Scene;
            friend Camera;
        public:
            Layer();
            virtual ~Layer();

            virtual void draw();

            const std::vector<Camera*>& getCameras() const { return cameras; }

            Node* pickNode(const Vector2& position, bool renderTargets = false) const;
            std::vector<Node*> pickNodes(const Vector2& position, bool renderTargets = false) const;
            std::vector<Node*> pickNodes(const std::vector<Vector2>& edges, bool renderTargets = false) const;

            int32_t getOrder() const { return order; }
            void setOrder(int32_t newOrder);

            Scene* getScene() const { return scene; }
            void removeFromScene();

        protected:
            virtual void addChildNode(Node* node) override;

            void addCamera(Camera* camera);
            void removeCamera(Camera* camera);

            virtual void recalculateProjection();
            virtual void enter() override;

            Scene* scene = nullptr;

            std::vector<Camera*> cameras;

            int32_t order = 0;
        };
    } // namespace scene
} // namespace ouzel
