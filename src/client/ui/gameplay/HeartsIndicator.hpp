#include <ui/basic/Node.hpp>
#include <memory>

class LocalPlayer;

class HeartsIndicator : public Node {
    std::shared_ptr<LocalPlayer> m_player;

public:
    HeartsIndicator(std::shared_ptr<LocalPlayer> player);

    void draw();
};