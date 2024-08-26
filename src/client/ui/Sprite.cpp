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

sandbox_ui::Sprite::~Sprite() {
	if (!m_doCleanup) return;

	UnloadTexture(m_texture);
}

void sandbox_ui::Sprite::draw() {
	Rectangle source = { 0, 0, m_texture.width, m_texture.height };

	DrawTexturePro(m_texture, source, getRectangle(), { 0.f, 0.f }, 0.f, getColor());
}

void sandbox_ui::Sprite::updateRect() {
	m_nodeRect.width = (float)m_texture.width * getScale();
	m_nodeRect.height = (float)m_texture.height * getScale();
}

Image sandbox_ui::Sprite::toImage() {
	return LoadImageFromTexture(m_texture);
}

std::string sandbox_ui::Sprite::getTexturePath() {
	return m_path;
}

void sandbox_ui::Sprite::cleanupOnRelease(bool flag) {
	m_doCleanup = flag;
}

void sandbox_ui::Sprite::save(const std::string &filename) {
	Image img = toImage();

	ExportImage(img, filename.c_str());
	UnloadImage(img);
}

void sandbox_ui::Sprite::setScale(float sz) {
	Node::setScale(sz);

	updateRect();
}