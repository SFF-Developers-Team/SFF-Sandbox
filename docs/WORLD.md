## World format 4

### Directory structure of [World Name] folder

| File path               | Description                          |
|-------------------------|--------------------------------------|
| /world.dat              | World meta info (version, seed, type)|
| /chunks/[X],[y].dat     | World chunk                          |
| /players/[username].dat | Player object                        |

### world.dat structure

| Type   | Offset | Size | Description          |
|--------|--------|------|----------------------|
| uint32 | 0      | 4    | World version (4)    |
| uint8  | 4      | 1    | World type           |
| uint64 | 5      | 8    | World seed           |
| uint64 | 12     | 8    | World time in ticks  |

## Tile tag structure

| Type         | Offset | Size | Description |
|--------------|--------|------|-------------|
| uint8        | 0      | 1    | Tag ID      |
| Color / bool | 1      |      |             |

### Tile structure

| Type  | Offset | Size          | Description                  |
|-------|--------|---------------|------------------------------|
| uint8 | 0      | 1             | Tile header (1)              |
| int32 | 1      | 4             | Tile X                       |
| int32 | 5      | 4             | Tile Y                       |
| int8  | 9      | 1             | Tile layer                   |
| uint8 | 10     | 1             | Tile ID                      |
| char  | 11     | 1             | Tags begin (Just a 'T' char) |
| uint8 | 12     | 1             | Tag count                    |
| Tag   | 13     | ? * tag count | Tags                         |

### Chunk structure

| Type   | Offset | Size           | Description |
|--------|--------|----------------|-------------|
| uint16 | 0      | 2              | Tile count  |
| Tile   | 1      | ? * tile count | Tiles       |