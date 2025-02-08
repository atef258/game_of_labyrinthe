#ifndef CELL_H
#define CELL_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>

class Cell {
public:
    char letter; // Lettre dans la cellule
    bool walls[4]; // Murs (haut, bas, gauche, droite)
    bool visited; // Utilis� pour la g�n�ration du labyrinthe
    bool isExit; // Indique si c'est la case d'arriv�e
    bool isCollected; // Indique si la lettre a �t� collect�e

    Cell(); // Constructeur

    // Dessiner la cellule avec ses murs
    void draw(sf::RenderWindow& window, float x, float y, float cellSize, const sf::Texture& wallTexture, const sf::Texture& badgeTexture);
};

#endif // CELL_H