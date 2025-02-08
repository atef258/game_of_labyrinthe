#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <stack>
#include <random>
#include <fstream>
#include <unordered_set>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include "Celll.h"

// Constructeur
Cell::Cell() : letter('\0'), visited(false), isExit(false), isCollected(false) {
    std::fill(walls, walls + 4, true); // Tous les murs sont présents au départ
}

// Dessiner la cellule avec ses murs
void Cell::draw(sf::RenderWindow& window, float x, float y, float cellSize, const sf::Texture& wallTexture, const sf::Texture& badgeTexture) {
    sf::RectangleShape cellShape(sf::Vector2f(cellSize, cellSize));
    cellShape.setPosition(x, y);
    cellShape.setFillColor(sf::Color::White); // Couleur du sol (couloir)
    window.draw(cellShape);

    // Dessiner les murs (tableaux noirs)
    if (walls[0]) { // Mur haut
        sf::RectangleShape wallShape(sf::Vector2f(cellSize, 5));
        wallShape.setPosition(x, y);
        wallShape.setTexture(&wallTexture);
        window.draw(wallShape);
    }
    if (walls[1]) { // Mur bas
        sf::RectangleShape wallShape(sf::Vector2f(cellSize, 5));
        wallShape.setPosition(x, y + cellSize - 5);
        wallShape.setTexture(&wallTexture);
        window.draw(wallShape);
    }
    if (walls[2]) { // Mur gauche
        sf::RectangleShape wallShape(sf::Vector2f(5, cellSize));
        wallShape.setPosition(x, y);
        wallShape.setTexture(&wallTexture);
        window.draw(wallShape);
    }
    if (walls[3]) { // Mur droit
        sf::RectangleShape wallShape(sf::Vector2f(5, cellSize));
        wallShape.setPosition(x + cellSize - 5, y);
        wallShape.setTexture(&wallTexture);
        window.draw(wallShape);
    }

    // Dessiner le badge si c'est la case d'arrivée
    if (isExit) {
        sf::RectangleShape badgeShape(sf::Vector2f(cellSize, cellSize));
        badgeShape.setPosition(x, y);
        badgeShape.setTexture(&badgeTexture);
        window.draw(badgeShape);
    }

    // Afficher la lettre dans la cellule
    if (letter != '\0') {
        sf::Font font;
        if (!font.loadFromFile("Roboto.ttf")) {
            std::cerr << "Erreur: Impossible de charger la police Roboto.ttf" << std::endl;
        }
        sf::Text letterText;
        letterText.setFont(font);
        letterText.setString(letter);
        letterText.setCharacterSize(24);
        letterText.setFillColor(isCollected ? sf::Color::Green : sf::Color::Black); // Lettre verte si collectée
        letterText.setPosition(x + cellSize / 3, y + cellSize / 4);
        window.draw(letterText);
    }
}