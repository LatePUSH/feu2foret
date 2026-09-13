

#include "tiny4D.h"
#include <GL4D/gl4dm.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static void ciao(void);
static void dessine(void);
static void cellular_automaton_step(void);
static void init_forest(double density);
static void light_the_fire(void);

enum ca_states_t {
    EMPTY = RGB(96, 46, 8), //marron 
    TREE = RGB(41, 175, 29), //vert 
    FIRE = RGB(235, 38, 31), // rouge
    CALCINED = RGB(94, 94, 94) // gris
};

/*!\brief ouverture de fenêtre, callbacks puis mainloop */
int main(int argc, char ** argv) {
  const int w = 256, h = 192;
  if (!gl4duwCreateWindow(argc, argv, /* args du programme */
                          "feu2forêt", 0, 0, /* titre et (x, y) de la fenêtre */
                          w << 2, h << 2, /* largeur (4w), hauteur (4h) */
                          GL4DW_SHOWN)) { /* état visible */
    return 1; /* code d'erreur - echec d'ouverture de fenêtre */
  }
  
  /* on force la synchronisation verticale */
  SDL_GL_SetSwapInterval(1);
  
  /* on créé un screen aux dimensions w*h, 16 fois moins que la fenêtre */
  gl4dpInitScreenWithDimensions(w, h);
  
  /* on initialise la forêt avec 65% de densité d'arbres */
  init_forest(0.65);
  
  /* on allume le feu sur quelques arbres */
  light_the_fire();
  
  /* on initialise la simulation avec la fonction idle */
  gl4duwIdleFunc(cellular_automaton_step);

  /* le reste est comme d'habitude ... */
  atexit(ciao);
  gl4duwDisplayFunc(dessine);
  gl4duwMainLoop();
  return 0;
}

/*!\brief fonction pour initialiser la forêt avec une densité donnée. */
static void init_forest(double density) {
    GLuint i;
    GLuint wh = gl4dpGetWidth() * gl4dpGetHeight();
    GLuint * forest = gl4dpGetPixels();
    for (i = 0; i < wh; ++i)
        forest[i] = (gl4dmURand() < density) ? TREE : EMPTY;
}

/*!\brief allume le feu sur certains arbres, ici sur le bord droit. */
static void light_the_fire(void) {
    GLuint i;
    GLuint w = gl4dpGetWidth();
    GLuint wh = w * gl4dpGetHeight();
    GLuint * forest = gl4dpGetPixels();
    for (i = w - 1; i < wh; i += w)
        forest[i] = (forest[i] == TREE) ? FIRE : forest[i];
}

/*!\brief simulation d'une étape de l'automate cellulaire. */
static void cellular_automaton_step(void) {
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
            const int d[][2] = { { 1, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 } };
            for (j = 0; j < 4; ++j) {
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

/*!\brief appelée au moment de sortir du programme (atexit), elle
 *  libère les éléments utilisés par GL4Dummies. */
void ciao(void) {
  gl4duClean(GL4DU_ALL);
}

/*!\brief fonction appelée à chaque display par la gl4duwMainLoop. */
void dessine(void) {
  gl4dpUpdateScreen(NULL);
}