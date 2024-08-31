#include <WorldGen.hpp>
#include <World.hpp>
#include <stdio.h>
#include <World.hpp>

WorldGen::WorldGen(World* world, uint64_t seed) : m_world(world), m_seed(seed) {}