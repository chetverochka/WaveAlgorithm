#include <SFML/Graphics.hpp>
#include "PathFinder.h"
using namespace sf;
typedef PathFinder::MapCell Cell;

void drawMap(sf::RenderWindow& window, PathFinder& algorithm, Vector2f tileSize, Vector2f spacing, Vector2f offSet) {
    PathFinder::Map map = algorithm.getMap();
    
    for (int y = 0; y < map.size(); y++) {
        for (int x = 0; x < map[y].size(); x++) {
            Cell cell = map[y][x];

            Vector2f tilePos;
            tilePos.x = offSet.x + x * tileSize.x + x * spacing.x;
            tilePos.y = offSet.y + y * tileSize.y + y * spacing.y;

            RectangleShape rectangle = RectangleShape(tileSize);
            rectangle.setPosition(tilePos);
            Color fillColor;

            switch (cell)
            {
            default:
            case Cell::EMPTY:
                fillColor = Color(0, 0, 0, 0);
                break;
            case Cell::WALL:
                fillColor = Color::White;
                break;
            case Cell::START:
                fillColor = Color::Yellow;
                break;
            case Cell::END:
                fillColor = Color::Red;
                break;
            }

            rectangle.setFillColor(fillColor);
            window.draw(rectangle);
        }
    }
}
void drawWave(sf::RenderWindow& window, const std::vector<std::vector<int>>& waveMatrix, Color color, Vector2f tileSize, Vector2f spacing, Vector2f offSet) {
    int h = (int)waveMatrix.size();
    if (h == 0) return;
    int w = (int)waveMatrix[0].size();

    bool showIndex = false;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int value = waveMatrix[y][x];
            if (value == -1) continue; // drawing only visited cells

            float percent = value / 10.f;
            if (percent < 0.f) percent = 0.f;
            if (percent > 1.f) percent = 1.f;

            sf::Uint8 r = static_cast<sf::Uint8>(color.r * percent);
            sf::Uint8 g = static_cast<sf::Uint8>(color.g * percent);
            sf::Uint8 b = static_cast<sf::Uint8>(color.b * percent);
            sf::Uint8 a = (percent > 0.f) ? color.a * percent : 0.f;

            Color fillColor(r/percent, g/percent, b/percent, a);

            Vector2f tilePos;
            tilePos.x = offSet.x + x * tileSize.x + x * spacing.x;
            tilePos.y = offSet.y + y * tileSize.y + y * spacing.y;

            RectangleShape rectangle(tileSize);
            rectangle.setPosition(tilePos);
            rectangle.setFillColor(fillColor);
            window.draw(rectangle);

            if (showIndex) {
                Font font;
                font.loadFromFile("arial.ttf");
                Text text(std::to_string(value), font, 20);
                text.setPosition(Vector2f(tilePos.x + 5, tilePos.y - 5));
                text.setFillColor(Color::Red);
                window.draw(text);
            }
        }
    }
}
void drawPath(sf::RenderWindow& window, std::vector<std::pair<int,int>> path, Color color, Vector2f tileSize, Vector2f spacing, Vector2f offSet) {
    for (int i = 0; i < path.size(); i++) {
        Vector2f tilePos;
        tilePos.x = offSet.x + path[i].first * tileSize.x + path[i].first * spacing.x;
        tilePos.y = offSet.y + path[i].second * tileSize.y + path[i].second * spacing.y;

        RectangleShape rectangle(tileSize);
        rectangle.setPosition(tilePos);
        rectangle.setFillColor(color);
        window.draw(rectangle);
    }
}
int main() {
    sf::RenderWindow window(sf::VideoMode(720, 720), "Wave algorithm");

    PathFinder algorithm;
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

    sf::Clock dtClock;
    sf::Clock tickTimer;
    float tickDelay = 3.5f;
    
    std::vector<std::pair<int, int>> finalPath = algorithm.getFinalPath();
    
    while (window.isOpen()) {
        float deltaTime = dtClock.getElapsedTime().asSeconds();
        dtClock.restart();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed){
                if (event.key.scancode == sf::Keyboard::Scan::Enter){
                    finalPath = {};
                    algorithm.setMap(R"(
1,1,1,1,1,1,1,1,1,1,1,1,1,1
1,0,0,0,0,0,0,0,0,0,0,0,0,1
1,0,2,0,0,0,0,0,0,0,0,0,0,1
1,0,0,0,1,1,1,1,1,0,0,0,0,1
1,0,0,0,1,0,3,0,1,0,0,0,0,1
1,0,0,0,1,0,0,0,1,1,1,1,1,1
1,0,0,0,0,0,0,0,0,0,0,0,0,1
1,1,1,1,1,1,1,1,1,1,1,1,1,1
)");
                }
            }
        }

        if (tickTimer.getElapsedTime().asSeconds() >= tickDelay) {
            printf("tick action...\n");
            tickTimer.restart();
            algorithm.process();
            finalPath = algorithm.calculatePath();
            printf("%s", algorithm.getMapAsString().c_str());
        }

        window.clear();
        drawMap(window, algorithm, Vector2f(30, 30), Vector2f(2, 2), Vector2f(100, 100));
        //drawWave(window, algorithm.getWaveMatrix(), Color::Green, Vector2f(30, 30), Vector2f(2, 2), Vector2f(100, 100));
        drawPath(window, finalPath, Color::Blue, Vector2f(30, 30), Vector2f(2, 2), Vector2f(100, 100));
        window.display();
    }


    return 0;
}