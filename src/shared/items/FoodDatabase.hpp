#include <Food.hpp>

class Apple : public Food<4> {
public:
    Apple(int id, uint8_t spriteIndex) : Food(id, spriteIndex) {};
    Apple(Apple const& apple) : Food(apple.m_id, apple.m_spriteIndex) {};
};