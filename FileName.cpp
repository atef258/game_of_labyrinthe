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

// Classe pour représenter une cellule du labyrinthe
class Cell {
public:
    char letter; // Lettre dans la cellule
    bool walls[8]; // Murs (haut, bas, gauche, droite, haut-gauche, haut-droite, bas-gauche, bas-droite)
    bool visited; // Utilisé pour la génération du labyrinthe
    bool isExit; // Indique si c'est la case d'arrivée

    Cell() : letter('\0'), visited(false), isExit(false) {
        std::fill(walls, walls + 8, true); // Tous les murs sont présents au départ
    }

    // Dessiner la cellule avec ses murs
    void draw(sf::RenderWindow& window, float x, float y, float cellSize, const sf::Texture& wallTexture, const sf::Texture& badgeTexture) {
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
        if (walls[4]) { // Mur haut-gauche
            sf::RectangleShape wallShape(sf::Vector2f(5, 5));
            wallShape.setPosition(x, y);
            wallShape.setTexture(&wallTexture);
            window.draw(wallShape);
        }
        if (walls[5]) { // Mur haut-droite
            sf::RectangleShape wallShape(sf::Vector2f(5, 5));
            wallShape.setPosition(x + cellSize - 5, y);
            wallShape.setTexture(&wallTexture);
            window.draw(wallShape);
        }
        if (walls[6]) { // Mur bas-gauche
            sf::RectangleShape wallShape(sf::Vector2f(5, 5));
            wallShape.setPosition(x, y + cellSize - 5);
            wallShape.setTexture(&wallTexture);
            window.draw(wallShape);
        }
        if (walls[7]) { // Mur bas-droite
            sf::RectangleShape wallShape(sf::Vector2f(5, 5));
            wallShape.setPosition(x + cellSize - 5, y + cellSize - 5);
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
            letterText.setFillColor(sf::Color::Black);
            letterText.setPosition(x + cellSize / 3, y + cellSize / 4);
            window.draw(letterText);
        }
    }
};
// Classe pour représenter le labyrinthe
class Maze {
public:
    std::vector<std::vector<Cell>> grid;
    int rows, cols;
    sf::Vector2i entry, exit;

    Maze(int r, int c) : rows(r), cols(c), grid(r, std::vector<Cell>(c)) {
        generateMaze();
        placeLetters();
        placeExit();
    }

    // Générer le labyrinthe avec l'algorithme DFS
    void generateMaze() {
        std::stack<sf::Vector2i> stack;
        sf::Vector2i current(0, 0);
        grid[current.x][current.y].visited = true;
        stack.push(current);

        while (!stack.empty()) {
            current = stack.top();
            std::vector<sf::Vector2i> neighbors = getUnvisitedNeighbors(current);
            if (!neighbors.empty()) {
                sf::Vector2i next = neighbors[rand() % neighbors.size()];
                removeWall(current, next);
                grid[next.x][next.y].visited = true;
                stack.push(next);
            }
            else {
                stack.pop();
            }
        }
    }

    // Placer des lettres aléatoires dans les cellules
    void placeLetters() {
        std::string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                grid[i][j].letter = letters[rand() % letters.size()];
            }
        }
    }

    // Placer la case d'arrivée (badge)
    void placeExit() {
        exit = sf::Vector2i(rows - 1, cols - 1);
        grid[exit.x][exit.y].isExit = true;
    }

    // Obtenir les voisins non visités
    std::vector<sf::Vector2i> getUnvisitedNeighbors(const sf::Vector2i& cell) {
        std::vector<sf::Vector2i> neighbors;
        int dx[] = { -1, 1, 0, 0 };
        int dy[] = { 0, 0, -1, 1 };
        for (int i = 0; i < 4; ++i) {
            int nx = cell.x + dx[i];
            int ny = cell.y + dy[i];
            if (nx >= 0 && nx < rows && ny >= 0 && ny < cols && !grid[nx][ny].visited) {
                neighbors.push_back(sf::Vector2i(nx, ny));
            }
        }
        return neighbors;
    }

    // Supprimer un mur entre deux cellules
    void removeWall(const sf::Vector2i& a, const sf::Vector2i& b) {
        if (a.x == b.x) {
            if (a.y < b.y) {
                grid[a.x][a.y].walls[3] = false; // Mur droit de a
                grid[b.x][b.y].walls[2] = false; // Mur gauche de b
            }
            else {
                grid[a.x][a.y].walls[2] = false; // Mur gauche de a
                grid[b.x][b.y].walls[3] = false; // Mur droit de b
            }
        }
        else if (a.y == b.y) {
            if (a.x < b.x) {
                grid[a.x][a.y].walls[1] = false; // Mur bas de a
                grid[b.x][b.y].walls[0] = false; // Mur haut de b
            }
            else {
                grid[a.x][a.y].walls[0] = false; // Mur haut de a
                grid[b.x][b.y].walls[1] = false; // Mur bas de b
            }
        }
        else if (a.x < b.x && a.y < b.y) {
            grid[a.x][a.y].walls[5] = false; // Mur haut-droite de a
            grid[b.x][b.y].walls[4] = false; // Mur haut-gauche de b
        }
        else if (a.x < b.x && a.y > b.y) {
            grid[a.x][a.y].walls[4] = false; // Mur haut-gauche de a
            grid[b.x][b.y].walls[5] = false; // Mur haut-droite de b
        }
        else if (a.x > b.x && a.y < b.y) {
            grid[a.x][a.y].walls[7] = false; // Mur bas-droite de a
            grid[b.x][b.y].walls[6] = false; // Mur bas-gauche de b
        }
        else if (a.x > b.x && a.y > b.y) {
            grid[a.x][a.y].walls[6] = false; // Mur bas-gauche de a
            grid[b.x][b.y].walls[7] = false; // Mur bas-droite de b
        }
    }
    // Dessiner le labyrinthe
    void draw(sf::RenderWindow& window, float cellSize, const sf::Texture& wallTexture, const sf::Texture& badgeTexture) {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                grid[i][j].draw(window, j * cellSize, i * cellSize, cellSize, wallTexture, badgeTexture);
            }
        }
    }

    // Vérifier si un chemin est ouvert entre deux cellules
    bool isPathClear(int x1, int y1, int x2, int y2) {
        if (x1 == x2) {
            if (y1 < y2) return !grid[x1][y1].walls[3]; // Mur droit
            else return !grid[x1][y1].walls[2]; // Mur gauche
        }
        else if (y1 == y2) {
            if (x1 < x2) return !grid[x1][y1].walls[1]; // Mur bas
            else return !grid[x1][y1].walls[0]; // Mur haut
        }
        else if (x1 < x2 && y1 < y2) {
            return !grid[x1][y1].walls[5]; // Mur haut-droite
        }
        else if (x1 < x2 && y1 > y2) {
            return !grid[x1][y1].walls[4]; // Mur haut-gauche
        }
        else if (x1 > x2 && y1 < y2) {
            return !grid[x1][y1].walls[7]; // Mur bas-droite
        }
        else if (x1 > x2 && y1 > y2) {
            return !grid[x1][y1].walls[6]; // Mur bas-gauche
        }
        return false;
    }
    // Trouver le chemin le plus court avec BFS
    std::vector<sf::Vector2i> findShortestPath(const sf::Vector2i& start, const sf::Vector2i& end) {
        std::queue<sf::Vector2i> queue;
        std::unordered_map<int, std::unordered_map<int, bool>> visited;
        std::unordered_map<int, std::unordered_map<int, sf::Vector2i>> parent;

        queue.push(start);
        visited[start.x][start.y] = true;

        while (!queue.empty()) {
            sf::Vector2i current = queue.front();
            queue.pop();

            if (current == end) {
                // Reconstruire le chemin
                std::vector<sf::Vector2i> path;
                for (sf::Vector2i at = end; at != start; at = parent[at.x][at.y]) {
                    path.push_back(at);
                }
                path.push_back(start);
                std::reverse(path.begin(), path.end());
                return path;
            }

            // Explorer les voisins
            int dx[] = { -1, 1, 0, 0 };
            int dy[] = { 0, 0, -1, 1 };
            for (int i = 0; i < 4; ++i) {
                int nx = current.x + dx[i];
                int ny = current.y + dy[i];
                if (nx >= 0 && nx < rows && ny >= 0 && ny < cols && !visited[nx][ny] && isPathClear(current.x, current.y, nx, ny)) {
                    queue.push(sf::Vector2i(nx, ny));
                    visited[nx][ny] = true;
                    parent[nx][ny] = current;
                }
            }
        }

        return {}; // Aucun chemin trouvé
    }
};

// Classe pour représenter le joueur (élève)
class Player {
public:
    sf::Vector2i position;
    std::string collectedLetters;
    std::vector<std::string> validWords;
    std::vector<sf::Vector2i> pathTaken; // Chemin pris par le joueur
    sf::Clock moveClock;

    Player() : position(0, 0) {}

    // Déplacer le joueur
    void move(sf::Vector2i direction, Maze& maze) {
        sf::Vector2i newPos = position + direction;

        // Vérifier si le déplacement est valide
        bool canMove = maze.isPathClear(position.x, position.y, newPos.x, newPos.y);

        if (canMove) {
            position = newPos;
            pathTaken.push_back(newPos); // Enregistrer le chemin pris
            collectedLetters += maze.grid[newPos.x][newPos.y].letter;
        }
    }

    // Dessiner le joueur
    void draw(sf::RenderWindow& window, float cellSize, const sf::Texture& playerTexture) {
        sf::RectangleShape playerShape(sf::Vector2f(cellSize, cellSize));
        playerShape.setPosition(position.y * cellSize, position.x * cellSize);
        playerShape.setTexture(&playerTexture);
        window.draw(playerShape);
    }
};

// Classe principale du jeu
class Game {
public:
    Maze maze;
    Player player;
    sf::Font font;
    sf::Text collectedLettersText;
    sf::Text scoreText;
    sf::Text validWordsText;
    sf::Text timeText;
    sf::Text victoryText;
    sf::Texture wallTexture, playerTexture, badgeTexture;
    std::unordered_set<std::string> dictionary;
    int score;
    sf::Clock gameClock; // Chronomètre pour mesurer le temps écoulé
    bool gameWon; // Indique si le joueur a gagné
    std::vector<sf::Vector2i> shortestPath; // Chemin le plus court

    Game() : maze(10, 10), score(0), gameWon(false) {
        if (!font.loadFromFile("Roboto.ttf")) {
            std::cerr << "Erreur: Impossible de charger la police Roboto.ttf" << std::endl;
        }
        if (!wallTexture.loadFromFile("wall.jpg")) {
            std::cerr << "Erreur: Impossible de charger la texture du mur (wall.png)" << std::endl;
        }
        if (!playerTexture.loadFromFile("player.png")) {
            std::cerr << "Erreur: Impossible de charger la texture du joueur (player.png)" << std::endl;
        }
        if (!badgeTexture.loadFromFile("bag.png")) {
            std::cerr << "Erreur: Impossible de charger la texture du badge (badge.png)" << std::endl;
        }
        collectedLettersText.setFont(font);
        collectedLettersText.setCharacterSize(24);
        collectedLettersText.setFillColor(sf::Color::Black);
        collectedLettersText.setPosition(10, 550);

        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::Black);
        scoreText.setPosition(10, 500);
        scoreText.setString("Score: 0");

        validWordsText.setFont(font);
        validWordsText.setCharacterSize(20);
        validWordsText.setFillColor(sf::Color::Black);
        validWordsText.setPosition(400, 500);
        validWordsText.setString("Mots valides: ");

        timeText.setFont(font);
        timeText.setCharacterSize(24);
        timeText.setFillColor(sf::Color::Black);
        timeText.setPosition(10, 450);

        victoryText.setFont(font);
        victoryText.setCharacterSize(40);
        victoryText.setFillColor(sf::Color::Green);
        victoryText.setPosition(200, 200);
        victoryText.setString("Victoire !");

        loadDictionary("dictionary.txt");

        // Trouver le chemin le plus court
        shortestPath = maze.findShortestPath(player.position, maze.exit);
    }

    void loadDictionary(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Erreur: Impossible d'ouvrir le fichier " << filename << std::endl;
            return;
        }
        std::string word;
        while (std::getline(file, word)) {
            std::transform(word.begin(), word.end(), word.begin(), ::toupper); // Convertir en majuscules
            dictionary.insert(word);
        }
    }

    void run() {
        sf::RenderWindow window(sf::VideoMode(800, 600), "Labyrinthe Scolaire");

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                // Gestion des touches pour déplacer le joueur
                if (event.type == sf::Event::KeyPressed && !gameWon) {
                    if (event.key.code == sf::Keyboard::Up) {
                        player.move(sf::Vector2i(-1, 0), maze);
                    }
                    else if (event.key.code == sf::Keyboard::Down) {
                        player.move(sf::Vector2i(1, 0), maze);
                    }
                    else if (event.key.code == sf::Keyboard::Left) {
                        player.move(sf::Vector2i(0, -1), maze);
                    }
                    else if (event.key.code == sf::Keyboard::Right) {
                        player.move(sf::Vector2i(0, 1), maze);
                    }
                    // Déplacements diagonaux
                    else if (event.key.code == sf::Keyboard::Q) { // Haut-Gauche
                        player.move(sf::Vector2i(-1, -1), maze);
                    }
                    else if (event.key.code == sf::Keyboard::E) { // Haut-Droite
                        player.move(sf::Vector2i(-1, 1), maze);
                    }
                    else if (event.key.code == sf::Keyboard::A) { // Bas-Gauche
                        player.move(sf::Vector2i(1, -1), maze);
                    }
                    else if (event.key.code == sf::Keyboard::D) { // Bas-Droite
                        player.move(sf::Vector2i(1, 1), maze);
                    }
                }
            }

            update();
            render(window);
        }
    }
    void update() {
        if (!gameWon) {
            // Vérifier si le joueur a atteint l'arrivée
            if (player.position == maze.exit) {
                gameWon = true;
                std::cout << "Félicitations ! Vous avez atteint l'arrivée en "
                    << gameClock.getElapsedTime().asSeconds() << " secondes." << std::endl;

                // Comparer le chemin pris avec le chemin le plus court
                if (player.pathTaken == shortestPath) {
                    score = 165; // Score maximal si le chemin le plus court est pris
                }
                else {
                    int extraSteps = player.pathTaken.size() - shortestPath.size();
                    score = 165 - (extraSteps * 10); // Réduire le score de 10 points par case supplémentaire
                    score = std::max(0, score); // Le score ne peut pas être négatif
                }
                scoreText.setString("Score: " + std::to_string(score));
            }

            collectedLettersText.setString("Lettres collectées: " + player.collectedLetters);

            // Vérifier si le mot formé est valide
            if (dictionary.find(player.collectedLetters) != dictionary.end()) {
                player.validWords.push_back(player.collectedLetters);
                player.collectedLetters.clear(); // Réinitialiser les lettres collectées

                // Mettre à jour l'affichage des mots valides
                std::string validWordsStr = "Mots valides: ";
                for (const std::string& word : player.validWords) {
                    validWordsStr += word + " ";
                }
                validWordsText.setString(validWordsStr);

                // Mettre à jour le score
                updateScore();
            }
        }
    }

    void updateScore() {
        int wordScore = 0;
        for (const std::string& word : player.validWords) {
            wordScore += word.length(); // 1 point par lettre
        }
        int timePenalty = static_cast<int>(gameClock.getElapsedTime().asSeconds()); // Pénalité de temps
        score = std::max(0, wordScore - timePenalty); // Le score ne peut pas être négatif
        scoreText.setString("Score: " + std::to_string(score));
    }

    void render(sf::RenderWindow& window) {
        window.clear(sf::Color::White);
        maze.draw(window, 50.f, wallTexture, badgeTexture);
        player.draw(window, 50.f, playerTexture);

        // Afficher le temps écoulé
        timeText.setString("Temps: " + std::to_string(static_cast<int>(gameClock.getElapsedTime().asSeconds())) + "s");
        window.draw(timeText);

        // Afficher les lettres collectées
        window.draw(collectedLettersText);

        // Afficher le score
        window.draw(scoreText);

        // Afficher les mots valides
        window.draw(validWordsText);

        // Afficher le message de victoire
        if (gameWon) {
            window.draw(victoryText);
        }

        window.display();
    }
};

// Fonction principale
int main() {
    srand(static_cast<unsigned int>(time(0)));
    Game game;
    game.run();
    return 0;
}