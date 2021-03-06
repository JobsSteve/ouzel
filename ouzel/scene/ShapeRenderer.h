// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "scene/Component.h"
#include "graphics/Renderer.h"
#include "math/Color.h"
#include "graphics/BlendState.h"
#include "graphics/Buffer.h"
#include "graphics/MeshBuffer.h"
#include "graphics/Shader.h"

namespace ouzel
{
    namespace scene
    {
        class ShapeRenderer: public Component
        {
        public:
            ShapeRenderer();

            virtual void draw(const Matrix4& transformMatrix,
                              const Color& drawColor,
                              const Matrix4& renderViewProjection,
                              const std::shared_ptr<graphics::Texture>& renderTarget,
                              const Rectangle& renderViewport,
                              bool depthWrite,
                              bool depthTest,
                              bool wireframe,
                              bool scissorTest,
                              const Rectangle& scissorRectangle) override;

            void clear();

            void line(const Vector2& start,
                      const Vector2& finish,
                      const Color& color,
                      float thickness = 1.0f);

            void circle(const Vector2& position,
                        float radius,
                        const Color& color,
                        bool fill = false,
                        uint32_t segments = 16,
                        float thickness = 1.0f);

            void rectangle(const Rectangle& rectangle,
                           const Color& color,
                           bool fill = false,
                           float thickness = 1.0f);

            void polygon(const std::vector<Vector2>& edges,
                         const Color& color,
                         bool fill = false,
                         float thickness = 1.0f);

            void curve(const std::vector<Vector2>& controlPoints,
                       const Color& color,
                       uint32_t segments = 16,
                       float thickness = 1.0f);

            virtual const std::shared_ptr<graphics::Shader>& getShader() const { return shader; }
            virtual void setShader(const std::shared_ptr<graphics::Shader>& newShader) { shader = newShader; }

            virtual const std::shared_ptr<graphics::BlendState>& getBlendState() const { return blendState; }
            virtual void setBlendState(const std::shared_ptr<graphics::BlendState>& newBlendState)  { blendState = newBlendState; }

        protected:
            struct DrawCommand
            {
                graphics::Renderer::DrawMode mode;
                uint32_t indexCount;
                uint32_t startIndex;
            };

            std::shared_ptr<graphics::Shader> shader;
            std::shared_ptr<graphics::BlendState> blendState;
            std::shared_ptr<graphics::MeshBuffer> meshBuffer;
            std::shared_ptr<graphics::Buffer> indexBuffer;
            std::shared_ptr<graphics::Buffer> vertexBuffer;

            std::vector<DrawCommand> drawCommands;

            std::vector<uint16_t> indices;
            std::vector<ouzel::graphics::VertexPC> vertices;
            bool dirty = false;
        };
    } // namespace scene
} // namespace ouzel
