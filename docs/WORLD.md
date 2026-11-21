
## World format 4
### Directory structure of [World Name] folder

|File path|description|
|---------|-----------|
|/world.dat|World meta info (version, seed, type)|
|/chunks/[X],[y].dat|World chunk|
|/players/[username].dat|Player object|

### world.dat structure
| type | offset | size | description |
|------|--------|------|-------------|
|uint32|0|4|world version (4)|
|uint8 |4|1|world type|
|uint64|5|8|world seed|
|uint64|12|8|world time in ticks|

## Block tag structure
| type | offset | size | description |
|------|--------|------|-------------|
|uint8 |0|1|Tag ID|
|Color\nbool|1|||

### Block structure
| type | offset | size | description |
|------|--------|------|-------------|
|uint8 |0|1|Block header (1)|
|int32 |1|4|Block X|
|int32 |5|4|Block Y|
|int8  |9|1|Block layer|
|uint8 |10|1|Block ID|
|char  |11|1|Tags begin (Just a 'T' char)|
|uint8 |12|1|Tag count|
|Tag   |13|?*[tag count]|Tags|

### Chunk structure
| type | offset | size | description |
|------|--------|------|-------------|
|uint16|0|2|Block count|
|Block|1|?*[block count]|Blocks|