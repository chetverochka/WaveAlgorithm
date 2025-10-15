#ifndef __PATH_FINDER_H__
#define __PATH_FINDER_H__
#include <vector>
#include <string>

class PathFinder {
public:
	enum class MapCell {
		EMPTY = 0, 
		WALL = 1, 
		START = 2, 
		END = 3
	};
	typedef std::vector<std::vector<MapCell>> Map;

	PathFinder();

	void setMap(Map newMap);
	void setMap(std::vector<MapCell> newMap, int cols);
	void setMap(std::string);

	Map getMap() { return m_map; }
	std::string getMapAsString();

	std::vector<std::vector<int>> getWaveMatrix() { return m_waveMatrix; }

	void reset();

	void process();
	void processStep();

	std::vector<std::pair<int, int>> calculatePath();

	bool isPointReached() { return m_reachedPoint; }
	std::vector<std::pair<int, int>> getFinalPath() { return m_finalPath; }
protected:
	Map m_map;
	std::vector<std::vector<int>> m_waveMatrix;
	std::vector<std::pair<int, int>> m_wave;
	std::vector<std::pair<int, int>> m_finalPath;
	std::vector<std::pair<int, int>> m_oldWave;
	int m_waveStep;

	int m_mapWidth;
	int m_mapHeight;

	bool m_reachedPoint;

	int m_startX, m_startY, m_endX, m_endY;
private:
	void log(std::string);
};

#endif //!__PATH_FINDER_H__