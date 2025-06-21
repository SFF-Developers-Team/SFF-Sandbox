#pragma once
#include <string_view>

std::string_view defaultConfig = R"(# Auto-generated config for SFFSandbox server
address="*"
port=7777
max-players=32
# downstream/upstream bandwidth of the host in bytes/second; 0=maximum
incoming-bandwidth=0
outcoming-bandwidth=0
world-name="world"
# 0=NORMAL; 
world-generator=0
# 0=random
world-seed=0)";
