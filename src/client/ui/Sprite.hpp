#pragma once

#include "Node.hpp"
#include <variant>
#include <memory>

namespace sandbox_ui {
	class Sprite : public Node {
	protected:
		std::variant<Texture2D, RenderTexture2D> m_texture = {};
		std::string m_path = "";
		bool m_doCleanup = true;

		Node *m_linkedNode = nullptr;

		void updateRect();
	public:
		Sprite(const std::string& path);
		Sprite(Image cpu_image);
		Sprite(Texture2D gpu_image);
		Sprite(Node *node);

		~Sprite();

		void draw() override;
		void save(const std::string& filename);

		Image toImage();
		Texture2D copyTexture();

		Texture2D getBaseTexture();

		std::string getTexturePath();

		void cleanupOnRelease(bool flag);

		void setScale(float sz) override;

		void updateTexture();
	};
}