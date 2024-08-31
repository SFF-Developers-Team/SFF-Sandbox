#pragma once

#include <raylib.h>
#include <string>
#include <memory>
#include <vector>

#include "NodeAction.hpp"

namespace sandbox_ui {
	class NodeRenderer;

	class Node {
	public:
		using Rect = ::Rectangle;
		using Color = Color;
		using Pos = Vector2;

		using ModType = float*;
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

		std::vector<std::shared_ptr<NodeAction>> m_actions;

		bool m_shouldDestroy = false;
	public:
		Node();

		virtual void update();
		virtual void draw();

		Pos getPosition();
		virtual void setPosition(Pos pos);

		virtual void setPositionX(float x);
		virtual void setPositionY(float x);

		float getPositionX();
		float getPositionY();

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

		float getWidth();
		float getHeight();

		void unscheduleUpdate();
		void scheduleUpdate();
		bool updateRequired();

		void updateActions();
		void runAction(NodeAction act);

		virtual void modifyParam(const std::string &param, double value);
	
		void release();
		bool shouldRelease();
	};
}