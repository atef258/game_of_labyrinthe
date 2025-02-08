#include "player.h"
#include "maze.h" // Assurez-vous que Maze.h est correctement inclus

Player::Player() : position(0, 0), pathIndex(0) {}

void Player::move(sf::Vector2i direction, Maze& maze) {
    sf::Vector2i newPos = position + direction;
    if (maze.isPathClear(position.x, position.y, newPos.x, newPos.y)) {
        position = newPos;
        collectedLetters += maze.grid[newPos.x][newPos.y].letter;
        maze.grid[newPos.x][newPos.y].isCollected = true; // Marquer la lettre comme collectée
        visitedPositions.push_back(newPos); // Ajouter la nouvelle position visitée
    }
}

void Player::autoMove(Maze& maze) {
    if (pathIndex < path.size() && moveClock.getElapsedTime().asSeconds() > 0.5f) {
        position = path[pathIndex];
        collectedLetters += maze.grid[position.x][position.y].letter;
        maze.grid[position.x][position.y].isCollected = true; // Marquer la lettre comme collectée
        visitedPositions.push_back(position); // Ajouter la nouvelle position visitée
        pathIndex++;
        moveClock.restart();
    }
}

void Player::draw(sf::RenderWindow& window, float cellSize, const sf::Texture& playerTexture) {
    sf::RectangleShape playerShape(sf::Vector2f(cellSize, cellSize));
    playerShape.setPosition(position.y * cellSize, position.x * cellSize);
    playerShape.setTexture(&playerTexture);
    window.draw(playerShape);
}

void Player::drawPath(sf::RenderWindow& window, float cellSize) {
    if (visitedPositions.size() < 2) return;

    sf::VertexArray lines(sf::LinesStrip, visitedPositions.size());
    for (size_t i = 0; i < visitedPositions.size(); ++i) {
        lines[i].position = sf::Vector2f(visitedPositions[i].y * cellSize + cellSize / 2,
            visitedPositions[i].x * cellSize + cellSize / 2);
        lines[i].color = sf::Color::Red;
    }
    window.draw(lines);
}