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
    bool isPath; // Indique si la case fait partie du plus court chemin

    std::vector<Node*> neighbors; // Liste des voisins accessibles

    Node() : letter('\0'), isExit(false), isBlocked(false), isSpecial(false), visited(false), isUsed(false), playerVisited(false), isPath(false) {}
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
            if (neighbor && !neighbor->visited && !neighbor->isBlocked && neighbor->letter == '\0') {
                neighbors.push_back(neighbor);
            }
        }
        return neighbors;
    }

    // Placer des lettres aléatoires dans les cellules
    void placeLetters(const std::string& filename) {
        std::vector<std::string> words = loadWordsFromDictionary(filename);
        fillMazeWithWords(graph, words);
    }

    // Lire les mots du fichier
    std::vector<std::string> loadWordsFromDictionary(const std::string& filename) {
        std::vector<std::string> words;
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Erreur: Impossible d'ouvrir le fichier " << filename << std::endl;
            return words;
        }

        std::string word;
        while (file >> word) {
            words.push_back(word);
        }

        file.close();
        return words;
    }

    // Remplir la matrice du labyrinthe avec les mots
    void fillMazeWithWords(Graph& graph, const std::vector<std::string>& words) {
        int wordIndex = 0;
        for (int i = 0; i < graph.rows; ++i) {
            for (int j = 0; j < graph.cols; ++j) {
                if (wordIndex < words.size()) {
                    std::string word = words[wordIndex];
                    for (int k = 0; k < word.length() && j < graph.cols; ++k, ++j) {
                        graph.grid[i][j].letter = toupper(word[k]);
                    }
                    wordIndex++;
                }
                else {
                    // Si on a épuisé tous les mots, on remplit le reste avec des lettres aléatoires
                    graph.grid[i][j].letter = 'A' + rand() % 26;
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
        std::vector<Node*> shortestPath = findShortestPath(graph.getNode(entry.x, entry.y), graph.getNode(exit.x, exit.y));

        while (placed < numBlocked) {
            int x = rand() % graph.rows;
            int y = rand() % graph.cols;
            Node* node = graph.getNode(x, y);

            // Vérifier si la case n'est pas sur le chemin le plus court
            bool isOnShortestPath = false;
            for (Node* pathNode : shortestPath) {
                if (pathNode == node) {
                    isOnShortestPath = true;
                    break;
                }
            }

            if (!isOnShortestPath && !node->isExit && !node->isSpecial && !node->isBlocked) {
                node->isBlocked = true;
                placed++;
            }
        }
    }

    // Placer des cases spéciales dans les cases vides
    void placeSpecialCells() {
        for (int i = 0; i < graph.rows; ++i) {
            for (int j = 0; j < graph.cols; ++j) {
                if (!graph.grid[i][j].isExit && !graph.grid[i][j].isBlocked && graph.grid[i][j].letter == '\0') {
                    graph.grid[i][j].isSpecial = true;
                    connectSpecialCell(i, j);
                }
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
    void draw(sf::RenderWindow& window, float cellSize, const sf::Texture& playerTexture, float startX, float startY) {
        if (position) {
            sf::RectangleShape playerShape(sf::Vector2f(cellSize, cellSize));
            playerShape.setPosition(startX + position->y * cellSize, startY + position->x * cellSize);
            playerShape.setTexture(&playerTexture);
            window.draw(playerShape);
        }
    }
};

// Classe pour gérer l'affichage des résultats
class ResultsScreen {
public:
    sf::Font font;
    sf::Text victoryText;
    sf::Text collectedLettersText;
    sf::Text validWordsText;
    sf::Text scoreText;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::Texture stickerTexture;
    sf::Sprite stickerSprite;

    ResultsScreen() {
        if (!font.loadFromFile("Roboto-Bold.ttf")) { // Utiliser une police en gras
            std::cerr << "Erreur: Impossible de charger la police Roboto-Bold.ttf" << std::endl;
        }
        if (!backgroundTexture.loadFromFile("victory_background.png")) { // Fond d'écran stylisé
            std::cerr << "Erreur: Impossible de charger la texture de l'arrière-plan (victory_background.png)" << std::endl;
        }
        backgroundSprite.setTexture(backgroundTexture);

        if (!stickerTexture.loadFromFile("sticker.png")) { // Sticker pour embellir
            std::cerr << "Erreur: Impossible de charger la texture du sticker (sticker.png)" << std::endl;
        }
        stickerSprite.setTexture(stickerTexture);
        stickerSprite.setPosition(800, 200); // Position du sticker

        victoryText.setFont(font);
        victoryText.setCharacterSize(60);
        victoryText.setFillColor(sf::Color::Blue);
        victoryText.setStyle(sf::Text::Bold);
        victoryText.setPosition(400, 10);
        victoryText.setString("Victoire !");

        collectedLettersText.setFont(font);
        collectedLettersText.setCharacterSize(20);
        collectedLettersText.setFillColor(sf::Color::Black);
        collectedLettersText.setStyle(sf::Text::Bold);
        collectedLettersText.setPosition(320, 95);

        validWordsText.setFont(font);
        validWordsText.setCharacterSize(20);
        validWordsText.setFillColor(sf::Color::Black);
        validWordsText.setStyle(sf::Text::Bold);
        validWordsText.setPosition(550, 250);

        scoreText.setFont(font);
        scoreText.setCharacterSize(20);
        scoreText.setFillColor(sf::Color::Black);
        scoreText.setStyle(sf::Text::Bold);
        scoreText.setPosition(550, 300);
    }

    void setResults(const std::string& collectedLetters, const std::vector<std::string>& validWords, int score) {
        collectedLettersText.setString("Lettres collectées: " + collectedLetters);

        std::string validWordsStr = "Mots valides: ";
        for (const std::string& word : validWords) {
            validWordsStr += word + " ";
        }
        validWordsText.setString(validWordsStr);

        scoreText.setString("Score: " + std::to_string(score));
    }

    void draw(sf::RenderWindow& window) {
        window.clear(sf::Color::White);
        window.draw(backgroundSprite);
        window.draw(victoryText);
        window.draw(collectedLettersText);
        window.draw(validWordsText);
        window.draw(scoreText);
        window.draw(stickerSprite); // Dessiner le sticker
        window.display();
    }
};

// Classe principale du jeu
class Game {
public:
    Maze maze;
    Player player;
    sf::Font font;
    sf::Text timeText;
    sf::Texture wallTexture, playerTexture, badgeTexture, blockedTexture, specialTexture, backgroundTexture;
    sf::Sprite backgroundSprite;
    std::unordered_set<std::string> dictionary;
    int score;
    int attempts; // Nombre de tentatives
    sf::Clock gameClock; // Chronomètre pour mesurer le temps écoulé
    bool gameWon; // Indique si le joueur a gagné
    bool gameLost; // Indique si le joueur a perdu
    std::vector<Node*> shortestPath; // Chemin le plus court
    int difficulty; // Niveau de difficulté
    ResultsScreen resultsScreen; // Écran des résultats
    bool showResults; // Indique si l'écran des résultats doit être affiché

    Game(int rows, int cols, int diff) : maze(rows, cols), score(0), gameWon(false), gameLost(false), difficulty(diff), showResults(false) {
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
            std::cerr << "Erreur: Impossible de charger la texture du badge (bag.png)" << std::endl;
        }
        if (!blockedTexture.loadFromFile("blocked.png")) {
            std::cerr << "Erreur: Impossible de charger la texture des cases bloquées (blocked.png)" << std::endl;
        }
        if (!specialTexture.loadFromFile("special.png")) {
            std::cerr << "Erreur: Impossible de charger la texture des cases spéciales (special.png)" << std::endl;
        }
        if (!backgroundTexture.loadFromFile("school_background.jpg")) {
            std::cerr << "Erreur: Impossible de charger la texture de l'arrière-plan (school_background.jpg)" << std::endl;
        }
        backgroundSprite.setTexture(backgroundTexture);

        timeText.setFont(font);
        timeText.setCharacterSize(24);
        timeText.setFillColor(sf::Color::Black);
        timeText.setPosition(10, 10);

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
    }

    void run() {
        sf::RenderWindow window(sf::VideoMode(1000, 100 + maze.graph.rows * 40), "Labyrinthe Scolaire");

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                // Gestion des touches pour déplacer le joueur
                if (event.type == sf::Event::KeyPressed && !gameWon && !gameLost && !showResults) {
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
        if (!gameWon && !gameLost && !showResults) {
            // Vérifier si le joueur a atteint l'arrivée
            if (player.position->isExit) {
                gameWon = true;
                showResults = true;
                resultsScreen.setResults(player.collectedLetters, player.validWords, score);
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

                    // Ajouter 50 points au score pour chaque mot valide
                    score += 50;

                    // Réinitialiser les lettres collectées et le chemin pris
                    player.collectedLetters.clear();
                    player.pathTaken.clear();
                }
            }

            // Vérifier si le joueur est sur une case spéciale et ajouter un espace
            if (player.position->isSpecial && !player.position->playerVisited) {
                player.collectedLetters += ' '; // Ajouter un espace pour les cases spéciales
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

    void render(sf::RenderWindow& window) {
        if (showResults) {
            resultsScreen.draw(window);
        }
        else {
            window.clear(sf::Color::White);

            // Dessiner l'arrière-plan
            window.draw(backgroundSprite);

            // Taille de la fenêtre
            float windowWidth = window.getSize().x;
            float windowHeight = window.getSize().y;

            // Taille du labyrinthe
            float mazeWidth = maze.graph.cols * 40.f;
            float mazeHeight = maze.graph.rows * 40.f;

            // Calculer la position de départ pour centrer le labyrinthe
            float startX = (windowWidth - mazeWidth) / 2;
            float startY = (windowHeight - mazeHeight) / 2;

            // Dessiner le labyrinthe
            for (int i = 0; i < maze.graph.rows; ++i) {
                for (int j = 0; j < maze.graph.cols; ++j) {
                    Node* node = maze.graph.getNode(i, j);
                    if (node) {
                        // Dessiner la cellule
                        sf::RectangleShape cellShape(sf::Vector2f(40.f, 40.f));
                        cellShape.setPosition(startX + j * 40.f, startY + i * 40.f);

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
                            sf::RectangleShape wallShape(sf::Vector2f(40.f, 5));
                            wallShape.setPosition(startX + j * 40.f, startY + i * 40.f);
                            wallShape.setTexture(&wallTexture);
                            window.draw(wallShape);
                        }
                        if (i < maze.graph.rows - 1 && !maze.isPathClear(node, maze.graph.getNode(i + 1, j))) {
                            sf::RectangleShape wallShape(sf::Vector2f(40.f, 5));
                            wallShape.setPosition(startX + j * 40.f, startY + (i + 1) * 40.f);
                            wallShape.setTexture(&wallTexture);
                            window.draw(wallShape);
                        }
                        if (j > 0 && !maze.isPathClear(node, maze.graph.getNode(i, j - 1))) {
                            sf::RectangleShape wallShape(sf::Vector2f(5, 40.f));
                            wallShape.setPosition(startX + j * 40.f, startY + i * 40.f);
                            wallShape.setTexture(&wallTexture);
                            window.draw(wallShape);
                        }
                        if (j < maze.graph.cols - 1 && !maze.isPathClear(node, maze.graph.getNode(i, j + 1))) {
                            sf::RectangleShape wallShape(sf::Vector2f(5, 40.f));
                            wallShape.setPosition(startX + (j + 1) * 40.f, startY + i * 40.f);
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
                            letterText.setPosition(startX + j * 40.f + 15, startY + i * 40.f + 10);
                            window.draw(letterText);
                        }

                        // Dessiner le badge si c'est la case d'arrivée
                        if (node->isExit) {
                            sf::RectangleShape badgeShape(sf::Vector2f(40.f, 40.f));
                            badgeShape.setPosition(startX + j * 40.f, startY + i * 40.f);
                            badgeShape.setTexture(&badgeTexture);
                            window.draw(badgeShape);
                        }

                        // Dessiner les cases bloquées
                        if (node->isBlocked) {
                            sf::RectangleShape blockedShape(sf::Vector2f(40.f, 40.f));
                            blockedShape.setPosition(startX + j * 40.f, startY + i * 40.f);
                            blockedShape.setTexture(&blockedTexture);
                            window.draw(blockedShape);
                        }

                        // Dessiner les cases spéciales
                        if (node->isSpecial) {
                            sf::RectangleShape specialShape(sf::Vector2f(40.f, 40.f));
                            specialShape.setPosition(startX + j * 40.f, startY + i * 40.f);
                            specialShape.setTexture(&specialTexture);
                            window.draw(specialShape);
                        }
                    }
                }
            }

            // Dessiner le joueur
            player.draw(window, 40.f, playerTexture, startX, startY);

            // Afficher le temps écoulé
            timeText.setString("Temps: " + std::to_string(static_cast<int>(gameClock.getElapsedTime().asSeconds())) + "s");
            timeText.setPosition(startX, startY + maze.graph.rows * 40.f + 10);
            window.draw(timeText);

            window.display();
        }
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