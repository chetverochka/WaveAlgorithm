#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

using namespace sf;

void drawMap(sf::RenderWindow& window, std::vector<std::string> map, Vector2f tileSize, Vector2f spacing, Vector2f offSet) {
    for (int y = 0; y < map.size(); y++) {
        for (int x = 0; x < map[y].size(); x++) {
            char symbol = map[y][x];

            Vector2f tilePos;
            tilePos.x = offSet.x + x * tileSize.x + x * spacing.x;
            tilePos.y = offSet.y + y * tileSize.y + y * spacing.y;

            RectangleShape rectangle = RectangleShape(tileSize);
            rectangle.setPosition(tilePos);
            Color fillColor;

            switch (symbol)
            {
            default:
            case ' ': // free space
                fillColor = Color(0,0,0,0);
                break;
            case '#': // wall
                fillColor = Color::White;
                break;
            case '@': // start point
                fillColor = Color::Yellow;
                break;
            case '*': // end point
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

            float percent = value / 34.f;
            if (percent < 0.f) percent = 0.f;
            if (percent > 1.f) percent = 1.f;

            sf::Uint8 r = static_cast<sf::Uint8>(color.r * percent);
            sf::Uint8 g = static_cast<sf::Uint8>(color.g * percent);
            sf::Uint8 b = static_cast<sf::Uint8>(color.b * percent);
            sf::Uint8 a = color.a;

            Color fillColor(r, g, b, a);

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

void getTargetPoints(std::vector<std::string> map, int& startX, int& startY, int& endX, int& endY) {
    bool startPointDefined = false, endPointDefined = false;
    for (int y = 0; y < map.size(); y++) {
        for (int x = 0; x < map[y].size(); x++) {
            char symbol = map[y][x];
            if (symbol == '@') {
                startX = x;
                startY = y;
                startPointDefined = true;
            }
            else if (symbol == '*') {
                endX = x;
                endY = y;
                endPointDefined = true;
            }

            if (startPointDefined && endPointDefined) {
                break;
            }
        }
    }
}

void processWave(std::vector<std::vector<int>>& waveMatrix, const std::vector<std::string>& map, const Vector2i start, const Vector2i end, int& step, std::vector<Vector2i>& oldWave, std::vector<Vector2i>& wave, bool& pointReached) {
    const int dirX[4] = { 0, 1, 0, -1 };
    const int dirY[4] = { -1, 0, 1, 0 };

    int h = waveMatrix.size();
    if (h == 0) return;
    int w = waveMatrix[0].size();

    int nextStep = step + 1;
    wave.clear();

    for (int i = 0; i < (int)oldWave.size(); i++) {
        for (int d = 0; d < 4; d++) {
            int newX = oldWave[i].x + dirX[d];
            int newY = oldWave[i].y + dirY[d];

            bool isBeyondMap = newX < 0 || newY < 0 || newX >= w || newY >= h;
            bool isOccupied = map[newY][newX] == '#';
            bool isWasVisited = waveMatrix[newY][newX] != -1;
            
            if (isBeyondMap || isOccupied || isWasVisited)
                continue;

            waveMatrix[newY][newX] = nextStep;
            wave.push_back(Vector2i(newX, newY));

            if (!pointReached && (newX == end.x && newY == end.y)) {
                pointReached = true;
                break;
            }
            
        }
        if (pointReached)
            break;
    }

    step = nextStep;
    oldWave = wave;
}

std::vector<Vector2i> buildPath(std::vector<std::vector<int>>& waveMatrix, Vector2i start, Vector2i end) {
    std::vector<Vector2i> path;

    Vector2i current = end;
    path.push_back(current);

    const int dirX[4] = { 0, 1, 0, -1 };
    const int dirY[4] = { -1, 0, 1, 0 };

    while (current != start) {
        int currentStep = waveMatrix[current.y][current.x];
        bool found = false;

        for (int d = 0; d < 4; d++) {
            int nx = current.x + dirX[d];
            int ny = current.y + dirY[d];

            if (ny < 0 || nx < 0 || ny >= waveMatrix.size() || nx >= waveMatrix[0].size())
                continue;

            if (waveMatrix[ny][nx] == currentStep - 1) {
                current = Vector2i(nx, ny);
                path.push_back(current);
                found = true;
                break;
            }
        }

        if (!found) {
            printf("Path reconstruction failed!");
            break;
        }
    }

    std::reverse(path.begin(), path.end());
    return path;
}

void drawPath(sf::RenderWindow& window, std::vector<Vector2i> path, Color color, Vector2f tileSize, Vector2f spacing, Vector2f offSet) {
    for (int i = 0; i < path.size(); i++) {
        Vector2f tilePos;
        tilePos.x = offSet.x + path[i].x * tileSize.x + path[i].x * spacing.x;
        tilePos.y = offSet.y + path[i].y * tileSize.y + path[i].y * spacing.y;

        RectangleShape rectangle(tileSize);
        rectangle.setPosition(tilePos);
        rectangle.setFillColor(color);
        window.draw(rectangle);
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Wave algorithm");
    //window.setFramerateLimit(60);
    sf::Clock dtClock;

    Font arialFont;
    if (!arialFont.loadFromFile("arial.ttf"))
        return -1;
    Text debugText = Text("-", arialFont);
    debugText.setPosition(Vector2f(5, 5));
    debugText.setFillColor(Color::White);

    std::vector<std::string> map = {
        "#############################",
        "#           #               #",
        "#   ######  #  *            #",
        "#   #       #               #",
        "#   #       ########        #",
        "##########                  #",
        "#      #                    #",
        "#      #     ###########    #",
        "#            #   #          #",
        "#        #   #   #   ####   #",
        "#    @   #   #   #   #  #   #",
        "#        #   #   ##### ##   #",
        "#            #              #",
        "#############################"
    };

    if (map.empty() || map[0].empty())
        return -1;

    int matrixWidth = map[0].size(), matrixHeight = map.size();
    int startX = 0, startY = 0, endX = 0, endY = 0;
    getTargetPoints(map, startX, startY, endX, endY);
    
    int waveStep = -1;
    // filling w x h matrix -1 numbers;
    std::vector<std::vector<int>> waveMatrix(matrixHeight, std::vector<int>(matrixWidth, waveStep));
    waveMatrix[startY][startX] = 0;

    float delayTime = 0.2f;
    Clock timer;
    
    std::vector<Vector2i> wave;
    std::vector<Vector2i> oldWave{ Vector2i(startX, startY) };

    bool pointWasReached = startX == endX && startY == endY;

    std::vector<Vector2i> finalPath = {};

    while (window.isOpen()) {
        float deltaTime = dtClock.getElapsedTime().asSeconds();
        dtClock.restart();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.scancode == sf::Keyboard::Scan::Enter)
                {
                    printf("resetting algorithm...");
                    waveStep = -1;
                    wave = {};
                    oldWave = { Vector2i(startX, startY) };
                    waveMatrix = std::vector<std::vector<int>>(matrixHeight, std::vector<int>(matrixWidth, waveStep));
                    pointWasReached = startX == endX && startY == endY;
                    timer.restart();
                }
                if (event.key.scancode == sf::Keyboard::Scan::Left) {
                    delayTime -= 0.01f;
                    timer.restart();
                }
                if (event.key.scancode == sf::Keyboard::Scan::Right) {
                    delayTime += 0.01f;
                    timer.restart();
                }

                Vector2i step = Vector2i(0, 0);

                switch (event.key.scancode){
                default:
                    step = Vector2i(0, 0);
                    break;
                case sf::Keyboard::Scan::W:
                    step.y = 1;
                    break;
                case sf::Keyboard::Scan::A:
                    step.x = -1;
                    break;
                case sf::Keyboard::Scan::S:
                    step.y = -1;
                    break;
                case sf::Keyboard::Scan::D:
                    step.x = 1;
                    break;
                }
                endX += step.x;
                endY += step.y;
            }
        }

        if (timer.getElapsedTime().asSeconds() > delayTime) {
            timer.restart();
            if (!oldWave.empty() && !pointWasReached) {
                processWave(waveMatrix, map, Vector2i(startX, startY), Vector2i(endX, endY), waveStep, oldWave, wave, pointWasReached);
            }
            if (pointWasReached) {
                finalPath = buildPath(waveMatrix, Vector2i(startX, startY), Vector2i(endX, endY));
            }
        }

        char debugBuffer[255] = "";
        snprintf(debugBuffer, 255, "dt: %f; FPS: %d\nWave step: %d; Delay time: %f", deltaTime, (int)(1.f / deltaTime), waveStep, delayTime);
        debugText.setString(std::string(debugBuffer));
        
        window.clear();
        drawWave(window, waveMatrix, Color::Yellow, Vector2f(30, 30), Vector2f(2, 2), Vector2f(100, 100));
        drawMap(window, map, Vector2f(30, 30), Vector2f(2, 2), Vector2f(100, 100));
        if (pointWasReached) {
            drawPath(window, finalPath, Color::Blue, Vector2f(30, 30), Vector2f(2, 2), Vector2f(100, 100));
        }

        window.draw(debugText);
        window.display();
    }


    return 0;
}