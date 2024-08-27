#include "Sprite.hpp"

sandbox_ui::Sprite::Sprite(const std::string& path) {
	if (path.empty()) return;

	m_texture = LoadTexture(path.c_str());
	m_path = path;

	updateRect();
}
sandbox_ui::Sprite::Sprite(Texture2D gpu_image) {
	m_texture = gpu_image;

	updateRect();
}
sandbox_ui::Sprite::Sprite(Image cpu_image) {
	m_texture = LoadTextureFromImage(cpu_image);

	updateRect();
}
sandbox_ui::Sprite::Sprite(Node *node) {
	m_linkedNode = node;
	if (m_linkedNode == nullptr) return;

	auto rect = node->getRectangle();

	m_texture = LoadRenderTexture(rect.width, rect.height);
}

sandbox_ui::Sprite::~Sprite() {
	if (!m_doCleanup) return;

	if (std::holds_alternative<RenderTexture2D>(m_texture)) {
		UnloadRenderTexture(std::get<RenderTexture2D>(m_texture));
	} else if (std::holds_alternative<Texture2D>(m_texture)) {
		UnloadTexture(std::get<Texture2D>(m_texture));
	}
}

void sandbox_ui::Sprite::draw() {
	auto txt = getBaseTexture();
	Rectangle source = { 0, 0, (float)txt.width, (float)txt.height };

	DrawTexturePro(txt, source, getRectangle(), { 0.f, 0.f }, 0.f, getColor());
}

void sandbox_ui::Sprite::updateRect() {
	auto txt = getBaseTexture();

	m_nodeRect.width = (float)txt.width * getScale();
	m_nodeRect.height = (float)txt.height * getScale();
}

Image sandbox_ui::Sprite::toImage() {
	return LoadImageFromTexture(getBaseTexture());
}

std::string sandbox_ui::Sprite::getTexturePath() {
	return m_path;
}

void sandbox_ui::Sprite::cleanupOnRelease(bool flag) {
	m_doCleanup = flag;
}

void sandbox_ui::Sprite::save(const std::string &filename) {
	Image img = toImage();

	if (std::holds_alternative<RenderTexture2D>(m_texture)) {
		ImageFlipVertical(&img);
	}

	ExportImage(img, filename.c_str());
	UnloadImage(img);
}

void sandbox_ui::Sprite::setScale(float sz) {
	Node::setScale(sz);

	updateRect();
}

Texture2D sandbox_ui::Sprite::copyTexture() {
	auto img = toImage();
	auto txt = LoadTextureFromImage(img);

	UnloadImage(img);

	return txt;
}

Texture2D sandbox_ui::Sprite::getBaseTexture() {
	if (std::holds_alternative<Texture2D>(m_texture)) {
		return std::get<Texture2D>(m_texture);
	} else if (std::holds_alternative<RenderTexture2D>(m_texture)) {
		return std::get<RenderTexture2D>(m_texture).texture;
	}

	Texture2D empty = {};
	return empty;
}

void sandbox_ui::Sprite::updateTexture() {
	if (!std::holds_alternative<RenderTexture2D>(m_texture)) return;
	if (m_linkedNode == nullptr) return;

	auto nd_pos = m_linkedNode->getPosition();
	m_linkedNode->setPosition({0.f, 0.f});

	BeginTextureMode(std::get<RenderTexture2D>(m_texture));
	m_linkedNode->draw();
	EndTextureMode();

	m_linkedNode->setPosition(nd_pos);
}