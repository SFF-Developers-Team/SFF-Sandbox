#include "Button.hpp"
#include "Rectangle.hpp"
#include "Text.hpp"
#include "NodeRenderer.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>

sandbox_ui::Button::Button(const std::string& text, sandbox_ui::Node::Pos sz, float scale) {
	sz.x *= scale;
	sz.y *= scale;

	m_nodeRect.width = sz.x;
	m_nodeRect.height = sz.y;

	m_font = LoadFont("assets/04B_08__.ttf");

	m_nodeContainer->setScaling(6.f * scale);

	auto base_rect = std::make_shared<sandbox_ui::Rectangle>(m_nodeRect);
	base_rect->setID("base-rect-1");

	auto rect_shadow = std::make_shared<sandbox_ui::Rectangle>(m_nodeRect);
	rect_shadow->setID("rect-shadow");
	rect_shadow->setColor(BLACK);
	rect_shadow->setPosition({ m_nodeContainer->getScaling(), m_nodeContainer->getScaling() });

	auto base_rect2 = std::make_shared<sandbox_ui::Rectangle>(m_nodeRect);
	base_rect2->setID("base-rect-2");
	base_rect2->setMode(base_rect2->Outlined);
	base_rect2->setThickness(6.f * scale);

	m_nodeContainer->addChild(rect_shadow, -1);
	m_nodeContainer->addChild(base_rect);
	m_nodeContainer->addChild(base_rect2);

	auto btn_text = std::make_shared<sandbox_ui::Text>(text);
	btn_text->setFont(m_font);
	btn_text->setBaseTextSize(32.f);
	btn_text->setID("text");
	btn_text->setScale(0.7f * scale);

	m_textWidth = btn_text->getRectangle().width;
	m_alignedTextPos = (sz.x - m_textWidth) / 2.f;

	float alignedY = (sz.y - btn_text->getRectangle().height) / 2.f;
			
	btn_text->setPosition({ m_alignedTextPos, alignedY });

	m_nodeContainer->addChild(btn_text, 1);

	printf("m_textWidth=%f; width=%f\n", m_textWidth, sz.x);

	setColor({ 26, 28, 71, 255 });
}

void sandbox_ui::Button::update() {
	Vector2 mpos = GetMousePosition();
	Vector2 btnPos = getGlobalPosition();

	sandbox_ui::Node::Rect r = getRectangle();
	r.x = btnPos.x;
	r.y = btnPos.y;
	r.width *= getScale();
	r.height *= getScale();

	if (CheckCollisionPointRec(mpos, r)) {
		// howered
		Node::Color n = increaseColBrightness(getColor(), 15);
		setColorToRects(n);

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			if (m_clickCallback != nullptr) {
				m_clickCallback(this);
			}
		}
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			float limit = m_currentRenderer->getScaling();
			setRenderOffset({ limit, limit });
		}
		else {
			setRenderOffset({ 0, 0 });
		}
	}
	else {
		setColorToRects();
		setRenderOffset({ 0, 0 });
	}
}

void sandbox_ui::Button::draw() {
	m_nodeContainer->setPosition(getPosition());
		
	auto btn_text = m_nodeContainer->getChildById("text");
	Vector2 old_pos = {};

	if (btn_text.has_value()) {
		old_pos = btn_text.value()->getPosition();
	}

	if (m_textWidth > m_nodeRect.width) {
		m_time += GetFrameTime();
	
		float limit = m_nodeContainer->getScaling() * 2.f;
		float area_unshown = (m_textWidth - m_nodeRect.width) + limit;

		m_textPosTrig = (float)std::sin(m_time) * (area_unshown) / 2.f;

		if (btn_text.has_value()) {
			auto new_pos = old_pos;
			new_pos.x += m_textPosTrig;

			//printf("new_pos = %f %f\n", new_pos.x, new_pos.y);
			
			btn_text.value()->setPosition(new_pos);
		}
	}

	float padding = m_nodeContainer->getScaling();

	BeginScissorMode(m_nodeRect.x - padding, m_nodeRect.y - padding, m_nodeRect.width + (padding * 2.f), m_nodeRect.height + (padding * 2.f));

	m_nodeContainer->setParent(this);
	m_nodeContainer->render();

	EndScissorMode();

	if (btn_text.has_value()) {
		btn_text.value()->setPosition(old_pos);
	}
}

void sandbox_ui::Button::setFont(Font fnt) {
	if (m_fontNotModified) {
		UnloadFont(m_font);
	}

	m_font = fnt;
	auto btn_text_nd = m_nodeContainer->getChildById("text");

	if (!btn_text_nd.has_value()) return;

	if (auto btn_text = dynamic_cast<sandbox_ui::Text*>(btn_text_nd.value().get())) {
		btn_text->setFont(fnt);

		m_textWidth = btn_text->getRectangle().width;
		m_alignedTextPos = m_textWidth / 2.f;
	}
	
	m_fontNotModified = false;
}
void sandbox_ui::Button::setClickCallback(Callback callback) {
	m_clickCallback = callback;
}

void sandbox_ui::Button::setColorToRects() {
	auto rect1 = m_nodeContainer->getChildById("base-rect-1");
	auto rect2 = m_nodeContainer->getChildById("base-rect-2");

	sandbox_ui::Node::Color c1 = getColor();
	sandbox_ui::Node::Color c2 = increaseColBrightness(c1, 15);

	if (rect1.has_value()) {
		rect1.value()->setColor(c1);
	}
	if (rect2.has_value()) {
		rect2.value()->setColor(c2);
	}
}

void sandbox_ui::Button::setColorToRects(Node::Color customColor) {
	auto rect1 = m_nodeContainer->getChildById("base-rect-1");
	auto rect2 = m_nodeContainer->getChildById("base-rect-2");

	sandbox_ui::Node::Color c1 = customColor;
	sandbox_ui::Node::Color c2 = increaseColBrightness(c1, 15);

	if (rect1.has_value()) {
		rect1.value()->setColor(c1);
	}
	if (rect2.has_value()) {
		rect2.value()->setColor(c2);
	}
}

sandbox_ui::Node::Color sandbox_ui::Button::increaseColBrightness(Node::Color c, unsigned char v) {
	unsigned char r = std::clamp(c.r + v, 0, 255);
	unsigned char g = std::clamp(c.g + v, 0, 255);
	unsigned char b = std::clamp(c.b + v, 0, 255);

	Color n = { r, g, b, 255 };

	if ((c.r == n.r) && (c.g == n.g) && (c.b == n.b)) {
		n = BLACK;
		n.a = c.a;
	}

	return n;
}

sandbox_ui::Node::Color sandbox_ui::Button::invertColor(sandbox_ui::Node::Color c) {
	Color n = c;

	n.r = ~c.r;
	n.g = ~c.g;
	n.b = ~c.b;

	return n;
}

void sandbox_ui::Button::setColor(Node::Color col) {
	Node::setColor(col);

	setColorToRects();
}

sandbox_ui::Button::~Button() {
	if (m_fontNotModified) {
		UnloadFont(m_font);
	}
}