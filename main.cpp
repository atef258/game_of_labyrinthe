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

// Structure pour représenter un nœud dans le graphe
struct Node {
    int x, y; // Coordonnées de la cellule
    char letter; // Lettre dans la cellule
    bool isExit; // Indique si c'est la case d'arrivée
    bool isBlocked; // Indique si la case est bloquée
    bool isSpecial; // Indique si la case est spéciale
    bool visited; // Utilisé pour la génération du labyrinthe
    bool isUsed; // Indique si la case a été utilisée pour former un mot
    bool playerVisited; // Indique si la case a été visitée par le joueur
    std::vector<Node*> neighbors; // Liste des voisins accessibles

    Node() : letter('\0'), isExit(false), isBlocked(false), isSpecial(false), visited(false), isUsed(false), playerVisited(false) {}
};

// Classe pour représenter le graphe
class Graph {
public:
    std::vector<std::vector<Node>> grid; // Grille de nœuds
    int rows, cols;

    Graph(int r, int c) : rows(r), cols(c), grid(r, std::vector<Node>(c)) {
        // Initialiser les coordonnées de chaque nœud
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                grid[i][j].x = i;
                grid[i][j].y = j;
            }
        }
    }

    // Ajouter une arête entre deux nœuds
    void addEdge(Node& a, Node& b) {
        a.neighbors.push_back(&b);
        b.neighbors.push_back(&a);
    }

    // Trouver le nœud correspondant à une position
    Node* getNode(int x, int y) {
        if (x >= 0 && x < rows && y >= 0 && y < cols) {
            return &grid[x][y];
        }
        return nullptr;
    }
};

// Classe pour représenter le labyrinthe
class Maze {
public:
    Graph graph;
    sf::Vector2i entry, exit;

    Maze(int r, int c) : graph(r, c) {
        generateMaze();
        placeLetters("dictionary.txt");
        placeExit();
        placeBlockedCells();
        placeSpecialCells();
    }

    // Générer le labyrinthe avec l'algorithme DFS
    void generateMaze() {
        std::stack<Node*> stack;
        Node* current = graph.getNode(0, 0);
        current->visited = true;
        stack.push(current);

        while (!stack.empty()) {
            current = stack.top();
            std::vector<Node*> neighbors = getUnvisitedNeighbors(current);
            if (!neighbors.empty()) {
                Node* next = neighbors[rand() % neighbors.size()];
                graph.addEdge(*current, *next);
                next->visited = true;
                stack.push(next);
            }
            else {
                stack.pop();
            }
        }
    }

    // Obtenir les voisins non visités
    std::vector<Node*> getUnvisitedNeighbors(Node* node) {
        std::vector<Node*> neighbors;
        int dx[] = { -1, 1, 0, 0 };
        int dy[] = { 0, 0, -1, 1 };
        for (int i = 0; i < 4; ++i) {
            int nx = node->x + dx[i];
            int ny = node->y + dy[i];
            Node* neighbor = graph.getNode(nx, ny);
            if (neighbor && !neighbor->visited && !neighbor->isBlocked) {
                neighbors.push_back(neighbor);
            }
        }
        return neighbors;
    }

    // Placer des lettres aléatoires dans les cellules
    void placeLetters(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Erreur: Impossible d'ouvrir le fichier " << filename << std::endl;
            return;
        }

        std::string word;
        std::string allLetters;

        // Lire tous les mots du fichier et extraire les lettres
        while (file >> word) {
            for (char letter : word) {
                if (isalpha(letter)) {
                    allLetters += toupper(letter);
                }
            }
        }

        file.close();

        // Placer les lettres dans la grille
        for (int i = 0; i < graph.rows; ++i) {
            for (int j = 0; j < graph.cols; ++j) {
                if (!allLetters.empty() && !graph.grid[i][j].isBlocked) {
                    graph.grid[i][j].letter = allLetters[rand() % allLetters.size()];
                }
                else {
                    graph.grid[i][j].letter = 'A'; // Valeur par défaut si le fichier est vide
                }
            }
        }
    }

    // Placer la case d'arrivée (badge)
    void placeExit() {
        exit = sf::Vector2i(graph.rows - 1, graph.cols - 1);
        graph.grid[exit.x][exit.y].isExit = true;
    }

    // Placer des cases bloquées (limité à 4)
    void placeBlockedCells() {
        int numBlocked = 4; // Limité à 4 cases bloquées
        int placed = 0;
        while (placed < numBlocked) {
            int x = rand() % graph.rows;
            int y = rand() % graph.cols;
            if (!graph.grid[x][y].isExit && !graph.grid[x][y].isSpecial && !graph.grid[x][y].isBlocked) {
                graph.grid[x][y].isBlocked = true;
                placed++;
            }
        }
    }

    // Placer des cases spéciales
    void placeSpecialCells() {
        int numSpecial = (graph.rows * graph.cols) / 20; // 5% des cases sont spéciales
        for (int i = 0; i < numSpecial; ++i) {
            int x = rand() % graph.rows;
            int y = rand() % graph.cols;
            if (!graph.grid[x][y].isExit && !graph.grid[x][y].isBlocked) {
                graph.grid[x][y].isSpecial = true;
                // Assurer que la case spéciale est connectée à ses quatre voisins
                connectSpecialCell(x, y);
            }
        }
    }

    // Connecter une case spéciale à ses quatre voisins
    void connectSpecialCell(int x, int y) {
        int dx[] = { -1, 1, 0, 0 };
        int dy[] = { 0, 0, -1, 1 };
        for (int i = 0; i < 4; ++i) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            Node* neighbor = graph.getNode(nx, ny);
            if (neighbor && !neighbor->isBlocked) {
                graph.addEdge(graph.grid[x][y], *neighbor);
            }
        }
    }

    // Placer la case de départ
    void placeEntry(int x, int y) {
        entry = sf::Vector2i(x, y);
    }

    // Vérifier si un chemin est ouvert entre deux nœuds
    bool isPathClear(Node* a, Node* b) {
        for (Node* neighbor : a->neighbors) {
            if (neighbor == b) {
                return true;
            }
        }
        return false;
    }

    // Trouver le chemin le plus court avec BFS
    std::vector<Node*> findShortestPath(Node* start, Node* end) {
        std::queue<Node*> queue;
        std::unordered_map<Node*, bool> visited;
        std::unordered_map<Node*, Node*> parent;

        queue.push(start);
        visited[start] = true;

        while (!queue.empty()) {
            Node* current = queue.front();
            queue.pop();

            if (current == end) {
                // Reconstruire le chemin
                std::vector<Node*> path;
                for (Node* at = end; at != start; at = parent[at]) {
                    path.push_back(at);
                }
                path.push_back(start);
                std::reverse(path.begin(), path.end());
                return path;
            }

            // Explorer les voisins
            for (Node* neighbor : current->neighbors) {
                if (!visited[neighbor] && !neighbor->isBlocked) { // Ignorer les cases bloquées
                    queue.push(neighbor);
                    visited[neighbor] = true;
                    parent[neighbor] = current;
                }
            }
        }

        return {}; // Aucun chemin trouvé
    }
};

// Classe pour représenter le joueur (élève)
class Player {
public:
    Node* position;
    std::string collectedLetters;
    std::vector<std::string> validWords;
    std::vector<Node*> pathTaken; // Chemin pris par le joueur
    sf::Clock moveClock;

    Player() : position(nullptr) {}

    // Déplacer le joueur
    void move(Node* newPos, Maze& maze) {
        if (maze.isPathClear(position, newPos) && !newPos->isUsed && !newPos->isBlocked) {
            position->playerVisited = true; // Marquer l'ancienne position comme visitée
            position = newPos;
            pathTaken.push_back(newPos);

            // Ajouter la lettre ou un espace si c'est une case spéciale
            if (newPos->isSpecial) {
                collectedLetters += ' '; // Ajouter un espace
                applySpecialEffect(newPos); // Appliquer l'effet spécial
            }
            else {
                collectedLetters += newPos->letter; // Ajouter la lettre
            }
        }
    }

    // Appliquer les effets des cases spéciales
    void applySpecialEffect(Node* node) {
        std::cout << "Case spéciale ! Bonus de 10 points !" << std::endl;
        // Ajouter 10 points au score
        // (Le score sera mis à jour dans la classe Game)
    }

    // Dessiner le joueur
    void draw(sf::RenderWindow& window, float cellSize, const sf::Texture& playerTexture) {
        if (position) {
            sf::RectangleShape playerShape(sf::Vector2f(cellSize, cellSize));
            playerShape.setPosition(position->y * cellSize, position->x * cellSize);
            playerShape.setTexture(&playerTexture);
            window.draw(playerShape);
        }
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
    sf::Text defeatText;
    sf::Texture wallTexture, playerTexture, badgeTexture, blockedTexture, specialTexture;
    std::unordered_set<std::string> dictionary;
    int score;
    int attempts; // Nombre de tentatives
    sf::Clock gameClock; // Chronomètre pour mesurer le temps écoulé
    bool gameWon; // Indique si le joueur a gagné
    bool gameLost; // Indique si le joueur a perdu
    std::vector<Node*> shortestPath; // Chemin le plus court
    int difficulty; // Niveau de difficulté

    Game(int rows, int cols, int diff) : maze(rows, cols), score(0), gameWon(false), gameLost(false), difficulty(diff) {
        // Initialiser le nombre de tentatives en fonction de la difficulté
        switch (difficulty) {
        case 1:
            attempts = 2; // Facile : 2 tentatives
            break;
        case 2:
            attempts = 3; // Moyen : 3 tentatives
            break;
        case 3:
            attempts = 5; // Difficile : 5 tentatives
            break;
        default:
            attempts = 3; // Par défaut : 3 tentatives
            break;
        }

        if (!font.loadFromFile("Roboto.ttf")) {
            std::cerr << "Erreur: Impossible de charger la police Roboto.ttf" << std::endl;
        }
        if (!wallTexture.loadFromFile("wall.jpg")) {
            std::cerr << "Erreur: Impossible de charger la texture du mur (wall.jpg)" << std::endl;
        }
        if (!playerTexture.loadFromFile("player.png")) {
            std::cerr << "Erreur: Impossible de charger la texture du joueur (player.png)" << std::endl;
        }
        if (!badgeTexture.loadFromFile("bag.png")) {
            std::cerr << "Erreur: Impossible de charger la texture du badge (badge.png)" << std::endl;
        }
        if (!blockedTexture.loadFromFile("blocked.png")) {
            std::cerr << "Erreur: Impossible de charger la texture des cases bloquées (blocked.png)" << std::endl;
        }
        if (!specialTexture.loadFromFile("special.png")) {
            std::cerr << "Erreur: Impossible de charger la texture des cases spéciales (special.png)" << std::endl;
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

        defeatText.setFont(font);
        defeatText.setCharacterSize(40);
        defeatText.setFillColor(sf::Color::Red);
        defeatText.setPosition(200, 200);
        defeatText.setString("Défaite !");

        loadDictionary("dictionary.txt");

        // Demander à l'utilisateur de choisir la case de départ ou de la générer aléatoirement
        std::cout << "Voulez-vous choisir la case de départ ? (o/n): ";
        char choice;
        std::cin >> choice;

        if (choice == 'o' || choice == 'O') {
            int startX, startY;
            std::cout << "Entrez les coordonnées de la case de départ (x y): ";
            std::cin >> startX >> startY;
            maze.placeEntry(startX, startY);
        }
        else {
            // Générer une case de départ aléatoire
            int startX = rand() % maze.graph.rows;
            int startY = rand() % maze.graph.cols;
            maze.placeEntry(startX, startY);
        }

        // Initialiser la position du joueur
        player.position = maze.graph.getNode(maze.entry.x, maze.entry.y);

        // Trouver le chemin le plus court
        shortestPath = maze.findShortestPath(player.position, maze.graph.getNode(maze.exit.x, maze.exit.y));
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

    void resetLevel() {
        // Réinitialiser le labyrinthe
        maze = Maze(maze.graph.rows, maze.graph.cols);

        // Réinitialiser la position du joueur
        player.position = maze.graph.getNode(maze.entry.x, maze.entry.y);
        player.collectedLetters.clear();
        player.pathTaken.clear();
        player.validWords.clear();

        // Réinitialiser les cases utilisées
        for (int i = 0; i < maze.graph.rows; ++i) {
            for (int j = 0; j < maze.graph.cols; ++j) {
                maze.graph.grid[i][j].isUsed = false;
                maze.graph.grid[i][j].playerVisited = false;
            }
        }

        // Réinitialiser le chemin le plus court
        shortestPath = maze.findShortestPath(player.position, maze.graph.getNode(maze.exit.x, maze.exit.y));

        // Réinitialiser le chronomètre
        gameClock.restart();

        // Réinitialiser les états de victoire et de défaite
        gameWon = false;
        gameLost = false;

        // Réinitialiser les tentatives (selon la difficulté)
        switch (difficulty) {
        case 1:
            attempts = 2;
            break;
        case 2:
            attempts = 3;
            break;
        case 3:
            attempts = 5;
            break;
        default:
            attempts = 3;
            break;
        }

        // Mettre à jour l'affichage du score et des mots valides
        scoreText.setString("Score: " + std::to_string(score));
        validWordsText.setString("Mots valides: ");
    }

    void run() {
        sf::RenderWindow window(sf::VideoMode(800, 600), "Labyrinthe Scolaire");

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                // Gestion des touches pour déplacer le joueur
                if (event.type == sf::Event::KeyPressed && !gameWon && !gameLost) {
                    if (event.key.code == sf::Keyboard::Up) {
                        Node* newPos = maze.graph.getNode(player.position->x - 1, player.position->y);
                        if (newPos) player.move(newPos, maze);
                    }
                    else if (event.key.code == sf::Keyboard::Down) {
                        Node* newPos = maze.graph.getNode(player.position->x + 1, player.position->y);
                        if (newPos) player.move(newPos, maze);
                    }
                    else if (event.key.code == sf::Keyboard::Left) {
                        Node* newPos = maze.graph.getNode(player.position->x, player.position->y - 1);
                        if (newPos) player.move(newPos, maze);
                    }
                    else if (event.key.code == sf::Keyboard::Right) {
                        Node* newPos = maze.graph.getNode(player.position->x, player.position->y + 1);
                        if (newPos) player.move(newPos, maze);
                    }
                    else if (event.key.code == sf::Keyboard::R) {
                        resetLevel(); // Réinitialiser le niveau avec la touche "R"
                    }
                }
            }

            update();
            render(window);
        }
    }

    void update() {
        if (!gameWon && !gameLost) {
            // Vérifier si le joueur a atteint l'arrivée
            if (player.position->isExit) {
                gameWon = true;
                std::cout << "Félicitations ! Vous avez atteint l'arrivée en "
                    << gameClock.getElapsedTime().asSeconds() << " secondes." << std::endl;

                // Comparer le chemin pris avec le chemin le plus court
                if (player.pathTaken == shortestPath) {
                    score += 50; // Bonus pour avoir trouvé le chemin le plus court
                }
                else {
                    int extraSteps = player.pathTaken.size() - shortestPath.size();
                    score += std::max(0, 50 - (extraSteps * 5)); // Réduire le bonus en fonction des étapes supplémentaires
                }
                scoreText.setString("Score: " + std::to_string(score));
            }

            // Vérifier si un mot a été formé
            if (player.collectedLetters.size() >= 2) { // Un mot doit avoir au moins 2 lettres
                std::string currentWord = player.collectedLetters;

                // Vérifier si le mot est dans le dictionnaire
                if (dictionary.find(currentWord) != dictionary.end()) {
                    // Le mot est valide, libérer les cases utilisées
                    for (Node* node : player.pathTaken) {
                        node->isUsed = false; // Libérer la case pour qu'elle puisse être réutilisée
                    }

                    // Ajouter le mot à la liste des mots valides
                    player.validWords.push_back(currentWord);

                    // Mettre à jour le score
                    updateScore();

                    // Réinitialiser les lettres collectées et le chemin pris
                    player.collectedLetters.clear();
                    player.pathTaken.clear();

                    // Mettre à jour l'affichage des mots valides
                    std::string validWordsStr = "Mots valides: ";
                    for (const std::string& word : player.validWords) {
                        validWordsStr += word + " ";
                    }
                    validWordsText.setString(validWordsStr);
                }
            }

            // Vérifier si le joueur est sur une case spéciale et ajouter 10 points
            if (player.position->isSpecial && !player.position->playerVisited) {
                score += 10; // Ajouter 10 points pour une case spéciale
                scoreText.setString("Score: " + std::to_string(score));
                player.position->playerVisited = true; // Marquer la case comme visitée pour éviter de compter plusieurs fois
            }

            // Vérifier si le joueur est bloqué (plus de mouvements possibles)
            if (isPlayerStuck()) {
                attempts--; // Réduire le nombre de tentatives
                if (attempts <= 0) {
                    gameLost = true; // Le joueur a perdu
                    std::cout << "Vous avez épuisé toutes vos tentatives. Défaite !" << std::endl;
                }
                else {
                    resetPlayerPosition(); // Réinitialiser la position du joueur pour une nouvelle tentative
                    std::cout << "Tentative restante : " << attempts << std::endl;
                }
            }
        }
    }

    // Vérifier si le joueur est bloqué (plus de mouvements possibles)
    bool isPlayerStuck() {
        int dx[] = { -1, 1, 0, 0 };
        int dy[] = { 0, 0, -1, 1 };
        for (int i = 0; i < 4; ++i) {
            int nx = player.position->x + dx[i];
            int ny = player.position->y + dy[i];
            Node* neighbor = maze.graph.getNode(nx, ny);
            if (neighbor && !neighbor->isBlocked && !neighbor->isUsed) {
                return false; // Il y a encore des mouvements possibles
            }
        }
        return true; // Le joueur est bloqué
    }

    // Réinitialiser la position du joueur pour une nouvelle tentative
    void resetPlayerPosition() {
        player.position = maze.graph.getNode(maze.entry.x, maze.entry.y);
        player.collectedLetters.clear();
        player.pathTaken.clear();
        for (int i = 0; i < maze.graph.rows; ++i) {
            for (int j = 0; j < maze.graph.cols; ++j) {
                maze.graph.grid[i][j].isUsed = false; // Réinitialiser toutes les cases utilisées          
            }
        }
    }

    void updateScore() {
        int wordScore = 0;
        for (const std::string& word : player.validWords) {
            wordScore += word.length(); // 1 point par lettre
            // Bonus pour les mots de plus de 5 lettres
            if (word.length() > 5) {
                wordScore += 10; // Bonus de 10 points pour les mots longs
            }
        }

        // Bonus pour le chemin le plus court
        if (player.pathTaken == shortestPath) {
            wordScore += 50; // Bonus de 50 points pour le chemin le plus court
        }

        // Pénalité de temps
        int timePenalty = static_cast<int>(gameClock.getElapsedTime().asSeconds());
        score = std::max(0, wordScore - timePenalty); // Le score ne peut pas être négatif
        scoreText.setString("Score: " + std::to_string(score));
    }

    void render(sf::RenderWindow& window) {
        window.clear(sf::Color::White);

        // Dessiner le labyrinthe
        float cellSize = 40.f;
        for (int i = 0; i < maze.graph.rows; ++i) {
            for (int j = 0; j < maze.graph.cols; ++j) {
                Node* node = maze.graph.getNode(i, j);
                if (node) {
                    // Dessiner la cellule
                    sf::RectangleShape cellShape(sf::Vector2f(cellSize, cellSize));
                    cellShape.setPosition(j * cellSize, i * cellSize);

                    // Colorer la cellule en gris si elle a été visitée par le joueur
                    if (node->playerVisited) {
                        cellShape.setFillColor(sf::Color(200, 200, 200)); // Gris clair
                    }
                    else {
                        cellShape.setFillColor(sf::Color::White);
                    }

                    cellShape.setOutlineThickness(1.f);
                    cellShape.setOutlineColor(sf::Color::Black);
                    window.draw(cellShape);

                    // Dessiner les murs
                    if (i > 0 && !maze.isPathClear(node, maze.graph.getNode(i - 1, j))) {
                        sf::RectangleShape wallShape(sf::Vector2f(cellSize, 5));
                        wallShape.setPosition(j * cellSize, i * cellSize);
                        wallShape.setTexture(&wallTexture);
                        window.draw(wallShape);
                    }
                    if (i < maze.graph.rows - 1 && !maze.isPathClear(node, maze.graph.getNode(i + 1, j))) {
                        sf::RectangleShape wallShape(sf::Vector2f(cellSize, 5));
                        wallShape.setPosition(j * cellSize, (i + 1) * cellSize);
                        wallShape.setTexture(&wallTexture);
                        window.draw(wallShape);
                    }
                    if (j > 0 && !maze.isPathClear(node, maze.graph.getNode(i, j - 1))) {
                        sf::RectangleShape wallShape(sf::Vector2f(5, cellSize));
                        wallShape.setPosition(j * cellSize, i * cellSize);
                        wallShape.setTexture(&wallTexture);
                        window.draw(wallShape);
                    }
                    if (j < maze.graph.cols - 1 && !maze.isPathClear(node, maze.graph.getNode(i, j + 1))) {
                        sf::RectangleShape wallShape(sf::Vector2f(5, cellSize));
                        wallShape.setPosition((j + 1) * cellSize, i * cellSize);
                        wallShape.setTexture(&wallTexture);
                        window.draw(wallShape);
                    }

                    // Dessiner la lettre
                    if (node->letter != '\0') {
                        sf::Text letterText;
                        letterText.setFont(font);
                        letterText.setString(node->letter);
                        letterText.setCharacterSize(24);
                        letterText.setFillColor(sf::Color::Black);
                        letterText.setPosition(j * cellSize + 15, i * cellSize + 10);
                        window.draw(letterText);
                    }

                    // Dessiner le badge si c'est la case d'arrivée
                    if (node->isExit) {
                        sf::RectangleShape badgeShape(sf::Vector2f(cellSize, cellSize));
                        badgeShape.setPosition(j * cellSize, i * cellSize);
                        badgeShape.setTexture(&badgeTexture);
                        window.draw(badgeShape);
                    }

                    // Dessiner les cases bloquées
                    if (node->isBlocked) {
                        sf::RectangleShape blockedShape(sf::Vector2f(cellSize, cellSize));
                        blockedShape.setPosition(j * cellSize, i * cellSize);
                        blockedShape.setTexture(&blockedTexture);
                        window.draw(blockedShape);
                    }

                    // Dessiner les cases spéciales
                    if (node->isSpecial) {
                        sf::RectangleShape specialShape(sf::Vector2f(cellSize, cellSize));
                        specialShape.setPosition(j * cellSize, i * cellSize);
                        specialShape.setTexture(&specialTexture);
                        window.draw(specialShape);
                    }
                }
            }
        }

        // Dessiner le joueur
        player.draw(window, cellSize, playerTexture);

        // Afficher le temps écoulé
        timeText.setString("Temps: " + std::to_string(static_cast<int>(gameClock.getElapsedTime().asSeconds())) + "s");
        window.draw(timeText);

        // Afficher les lettres collectées
        window.draw(collectedLettersText);

        // Afficher le score
        window.draw(scoreText);

        // Afficher les mots valides
        window.draw(validWordsText);

        // Afficher le message de victoire ou de défaite
        if (gameWon) {
            window.draw(victoryText);
        }
        else if (gameLost) {
            window.draw(defeatText);
        }

        window.display();
    }
};

// Fonction principale
int main() {
    srand(static_cast<unsigned int>(time(0)));

    // Définir la taille de la grille en fonction de la difficulté
    int rows = 10; // Par défaut
    int cols = 10; // Par défaut

    // Exemple : ajuster la taille en fonction de la difficulté
    std::cout << "Choisissez le niveau de difficulté (1: Facile, 2: Moyen, 3: Difficile): ";
    int difficulty;
    std::cin >> difficulty;

    switch (difficulty) {
    case 1:
        rows = 8;
        cols = 13;
        break;
    case 2:
        rows = 12;
        cols = 17;
        break;
    case 3:
        rows = 15;
        cols = 20;
        break;
    default:
        std::cerr << "Niveau de difficulté invalide. Utilisation de la taille par défaut." << std::endl;
        break;
    }

    Game game(rows, cols, difficulty);
    game.run();
    return 0;
}