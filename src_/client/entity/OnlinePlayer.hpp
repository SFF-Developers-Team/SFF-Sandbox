#include <entity/SimplePlayer.hpp>
#include <array>

struct OnlinePosition {
    float x;
    float y;
    float time;
};

class OnlinePlayer : public SimplePlayer {
protected:
    std::array<OnlinePosition, 2> m_positions;
    int m_index;

public:
    OnlinePlayer(std::shared_ptr<World> world);

    void update();
    size_t deserialize(ByteVector const& bytes);
};