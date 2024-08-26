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

		Pos m_renderingOffset = {};

		int m_order = 0;

		bool m_visible = true;
		bool m_limitRendering = false;

		NodeRenderer* m_currentRenderer = nullptr;

		bool m_updateRequired = true;
	public:
		Node();

		virtual void update();
		virtual void draw();

		Pos getPosition();
		virtual void setPosition(Pos pos);

		virtual void setID(const std::string& id);
		std::string getID();

		void setRenderer(NodeRenderer* renderer);
		NodeRenderer* getRenderer();

		float getScale();
        virtual void setScale(float sz);

		Color getColor();
        virtual void setColor(Color col);

		Node::Rect getRectangle();

		virtual void setZOrder(int zOrder);
		int getZOrder();

		bool isVisible();
		virtual void setVisible(bool flag);

		Pos getGlobalPosition();

		virtual void limitRenderToRect(bool flag);
		bool renderingLimited();

		Pos getRenderOffset();
		virtual void setRenderOffset(Pos offset);

		virtual void setSize(float width, float height);
		virtual void setSize(Pos sz);

		virtual void setWidth(float width);
		virtual void setHeight(float height);

		void unscheduleUpdate();
		void scheduleUpdate();
		bool updateRequired();
	};
}