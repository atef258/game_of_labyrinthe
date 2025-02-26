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
#include <sstream>

// Classe pour la page d'accueil
class HomeScreen {
public:
    sf::Font font;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::Texture logoTexture;
    sf::Sprite logoSprite;

    // Boutons principaux
    sf::RectangleShape difficultyButton;
    sf::Text difficultyButtonText;
    sf::RectangleShape startPositionButton;
    sf::Text startPositionButtonText;
    sf::RectangleShape startGameButton;
    sf::Text startGameButtonText;
    sf::RectangleShape timeLimitButton;
    sf::Text timeLimitButtonText;
    sf::RectangleShape usernameButton; // Nouveau bouton pour le nom d'utilisateur
    sf::Text usernameButtonText;

    // Sous-menus
    bool showDifficultyOptions;
    sf::RectangleShape easyButton;
    sf::Text easyButtonText;
    sf::RectangleShape mediumButton;
    sf::Text mediumButtonText;
    sf::RectangleShape hardButton;
    sf::Text hardButtonText;

    bool showStartPositionOptions;
    sf::RectangleShape yesButton;
    sf::Text yesButtonText;
    sf::RectangleShape noButton;
    sf::Text noButtonText;
    sf::Text coordinatesPromptText;
    sf::Text coordinatesInputText;

    bool showTimeLimitOptions;
    sf::RectangleShape noTimeLimitButton;
    sf::Text noTimeLimitButtonText;
    sf::RectangleShape shortTimeLimitButton;
    sf::Text shortTimeLimitButtonText;
    sf::RectangleShape longTimeLimitButton;
    sf::Text longTimeLimitButtonText;

    bool showUsernameInput; // Nouveau booléen pour afficher le champ de saisie du nom d'utilisateur
    sf::Text usernamePromptText;
    sf::Text usernameInputText;
    std::string usernameInputString;

    // Variables pour stocker les choix
    int selectedDifficulty; // 1: Facile, 2: Moyen, 3: Difficile
    bool chooseStart; // true: choisir manuellement, false: générer aléatoirement
    std::string coordinatesInputString;
    int timeLimit; // Temps limite en secondes (0 pour aucun temps limite)
    bool startGame;
    std::string errorMessage; // Message d'erreur
    sf::Vector2i startPosition;

    HomeScreen() : selectedDifficulty(1), chooseStart(false), timeLimit(0), startGame(false), showDifficultyOptions(false), showStartPositionOptions(false), showTimeLimitOptions(false), showUsernameInput(false), startPosition(-1, -1) {
        // Charger la police de caractères
        if (!font.loadFromFile("Roboto.ttf")) {
            std::cerr << "Erreur: Impossible de charger la police Roboto.ttf" << std::endl;
        }

        // Charger l'arrière-plan
        if (!backgroundTexture.loadFromFile("poster.png")) {
            std::cerr << "Erreur: Impossible de charger l'arrière-plan poster.png" << std::endl;
        }
        backgroundSprite.setTexture(backgroundTexture);

        // Redimensionner l'arrière-plan
        float scaleFactor = 0.45f; // Par exemple, réduire la taille de moitié
        backgroundSprite.setScale(scaleFactor, scaleFactor);

        // Centrer l'arrière-plan
        sf::Vector2u windowSize(800, 900); // Taille de la fenêtre
        sf::FloatRect backgroundBounds = backgroundSprite.getLocalBounds();
        float posX = (windowSize.x - backgroundBounds.width * scaleFactor) / 2;
        float posY = (windowSize.y - backgroundBounds.height * scaleFactor) / 2;
        backgroundSprite.setPosition(posX, posY);

        // Configurer les boutons principaux
        setupButton(difficultyButton, difficultyButtonText, "Choisir la difficulte", 110, 280);
        setupButton(startPositionButton, startPositionButtonText, "Choisir la case de depart", 250, 375);
        setupButton(timeLimitButton, timeLimitButtonText, "Temps limite", 450, 480);
        setupButton(startGameButton, startGameButtonText, "   ", 250, 700);
        setupButton(usernameButton, usernameButtonText, "     ", 400, 200); // Nouveau bouton

        // Configurer les sous-menus
        setupButton(easyButton, easyButtonText, "Facile", 400, 230);
        setupButton(mediumButton, mediumButtonText, "Moyen", 400, 280);
        setupButton(hardButton, hardButtonText, "Difficile", 400, 330);

        setupButton(yesButton, yesButtonText, "Oui", 520, 350);
        setupButton(noButton, noButtonText, "Non", 520, 400);

        setupButton(noTimeLimitButton, noTimeLimitButtonText, "Aucun", 550, 550);
        setupButton(shortTimeLimitButton, shortTimeLimitButtonText, "Court (60s)", 550, 600);
        setupButton(longTimeLimitButton, longTimeLimitButtonText, "Long (120s)", 550, 650);

        // Configurer les textes
        coordinatesPromptText.setFont(font);
        coordinatesPromptText.setString("Entrez les coordonnees (x y):");
        coordinatesPromptText.setCharacterSize(20);
        coordinatesPromptText.setFillColor(sf::Color::Black);
        coordinatesPromptText.setPosition(265, 420);

        coordinatesInputText.setFont(font);
        coordinatesInputText.setCharacterSize(30);
        coordinatesInputText.setFillColor(sf::Color::Black);
        coordinatesInputText.setPosition(550, 410);

        usernamePromptText.setFont(font);
        usernamePromptText.setString("      ");
        usernamePromptText.setCharacterSize(20);
        usernamePromptText.setFillColor(sf::Color::Black);
        usernamePromptText.setPosition(100, 200);

        usernameInputText.setFont(font);
        usernameInputText.setCharacterSize(30);
        usernameInputText.setFillColor(sf::Color::Black);
        usernameInputText.setPosition(420, 200);

        errorMessage = "";
    }

    void setupButton(sf::RectangleShape& button, sf::Text& buttonText, const std::string& label, float x, float y) {
        button.setSize(sf::Vector2f(300, 50));
        button.setPosition(x, y);
        button.setFillColor(sf::Color::Transparent);
        buttonText.setFont(font);
        buttonText.setString(label);
        buttonText.setCharacterSize(26);
        buttonText.setFillColor(sf::Color::Black);
        buttonText.setPosition(x + 10, y + 10);
    }


    void handleEvent(sf::Event& event) {
        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2f mousePos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);

            // Gestion du bouton de difficulté
            if (difficultyButton.getGlobalBounds().contains(mousePos)) {
                showDifficultyOptions = !showDifficultyOptions;
                showStartPositionOptions = false;
                showTimeLimitOptions = false;
                showUsernameInput = false;
            }
            // Gestion des options de difficulté
            if (showDifficultyOptions) {
                if (easyButton.getGlobalBounds().contains(mousePos)) {
                    selectedDifficulty = 1;
                    showDifficultyOptions = false;
                }
                else if (mediumButton.getGlobalBounds().contains(mousePos)) {
                    selectedDifficulty = 2;
                    showDifficultyOptions = false;
                }
                else if (hardButton.getGlobalBounds().contains(mousePos)) {
                    selectedDifficulty = 3;
                    showDifficultyOptions = false;
                }
            }

            // Gestion du bouton de case de départ
            if (startPositionButton.getGlobalBounds().contains(mousePos)) {
                showStartPositionOptions = !showStartPositionOptions;
                showDifficultyOptions = false;
                showTimeLimitOptions = false;
                showUsernameInput = false;
            }
            // Gestion de la saisie des coordonnées
            if (chooseStart && event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b' && !coordinatesInputString.empty()) {
                    coordinatesInputString.pop_back();
                }
                else if (event.text.unicode < 128) {
                    coordinatesInputString += static_cast<char>(event.text.unicode);
                }
                coordinatesInputText.setString("(x = " + coordinatesInputString + ", y = " + coordinatesInputString + ")");
            }
            // Gestion des options de case de départ
            if (showStartPositionOptions) {
                if (yesButton.getGlobalBounds().contains(mousePos)) {
                    chooseStart = true;
                    showStartPositionOptions = false;
                }
                else if (noButton.getGlobalBounds().contains(mousePos)) {
                    chooseStart = false;
                    showStartPositionOptions = false;
                }
            }

            // Gestion du bouton de temps limite
            if (timeLimitButton.getGlobalBounds().contains(mousePos)) {
                showTimeLimitOptions = !showTimeLimitOptions;
                showDifficultyOptions = false;
                showStartPositionOptions = false;
                showUsernameInput = false;
            }

            // Gestion des options de temps limite
            if (showTimeLimitOptions) {
                if (noTimeLimitButton.getGlobalBounds().contains(mousePos)) {
                    timeLimit = 0;
                    showTimeLimitOptions = false;
                }
                else if (shortTimeLimitButton.getGlobalBounds().contains(mousePos)) {
                    timeLimit = 60;
                    showTimeLimitOptions = false;
                }
                else if (longTimeLimitButton.getGlobalBounds().contains(mousePos)) {
                    timeLimit = 120;
                    showTimeLimitOptions = false;
                }
            }

            // Gestion du bouton de nom d'utilisateur
            if (usernameButton.getGlobalBounds().contains(mousePos)) {
                showUsernameInput = !showUsernameInput;
                showDifficultyOptions = false;
                showStartPositionOptions = false;
                showTimeLimitOptions = false;
            }
            // Gestion de la saisie du nom d'utilisateur
            if (showUsernameInput && event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b' && !usernameInputString.empty()) {
                    usernameInputString.pop_back();
                }
                else if (event.text.unicode < 128) {
                    usernameInputString += static_cast<char>(event.text.unicode);
                }
                usernameInputText.setString(usernameInputString);
            }
            // Gestion du bouton de démarrage
            if (startGameButton.getGlobalBounds().contains(mousePos)) {
                if (validateInputs()) {
                    startGame = true;
                }
            }
        }

        // Gestion de la saisie des coordonnées
        if (chooseStart && event.type == sf::Event::TextEntered) {
            if (event.text.unicode == '\b' && !coordinatesInputString.empty()) {
                coordinatesInputString.pop_back();
            }
            else if (event.text.unicode < 128) {
                coordinatesInputString += static_cast<char>(event.text.unicode);
            }
            coordinatesInputText.setString(coordinatesInputString);
        }

        // Gestion de la saisie du nom d'utilisateur
        if (showUsernameInput && event.type == sf::Event::TextEntered) {
            if (event.text.unicode == '\b' && !usernameInputString.empty()) {
                usernameInputString.pop_back();
            }
            else if (event.text.unicode < 128) {
                usernameInputString += static_cast<char>(event.text.unicode);
            }
            usernameInputText.setString(usernameInputString);
        }
    }

    bool validateInputs() {
        if (chooseStart) {
            std::istringstream iss(coordinatesInputString);
            int x, y;
            iss >> x >> y;
            if (x < 0 || x >= 15 || y < 0 || y >= 20) { // Taille par défaut du labyrinthe
                errorMessage = "Coordonnees invalides !";
                return false;
            }
        }
        if (usernameInputString.empty()) {
            errorMessage = "Veuillez entrer un nom d'utilisateur !";
            return false;
        }
        errorMessage = "";
        return true;
    }

    void draw(sf::RenderWindow& window) {
        window.clear();
        window.draw(backgroundSprite); // Dessiner l'arrière-plan redimensionné et centré
        window.draw(logoSprite);

        // Dessiner les boutons principaux
        window.draw(difficultyButton);
        window.draw(difficultyButtonText);
        window.draw(startPositionButton);
        window.draw(startPositionButtonText);
        window.draw(timeLimitButton);
        window.draw(timeLimitButtonText);
        window.draw(startGameButton);
        window.draw(startGameButtonText);
        window.draw(usernameButton);
        window.draw(usernameButtonText);

        // Dessiner les sous-menus si nécessaire
        if (showDifficultyOptions) {
            window.draw(easyButton);
            window.draw(easyButtonText);
            window.draw(mediumButton);
            window.draw(mediumButtonText);
            window.draw(hardButton);
            window.draw(hardButtonText);
        }

        if (showStartPositionOptions) {
            window.draw(yesButton);
            window.draw(yesButtonText);
            window.draw(noButton);
            window.draw(noButtonText);
        }

        if (showTimeLimitOptions) {
            window.draw(noTimeLimitButton);
            window.draw(noTimeLimitButtonText);
            window.draw(shortTimeLimitButton);
            window.draw(shortTimeLimitButtonText);
            window.draw(longTimeLimitButton);
            window.draw(longTimeLimitButtonText);
        }

        // Afficher le champ de saisie des coordonnées si nécessaire
        if (chooseStart) {
            window.draw(coordinatesPromptText);
            window.draw(coordinatesInputText);
        }

        // Afficher le champ de saisie du nom d'utilisateur si nécessaire
        if (showUsernameInput) {
            window.draw(usernamePromptText);
            window.draw(usernameInputText);
        }

        // Afficher un message d'erreur
        if (!errorMessage.empty()) {
            sf::Text errorText(errorMessage, font, 24);
            errorText.setFillColor(sf::Color::Red);
            errorText.setPosition(250, 800);
            window.draw(errorText);
        }

        window.display();
    }
};
struct Node {
    int x, y; // Coordonnées de la cellule
    char letter; // Lettre dans la cellule
    bool isExit; // Indique si c'est la case d'arrivée
    bool isBlocked; // Indique si la case est bloquée
    int specialBonus; // Bonus de la case spéciale
    bool isSpecial; // Indique si la case est spéciale
    bool visited; // Utilisé pour la génération du labyrinthe
    bool isUsed; // Indique si la case a été utilisée pour former un mot
    bool playerVisited; // Indique si la case a été visitée par le joueur
    std::vector<Node*> neighbors; // Liste des voisins accessibles

    Node() : letter('\0'), isExit(false), isBlocked(false), isSpecial(false), specialBonus(false), visited(false), isUsed(false), playerVisited(false) {}
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
        placeSpecialCells(5);
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
        // Charger les mots du dictionnaire et les concaténer
        std::string concatenatedWords = loadWordsFromDictionary(filename);

        // Trouver le chemin le plus court
        std::vector<Node*> shortestPath = findShortestPath(graph.getNode(0, 0), graph.getNode(graph.rows - 1, graph.cols - 1));

        // Remplir le chemin le plus court avec les lettres concaténées
        fillShortestPathWithLetters(graph, shortestPath, concatenatedWords);

        // Remplir les cases restantes avec des lettres aléatoires
        fillRemainingCells(graph);
    }

    // Lire les mots du fichier
    std::string loadWordsFromDictionary(const std::string& filename) {
        std::string concatenatedWords;
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Erreur: Impossible d'ouvrir le fichier " << filename << std::endl;
            return concatenatedWords;
        }

        std::string word;
        while (file >> word) {
            concatenatedWords += word;
        }

        file.close();
        return concatenatedWords;
    }
    // Remplir le chemin le plus court avec les lettres concaténées
    void fillShortestPathWithLetters(Graph& graph, const std::vector<Node*>& shortestPath, const std::string& concatenatedWords) {
        for (size_t i = 0; i < shortestPath.size(); ++i) {
            if (i < concatenatedWords.length()) {
                shortestPath[i]->letter = toupper(concatenatedWords[i]);
            }
            else {
                shortestPath[i]->letter = 'A' + rand() % 26;
            }
        }
    }

    // Remplir les cases restantes avec des lettres aléatoires
    void fillRemainingCells(Graph& graph) {
        for (int i = 0; i < graph.rows; ++i) {
            for (int j = 0; j < graph.cols; ++j) {
                if (graph.grid[i][j].letter == '\0') {
                    graph.grid[i][j].letter = 'A' + rand() % 26;
                }
            }
        }
    }
    // Placer la case d'arrivée (bag)
    void placeExit() {
        exit = sf::Vector2i(graph.rows - 1, graph.cols - 1);
        graph.grid[exit.x][exit.y].isExit = true;
    }
    void placeBonusCells(int numBonusCells) {
        int placed = 0;
        while (placed < numBonusCells) {
            int x = rand() % graph.rows;
            int y = rand() % graph.cols;
            Node* node = graph.getNode(x, y);

            if (node && !node->isExit && !node->isBlocked && !node->isSpecial && !node->isSpecial) {
                node->isSpecial = true;
                placed++;
            }
        }
    }

    // Placer des cases bloquées 
    void placeBlockedCells() {
        int numBlocked = 6; // Limité à 6 cases bloquées
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
    void placeSpecialCells(int numSpecialCells) {
        int placed = 0;
        while (placed < numSpecialCells) {
            int x = rand() % graph.rows;
            int y = rand() % graph.cols;
            Node* node = graph.getNode(x, y);

            if (node && !node->isExit && !node->isBlocked && !node->isSpecial) {
                node->isSpecial = true;
                node->specialBonus = 5 + rand() % 10; // Bonus aléatoire entre 5 et 15 points
                placed++;
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

    // Vérifier si un déplacement diagonal est autorisé
    bool isDiagonalMoveAllowed(Node* from, Node* to) {
        if (!to || to->isBlocked || to->isUsed) {
            return false; // La case diagonale est bloquée ou déjà utilisée
        }

        int dx = to->x - from->x;
        int dy = to->y - from->y;

        // Vérifier les deux directions orthogonales
        if (dx == 1 && dy == 1) { // Déplacement en bas-droite
            Node* bottom = graph.getNode(from->x + 1, from->y);
            Node* right = graph.getNode(from->x, from->y + 1);
            return bottom && right && isPathClear(from, bottom) && isPathClear(from, right);
        }
        else if (dx == 1 && dy == -1) { // Déplacement en bas-gauche
            Node* bottom = graph.getNode(from->x + 1, from->y);
            Node* left = graph.getNode(from->x, from->y - 1);
            return bottom && left && isPathClear(from, bottom) && isPathClear(from, left);
        }
        else if (dx == -1 && dy == 1) { // Déplacement en haut-droite
            Node* top = graph.getNode(from->x - 1, from->y);
            Node* right = graph.getNode(from->x, from->y + 1);
            return top && right && isPathClear(from, top) && isPathClear(from, right);
        }
        else if (dx == -1 && dy == -1) { // Déplacement en haut-gauche
            Node* top = graph.getNode(from->x - 1, from->y);
            Node* left = graph.getNode(from->x, from->y - 1);
            return top && left && isPathClear(from, top) && isPathClear(from, left);
        }

        return false; // Par défaut, le déplacement diagonal n'est pas autorisé
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
    void move(Node* newPos, Maze& maze, const std::unordered_set<std::string>& dictionary) {
        if (newPos && !newPos->isUsed && !newPos->isBlocked) {
            position->playerVisited = true; // Marquer l'ancienne position comme visitée
            position = newPos;
            pathTaken.push_back(newPos);

            // Ajouter la lettre ou un espace si c'est une case spéciale
            if (newPos->isSpecial) {
                applySpecialEffect(newPos); // Appliquer l'effet spécial
            }
            else {
                collectedLetters += newPos->letter; // Ajouter la lettre
                std::cout << "Lettres collectées : " << collectedLetters << std::endl; // Afficher toutes les lettres sur une seule ligne
                checkForValidWords(dictionary); // Vérifier les mots valides
            }
        }
    }

    void applySpecialEffect(Node* node) {
        std::cout << "Case spéciale traversée ! Bonus de " << node->specialBonus << " points !" << std::endl;
        // Le bonus sera ajouté au score dans la classe Game
    }

    // Vérifier si les lettres collectées forment un mot valide
    void checkForValidWords(const std::unordered_set<std::string>& dictionary) {
        for (const std::string& word : dictionary) {
            if (collectedLetters.find(word) != std::string::npos) {
                // Si le mot est valide et n'a pas déjà été trouvé
                if (std::find(validWords.begin(), validWords.end(), word) == validWords.end()) {
                    validWords.push_back(word); // Sauvegarder le mot valide
                    std::cout << "Mot valide trouvé : " << word << std::endl;
                }
            }
        }
    }
    // Récupérer les mots valides trouvés
    const std::vector<std::string>& getValidWords() const {
        return validWords;
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
    sf::Text timeText;
    //sf::Texture backgroundTextureVictory; // Texture pour la victoire
    sf::Texture backgroundTextureDefeat;  // Texture pour la défaite
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::Text usernameText;


    ResultsScreen() {
        if (!font.loadFromFile("Roboto-Bold.ttf")) {
            std::cerr << "Erreur: Impossible de charger la police Roboto-Bold.ttf" << std::endl;
        }

        // Initialiser les textes
        victoryText.setFont(font);
        victoryText.setCharacterSize(60);
        victoryText.setFillColor(sf::Color::Red);
        victoryText.setStyle(sf::Text::Bold);
        victoryText.setPosition(400, 10);

        collectedLettersText.setFont(font);
        collectedLettersText.setCharacterSize(24);
        collectedLettersText.setFillColor(sf::Color::Black);
        collectedLettersText.setPosition(50, 100);

        validWordsText.setFont(font);
        validWordsText.setCharacterSize(24);
        validWordsText.setFillColor(sf::Color::Black);
        validWordsText.setPosition(50, 150);

        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::Black);
        scoreText.setPosition(50, 200);

        timeText.setFont(font);
        timeText.setCharacterSize(24);
        timeText.setFillColor(sf::Color::Black);
        timeText.setPosition(50, 250);
        // Initialiser les textes
        usernameText.setFont(font);
        usernameText.setCharacterSize(24);
        usernameText.setFillColor(sf::Color::Black);
        usernameText.setPosition(50, 50);

        // Charger l'arrière-plan (si nécessaire)
        if (!backgroundTexture.loadFromFile("image8.jpg")) {
            std::cerr << "Erreur: Impossible de charger la texture de l'arrière-plan (victory_background.png)" << std::endl;
        }
        if (!backgroundTextureDefeat.loadFromFile("image4.jpg")) {
            std::cerr << "Erreur: Impossible de charger la texture de l'arrière-plan (defeat_background.png)" << std::endl;
        }

        backgroundSprite.setTexture(backgroundTexture);

        // Redimensionner l'arrière-plan pour qu'il couvre toute la fenêtre
        sf::Vector2u windowSize(800, 800); // Taille de la fenêtre
        sf::Vector2u textureSize = backgroundTexture.getSize();
        float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
        float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
        backgroundSprite.setScale(scaleX, scaleY);



    }
    void setUsername(const std::string& username) {
        usernameText.setString("Nom d'utilisateur: " + username);
    }
    void setResults(const std::string& collectedLetters, const std::vector<std::string>& validWords, int score, int timeElapsed, bool isVictory) {
        collectedLettersText.setString("Lettres collectees: " + collectedLetters);

        // Afficher les mots valides trouvés
        std::string validWordsStr = "Mots valides trouves: ";
        for (const std::string& word : validWords) {
            validWordsStr += word + " ";
        }
        validWordsText.setString(validWordsStr);

        scoreText.setString("Score: " + std::to_string(score));

        timeText.setString("Temps ecoule: " + std::to_string(timeElapsed) + " secondes");

        // Changer le message en fonction de la victoire ou de la défaite
        if (isVictory) {
            victoryText.setString("Victoire !");
            victoryText.setFillColor(sf::Color::Green);
            backgroundSprite.setTexture(backgroundTexture); // Utiliser l'arrière-plan de victoire
        }
        else {
            victoryText.setString("Defaite !");
            victoryText.setFillColor(sf::Color::Red);
            backgroundSprite.setTexture(backgroundTextureDefeat); // Utiliser l'arrière-plan de défaite
        }

        // Afficher les résultats dans le terminal
        std::cout << "Lettres collectees: " << collectedLetters << std::endl;
        std::cout << "Mots valides trouves: ";
        for (const std::string& word : validWords) {
            std::cout << word << " ";
        }
        std::cout << std::endl;
        std::cout << "Score: " << score << std::endl;
        std::cout << "Temps ecoule: " << timeElapsed << " secondes" << std::endl;
        std::cout << (isVictory ? "Victoire !" : "Defaite !") << std::endl;
    }

    void draw(sf::RenderWindow& window) {
        // Définir la taille de la fenêtre des résultats (800x800)
        window.setSize(sf::Vector2u(800, 800));
        window.setView(sf::View(sf::FloatRect(0, 0, 800, 800))); // Ajuster la vue

        window.clear(sf::Color::White);

        // Dessiner l'arrière-plan (si nécessaire)
        window.draw(backgroundSprite);

        // Dessiner les textes avec des bordures pour améliorer la visibilité
        sf::RectangleShape textBackground(sf::Vector2f(700, 400));
        textBackground.setPosition(50, 50);
        textBackground.setFillColor(sf::Color(255, 255, 255, 200)); // Fond semi-transparent
        window.draw(textBackground);

        // Dessiner les textes
        usernameText.setFillColor(sf::Color::Black);
        usernameText.setPosition(100, 190);
        window.draw(usernameText);

        victoryText.setFillColor(sf::Color::Black);
        victoryText.setPosition(300, 70);
        window.draw(victoryText);

        collectedLettersText.setFillColor(sf::Color::Black);
        collectedLettersText.setPosition(100, 240);
        window.draw(collectedLettersText);

        validWordsText.setFillColor(sf::Color::Black);
        validWordsText.setPosition(100, 290);
        window.draw(validWordsText);

        scoreText.setFillColor(sf::Color::Black);
        scoreText.setPosition(100, 340);
        window.draw(scoreText);

        timeText.setFillColor(sf::Color::Black);
        timeText.setPosition(100, 390);
        window.draw(timeText);

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
    sf::Text scoreText;
    sf::Texture wallTexture, playerTexture, badgeTexture, blockedTexture, specialTexture, backgroundTexture;
    sf::Sprite backgroundSprite;
    std::unordered_set<std::string> dictionary;
    int score;
    int attempts;
    sf::Clock gameClock;
    bool gameWon;
    bool gameLost;
    std::vector<Node*> shortestPath;
    int difficulty;
    ResultsScreen resultsScreen;
    bool showResults;

    // Nouveau : Ajouter un chronomètre pour le temps limite
    int timeLimit; // Temps limite en secondes
    sf::Clock limitClock; // Chronomètre pour le temps limite

    // Nouveau : Temps estimé pour chaque niveau de difficulté
    int estimatedTime; // Temps estimé pour atteindre la case d'arrivée

    Game(int rows, int cols, int diff, int timeLimit = 0, const std::string& username = "", int startX = 0, int startY = 0)
        : maze(rows, cols), score(0), gameWon(false), gameLost(false), difficulty(diff), showResults(false), timeLimit(timeLimit) {
        // Initialiser la position du joueur
        player.position = maze.graph.getNode(startX, startY);
        maze.placeEntry(startX, startY);
        // Initialiser le nom d'utilisateur
        resultsScreen.setUsername(username);

        // Trouver le chemin le plus court
        shortestPath = maze.findShortestPath(player.position, maze.graph.getNode(maze.exit.x, maze.exit.y));

        // Démarrer le chronomètre du temps limite
        limitClock.restart();

        // Initialiser le nombre de tentatives en fonction de la difficulté
        switch (difficulty) {
        case 1:
            attempts = 2;
            estimatedTime = 10; // Facile : 10 secondes
            break;
        case 2:
            attempts = 3;
            estimatedTime = 20; // Moyen : 20 secondes
            break;
        case 3:
            attempts = 5;
            estimatedTime = 30; // Difficile : 30 secondes
            break;
        default:
            attempts = 3;
            estimatedTime = 20; // Par défaut : Moyen
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

        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::Black);
        scoreText.setPosition(10, 40);

        loadDictionary("dictionary.txt");

        // Initialiser la position du joueur
        player.position = maze.graph.getNode(maze.entry.x, maze.entry.y);

        // Trouver le chemin le plus court
        shortestPath = maze.findShortestPath(player.position, maze.graph.getNode(maze.exit.x, maze.exit.y));

        // Démarrer le chronomètre du temps limite
        limitClock.restart();
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
                        if (newPos && maze.isPathClear(player.position, newPos)) player.move(newPos, maze, dictionary);

                    }
                    else if (event.key.code == sf::Keyboard::Down) {
                        Node* newPos = maze.graph.getNode(player.position->x + 1, player.position->y);
                        if (newPos && maze.isPathClear(player.position, newPos)) player.move(newPos, maze, dictionary);

                    }
                    else if (event.key.code == sf::Keyboard::Left) {
                        Node* newPos = maze.graph.getNode(player.position->x, player.position->y - 1);
                        if (newPos && maze.isPathClear(player.position, newPos)) player.move(newPos, maze, dictionary);

                    }
                    else if (event.key.code == sf::Keyboard::Right) {
                        Node* newPos = maze.graph.getNode(player.position->x, player.position->y + 1);
                        if (newPos && maze.isPathClear(player.position, newPos)) player.move(newPos, maze, dictionary);

                    }
                    // Déplacements diagonaux
                    else if (event.key.code == sf::Keyboard::Q) { // Haut-Gauche
                        Node* newPos = maze.graph.getNode(player.position->x - 1, player.position->y - 1);
                        if (newPos && maze.isDiagonalMoveAllowed(player.position, newPos)) player.move(newPos, maze, dictionary);

                    }
                    else if (event.key.code == sf::Keyboard::E) { // Haut-Droite
                        Node* newPos = maze.graph.getNode(player.position->x - 1, player.position->y + 1);
                        if (newPos && maze.isDiagonalMoveAllowed(player.position, newPos)) player.move(newPos, maze, dictionary);

                    }
                    else if (event.key.code == sf::Keyboard::A) { // Bas-Gauche
                        Node* newPos = maze.graph.getNode(player.position->x + 1, player.position->y - 1);
                        if (newPos && maze.isDiagonalMoveAllowed(player.position, newPos)) player.move(newPos, maze, dictionary);

                    }
                    else if (event.key.code == sf::Keyboard::D) { // Bas-Droite
                        Node* newPos = maze.graph.getNode(player.position->x + 1, player.position->y + 1);
                        if (newPos && maze.isDiagonalMoveAllowed(player.position, newPos)) player.move(newPos, maze, dictionary);

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
    void handlePlayerMovement(sf::Event& event) {
        if (event.type == sf::Event::KeyPressed && !gameWon && !gameLost && !showResults) {
            Node* newPos = nullptr;
            if (event.key.code == sf::Keyboard::Up) {
                newPos = maze.graph.getNode(player.position->x - 1, player.position->y);
            }
            else if (event.key.code == sf::Keyboard::Down) {
                newPos = maze.graph.getNode(player.position->x + 1, player.position->y);
            }
            else if (event.key.code == sf::Keyboard::Left) {
                newPos = maze.graph.getNode(player.position->x, player.position->y - 1);
            }
            else if (event.key.code == sf::Keyboard::Right) {
                newPos = maze.graph.getNode(player.position->x, player.position->y + 1);
            }

            if (newPos && maze.isPathClear(player.position, newPos)) {
                player.move(newPos, maze, dictionary);
            }
        }
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

    // Trouver tous les sous-mots valides dans une séquence de lettres
    std::vector<std::string> findValidWords(const std::string& letters) {
        std::vector<std::string> validWords;
        int n = letters.length();

        // Parcourir toutes les sous-chaînes possibles
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j <= n; ++j) {
                std::string substring = letters.substr(i, j - i);
                if (dictionary.find(substring) != dictionary.end()) {
                    validWords.push_back(substring);
                }
            }
        }

        return validWords;
    }
    void update() {
        if (!gameWon && !gameLost && !showResults) {
            // Vérifier si le joueur a atteint l'arrivée
            if (player.position->isExit) {
                gameWon = true;
                showResults = true;
                // Calculer le score final
                calculateFinalScore();

                // Passer les résultats à ResultsScreen
                resultsScreen.setResults(
                    player.collectedLetters,
                    player.validWords,
                    score,
                    static_cast<int>(gameClock.getElapsedTime().asSeconds()),
                    true
                );
            }

            // Vérifier si le temps est écoulé
            if (timeLimit > 0 && limitClock.getElapsedTime().asSeconds() >= timeLimit) {
                gameLost = true;
                showResults = true;

                // Passer les résultats à ResultsScreen
                resultsScreen.setResults(
                    player.collectedLetters,
                    player.validWords,
                    score,
                    static_cast<int>(gameClock.getElapsedTime().asSeconds()),
                    false
                );
            }

            // Vérifier si un mot a été formé
            if (player.collectedLetters.size() >= 2) {
                player.checkForValidWords(dictionary);

                // Afficher un message si aucun mot valide n'a été trouvé
                if (player.validWords.empty()) {
                    std::cout << "Aucun mot valide trouvé." << std::endl;
                }
            }
            // Vérifier si le joueur traverse une case spéciale
            if (player.position->isSpecial) {
                score += player.position->specialBonus;
                std::cout << "Case spéciale traversée ! +" << player.position->specialBonus << " points. Score actuel : " << score << std::endl;
                player.position->isSpecial = false; // Désactiver la case spéciale après utilisation
            }


        }
    }
    void calculateFinalScore() {
        // Ajouter le bonus de difficulté
        switch (difficulty) {
        case 1:
            score += 0; // Facile : +0 points
            break;
        case 2:
            score += 50; // Moyen : +50 points
            break;
        case 3:
            score += 100; // Difficile : +100 points
            break;
        default:
            break;
        }

        // Ajouter des points pour les lettres collectées dans ou hors du chemin le plus court
        for (Node* node : player.pathTaken) {
            if (std::find(shortestPath.begin(), shortestPath.end(), node) != shortestPath.end()) {
                score += 15; // Lettre collectée dans le chemin le plus court
            }
            else {
                score += 5; // Lettre collectée hors du chemin le plus court
            }
        }

        // Vérifier si le joueur a atteint la case d'arrivée dans le temps estimé
        if (gameClock.getElapsedTime().asSeconds() <= estimatedTime) {
            score += 50; // Ajouter 50 points si le joueur a terminé dans le temps estimé
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

                        // Colorer la case en rouge si elle fait partie d'un mot valide
                        if (node->isUsed) {
                            cellShape.setFillColor(sf::Color::Red); // Case en rouge
                        }
                        else if (node->playerVisited) {
                            cellShape.setFillColor(sf::Color(200, 200, 200)); // Case visitée
                        }
                        else {
                            cellShape.setFillColor(sf::Color::White); // Case normale
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

                        if (node->isExit) {
                            // Réduire la taille du rectangle (par exemple, 20x20 au lieu de 40x40)
                            sf::RectangleShape badgeShape(sf::Vector2f(32.f, 32.f));

                            // Ajuster la position pour centrer l'image si nécessaire
                            float offsetX = (40.f - 32.f) / 2.f; // Ajustement pour centrer horizontalement
                            float offsetY = (40.f - 32.f) / 2.f; // Ajustement pour centrer verticalement

                            badgeShape.setPosition(startX + j * 40.f + offsetX, startY + i * 40.f + offsetY);
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

                        if (node->isSpecial) {
                            // Définir la taille de la case spéciale (par exemple, 32x32 au lieu de 40x40)
                            float specialSize = 32.f;
                            sf::RectangleShape specialShape(sf::Vector2f(specialSize, specialSize));

                            // Calculer la position pour centrer la case spéciale dans la case du labyrinthe
                            float offset = (40.f - specialSize) / 2.f; // 40.f est la taille de la case du labyrinthe
                            specialShape.setPosition(startX + j * 40.f + offset, startY + i * 40.f + offset);

                            // Appliquer la texture
                            specialShape.setTexture(&specialTexture);

                            // Dessiner la case spéciale
                            window.draw(specialShape);
                        }
                    }
                }
            }

            // Dessiner le joueur
            player.draw(window, 40.f, playerTexture, startX, startY);

            // Afficher le temps écoulé
            int remainingTime = timeLimit - static_cast<int>(limitClock.getElapsedTime().asSeconds());
            if (remainingTime < 0) remainingTime = 0;
            timeText.setString("Temps restant: " + std::to_string(remainingTime) + "s");
            timeText.setPosition(startX, startY + maze.graph.rows * 40.f + 10);
            window.draw(timeText);

            // Afficher le score
            scoreText.setString("Score: " + std::to_string(score));
            scoreText.setPosition(startX, startY + maze.graph.rows * 40.f + 40);
            window.draw(scoreText);

            window.display();
        }
    }
};
int main() {
    srand(static_cast<unsigned int>(time(0)));

    sf::RenderWindow homeWindow(sf::VideoMode(800, 800), "Labyrinthe Scolaire - Accueil");
    HomeScreen homeScreen;

    while (homeWindow.isOpen()) {
        sf::Event event;
        while (homeWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                homeWindow.close();

            homeScreen.handleEvent(event);
        }

        homeScreen.draw(homeWindow);

        if (homeScreen.startGame) {
            homeWindow.close();

            // Lancer le jeu avec les paramètres choisis
            int rows = 0, cols = 0;
            switch (homeScreen.selectedDifficulty) {
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
                std::cerr << "Niveau de difficulte invalide. Utilisation de la taille par defaut." << std::endl;
                break;
            }

            // Passer le temps limite choisi au jeu
            Game game(rows, cols, homeScreen.selectedDifficulty, homeScreen.timeLimit);
            // Passer le nom d'utilisateur à ResultsScreen
            game.resultsScreen.setUsername(homeScreen.usernameInputString);
            // Passer les coordonnées de départ au labyrinthe
            game.maze.placeEntry(homeScreen.startPosition.x, homeScreen.startPosition.y);
            // Placer les cases bonus
            game.maze.placeBonusCells(5); // Par exemple, placer 5 cases bonus
            game.run();
        }
    }
    return 0;
}