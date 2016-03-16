void programs_init (void);
void program_cube_use (void);
void program_bkgd_use (void);

enum LocBkgd {
	LOC_BKGD_VERTEX,
	LOC_BKGD_TEXTURE,
};

enum LocCube {
	LOC_CUBE_VIEW,
	LOC_CUBE_MODEL,
	LOC_CUBE_VERTEX,
	LOC_CUBE_VCOLOR,
	LOC_CUBE_NORMAL,
};

GLint program_bkgd_loc (const enum LocBkgd);
GLint program_cube_loc (const enum LocCube);
