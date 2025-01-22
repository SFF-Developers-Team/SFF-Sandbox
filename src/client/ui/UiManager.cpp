#include <TextureManager.hpp>
#include <RenderManager.hpp>
#include <ui/UiManager.hpp>
#include <raygui.h>
#include <Logger.hpp>
#include <string>
#include <cmath>



UiManager::UiManager() {}

void UiManager::addChild(std::shared_ptr<Node> node) {
    m_childs.push_back(node);
}

std::shared_ptr<Node> UiManager::getChild(size_t index) {
    return m_childs[index];
}

std::shared_ptr<Node> UiManager::getChild(std::string const& tag) {
    auto i = std::find_if(m_childs.begin(), m_childs.end(), [&tag](auto const& node) {
        return node->getTag() == tag;
    });

    return (i != m_childs.end() ? *i : nullptr);
}

bool UiManager::hasChild(size_t index) {
    return index < m_childs.size();
}

bool UiManager::hasChild(std::string const& tag) {
    auto i = std::find_if(m_childs.begin(), m_childs.end(), [tag](auto& node) {
        return node->getTag() == tag;
    });

    return i != m_childs.end();
}