
#ifndef _TINY4D_H
#  define _TINY4D_H

#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>

#  ifdef __cplusplus
extern "C" {
#  endif

  /*!\brief compose une couleur RGBA à partir des 4 octets de chaque composante. */
  static inline GLuint _rgba(GLubyte r, GLubyte g, GLubyte b, GLubyte a) {
    return (((GLuint)r) << R_SHIFT | ((GLuint)g) << G_SHIFT |  ((GLuint)b) << B_SHIFT | ((GLuint)a) << A_SHIFT );
  }
  
  /*!\brief compose une couleur RGBA à partir de 3 octets RGB + un alpha à 255. */
  static inline GLuint _rgb(GLubyte r, GLubyte g, GLubyte b) {
    return _rgba(r, g, b, 255);
  }
  
  /*!\brief extrait n'importe quelle composante à l'aide de son décalage. */
  static inline GLubyte _extract_comp(GLuint coul, int shift) {
    return (GLubyte)((((GLuint)(coul)) >> (shift)) & 0xFF);
  }
  
  /*!\brief extrait la composante rouge (0 à 255). */
  static inline GLubyte _red(GLuint coul) {
    return _extract_comp(coul, R_SHIFT);
  }
  
  /*!\brief extrait la composante verte (0 à 255). */
  static inline GLubyte _green(GLuint coul) {
    return _extract_comp(coul, G_SHIFT);
  }
  
  /*!\brief extrait la composante bleue (0 à 255). */
  static inline GLubyte _blue(GLuint coul) {
    return _extract_comp(coul, B_SHIFT);
  }
  
  /*!\brief extrait la composante alpha (0 à 255). */
  static inline GLubyte _alpha(GLuint coul) {
    return _extract_comp(coul, A_SHIFT);
  }

  /*!\brief teste si un pixel a une coordonnée valable dans un écran de dimensions (w,h) */
  static inline int _in_screen(int x, int y, int w, int h) {
    return x >= 0 && x < w && y >= 0 && y < h;
  }

  /*!\brief récupère la couleur du pixel (x,y) dans le screen en cours.
   *
   * Non optimale car récupère le pointeur de pixels et la largeur de
   * l'image à chaque appel. Attention, elle ne teste pas si le pixel
   * a une coordonnée valable.
   */
  static inline GLuint _get_pixel(int x, int y) {
    GLuint * pixels = gl4dpGetPixels();
    int w = gl4dpGetWidth();
    return pixels[y * w + x];
  }

  /*!\brief écrit la couleur du pixel (x,y) dans le screen en cours.
   *
   * Non optimale car récupère le pointeur de pixels et la largeur de
   * l'image à chaque appel. Attention, elle ne teste pas si le pixel
   * a une coordonnée valable.
   */
  static inline void _put_pixel(int x, int y, GLuint color) {
    GLuint * pixels = gl4dpGetPixels();
    int w = gl4dpGetWidth();
    pixels[y * w + x] = color;
  }

  /*!\brief calcule la luminance d'une couleur (c'est à dire la
   * grandeur correspondant à la sensation visuelle de luminosité).
   *
   * Les valeurs utilisées sont tirées du Wiki de "luminance". Ainsi,
   * selon la recommandation UIT-R BT 601, elle est :
   *
   * (0,299 x R) + (0,587 x V) + (0,114 x B)
   *
   * \param color la couleur RGBA en entier non signé 32 bits
   *
   * \return la luminance en flottant compris entre 0.0f (aucune) et
   * 1.0f (maximale).
   */
  static inline float _luminance(GLuint color) {
    float intensite_rouge =   _red(color) / 255.0f;
    float intensite_vert  = _green(color) / 255.0f;
    float intensite_bleu  =  _blue(color) / 255.0f;
    return 0.299f * intensite_rouge + 0.587f * intensite_vert + 0.114f * intensite_bleu;
  }
  
#  ifdef __cplusplus
}
#  endif

#endif
