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

// Classe pour représenter le joueur (élève)
class Player {
public:
    Node* position;
    std::vector<char> collectedLetters; // Liste des lettres collectées
    std::vector<std::string> validWords;
    std::vector<Node*> pathTaken;
    sf::Clock moveClock;

    Player() : position(nullptr) {}

    void move(Node* newPos, Maze& maze, const std::unordered_set<std::string>& dictionary) {
        if (newPos && !newPos->isUsed && !newPos->isBlocked) {
            position->playerVisited = true;
            position = newPos;
            pathTaken.push_back(newPos);

            if (newPos->isSpecial) {
                applySpecialEffect(newPos);
            }
            else {
                collectedLetters.push_back(newPos->letter); // Ajouter la lettre à la liste
                checkForValidWords(dictionary);
            }
        }
    }

    std::string getCollectedLetters() const {
        return std::string(collectedLetters.begin(), collectedLetters.end());
    }

    void checkForValidWords(const std::unordered_set<std::string>& dictionary) {
        std::string currentLetters(collectedLetters.begin(), collectedLetters.end());
        if (currentLetters.length() >= 2) {
            for (size_t i = 0; i < currentLetters.length(); ++i) {
                for (size_t j = i + 2; j <= currentLetters.length(); ++j) {
                    std::string substring = currentLetters.substr(i, j - i);
                    if (dictionary.find(substring) != dictionary.end()) {
                        if (std::find(validWords.begin(), validWords.end(), substring) == validWords.end()) {
                            validWords.push_back(substring);
                            std::cout << "Mot valide trouvé : " << substring << std::endl;
                            for (size_t k = i; k < j; ++k) {
                                if (k < pathTaken.size()) {
                                    pathTaken[k]->isUsed = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void applySpecialEffect(Node* node) {
        std::cout << "Case spéciale ! Bonus de 5 points !" << std::endl;
    }

    const std::vector<std::string>& getValidWords() const {
        return validWords;
    }

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

        usernameText.setFont(font);
        usernameText.setCharacterSize(24);
        usernameText.setFillColor(sf::Color::Black);
        usernameText.setPosition(50, 50);

        // Charger l'arrière-plan (si nécessaire)
        if (!backgroundTexture.loadFromFile("victory_background.png")) {
            std::cerr << "Erreur: Impossible de charger la texture de l'arrière-plan (victory_background.png)" << std::endl;
        }
        backgroundSprite.setTexture(backgroundTexture);
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
        }
        else {
            victoryText.setString("Defaite !");
            victoryText.setFillColor(sf::Color::Red);
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
        usernameText.setPosition(100, 60);
        window.draw(usernameText);

        victoryText.setFillColor(sf::Color::Black);
        victoryText.setPosition(100, 60);
        window.draw(victoryText);

        collectedLettersText.setFillColor(sf::Color::Black);
        collectedLettersText.setPosition(100, 120);
        window.draw(collectedLettersText);

        validWordsText.setFillColor(sf::Color::Black);
        validWordsText.setPosition(100, 180);
        window.draw(validWordsText);

        scoreText.setFillColor(sf::Color::Black);
        scoreText.setPosition(100, 240);
        window.draw(scoreText);

        timeText.setFillColor(sf::Color::Black);
        timeText.setPosition(100, 300);
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
    sf::Text collectedLettersText;
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
    int timeLimit;
    sf::Clock limitClock;
    int estimatedTime;

    Game(int rows, int cols, int diff, int timeLimit = 0, const std::string& username = "", int startX = 0, int startY = 0)
        : maze(rows, cols), score(0), gameWon(false), gameLost(false), difficulty(diff), showResults(false), timeLimit(timeLimit) {
        player.position = maze.graph.getNode(startX, startY);
        maze.placeEntry(startX, startY);
        resultsScreen.setUsername(username);
        shortestPath = maze.findShortestPath(player.position, maze.graph.getNode(maze.exit.x, maze.exit.y));
        limitClock.restart();

        switch (difficulty) {
        case 1:
            attempts = 2;
            estimatedTime = 10;
            break;
        case 2:
            attempts = 3;
            estimatedTime = 20;
            break;
        case 3:
            attempts = 5;
            estimatedTime = 30;
            break;
        default:
            attempts = 3;
            estimatedTime = 20;
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

        collectedLettersText.setFont(font);
        collectedLettersText.setCharacterSize(24);
        collectedLettersText.setFillColor(sf::Color::Black);
        collectedLettersText.setPosition(10, 10);

        timeText.setFont(font);
        timeText.setCharacterSize(24);
        timeText.setFillColor(sf::Color::Black);
        timeText.setPosition(10, 10);

        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::Black);
        scoreText.setPosition(10, 40);

        loadDictionary("dictionary.txt");

        player.position = maze.graph.getNode(maze.entry.x, maze.entry.y);
        shortestPath = maze.findShortestPath(player.position, maze.graph.getNode(maze.exit.x, maze.exit.y));
        limitClock.restart();
    }

    void run() {
        sf::RenderWindow window(sf::VideoMode(1000, 100 + maze.graph.rows * 40), "Labyrinthe Scolaire");

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

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
                    else if (event.key.code == sf::Keyboard::R) {
                        resetLevel();
                    }
                }
            }

            update();
            render(window);
        }
    }

    void resetLevel() {
        maze = Maze(maze.graph.rows, maze.graph.cols);
        player.position = maze.graph.getNode(maze.entry.x, maze.entry.y);
        player.collectedLetters.clear();
        player.pathTaken.clear();
        player.validWords.clear();

        for (int i = 0; i < maze.graph.rows; ++i) {
            for (int j = 0; j < maze.graph.cols; ++j) {
                maze.graph.grid[i][j].isUsed = false;
                maze.graph.grid[i][j].playerVisited = false;
            }
        }

        shortestPath = maze.findShortestPath(player.position, maze.graph.getNode(maze.exit.x, maze.exit.y));
        gameClock.restart();
        gameWon = false;
        gameLost = false;

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

    void loadDictionary(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Erreur: Impossible d'ouvrir le fichier " << filename << std::endl;
            return;
        }
        std::string word;
        while (std::getline(file, word)) {
            std::transform(word.begin(), word.end(), word.begin(), ::toupper);
            dictionary.insert(word);
        }
    }

    void update() {
        if (!gameWon && !gameLost && !showResults) {
            if (player.position->isExit) {
                gameWon = true;
                showResults = true;
                calculateFinalScore();

                std::cout << "L'ensemble des lettres collectees : " << player.getCollectedLetters() << std::endl;

                resultsScreen.setResults(
                    player.getCollectedLetters(),
                    player.validWords,
                    score,
                    static_cast<int>(gameClock.getElapsedTime().asSeconds()),
                    true
                );
            }

            if (timeLimit > 0 && limitClock.getElapsedTime().asSeconds() >= timeLimit) {
                gameLost = true;
                showResults = true;

                std::cout << "L'ensemble des lettres collectees : " << player.getCollectedLetters() << std::endl;

                resultsScreen.setResults(
                    player.getCollectedLetters(),
                    player.validWords,
                    score,
                    static_cast<int>(gameClock.getElapsedTime().asSeconds()),
                    false
                );
            }

            if (player.collectedLetters.size() >= 2) {
                std::vector<std::string> newValidWords = findValidWords(player.getCollectedLetters());

                for (const std::string& word : newValidWords) {
                    if (std::find(player.validWords.begin(), player.validWords.end(), word) == player.validWords.end()) {
                        player.validWords.push_back(word);
                        score += 50;
                        std::cout << "Mot valide trouvé : " << word << std::endl;
                        std::cout << "Nouveau score : " << score << std::endl;
                    }
                }

                player.collectedLetters.clear();
                player.pathTaken.clear();
            }

            if (player.position->isSpecial) {
                score += 5;
                std::cout << "Case spéciale traversée ! +5 points. Score actuel : " << score << std::endl;
                player.position->isSpecial = false;
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

            // Afficher uniquement le temps pendant le jeu
            timeText.setString("Temps : " + std::to_string(static_cast<int>(gameClock.getElapsedTime().asSeconds())) + "s");
            timeText.setPosition(10, 10);
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

            game.run();
        }
    }

    return 0;
}