#include <raylib.h>
#include <stdlib.h>
#include <time.h>



//global values

#define grid_w 300
#define grid_h 150
#define bg_color (Color) {66, 66, 70, 255}
#define dead_color (Color) {100, 100, 105, 255}
#define alive_color (Color) {190, 191, 196, 255}



//types

typedef struct {
	int width;
	int height;
	int *tab; //values
} Board;

Board* init_board(int n, int m) {
	Board *b = malloc(sizeof(Board));
	b->width = n;
	b->height = m;
	b->tab = malloc(n*m * (sizeof(int)));
	return b;
}

void free_board(Board *b) {
	free(b->tab);
	free(b);
}



//functions

void window_init(Color bg) {
	InitWindow(0, 0, "Game of Life");
	ClearBackground(bg);
	SetTargetFPS(100);
}

int min(int x, int y) {
	if (x < y) x;
	else y;
}

void draw(Board *b, bool display_grid) {
	int w = GetScreenWidth();
	int h = GetScreenHeight();
	int px = min((w-200) / grid_w, (h-200) / grid_h);
	int start_x = h / 2 - px * grid_h / 2;
	int start_y = w / 2 - px * grid_w / 2;

	ClearBackground(bg_color);
	for (int i=0; i < grid_h - 1; i++) {
		for (int j=0; j < grid_w - 1; j++) {
			int cell_size = px;
			if (display_grid) cell_size--;
			if (b->tab[j*grid_h + i] == 1) 
				DrawRectangle(start_y + j * px, start_x + i * px, cell_size, cell_size, alive_color);
			else
				DrawRectangle(start_y + j * px, start_x + i * px, cell_size, cell_size, dead_color);
		}
	}
}

int neighbors(Board *b, int x, int y) {
	//count neighbors without index out of bounds
	int count = 0;
	if (x > 0) count += b->tab[y * grid_h + (x-1)];
	if (x < grid_h - 1) count += b->tab[y * grid_h + (x+1)];
	if (y > 0) count += b->tab[(y-1) * grid_h + x];
	if (y < grid_w - 1) count += b->tab[(y+1) * grid_h + x];
	if (x > 0 && y > 0) count += b->tab[(y-1) * grid_h + (x-1)];
	if (x > 0 && y < grid_w - 1) count += b->tab[(y+1) * grid_h + (x-1)];
	if (x < grid_h - 1 && y > 0) count += b->tab[(y-1) * grid_h + (x+1)];
	if (x < grid_h - 1 && y < grid_w - 1) count += b->tab[(y+1) * grid_h + (x+1)];
	return count;
}

void update(Board *b) {
	//new updated board
	Board *b2 = init_board(grid_w, grid_h);
	for (int i=0; i < grid_h - 1; i++) {
		for (int j=0; j < grid_w - 1; j++) {
			//apply Game of Life rules
			int new_value = 0;
			if (b->tab[j * grid_h + i] == 0) {
				if (neighbors(b, i, j) == 3) new_value = 1;
			}
			else {
				if (neighbors(b, i, j) == 2 || neighbors(b, i, j) == 3) new_value = 1;
			}
			b2->tab[j * grid_h + i] = new_value;
		}
	}
	//paste values in main board and free the temp board
	for (int i=0; i < grid_h - 1; i++) {
		for (int j=0; j < grid_w - 1; j++) {
			b->tab[j * grid_h + i] = b2->tab[j * grid_h + i];
		}
	}
	free_board(b2);
}

void main() {
	//initialization
	srand(time(NULL));
	window_init(bg_color);
	bool display_grid = false;
	bool running = true;

	//create randomized board
	Board *b = init_board(grid_w, grid_h);
	for (int i=0; i < grid_h - 1; i++) {
		for (int j=0; j < grid_w - 1; j++) {
			b->tab[j*grid_h + i] = rand() % 2;
		}
	}

	//loop
	while (!WindowShouldClose()) {
		//controls : pause - unpause
		if (IsKeyPressed(KEY_SPACE)) running = !running;

		//controls : switch grid
		if (IsKeyPressed(KEY_G)) {display_grid = !display_grid; draw(b, display_grid);}

		//controls : clear grid
		if (IsKeyPressed(KEY_C)) {
			for (int i=0; i < grid_h - 1; i++) {
				for (int j=0; j < grid_w - 1; j++) {
					b->tab[j*grid_h + i] = 0;
				}
			}
			draw(b, display_grid);
		}

		//controls : randomize grid
		if (IsKeyPressed(KEY_R)) {
			srand(time(NULL));
			for (int i=0; i < grid_h - 1; i++) {
				for (int j=0; j < grid_w - 1; j++) {
					b->tab[j*grid_h + i] = rand() % 2;
				}
			}
			draw(b, display_grid);
		}

		//controls : add cells
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			int w = GetScreenWidth();
			int h = GetScreenHeight();
			int px = min((w-200) / grid_w, (h-200) / grid_h);
			int start_x = h / 2 - px * grid_h / 2;
			int start_y = w / 2 - px * grid_w / 2;
			int mx = GetMouseX();
			int my = GetMouseY();
			if ((mx > start_y) && (my > start_x) && (mx < w - start_y) && (my < h - start_x)) {
				b->tab[grid_h * ((mx - start_y) / px) + ((my - start_x) / px)] = 1;
				draw(b, display_grid);
			}
		}

		//controls : delete cells
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			int w = GetScreenWidth();
			int h = GetScreenHeight();
			int px = min((w-200) / grid_w, (h-200) / grid_h);
			int start_x = h / 2 - px * grid_h / 2;
			int start_y = w / 2 - px * grid_w / 2;
			int mx = GetMouseX();
			int my = GetMouseY();
			if ((mx > start_y) && (my > start_x) && (mx < w - start_y) && (my < h - start_x)) {
				b->tab[grid_h * ((mx - start_y) / px) + ((my - start_x) / px)] = 0;
				draw(b, display_grid);
			}
		}

		//update and draw board
		if (running) {
			update(b);
			draw(b, display_grid);
		}

		BeginDrawing();
		EndDrawing();
	}

	//de-initialization
	free_board(b);
}
