#pragma once

#include <raylib.h>
#include <string>

namespace sandbox_ui {
	class NodeRenderer;

	class Node {
	public:
		using Rect = ::Rectangle;
		using Color = Color;
		using Pos = Vector2;
	protected:
		Rect m_nodeRect = {};
		std::string m_id = "";
		float m_size = 1.f;
		Color m_color = WHITE;

		NodeRenderer* m_currentRenderer = nullptr;
	public:
		Node();

		virtual void update();
		virtual void draw();

		Pos getPosition();
		void setPosition(Pos pos);

		void setID(const std::string& id);
		std::string getID();

		void setRenderer(NodeRenderer* renderer);
		NodeRenderer* getRenderer();

		float getScale();
        void setSize(float sz);

		Color getColor();
        void setColor(Color col);

		Node::Rect getRectangle();
	};
}