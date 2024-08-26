#include "InitialMenu.hpp"
#include "NodeRenderer.hpp"
#include "Text.hpp"
#include "Rectangle.hpp"
#include "CallbackNode.hpp"
#include "Button.hpp"
#include <cmath>

void sandbox_ui::InitialMenu::draw() {
	Node::draw();

	m_nodeRect.x = 0;
	m_nodeRect.y = 0;
	m_nodeRect.width = GetRenderWidth();
	m_nodeRect.height = GetRenderHeight();

	// DrawRectangleGradientV(m_nodeRect.x, m_nodeRect.y, m_nodeRect.width, m_nodeRect.height, GRAY, WHITE);

	m_nodeContainer->setPosition(getPosition());
	m_nodeContainer->setScaling(m_currentRenderer->getScaling());

	m_nodeContainer->render();
}

sandbox_ui::InitialMenu::InitialMenu(){
	m_nodeRect.x = 0;
	m_nodeRect.y = 0;
	m_nodeRect.width = GetRenderWidth();
	m_nodeRect.height = GetRenderHeight();

	Color bg1 = BLACK;
	bg1.a = 128;

	auto rect2 = std::make_shared<sandbox_ui::Rectangle>(m_nodeRect);
	rect2->setColor(bg1);

	m_nodeContainer->addChild(rect2, 0);

	float spacing = 20.f;

	auto buttons = buildButtons({
		{"SINGLEPLAYER", nullptr},
		{"MULTIPLAYER", nullptr},
		{"OPTIONS", nullptr},
	}, { 100, 100 });

	m_nodeContainer->addChild(buttons, 10);

	auto gradNode = std::make_shared<CallbackNode>();
	gradNode->setDrawCallback([this](CallbackNode* nd) {
		auto rect = nd->getRectangle();

		Color c1 = BLACK;
		Color c2 = c1;

		c1.a = 255;
		c2.a = 0;
		
		DrawRectangleGradientH(rect.x, rect.y, rect.width, rect.height, c1, c2);
	});

	auto gradRect = m_nodeRect;
	gradRect.width /= 1.5f;

	gradNode->setSize({ gradRect.width, gradRect.height });

	m_nodeContainer->addChild(gradNode, 1);
}

void sandbox_ui::InitialMenu::update() {
	m_timeTest += (double)GetFrameTime();

	auto textTest = m_nodeContainer->getChildById("text-test");
	if (textTest.has_value()) {
		textTest.value()->setPosition(GetMousePosition());
	}

	auto rectTest = m_nodeContainer->getChildById("rect-test");
	if (rectTest.has_value()) {
		auto pos = rectTest.value()->getPosition();

		rectTest.value()->setPosition({pos.x, 100 * (float)sin(m_timeTest * 4.f) + 200});
	}
}

std::vector<std::shared_ptr<sandbox_ui::Node>> sandbox_ui::InitialMenu::buildButtons(const std::unordered_map<std::string, std::function<void(Button*)>>& btnMapping, Node::Pos basePos) {
	Vector2 posTarget = basePos;

	float spacing = 20.f;

	std::vector<std::shared_ptr<sandbox_ui::Node>> result;

	for (auto& [k, v] : btnMapping) {
		auto btn = std::make_shared<sandbox_ui::Button>(k, Vector2{ 300.f, 40.f });
		btn->setPosition(posTarget);
		btn->setClickCallback(v);

		posTarget.y += spacing + btn->getRectangle().height;
	
		result.push_back(btn);
	}

	return result;
}