#define SC_IN		(dev_res)
#define SC_PT		(SC_IN / 72)

#define FNLEN		32
#define FNGLYPHS	512
#define FNNAME		32
#define LLEN		128

/* number registers */
extern int nreg[];
int num_get(int id);
int num_set(int id, int n);

/* builtin number registers; n_X for .X register */
#define N_ID(c1, c2)	((c1) * 256 + (c2))
#define n_f		nreg[N_ID('.', 'f')]
#define n_s		nreg[N_ID('.', 's')]
#define n_o		nreg[N_ID('.', 'o')]
#define n_p		nreg[N_ID('.', 'p')]
#define n_l		nreg[N_ID('.', 'l')]
#define n_v		nreg[N_ID('.', 'v')]
#define n_i		nreg[N_ID('.', 'i')]

/* device related variables */
extern int dev_res;
extern int dev_uwid;
extern int dev_hor;
extern int dev_ver;

struct glyph {
	char name[FNNAME];	/* name of the glyph */
	char id[FNNAME];	/* device-dependent glyph identifier */
	struct font *font;	/* glyph font */
	int wid;		/* character width */
	int type;		/* character type; ascender/descender */
};

struct font {
	char name[FNLEN];
	struct glyph glyphs[FNGLYPHS];
	int nglyphs;
	int spacewid;
	int special;
	char c[FNGLYPHS][FNNAME];	/* character names in charset */
	struct glyph *g[FNGLYPHS];	/* character glyphs in charset */
	int n;				/* number of characters in charset */
};

/* output device functions */
int dev_open(char *path);
void dev_close(void);
int dev_mnt(int pos, char *id, char *name);
int dev_font(char *id);

/* font-related functions */
struct font *font_open(char *path);
void font_close(struct font *fn);
struct glyph *font_glyph(struct font *fn, char *id);
struct glyph *font_find(struct font *fn, char *name);

/* glyph handling functions */
struct glyph *dev_glyph(char *c);
struct glyph *dev_glyph_byid(char *id);
int dev_spacewid(void);

/* different layers of neatroff */
int in_next(void);
int cp_next(void);
int tr_next(void);
void cp_back(int c);

/* rendering */
void render(void);

/* error messages */
void errmsg(char *msg);

/* troff commands */
void tr_br(int argc, char **args);
void tr_sp(int argc, char **args);
void tr_nr(int argc, char **args);
