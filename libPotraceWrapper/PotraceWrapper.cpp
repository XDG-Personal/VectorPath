#include "PotraceWrapper.h"

#include "../potrace/potracelib.h"

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "../x64/Debug/libpotraced.lib")
#else
#pragma comment(lib, "../x64/Release/libpotrace.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "../Debug/libpotraced.lib")
#else
#pragma comment(lib, "../Release/libpotrace.lib")
#endif
#endif

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

_gdiplus_initializer PotraceWrapper::_initializer_;

// From potrace --------- Begin
#define BM_WORDSIZE ((int)sizeof(potrace_word))
#define BM_WORDBITS (8 * BM_WORDSIZE)
#define BM_HIBIT (((potrace_word)1) << (BM_WORDBITS - 1))
#define bm_scanline(bm, y) ((bm)->map + (y) * (bm)->dy)
#define bm_index(bm, x, y) (&bm_scanline(bm, y)[(x) / BM_WORDBITS])
#define bm_mask(x) (BM_HIBIT >> ((x) & (BM_WORDBITS - 1)))
#define bm_range(x, a) ((int)(x) >= 0 && (int)(x) < (a))
#define bm_safe(bm, x, y) (bm_range(x, (bm)->w) && bm_range(y, (bm)->h))
#define BM_USET(bm, x, y) (*bm_index(bm, x, y) |= bm_mask(x))
#define BM_UCLR(bm, x, y) (*bm_index(bm, x, y) &= ~bm_mask(x))
#define BM_UPUT(bm, x, y, b) ((b) ? BM_USET(bm, x, y) : BM_UCLR(bm, x, y))
#define BM_PUT(bm, x, y, b) (bm_safe(bm, x, y) ? BM_UPUT(bm, x, y, b) : 0)

/* calculate the size, in bytes, required for the data area of a
   bitmap of the given dy and h. Assume h >= 0. Return -1 if the size
   does not fit into the ptrdiff_t type. */
ptrdiff_t getsize(int dy, int h) {
  ptrdiff_t size;

  if (dy < 0) {
    dy = -dy;
  }

  size = (ptrdiff_t)dy * (ptrdiff_t)h * (ptrdiff_t)BM_WORDSIZE;

  /* check for overflow error */
  if (size < 0 || (h != 0 && dy != 0 && size / h / dy != BM_WORDSIZE)) {
    return -1;
  }

  return size;
}

/* return new bitmap initialized to 0. NULL with errno on error.
   Assumes w, h >= 0. */
potrace_bitmap_t *bm_new(int w, int h) {
  potrace_bitmap_t *bm;
  int dy = w == 0 ? 0 : (w - 1) / BM_WORDBITS + 1;
  ptrdiff_t size;

  size = getsize(dy, h);
  if (size < 0) {
    errno = ENOMEM;
    return NULL;
  }
  if (size == 0) {
    size = 1; /* make sure calloc() doesn't return NULL */
  }

  bm = (potrace_bitmap_t *)malloc(sizeof(potrace_bitmap_t));
  if (!bm) {
    return NULL;
  }
  bm->w = w;
  bm->h = h;
  bm->dy = dy;
  bm->map = (potrace_word *)calloc(1, size);
  if (!bm->map) {
    free(bm);
    return NULL;
  }
  return bm;
}

/* free a bitmap */
void bm_free(potrace_bitmap_t *bm) {
  if (bm != NULL) {
    free(bm->map);
  }
  free(bm);
}

// From potrace --------- End

void graphicsPath_draw(GraphicsPath *dest, potrace_path_t *path) {
  if (!path || !dest)
    return;
  int n = path->curve.n;
  if (!n)
    return;
  int *tag = path->curve.tag;
  potrace_dpoint_t(*c)[3] = path->curve.c;
  // 开始一段新的路径绘制
  dest->StartFigure();
  // 最后一段路径的终点为第一段路径的起点(potracelib.pdf)
  Gdiplus::PointF startPt(c[n - 1][2].x, c[n - 1][2].y);
  for (int i = 1; i < n; ++i) {
    // 该段路径的起点
    Gdiplus::PointF a;
    if (i == 1)
      a = startPt;
    else
      a = Gdiplus::PointF(c[i - 1][2].x, c[i - 1][2].y);
    switch (tag[i]) {
      // 折角
    case POTRACE_CORNER: {
      Gdiplus::PointF v(c[i][1].x, c[i][1].y);
      Gdiplus::PointF b(c[i][2].x, c[i][2].y);
      dest->AddLine(a, v);
      dest->AddLine(v, b);
    } break;
      // 贝塞尔曲线
    case POTRACE_CURVETO: {
      Gdiplus::PointF u(c[i][0].x, c[i][0].y);
      Gdiplus::PointF w(c[i][1].x, c[i][1].y);
      Gdiplus::PointF b(c[i][2].x, c[i][2].y);
      dest->AddBezier(a, u, w, b);
    } break;
    }
  }
  dest->CloseFigure();
}

enum HoleMode { Boundary, Hole, Both };

void potracePath_to_graphicsPath(GraphicsPath *dest, potrace_path_t *path,
                                 HoleMode mode = Both) {
  while (path) {
    if (path->sign == '+') {
      // boundary
      if (mode == Boundary || mode == Both)
        graphicsPath_draw(dest, path);
      // hole
      if (mode == Hole || mode == Both) {
        potrace_path_s *pathc = path->childlist;
        if (!pathc) {
          path = path->next;
          continue;
        }

        while (pathc) {
          if (pathc->sign == '-')
            graphicsPath_draw(dest, path);
          pathc = pathc->sibling;
        }
      }
    }
    path = path->next;
  }
}

potrace_bitmap_t *potrace_bm_from_mat(const cv::Mat &image, uchar threshold) {
  potrace_bitmap_t *bitmap = bm_new(image.cols, image.rows);
  for (int row = 0; row < image.rows; ++row) {
    const uchar *ptr = image.ptr<uchar>(row);
    for (int col = 0; col < image.cols; ++col) {
      if (ptr[col] > threshold)
        BM_PUT(bitmap, col, row, 0);
      else
        BM_PUT(bitmap, col, row, 1);
    }
  }
  return bitmap;
}

PotraceWrapper::PotraceWrapper() {
  m_alphaMax = 1.0;
  m_turdSize = 2;
  m_curveTolerance = 0.1;
  m_threshold = 128;
  m_opticurve = 0;
  m_turnpolicy = POTRACE_TURNPOLICY_MINORITY;

  m_pTracedPath = NULL;
}

PotraceWrapper::~PotraceWrapper() {}

bool PotraceWrapper::trace(const cv::Mat *image) {
  if (!image)
    return false;
  if (image->channels() == 3)
    return false;
  potrace_bitmap_t *pbitmap = potrace_bm_from_mat(*image, m_threshold);

  m_width = image->cols;
  m_height = image->rows;

  potrace_param_t *pparams = potrace_param_default();
  pparams->alphamax = m_alphaMax;
  pparams->opticurve = m_opticurve;
  pparams->turnpolicy = m_turnpolicy;
  pparams->opttolerance = m_curveTolerance;
  pparams->turdsize = m_turdSize;
  // 如果需要得知转换进度,请参阅potrace文档
  // pparams->progress.callback

  potrace_state_t *pstate = potrace_trace(pparams, pbitmap);
  bm_free(pbitmap);
  potrace_param_free(pparams);

  if (!pstate || pstate->status != POTRACE_STATUS_OK) {
    return false;
  }

  {
    if (m_pTracedPath)
      delete m_pTracedPath;
    m_pTracedPath = new GraphicsPath;
    potracePath_to_graphicsPath(m_pTracedPath, pstate->plist);
  }

  potrace_state_free(pstate);
  return true;
}