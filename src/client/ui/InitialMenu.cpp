#include "InitialMenu.hpp"
#include "NodeRenderer.hpp"
#include "Text.hpp"
#include "Rectangle.hpp"
#include <cmath>

void sandbox_ui::InitialMenu::draw() {
	Node::draw();

	m_nodeRect.x = 0;
	m_nodeRect.y = 0;
	m_nodeRect.width = GetRenderWidth();
	m_nodeRect.height = GetRenderHeight();

	DrawRectangleGradientV(m_nodeRect.x, m_nodeRect.y, m_nodeRect.width, m_nodeRect.height, GRAY, WHITE);

	m_nodeContainer->setPosition(getPosition());
	m_nodeContainer->setScaling(m_currentRenderer->getScaling());

	m_nodeContainer->render();
}

sandbox_ui::InitialMenu::InitialMenu(){
	m_nodeContainer = std::make_shared<NodeRenderer>();

	auto textTest = std::make_shared<sandbox_ui::Text>("test");
	textTest->setID("text-test");

	auto rect = std::make_shared<sandbox_ui::Rectangle>();
	rect->setSize(100, 100);
	rect->setColor(RED);
	rect->setID("rect-test");
	rect->setPosition({100, 100});

	textTest->setColor(BLACK);
	textTest->setSize(3.f);

	m_nodeContainer->addChild(rect);
	m_nodeContainer->addChild(textTest);
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
		
		pos = rectTest.value()->getPosition();
		
		printf("pos: %f %f\n", pos.x, pos.y);
	}
}