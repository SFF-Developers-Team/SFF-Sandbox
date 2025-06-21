#include <ui/basic/Node.hpp>
#include <memory>

class Player;

class HeartsIndicator : public Node {
    std::shared_ptr<Player> m_player;

public:
    HeartsIndicator(std::shared_ptr<Player> player);

    void draw();
};