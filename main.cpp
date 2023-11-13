#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

const string data_file = "data.txt";
const char DEAD_CELL = '-';
const char ALIVE_CELL = '*';
const char EMPTY_CELL = static_cast<char>(0);
string CELLS_DEAD_MSG = "All cells are dead. World is empty. Game over";
string STABLE_COND_MSG = "The game is in stable condition. Game over";
const int MAX_CELLS_COUNT = 500;
const int MAX_COUNT_OF_GENERATIONS = 100;

int get_number(ifstream &file_data) {
  int buffer_number = 0;
  file_data >> buffer_number;
  return buffer_number;
}

char **create_field(int rows, int cols) {
  char **field = new char *[rows];
  for (int i = 0; i < rows; i++) {
    field[i] = new char[cols];
  }
  return field;
}

void init_field(char **field, int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      field[i][j] = DEAD_CELL;
    }
  }
}

void fill_field(char **field, int x, int y) { field[x][y] = ALIVE_CELL; }

void print_field(char **field, int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      cout << field[i][j] << " ";
    }
    cout << endl;
  }
}

char get_current_cell(char **field, int current_row, int current_col, int rows,
                      int cols) {
  if (current_row < 0 || current_row >= rows || current_col < 0 ||
      current_col >= cols) {
    return EMPTY_CELL;
  }
  return field[current_row][current_col];
}

int get_neighbours_count(char **field, int current_row, int current_col,
                        int rows, int cols) {
  int neighbours_count = 0;
  get_current_cell(field, current_row - 1, current_col - 1, rows, cols) ==
          ALIVE_CELL &&
      ++neighbours_count;
  get_current_cell(field, current_row - 1, current_col, rows, cols) ==
          ALIVE_CELL &&
      ++neighbours_count;
  get_current_cell(field, current_row - 1, current_col + 1, rows, cols) ==
          ALIVE_CELL &&
      ++neighbours_count;

  get_current_cell(field, current_row + 1, current_col - 1, rows, cols) ==
          ALIVE_CELL &&
      ++neighbours_count;
  get_current_cell(field, current_row + 1, current_col, rows, cols) ==
          ALIVE_CELL &&
      ++neighbours_count;
  get_current_cell(field, current_row + 1, current_col + 1, rows, cols) ==
          ALIVE_CELL &&
      ++neighbours_count;

  get_current_cell(field, current_row, current_col - 1, rows, cols) ==
          ALIVE_CELL &&
      ++neighbours_count;
  get_current_cell(field, current_row, current_col + 1, rows, cols) ==
          ALIVE_CELL &&
      ++neighbours_count;
  return neighbours_count;
}

bool revive_cell(char cell, int live_neighbours) {
  return cell == DEAD_CELL && live_neighbours == 3;
}

bool kill_cell(char cell, int live_neighbours) {
  return cell == ALIVE_CELL && (live_neighbours < 2 || live_neighbours > 3);
}

char **update_field(char **field, int rows, int cols) {
  char** new_field = create_field(rows, cols);
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      char current_cell = get_current_cell(field, i, j, rows, cols);
      int neighbours_count = get_neighbours_count(field, i, j, rows, cols);
      if (revive_cell(current_cell, neighbours_count)) {
        new_field[i][j] = ALIVE_CELL;
      } else if (kill_cell(current_cell, neighbours_count)) {
        new_field[i][j] = DEAD_CELL;
      } else {
        new_field[i][j] = current_cell;
      }
    }
  }
  return new_field;
}

bool compare_fields(char **field1, char **field2, int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (field1[i][j] != field2[i][j]) {
        return true;
      }
    }
  }
  return false;
}

int count_alive_cells(char **field, int rows, int cols) {
  int alive_cells = 0;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (field[i][j] == ALIVE_CELL) {
        ++alive_cells;
      }
    }
  }
  return alive_cells;
}

void clear_memory(char **field, int rows) {
  for (int i = 0; i < rows; i++) {
    delete[] field[i];
  }
  delete[] field;
}

void game_status(int generation, int alive_cells) {
  cout << "Generation: " << generation << ". Alive cells: " << alive_cells
      << endl;
}

void game_core(char **field, int rows, int cols) {
  int generation = 1;
  char **new_field = 0;
  int alive_cells = count_alive_cells(field, rows, cols);
  print_field(field, rows, cols);
  game_status(generation, alive_cells);
  cout << endl;
  sleep(2);
  while (++generation < MAX_COUNT_OF_GENERATIONS) {
    new_field = update_field(field, rows, cols);
    const bool changes = compare_fields(field, new_field, rows, cols);
    alive_cells = count_alive_cells(new_field, rows, cols);
    print_field(new_field, rows, cols);
    game_status(generation, alive_cells);
    clear_memory(field, rows);
    field = nullptr;
    if (!changes || alive_cells == 0) {
      const string message = changes ? CELLS_DEAD_MSG : STABLE_COND_MSG;
      cout << message << std::endl;
      break;
    }
    field = new_field;
  }
  clear_memory(new_field, rows);
  new_field = nullptr;
}

int main(int argc, char **argv) {
  setlocale(LC_ALL, "RUSSIAN");
  system("clear");
  ifstream file_data(data_file);
  if (file_data.is_open()) {
    const int ROWS = get_number(file_data);
    const int COLS = get_number(file_data);
    char **field = create_field(ROWS, COLS);
    init_field(field, ROWS, COLS);
    int x = 0, y = 0;
    while (!file_data.eof()) {
      file_data >> x >> y;
      fill_field(field, x, y);
    }
    file_data.close();
    game_core(field, ROWS, COLS);
  } else {
    cout << "Ошибка! Файл" << data_file << "не открыт!" << endl;
    return 1;
  }
  return 0;
}