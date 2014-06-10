// Spiro.h
#pragma once

#include <iostream>
#include "CGAL_Headers.h"

using namespace std;

typedef struct {
    double x;
    double y;
    char ty;
} spiro_cp;

struct BezierCP {
  Point_2 in;
  Point_2 control_point;
  Point_2 out;
};

typedef struct spiro_seg_s spiro_seg;

class SpiroBezierCtx {
public:
  std::vector<BezierCP> cps;
  void moveto(double x, double y, int is_open) {
    cps.push_back(BezierCP(Point_2(x, y), Point_2(x, y), Point_2(x, y)));
  }
  void lineto(double x, double y) {
    cps.push_back(BezierCP(Point_2(x, y), Point_2(x, y), Point_2(x, y)));
  }
  void quadto(double x1, double y1, double x2, double y2); // not used
  void curveto(double x1, double y1, double x2, double y2, double x3, double y3) {
    cps.push_back(BezierCP(Point_2(x1, y1), Point_2(x2, y2), Point_2(x3, y3)));
  }
  void mark_knot(int knot_idx) {
    cout << "marking knot?" << endl;
  }
};

typedef SpiroBezierCtx bezctx;

class Spiro {
private:
	static void integrate_spiro(const double ks[4], double xy[2]);
	static double compute_ends(const double ks[4], double ends[2][4], double seg_ch);
	void compute_pderivs(const spiro_seg *s, double ends[2][4], double derivs[4][2][4],
		int jinc);
	static double mod_2pi(double th);
	static spiro_seg * setup_path(const spiro_cp *src, int n);
	static void bandec11(bandmat *m, int *perm, int n);
	static void banbks11(const bandmat *m, const int *perm, double *v, int n);
	int compute_jinc(char ty0, char ty1);
	int count_vec(const spiro_seg *s, int nseg);
	static void add_mat_line(bandmat *m, double *v,
	     double derivs[4], double x, double y, int j, int jj, int jinc,
	     int nmat);
	static double spiro_iter(spiro_seg *s, bandmat *m, int *perm, double *v, int n);
	int solve_spiro(spiro_seg *s, int nseg);
  spiro_seg * run_spiro(const spiro_cp *src, int n);
public:
	static void spiro_seg_to_bpath(const double ks[4],
		  double x0, double y0, double x1, double y1,
		  bezctx *bc, int depth);
  void
  std::vector< BezierCP > TaggedSpiroCPsToBezier(std::vector<spiro_cp> spiros_v)
  {
      spiro_seg *s;
      int n;
      spiro_cp[] spiros = new spiro_cp[spiros_v.size()];
      for(int i = 0; i < spiros_v.size(); ++i) {
        spiros[i] = spiros_v[i];
      }
      for ( n=0; spiros[n].ty!='z' && spiros[n].ty!='}'; ++n );
      if ( spiros[n].ty == '}' ) ++n;

      if ( n<1 )
        return std::vector<BezierCP>;
    
      s = run_spiro(spiros,n);

      SpiroBezierCtx * bc = new SpiroBezierCtx();

      spiro_to_bpath(s, n, bc);
      // free_spiro(s);
  }
};
