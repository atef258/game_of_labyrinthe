#include "Maze.h"
#include <algorithm>
#include <cstdlib>

Maze::Maze(int r, int c) : rows(r), cols(c), grid(r, std::vector<Cell>(c)) {
    generateMaze();
    placeLetters();
    placeExit();
}

void Maze::generateMaze() {
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

void Maze::placeLetters() {
    std::string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            grid[i][j].letter = letters[rand() % letters.size()];
        }
    }
}

void Maze::placeExit() {
    exit = sf::Vector2i(rows - 1, cols - 1);
    grid[exit.x][exit.y].isExit = true;
}

std::vector<sf::Vector2i> Maze::getUnvisitedNeighbors(const sf::Vector2i& cell) {
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

void Maze::removeWall(const sf::Vector2i& a, const sf::Vector2i& b) {
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
}

void Maze::draw(sf::RenderWindow& window, float cellSize, const sf::Texture& wallTexture, const sf::Texture& badgeTexture) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            grid[i][j].draw(window, j * cellSize, i * cellSize, cellSize, wallTexture, badgeTexture);
        }
    }
}

bool Maze::isPathClear(int x1, int y1, int x2, int y2) {
    if (x1 == x2) {
        if (y1 < y2) return !grid[x1][y1].walls[3]; // Mur droit
        else return !grid[x1][y1].walls[2]; // Mur gauche
    }
    else if (y1 == y2) {
        if (x1 < x2) return !grid[x1][y1].walls[1]; // Mur bas
        else return !grid[x1][y1].walls[0]; // Mur haut
    }
    return false;
}

std::vector<sf::Vector2i> Maze::findShortestPath(const sf::Vector2i& start, const sf::Vector2i& end) {
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