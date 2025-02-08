#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Maze; // Forward declaration pour éviter les dépendances circulaires

class Player {
public:
    sf::Vector2i position;
    std::string collectedLetters;
    std::vector<std::string> validWords;
    std::vector<sf::Vector2i> path;
    std::vector<sf::Vector2i> visitedPositions; // Nouveau: pour stocker les positions visitées
    int pathIndex;
    sf::Clock moveClock;

    Player();
    void move(sf::Vector2i direction, Maze& maze);
    void autoMove(Maze& maze);
    void draw(sf::RenderWindow& window, float cellSize, const sf::Texture& playerTexture);
    void drawPath(sf::RenderWindow& window, float cellSize);
};

#endif // PLAYER_H