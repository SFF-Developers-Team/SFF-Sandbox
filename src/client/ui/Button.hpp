#pragma once

#include "Node.hpp"
#include "ContainerizedNode.hpp"
#include <functional>
#include <array>

namespace sandbox_ui {
	class Button : public Node, public ContainerizedNode {
	public:
		using Callback = std::function<void(Button*)>;
	protected:
		std::string m_text = "";
		Callback m_clickCallback = nullptr;

		float m_alignedTextPos = 0.f;
		float m_textPosTrig = 0.f;
		float m_textWidth = 0.f;

		double m_time = 0.f;

		Font m_font = {};

		void setColorToRects();
		void setColorToRects(Node::Color customColor);
		Node::Color increaseColBrightness(Node::Color c, unsigned char v);
		Node::Color invertColor(Node::Color c);

		bool m_fontNotModified = true;
	public:
		Button(const std::string& text, Node::Pos buttonSize, float scale = 1.f);

		void update() override;
		void draw() override;

		void setFont(Font fnt);
		void setClickCallback(Callback callback);

		void setColor(Node::Color col) override;

		~Button();
	};
}