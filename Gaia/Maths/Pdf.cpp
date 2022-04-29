#include "Pdf.h"

double PowerHeuristic(int nf, double fPdf, int ng, double gPdf) {
	double f = nf * fPdf, g = ng * gPdf;
	return (f * f) / (f * f + g * g);
}

double BalancedHeuristic(int nf, double fPdf, int ng, double gPdf) {
	double f = nf * fPdf, g = ng * gPdf;
	return (f) / (f + g);
}