/*
 *  Copyright 2018 Mike Reed
 */

#include "GPath.h"
#include "GMatrix.h"

GPath::GPath() {}
GPath::~GPath() {}

GPath& GPath::operator=(const GPath& src) {
    if (this != &src) {
        fPts = src.fPts;
        fVbs = src.fVbs;
    }
    return *this;
}

GPath& GPath::reset() {
    fPts.clear();
    fVbs.clear();
    return *this;
}

void GPath::dump() const {
    Iter iter(*this);
    GPoint pts[GPath::kMaxNextPoints];
    for (;;) {
        switch (iter.next(pts)) {
            case kMove:
                printf("M %g %g\n", pts[0].fX, pts[0].fY);
                break;
            case kLine:
                printf("L %g %g\n", pts[1].fX, pts[1].fY);
                break;
            case kQuad:
                printf("Q %g %g  %g %g\n", pts[1].fX, pts[1].fY, pts[2].fX, pts[2].fY);
                break;
            case kCubic:
                printf("C %g %g  %g %g  %g %g\n",
                       pts[1].fX, pts[1].fY,
                       pts[2].fX, pts[2].fY,
                       pts[3].fX, pts[3].fY);
                break;
            case kDone: return;
        }
    }
}

GPath& GPath::quadTo(GPoint p1, GPoint p2) {
    assert(fVbs.size() > 0);
    fPts.push_back(p1);
    fPts.push_back(p2);
    fVbs.push_back(kQuad);
    return *this;
}

GPath& GPath::cubicTo(GPoint p1, GPoint p2, GPoint p3) {
    assert(fVbs.size() > 0);
    fPts.push_back(p1);
    fPts.push_back(p2);
    fPts.push_back(p3);
    fVbs.push_back(kCubic);
    return *this;
}

/////////////////////////////////////////////////////////////////

GPath::Iter::Iter(const GPath& path) {
    fPrevMove = nullptr;
    fCurrPt = path.fPts.data();
    fCurrVb = path.fVbs.data();
    fStopVb = fCurrVb + path.fVbs.size();
}

GPath::Verb GPath::Iter::next(GPoint pts[]) {
    assert(fCurrVb <= fStopVb);
    if (fCurrVb == fStopVb) {
        return kDone;
    }
    Verb v = *fCurrVb++;
    switch (v) {
        case kMove:
            fPrevMove = fCurrPt;
            pts[0] = *fCurrPt++;
            break;
        case kLine:
            pts[0] = fCurrPt[-1];
            pts[1] = *fCurrPt++;
            break;
        case kQuad:
            pts[0] = fCurrPt[-1];
            pts[1] = *fCurrPt++;
            pts[2] = *fCurrPt++;
            break;
        case kCubic:
            pts[0] = fCurrPt[-1];
            pts[1] = *fCurrPt++;
            pts[2] = *fCurrPt++;
            pts[3] = *fCurrPt++;
            break;
        case kDone:
            assert(false); // not reached
    }
    return v;
}

GPath::Edger::Edger(const GPath& path) {
    fPrevMove = nullptr;
    fCurrPt = path.fPts.data();
    fCurrVb = path.fVbs.data();
    fStopVb = fCurrVb + path.fVbs.size();
    fPrevVerb = kDone;
}

GPath::Verb GPath::Edger::next(GPoint pts[]) {
    assert(fCurrVb <= fStopVb);
    bool do_return = false;
    while (fCurrVb < fStopVb) {
        switch (*fCurrVb++) {
            case kMove:
                if (fPrevVerb == kLine) {
                    pts[0] = fCurrPt[-1];
                    pts[1] = *fPrevMove;
                    do_return = true;
                }
                fPrevMove = fCurrPt++;
                fPrevVerb = kMove;
                break;
            case kLine:
                pts[0] = fCurrPt[-1];
                pts[1] = *fCurrPt++;
                fPrevVerb = kLine;
                return kLine;
            case kQuad:
                pts[0] = fCurrPt[-1];
                pts[1] = *fCurrPt++;
                pts[2] = *fCurrPt++;
                fPrevVerb = kQuad;
                return kQuad;
            case kCubic:
                pts[0] = fCurrPt[-1];
                pts[1] = *fCurrPt++;
                pts[2] = *fCurrPt++;
                pts[3] = *fCurrPt++;
                fPrevVerb = kCubic;
                return kCubic;
            default:
                assert(false); // not reached
        }
        if (do_return) {
            return kLine;
        }
    }
    if (fPrevVerb >= kLine && fPrevVerb <= kCubic) {
        pts[0] = fCurrPt[-1];
        pts[1] = *fPrevMove;
        fPrevVerb = kDone;
        return kLine;
    } else {
        return kDone;
    }
}
