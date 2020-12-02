/**
 *  Copyright 2015 Mike Reed
 */

#include "bench.h"
#include "GCanvas.h"
#include "GBitmap.h"
#include "GTime.h"
#include <memory>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>

static void setup_bitmap(GBitmap* bitmap, int w, int h) {
    size_t rb = w * sizeof(GPixel);
    bitmap->reset(w, h, rb, (GPixel*)calloc(h, rb), GBitmap::kNo_IsOpaque);
}

enum Mode {
    kNormal,
    kForever,
    kOnce,
};

static double handle_proc(GBenchmark* bench, const char path[], GBitmap* bitmap, Mode mode) {
    GISize size = bench->size();
    setup_bitmap(bitmap, size.fWidth, size.fHeight);

    auto canvas = GCreateCanvas(*bitmap);
    if (!canvas) {
        fprintf(stderr, "failed to create canvas for [%d %d] %s\n",
                size.fWidth, size.fHeight, bench->name());
        return 0;
    }

    int N = 100;
    bool forever = false;
    switch (mode) {
        case kNormal: break;
        case kForever: forever = true; break;
        case kOnce: N = 4; break;
    }

    GMSec now = GTime::GetMSec();
    for (int i = 0; i < N || forever; ++i) {
        bench->draw(canvas.get());
    }
    GMSec dur = GTime::GetMSec() - now;
    return dur * 1.0 / N;
}

static bool is_arg(const char arg[], const char name[]) {
    std::string str("--");
    str += name;
    if (!strcmp(arg, str.c_str())) {
        return true;
    }

    char shortVers[3];
    shortVers[0] = '-';
    shortVers[1] = name[0];
    shortVers[2] = 0;
    return !strcmp(arg, shortVers);
}

static std::vector<double> load_scores(const char filename[], int count) {
    std::vector<double> scores;
    FILE* f = fopen(filename, "r");
    for (int i = 0; i < count; ++i) {
        double value;
        int n = fscanf(f, "%lg", &value);
        if (n != 1) {
            printf("%p [%d] FAILED TO LOAD SCORES %d %s\n", f, i, n, filename);
            break;
        }
        scores.push_back(value);
    }
    fclose(f);
    return scores;
}

int main(int argc, char** argv) {
    Mode mode = kNormal;
    const char* match = nullptr;
    const char* scoreFile = nullptr;
    const char* outScores = nullptr;
    std::vector<double> inScores;
    bool chatty_mode = true;
    bool write_images = false;

    int count = -1;
    while (gBenchFactories[++count]);

    for (int i = 1; i < argc; ++i) {
        if (is_arg(argv[i], "once")) {
            mode = kOnce;
        } else if (is_arg(argv[i], "match") && i+1 < argc) {
            match = argv[++i];
        } else if (is_arg(argv[i], "forever")) {
            mode = kForever;
        } else if (is_arg(argv[i], "scoreFile") && i+1 < argc) {
            scoreFile = argv[++i];
        } else if (is_arg(argv[i], "outScores") && i+1 < argc) {
            outScores = argv[++i];
        } else if (is_arg(argv[i], "inScores") && i+1 < argc) {
            inScores = load_scores(argv[++i], count);
            if (inScores.size() != count) {
                return -1;
            }
        } else if (is_arg(argv[i], "quiet")) {
            chatty_mode = false;
        } else if (is_arg(argv[i], "writeImages")) {
            write_images = true;
        } else {
            printf("Unknown arg %s\n", argv[i]);
            return -1;
        }
    }

    if (scoreFile && inScores.size() == 0) {
        printf("Can't compute --scoreFile without --inScores\n");
        return -1;
    }

    std::vector<double> durs;
    double quotient = 0;
    for (int i = 0; i < count; ++i) {
        std::unique_ptr<GBenchmark> bench(gBenchFactories[i]());
        const char* name = bench->name();
        
        if (match && !strstr(name, match)) {
            continue;
        }

        GBitmap testBM;
        double dur = handle_proc(bench.get(), name, &testBM, mode);
        if (chatty_mode) {
            printf("bench: %s %g", name, dur);
        }
        if (inScores.size()) {
            if (chatty_mode) {
                printf(" %g [%.2f]", inScores[i], dur / inScores[i]);
            }
            quotient += dur / inScores[i];
        }
        if (chatty_mode) {
            printf("\n");
        }
        durs.push_back(dur);

        if (write_images) {
            std::string str(name);
            str += ".png";
            testBM.writeToFile(str.c_str());
        }
        free(testBM.pixels());
    }

    if (inScores.size()) {
        printf("score %.2f\n", quotient / count);
        if (scoreFile) {
            FILE* f = fopen(scoreFile, "w");
            if (f) {
                fprintf(f, "%g\n", quotient / count);
                fclose(f);
            } else {
                printf("FAILED TO OPEN %s\n", scoreFile);
                return -1;
            }
        }
    }

    if (durs.size() > 0 && outScores != nullptr) {
        FILE* f = fopen(outScores, "w");
        if (f) {
            for (size_t i = 0; i < durs.size(); ++i) {
                fprintf(f, " %g", durs[i]);
            }
            fprintf(f, "\n");
            fclose(f);
        } else {
            printf("FAILED TO WRITE TO %s\n", outScores);
            return -1;
        }
    }
    return 0;
}
