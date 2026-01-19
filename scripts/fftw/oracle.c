#include <fftw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(const char *prog) {
  fprintf(stderr,
          "Usage: %s --size N [--inverse] [--input path] [--output path]\n"
          "Reads N complex numbers (re im) and writes FFT results.\n",
          prog);
}

int main(int argc, char **argv) {
  int n = 0;
  int inverse = 0;
  const char *input_path = NULL;
  const char *output_path = NULL;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--size") == 0 && i + 1 < argc) {
      n = atoi(argv[++i]);
    } else if (strcmp(argv[i], "--inverse") == 0) {
      inverse = 1;
    } else if (strcmp(argv[i], "--input") == 0 && i + 1 < argc) {
      input_path = argv[++i];
    } else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc) {
      output_path = argv[++i];
    } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
      usage(argv[0]);
      return 0;
    } else {
      usage(argv[0]);
      return 2;
    }
  }

  if (n <= 0) {
    usage(argv[0]);
    return 2;
  }

  FILE *in = input_path ? fopen(input_path, "r") : stdin;
  if (!in) {
    perror("fopen input");
    return 1;
  }
  FILE *out = output_path ? fopen(output_path, "w") : stdout;
  if (!out) {
    perror("fopen output");
    if (in != stdin) {
      fclose(in);
    }
    return 1;
  }

  fftw_complex *inbuf = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * n);
  fftw_complex *outbuf = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * n);
  if (!inbuf || !outbuf) {
    fprintf(stderr, "Allocation failed for size %d\n", n);
    if (in != stdin) {
      fclose(in);
    }
    if (out != stdout) {
      fclose(out);
    }
    if (inbuf) {
      fftw_free(inbuf);
    }
    if (outbuf) {
      fftw_free(outbuf);
    }
    return 1;
  }

  for (int i = 0; i < n; i++) {
    double re = 0.0;
    double im = 0.0;
    if (fscanf(in, "%lf %lf", &re, &im) != 2) {
      fprintf(stderr, "Failed to read input at index %d\n", i);
      if (in != stdin) {
        fclose(in);
      }
      if (out != stdout) {
        fclose(out);
      }
      fftw_free(inbuf);
      fftw_free(outbuf);
      return 1;
    }
    inbuf[i][0] = re;
    inbuf[i][1] = im;
  }

  int direction = inverse ? FFTW_BACKWARD : FFTW_FORWARD;
  fftw_plan plan = fftw_plan_dft_1d(n, inbuf, outbuf, direction, FFTW_ESTIMATE);
  if (!plan) {
    fprintf(stderr, "Failed to create FFTW plan\n");
    if (in != stdin) {
      fclose(in);
    }
    if (out != stdout) {
      fclose(out);
    }
    fftw_free(inbuf);
    fftw_free(outbuf);
    return 1;
  }

  fftw_execute(plan);

  for (int i = 0; i < n; i++) {
    fprintf(out, "%.17g %.17g\n", outbuf[i][0], outbuf[i][1]);
  }

  fftw_destroy_plan(plan);
  fftw_free(inbuf);
  fftw_free(outbuf);

  if (in != stdin) {
    fclose(in);
  }
  if (out != stdout) {
    fclose(out);
  }

  return 0;
}
