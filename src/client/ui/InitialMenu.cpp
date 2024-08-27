#include "InitialMenu.hpp"
#include "NodeRenderer.hpp"
#include "Text.hpp"
#include "Rectangle.hpp"
#include "CallbackNode.hpp"
#include "Button.hpp"
#include "Sprite.hpp"
#include "TomlNode.hpp"
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
	rect2->setID("background");

	m_nodeContainer->addChild(rect2, 0);

	float spacing = 20.f;

	auto gradNode = std::make_shared<CallbackNode>();
	gradNode->setDrawCallback([this](CallbackNode* nd) {
		auto rect = nd->getRectangle();

		Color c1 = BLACK;
		Color c2 = c1;

		c1.a = 255;
		c2.a = 0;
		
		DrawRectangleGradientH(rect.x, rect.y, rect.width, rect.height, c1, c2);
	});
	gradNode->setID("gradient");

	auto gradRect = m_nodeRect;
	gradRect.width /= 1.5f;

	gradNode->setSize({ gradRect.width, gradRect.height });

	m_nodeContainer->addChild(gradNode, 1);

	auto sff_icon = std::make_shared<sandbox_ui::Sprite>("assets/sff2.png");
	sff_icon->setScale(1.5f);
	sff_icon->setPosition({ 100.f, 100.f });

	m_nodeContainer->addChild(sff_icon, 10);

	auto buttons = buildButtons({
		{"SINGLEPLAYER", nullptr},
		{"MULTIPLAYER", nullptr},
		{"OPTIONS", nullptr},
		{"CREDITS", [this](auto btn) {this->onCreditsClick();}}
	}, { 100, sff_icon->getPosition().y + sff_icon->getRectangle().height + 50.f });

	float btn_width = buttons[0]->getRectangle().width;
	float alignX = (btn_width - sff_icon->getRectangle().width) / 2.f;

	sff_icon->setPosition({ 100.f + alignX, 100.f });
	sff_icon->setID("sff-icon");

	m_nodeContainer->addChild(buttons, 10);

	m_nodeContainer->setScalingForObject("sff-icon", 3);

	setID("InitialMenu");
}

void sandbox_ui::InitialMenu::update() {
	double delta = GetFrameTime();

	m_timeTest += delta;

	auto sffIcon = m_nodeContainer->getChildById("sff-icon");
	if (sffIcon.has_value()) {
		auto pos = sffIcon.value()->getPosition();
		pos.y = 100.f + ((float)sin(m_timeTest) * 30);

		sffIcon.value()->setPosition(pos);
	}

	if (IsKeyPressed(KEY_F1)) {
		saveTest();
	}

	// auto creditsCard = m_nodeContainer->getChildById("credits-card");
	// if (creditsCard.has_value()) {
	// 	creditsCard.value()->setPosition(GetMousePosition());
	// }
}

std::vector<std::shared_ptr<sandbox_ui::Node>> sandbox_ui::InitialMenu::buildButtons(const std::unordered_map<std::string, std::function<void(Button*)>>& btnMapping, Node::Pos basePos) {
	m_nodeContainer->setParent(this);
	
	Vector2 posTarget = basePos;

	float spacing = 20.f;

	std::vector<std::shared_ptr<sandbox_ui::Node>> result;

	for (auto& [k, v] : btnMapping) {
		auto btn = std::make_shared<sandbox_ui::Button>(k, Vector2{ 300.f, 40.f });
		btn->setPosition(posTarget);
		btn->setClickCallback(v);
		btn->setID("btn-" + k);

		posTarget.y += spacing + btn->getRectangle().height;
	
		result.push_back(btn);
	}

	return result;
}

void sandbox_ui::InitialMenu::saveTest() {
	sandbox_ui::Sprite *spr = new sandbox_ui::Sprite(this);
	spr->updateTexture();
	spr->save("test.png");

	delete spr;

	for (auto obj : m_nodeContainer->getRenderableChildren()) {
		spr =  new sandbox_ui::Sprite(obj.get());
		spr->updateTexture();
		spr->save("obj-" + obj.get()->getID() + ".png");
	
		delete spr;
	}
}

void sandbox_ui::InitialMenu::onCreditsClick() {
	auto tomlTest = std::make_shared<TomlNode>("assets/ui_credits.toml");
	m_nodeContainer->addChild(tomlTest, 11);

	tomlTest->setID("credits-card");

	Vector2 pos1 = {
		m_nodeRect.width - tomlTest->getRectangle().width - 80,
		(m_nodeRect.height - tomlTest->getRectangle().height) / 2.f
	};

	float y = pos1.y;
	pos1.y = m_nodeRect.height;

	NodeAction act;
	act.createKeyframe(0.3f, y - pos1.y, renderer_tweak_type::TOOutBack);
	act.setParameterToModify("pos.y");

	act.setStartingValue(pos1.y);

	tomlTest->setPosition(pos1);
	tomlTest->runAction(act);

	if (m_currentCard != nullptr) {
		auto card = m_currentCard;

		NodeAction act2;
		act2.createKeyframe(1.f, -m_currentCard->getHeight() - m_currentCard->getPositionY(), renderer_tweak_type::TOInBack);
		act2.setStartingValue(m_currentCard->getPositionY());
		act2.setParameterToModify("pos.y");
		act2.setOnComplete([this, card](auto action) {
			printf("--------- RELEASING! ---------\n");

			card->release();
		});

		auto col = m_currentCard->getColor();

		std::vector<NodeRenderer::Object> nodesToModify = {};
		auto container = dynamic_cast<ContainerizedNode *>(m_currentCard.get());

		if (container) {
			auto nc = container->getNodeContainer();

			nodesToModify.push_back(nc->getChildById("base-rect").value());
			nodesToModify.push_back(nc->getChildById("line-rect").value());
		}

		double colTime = 0.5f;

		{
			NodeAction actc;
			actc.createKeyframe(colTime, -col.r);
			actc.setParameterToModify("col.r");
			actc.setStartingValue(col.r);

			for (auto obj : nodesToModify) {
				obj->runAction(actc);
			}
		}

		{
			NodeAction actc;
			actc.createKeyframe(colTime, -col.g);
			actc.setParameterToModify("col.g");
			actc.setStartingValue(col.g);

			for (auto obj : nodesToModify) {
				obj->runAction(actc);
			}
		}

		{
			NodeAction actc;
			actc.createKeyframe(colTime, -col.b);
			actc.setParameterToModify("col.b");
			actc.setStartingValue(col.b);

			for (auto obj : nodesToModify) {
				obj->runAction(actc);
			}
		}

		m_currentCard->runAction(act2);
	}

	m_currentCard = tomlTest;
}