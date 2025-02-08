#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <stack>
#include <queue>
#include <unordered_map>
#include <string>
#include <SFML/Graphics.hpp>

struct Cell {
    bool visited = false;
    bool isExit = false;
    char letter = ' ';
    bool walls[4] = { true, true, true, true }; // Haut, Bas, Gauche, Droit

    void draw(sf::RenderWindow& window, float x, float y, float cellSize, const sf::Texture& wallTexture, const sf::Texture& badgeTexture);
};

class Maze {
public:
    std::vector<std::vector<Cell>> grid;
    int rows, cols;
    sf::Vector2i entry, exit;

    Maze(int r, int c);
    void generateMaze();
    void placeLetters();
    void placeExit();
    std::vector<sf::Vector2i> getUnvisitedNeighbors(const sf::Vector2i& cell);
    void removeWall(const sf::Vector2i& a, const sf::Vector2i& b);
    void draw(sf::RenderWindow& window, float cellSize, const sf::Texture& wallTexture, const sf::Texture& badgeTexture);
    bool isPathClear(int x1, int y1, int x2, int y2);
    std::vector<sf::Vector2i> findShortestPath(const sf::Vector2i& start, const sf::Vector2i& end);
};

#endif // MAZE_H