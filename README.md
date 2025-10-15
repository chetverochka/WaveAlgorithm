# WaveAlgorithm
Simple Path finding class 

## Usage:
```
PathFinder algorithm;

/*
You can pass the map as a string, or a two-dimensional array std::vector<std::vector<PathFinder::MapCell>>:
EMPTY = 0
WALL = 1
START = 2
END = 3
*/
algorithm.setMap(R"(
1,1,1,1,1,1,1,1,1,1,1,1,1,1
1,0,0,0,0,0,0,0,0,0,0,0,0,1
1,0,2,0,0,0,0,0,0,0,0,0,0,1
1,0,0,0,1,1,1,1,1,0,0,0,0,1
1,0,0,0,1,0,0,0,1,0,0,0,0,1
1,0,0,0,1,0,3,0,1,0,0,0,0,1
1,0,0,0,1,0,0,0,0,0,0,0,0,1
1,1,1,1,1,1,1,1,1,1,1,1,1,1
)");
algorithm.process();
std::vector<std::pair<int, int>> finalPath = algorithm.getFinalPath();
```

## Examples:
`main.cpp` and `main_no_PathFinder_class.cpp` contain visual examples in SFML. The first file uses the PathFinder class, the second does not (isolated algorithm).
