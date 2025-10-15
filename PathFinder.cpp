#include "PathFinder.h"
#include <cctype>

PathFinder::PathFinder() {
    m_map = {};
    m_waveMatrix = {};
    m_waveStep = -1;
    m_startX = m_startY = m_endX = m_endY = 0;
    m_reachedPoint = false;
}

void PathFinder::log(std::string logString) {
#if defined(_MSC_VER)
	printf("[PathFinder log message] %s\n", logString.c_str());
#endif
}

void PathFinder::setMap(PathFinder::Map newMap) {
	if (newMap.empty())
		return;

	int height = newMap.size();
	int width = newMap[0].size();

	if (width < 1)
		return;
	
	for (int i = 0; i < newMap.size(); i++) {
        std::vector<MapCell>& row = newMap[i];
		if (width != row.size()) {
			log("matrix have different width");
			return;
		}

        for (int j = 0; j < width; j++) {
            MapCell cell = row[j];

            if (cell == MapCell::START) {
                m_startX = j;
                m_startY = i;
            }
            else if (cell == MapCell::END) {
                m_endX = j;
                m_endY = i;
            }
        }
	}

	m_map = newMap;
	m_mapWidth = width;
	m_mapHeight = height;
    reset();
}

/*
string map data parser
write maps in format:
0,0,0,0,0,0
0,0,1,0,0,0
0,0,0,1,0,0
0,0,0,0,1,0
0,0,0,0,0,0

EMPTY = 0
WALL = 1
START = 2
END = 3
*/
void PathFinder::setMap(std::string map) {
    Map newMap = {};
    std::vector<MapCell> row = {};
    std::string cellStr = "";
    for (int i = 0; i < map.length(); i++) {
        bool isDigit = std::isdigit(map[i]);
        if (isDigit) {
            cellStr += map[i];
        }
        else if (map[i] == ',' || map[i] == '\n') {
            try {
                if (!cellStr.empty()) {
                    //log("setMap(string): passing a string [" + cellStr + "]");
                    row.push_back((MapCell)(std::stoi(cellStr)));
                }
            }
            catch (std::exception e) {
                log(std::string("setMap(string): " + std::string(e.what())));
            }

            if (map[i] == '\n') {
                if (!row.empty())
                    newMap.push_back(row);
                row = {};
            }

            cellStr = "";
        }
    }
    setMap(newMap);
}

std::string PathFinder::getMapAsString() {
    std::string dataStr = "";
    for (int y = 0; y < m_map.size(); y++) {
        for (int x = 0; x < m_map[y].size(); x++) {
            dataStr += std::to_string((int)m_map[y][x]);

            if (x >= m_map[y].size() - 1) {
                dataStr += '\n';
            }
            else {
                dataStr += ',';
            }
        }
    }
    return dataStr;
}

void PathFinder::reset() {
	m_waveStep = -1;
	m_waveMatrix = std::vector<std::vector<int>>(m_mapHeight, std::vector<int>(m_mapWidth, -1));
    m_reachedPoint = m_startX == m_endX && m_startY == m_endY;
    m_wave = {};
    m_finalPath = {};
    m_oldWave = { {m_startX, m_startY} };
    m_waveMatrix[m_startY][m_startX] = 0;
}

void PathFinder::processStep() {
    if (m_reachedPoint || m_oldWave.empty())
        return;
    const int dirX[4] = { 0, 1, 0, -1 };
    const int dirY[4] = { -1, 0, 1, 0 };

    int h = m_mapHeight;
    if (h == 0) return;
    int w = m_mapWidth;

    std::vector<std::pair<int,int>>& oldWave = m_oldWave;
    std::vector<std::pair<int, int>>& wave = m_wave;
    bool& pointReached = m_reachedPoint;
    PathFinder::Map map = m_map;
    std::vector<std::vector<int>>& waveMatrix = m_waveMatrix;

    int nextStep = m_waveStep + 1;
    wave.clear();

    for (int i = 0; i < (int)oldWave.size(); i++) {
        for (int d = 0; d < 4; d++) {
            int newX = oldWave[i].first + dirX[d];
            int newY = oldWave[i].second + dirY[d];

            bool isBeyondMap = newX < 0 || newY < 0 || newX >= w || newY >= h;
            bool isOccupied = map[newY][newX] == PathFinder::MapCell::WALL;
            bool isWasVisited = waveMatrix[newY][newX] != -1;

            if (isBeyondMap || isOccupied || isWasVisited)
                continue;

            waveMatrix[newY][newX] = nextStep;
            wave.push_back({newX, newY});

            if ((newX == m_endX && newY == m_endY)) {
                pointReached = true;
                break;
            }

        }
        if (pointReached)
            break;
    }
    m_waveStep = nextStep;
    oldWave = wave;
}

void PathFinder::process() {
    while (!m_oldWave.empty() && !m_reachedPoint)
        processStep();
    m_finalPath = calculatePath();
}

std::vector<std::pair<int, int>> PathFinder::calculatePath() {
    std::vector<std::pair<int, int>> path;
    if (!m_reachedPoint)
        return path;

    std::pair<int, int> current = { m_endX, m_endY };
    path.push_back(current);
    
    const int dirX[4] = { 0, 1, 0, -1 };
    const int dirY[4] = { -1, 0, 1, 0 };
    
    while (current.first != m_startX || current.second != m_startY) {
        int currentStep = m_waveMatrix[current.second][current.first];
        bool found = false;
    
        for (int d = 0; d < 4; d++) {
            int nx = current.first + dirX[d];
            int ny = current.second + dirY[d];
    
            if (ny < 0 || nx < 0 || ny >= m_waveMatrix.size() || nx >= m_waveMatrix[0].size())
                continue;
    
            if (m_waveMatrix[ny][nx] == currentStep - 1) {
                current = {nx, ny};
                path.push_back(current);
                found = true;
                break;
            }
        }
    
        if (!found) {
            //printf("Path reconstruction failed!");
            break;
        }
    }
    std::reverse(path.begin(), path.end());
    return path;
}