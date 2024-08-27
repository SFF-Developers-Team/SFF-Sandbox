#include "TomlNode.hpp"
#include "NodeRenderer.hpp"
#include <toml.hpp>
#include <iostream>
#include "Rectangle.hpp"
#include "Text.hpp"
#include "Sprite.hpp"
#include "Button.hpp"

sandbox_ui::TomlNode::TomlNode(const std::string &filepath) {
    if (!FileExists(filepath.c_str())) return;

    auto node_config = toml::parse_file(filepath);

    Vector2 root_size = {
        (float)node_config["root"]["size"][0].value_or(0),
        (float)node_config["root"]["size"][1].value_or(0)
    };

    float chScaling = (float)node_config["root"]["child_scaling"].value_or(8.f);
    std::string rootId = node_config["root"]["id"].value_or("root");

    setID(rootId);

    m_nodeContainer->setScaling(chScaling);
    setSize(root_size);

    for (auto&& [k, v] : node_config) {
        if (k == "root") continue;

        std::string nodeId = k.data();
        std::string objectType = v[toml::path("object_type")].value_or("Node");
    
        Vector2 obj_size = {
            (float)v[toml::path("size")][0].value_or(0.f),
            (float)v[toml::path("size")][1].value_or(0.f)
        };

        float scale = v[toml::path("scale")].value_or(1.f);

        std::shared_ptr<Node> created_obj = nullptr;
        if (objectType == "Node") {
            created_obj = std::make_shared<Node>();
        } else if (objectType == "Rectangle") {
            auto rect_obj = std::make_shared<sandbox_ui::Rectangle>();

            bool solid = v[toml::path("solid")].value_or(true);
            float thickness = v[toml::path("thickness")].value_or(1.f);

            if (!solid) {
                rect_obj->setMode(rect_obj->Outlined);
                rect_obj->setThickness(thickness);
            }

            created_obj = rect_obj;
        } else if (objectType == "Text") {
            std::string label = v[toml::path("label")].value_or("no label provided");
            std::string font = v[toml::path("font")].value_or("");
            
            auto text_obj = std::make_shared<sandbox_ui::Text>(label);
            
            float textSize = v[toml::path("font_size")].value_or(20.f);
            float spacing = v[toml::path("spacing")].value_or(1.f);
            
            text_obj->setBaseTextSize(textSize);
            text_obj->setCharSpacing(spacing);

            if (!font.empty()) {
                Font fnt = LoadFontEx(font.c_str(), textSize, nullptr, 0);
                
                text_obj->setFont(fnt);
            }

            bool shadowEnabled = v[toml::path("shadow_enabled")].value_or(false);
            text_obj->enableShadow(shadowEnabled, BLACK);

            created_obj = text_obj;

            obj_size = Vector2{created_obj->getRectangle().width, created_obj->getRectangle().height};
        } else if (objectType == "Sprite") {
            std::string image_path = v[toml::path("image_path")].value_or("");
        
            created_obj = std::make_shared<sandbox_ui::Sprite>(image_path);

            obj_size = Vector2{created_obj->getRectangle().width, created_obj->getRectangle().height};
        } else if (objectType == "Button") {
            std::string label = v[toml::path("label")].value_or("no label provided");
            std::string font = v[toml::path("font")].value_or("");

            auto btn_obj = std::make_shared<sandbox_ui::Button>(label, obj_size, scale);
            
            if (!font.empty()) {
                float textSize = v[toml::path("font_size")].value_or(20.f);
                Font fnt = LoadFontEx(font.c_str(), textSize, nullptr, 0);

                btn_obj->setFont(fnt);
            }
            
            created_obj = btn_obj;

            obj_size = Vector2{created_obj->getRectangle().width, created_obj->getRectangle().height};
        }

        if (created_obj != nullptr) {
            created_obj->setID(nodeId);
            created_obj->setScale(scale);
            created_obj->setSize(obj_size);

            Vector2 obj_pos = {
                (float)v[toml::path("pos")][0].value_or(0.f),
                (float)v[toml::path("pos")][1].value_or(0.f)
            };

            if (v[toml::path("col")].is_array()) {
                Color obj_color = {
                    (unsigned char)v[toml::path("col")][0].value_or(0x00),
                    (unsigned char)v[toml::path("col")][1].value_or(0x00),
                    (unsigned char)v[toml::path("col")][2].value_or(0x00),
                    (unsigned char)v[toml::path("col")][3].value_or(0xFF)
                };

                created_obj->setColor(obj_color);
            }

            created_obj->setPosition(obj_pos);

            bool aligned[2] = {
                v[toml::path("aligned_x")].value_or(false),
                v[toml::path("aligned_y")].value_or(false),
            };

            obj_size = Vector2{created_obj->getRectangle().width, created_obj->getRectangle().height};

            float alX = 0.f;
            float alY = 0.f;

            if (aligned[0]) {
                alX = (root_size.x - obj_size.x) / 2.f;
            }
            if (aligned[1]) {
                alY = (root_size.y - obj_size.y) / 2.f;
            }

            auto pos = created_obj->getPosition();
            pos.x += alX;
            pos.y += alY;

            created_obj->setPosition(pos);

            int zOrder = v[toml::path("z_order")].value_or(0);

            m_nodeContainer->addChild(created_obj, zOrder);
        }
    }
}

void sandbox_ui::TomlNode::draw() {
    m_nodeContainer->setPosition(getPosition());
    m_nodeContainer->render();
}
void sandbox_ui::TomlNode::update() {

}