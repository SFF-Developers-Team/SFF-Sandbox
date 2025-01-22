#pragma once
#include <memory>
#include <string>
#include <functional>
#include <Types.hpp>
#include <MiniFunction.hpp>
#include <ui/nodes/Node.hpp>

class UiManager {
protected:
    std::vector<std::shared_ptr<Node>> m_childs;

public:
    UiManager();

    void addChild(std::shared_ptr<Node> node);
    bool hasChild(size_t index);
    bool hasChild(std::string const& tag);
    std::shared_ptr<Node> getChild(size_t index);
    std::shared_ptr<Node> getChild(std::string const& tag);
};