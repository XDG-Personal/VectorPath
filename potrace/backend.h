#pragma once

#include <stdio.h>
#include "potracelib.h"
#include "main.h"

#ifndef BACKEND_DXF_H
#define BACKEND_DXF_H
int page_dxf(FILE *fout, potrace_path_t *plist, imginfo_t *imginfo);
#endif /* BACKEND_DXF_H */

#ifndef BACKEND_EPS_H
#define BACKEND_EPS_H
int init_ps(FILE *fout);
int page_ps(FILE *fout, potrace_path_t *plist, imginfo_t *imginfo);
int term_ps(FILE *fout);

int page_eps(FILE *fout, potrace_path_t *plist, imginfo_t *imginfo);

#endif /* BACKEND_EPS_H */

#ifndef BACKEND_GEO_H
#define BACKEND_GEO_H
int page_geojson(FILE *fout, potrace_path_t *plist, imginfo_t *imginfo);

#endif /* BACKEND_GEO_H */

#ifndef BACKEND_PDF_H
#define BACKEND_PDF_H
int init_pdf(FILE *fout);
int page_pdf(FILE *fout, potrace_path_t *plist, imginfo_t *imginfo);
int term_pdf(FILE *fout);

int page_pdf(FILE *fout, potrace_path_t *plist, imginfo_t *imginfo);
int page_pdfpage(FILE *fout, potrace_path_t *plist, imginfo_t *imginfo);

#endif /* BACKEND_PDF_H */

#ifndef BACKEND_PGM_H
#define BACKEND_PGM_H

int page_pgm(FILE *fout, potrace_path_t *plist, imginfo_t *imginfo);

#endif /* BACKEND_PGM_H */

#ifndef BACKEND_SVG_H
#define BACKEND_SVG_H

int page_svg(FILE *fout, potrace_path_t *plist, imginfo_t *imginfo);
int page_gimp(FILE *fout, potrace_path_t *plist, imginfo_t *imginfo);

#endif /* BACKEND_SVG_H */

#ifndef BACKEND_XFIG_H
#define BACKEND_XFIG_H

int page_xfig(FILE *fout, potrace_path_t *plist, imginfo_t *imginfo);

#endif /* BACKEND_XFIG_H */