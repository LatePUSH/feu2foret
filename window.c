

#include "tiny4D.h"
#include <GL4D/gl4dm.h>
#include <SDL.h>        // Use SDL for loading BMP image
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_WINDOW_WIDTH 1200
#define MAX_WINDOW_HEIGHT 1200

static void ciao(void);
static void dessine(void);
static void cellular_automaton_step(void);
static void init_forest_from_image(SDL_Surface *src, double density);
static void light_the_fire(void);
static void handle_input(int keycode); 

static int paused = 0; 

enum ca_states_t {
    EMPTY = RGB(219, 228, 255),  
    TREE = RGB(166, 165, 175), 
    FIRE = RGB(235, 38, 31), 
    CALCINED = RGB(83, 83, 84) 
};


int main(int argc, char ** argv) {
  if (argc != 2) {
      fprintf(stderr, "usage : %s <path_to_a_file.bmp>\n", argv[0]);
      return 2;
  }

  double density = 0.50;  // density

  // load bmp
  SDL_Surface *image = SDL_LoadBMP(argv[1]);
  if (!image) {
      fprintf(stderr, "Erreur : impossible de charger l'image %s\n", argv[1]);
      return 1;
  }

  // convert bmp to 32 bits rgba  
  SDL_Surface *formatted_image = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGBA32, 0);
  if (!formatted_image) {
      fprintf(stderr, "Erreur : impossible de convertir l'image %s\n", argv[1]);
      SDL_FreeSurface(image);
      return 1;
  }
  SDL_FreeSurface(image); 
  image = formatted_image; 

  
  int window_width = image->w;
  int window_height = image->h;
  float ratio_w = (float)MAX_WINDOW_WIDTH / (float)window_width;
  float ratio_h = (float)MAX_WINDOW_HEIGHT / (float)window_height;
  float ratio = (ratio_w < ratio_h) ? ratio_w : ratio_h;

  if (window_width > MAX_WINDOW_WIDTH || window_height > MAX_WINDOW_HEIGHT) {
      window_width = (int)(window_width * ratio);
      window_height = (int)(window_height * ratio);
  }


  SDL_Surface *resized_image = SDL_CreateRGBSurface(0, window_width, window_height, 32, 
      image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask);
  
  if (!resized_image) {
    fprintf(stderr, "Erreur : impossible de redimensionner l'image\n");
    SDL_FreeSurface(image);
    return 1;
  }


  SDL_BlitScaled(image, NULL, resized_image, NULL);
  SDL_FreeSurface(image); 
  image = resized_image; 


  if (!gl4duwCreateWindow(argc, argv, "feu2forêt", 0, 0, window_width, window_height, GL4DW_SHOWN)) {
    SDL_FreeSurface(image);
    return 1;
  }
  

  SDL_GL_SetSwapInterval(1);
  

  gl4dpInitScreenWithDimensions(window_width, window_height);


  init_forest_from_image(image, density);
  SDL_FreeSurface(image); 
  

  light_the_fire();
  

  gl4duwIdleFunc(cellular_automaton_step);


  atexit(ciao);
  gl4duwDisplayFunc(dessine);
  gl4duwKeyUpFunc(handle_input);  
  gl4duwMainLoop();
  return 0;
}


static void init_forest_from_image(SDL_Surface *src, double density) {
    int width = gl4dpGetWidth();
    int height = gl4dpGetHeight();
    GLuint * forest = gl4dpGetPixels();
    GLuint * src_pixels = (GLuint *)src->pixels;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index_src = (height - 1 - y) * src->w + x; // Inversion verticale pour correspondre au système de coordonnées
            int index_dst = y * width + x;
            GLuint color = src_pixels[index_src];
            float lum = _luminance(color); // Calculer la luminance de la couleur du pixel

            /* Probabilité que ce pixel soit un arbre en fonction de la luminance et de la densité */
            if (lum > 0.5f && gl4dmURand() < density) {
                forest[index_dst] = TREE;  // Si la luminance est élevée et la densité le permet, c'est un arbre
            } else {
                forest[index_dst] = EMPTY; // Sinon, c'est une cellule vide
            }
        }
    }

    gl4dpScreenHasChanged(); /* Indique que l'écran doit être mis à jour */
}

/*!\brief allume le feu sur certains arbres, ici en haut et en bas de la forêt. */
static void light_the_fire(void) {
    GLuint i;
    GLuint w = gl4dpGetWidth();
    GLuint h = gl4dpGetHeight();
    GLuint * forest = gl4dpGetPixels();

    // Allumer le feu sur la première ligne (en haut)
    for (i = 0; i < w; ++i) {
        if (forest[i] == TREE) {
            forest[i] = FIRE;
        }
    }

    // Allumer le feu sur la dernière ligne (en bas)
    for (i = w * (h - 1); i < w * h; ++i) {
        if (forest[i] == TREE) {
            forest[i] = FIRE;
        }
    }
}

/*!\brief simulation d'une étape de l'automate cellulaire avec 8-connexité. */
static void cellular_automaton_step(void) {
    if (paused) {
        return;  // Si la simulation est en pause, on ne fait rien
    }

    GLuint i;
    GLuint w = gl4dpGetWidth();
    GLuint h = gl4dpGetHeight();
    GLuint wh = w * h;
    GLuint * forest = gl4dpGetPixels();
    GLuint * forest_copy = malloc(wh * sizeof *forest_copy);
    assert(forest_copy);
    memcpy(forest_copy, forest, wh * sizeof *forest_copy);

    for (i = 0; i < wh; ++i) {
        switch (forest[i]) {
        case EMPTY:
        case CALCINED:
            break;
        case FIRE:
            forest[i] = CALCINED;
            break;
        case TREE: {
            int x = i % w, y = i / w;
            int nx, ny, j;
            // Les 8 directions de propagation du feu (4-connexité + diagonales)
            const int d[][2] = {
                { 1, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 }, // 4-connexité
                { 1, 1 }, { 1, -1 }, { -1, 1 }, { -1, -1 } // Diagonales (pour la 8-connexité)
            };
            for (j = 0; j < 8; ++j) {
                nx = x + d[j][0];
                ny = y + d[j][1];
                if (_in_screen(nx, ny, w, h) && forest_copy[ny * w + nx] == FIRE) {
                    forest[i] = FIRE;
                    break;
                }
            }
        }
            break;
        }
    }

    free(forest_copy);
    gl4dpScreenHasChanged();
}

/*!\brief appelée au moment de sortir du programme (atexit), elle libère les éléments utilisés par GL4Dummies. */
void ciao(void) {
  gl4duClean(GL4DU_ALL);
}

/*!\brief fonction appelée à chaque display par la gl4duwMainLoop. */
void dessine(void) {
  gl4dpUpdateScreen(NULL);
}

/*!\brief fonction pour gérer les événements de touches */
static void handle_input(int keycode) {
    if (keycode == SDLK_SPACE) {
        paused = !paused;  // Inverse l'état de pause
    }
}
