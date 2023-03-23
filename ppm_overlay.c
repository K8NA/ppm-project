
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

typedef struct pixel
{
    int r;
    int g;
    int b;
} pixel;

typedef struct metadados
{
    char magic_num[2];
    int lin;
    int col;
    int max;
} metadados;

FILE* skipcom(FILE* fp) {
  char c;
  c = getc(fp);

  if (c == ' ') {
    while (c == ' ')
      c = getc(fp);
  }

  if (c == '\n')
     c = getc(fp);

  while (c == '#') {
      while (getc(fp) != '\n');
      c = getc(fp);
  }

  if (c != '\n')
    ungetc(c,fp);

  return fp;
}

void skipcomconsole() {
  char c;
  c = getc(stdin);

  if (c == ' '){
    while (c == ' ')
      c = getc(stdin);
  }
  if (c == '\n')
     c = getc(stdin);

  while (c == '#') {
      while (getc(stdin) != '\n');
      c = getc(stdin);
  }

  if (c != '\n')
    ungetc(c,stdin);
}


pixel **read(char *file, metadados *img) {
  int i, j;
  int sav[2];
  char buff[16];
  pixel **data;

  if(strcmp(file, "0") != 0) {
    FILE *fp = fopen(file, "r");

    if(fp == NULL) {
        printf("Não é possível abrir esse ficheiro.");
        exit(1);
    }

    fgets(buff, sizeof(buff), fp);

    if(buff[0] != 'P' || buff[1] != '3' ) {
        printf("Ficheiro tem de ser P3");
        exit(1);
    }

    strcpy(img->magic_num,"P3");

    fp = skipcom(fp);

    fscanf(fp, "%d %d", &sav[0], &sav[1]); //colums and rows

    fp = skipcom(fp);

    fscanf(fp, "%d", &img->max); //scan for max value

    fp = skipcom(fp);

    img->col = sav[0]; img->lin = sav[1]; //save rows and cols

    data = (pixel **)malloc(sav[1] * sizeof(pixel *)); //allocate some rows

    for (i=0; i < sav[1]; i++)
        data[i] = (pixel *)malloc(sav[0] * sizeof(pixel)); //allocate a row for each column

        //using the dot operator because the data sctruct is created in the function
        //only use arrow for pointer of struct
    for (i = 0; i < sav[1]; i++) {
        for (j = 0; j < sav[0]; j++) {
            fscanf(fp,"%d %d %d", &data[i][j].r, &data[i][j].g, &data[i][j].b);
            fp = skipcom(fp);
        }
    }

    fclose(fp);

  } else {

    fgets(buff, sizeof(buff), stdin);

    if(buff[0] != 'P' || buff[1] != '3' ) {
        printf("O formato tem de ser P3");
        exit(1);
    }

    strcpy(img->magic_num,"P3");

    skipcomconsole();

    scanf("%d %d", &sav[0], &sav[1]);

    skipcomconsole();

    scanf("%d", &img->max);

    skipcomconsole();

    img->col = sav[0]; img->lin = sav[1]; //save rows and cols

    data = (pixel **)malloc(sav[1] * sizeof(pixel *)); //allocate some rows

    for (i=0; i < sav[1]; i++)
        data[i] = (pixel *)malloc(sav[0] * sizeof(pixel)); //allocate a row for each column

    for (i = 0; i < sav[1]; i++) {
        for (j = 0; j < sav[0]; j++){
            scanf("%d %d %d", &data[i][j].r, &data[i][j].g, &data[i][j].b);
            skipcomconsole();
        }
    }
  }

  return data;
}

void print(char *file, metadados *img, pixel **tela) {
    int i, j;

    if (strcmp(file, "0") != 0) {
      FILE *fp = fopen(file,"w+");

      fprintf(fp, "%s\n", img->magic_num);
      fprintf(fp, "%d %d\n", img->col, img->lin);
      fprintf(fp, "%d\n", img->max);

      for (i = 0; i < img->lin; i++){
          for (j = 0; j < img->col; j++)
              fprintf(fp, "%d %d %d\n", tela[i][j].r, tela[i][j].g, tela[i][j].b);
    }

    fclose(fp);

    } else {  //print onto the console

        printf("%s\n", img->magic_num);
        printf("%d %d\n", img->col, img->lin);
        printf("%d\n", img->max);

        for (i = 0; i < img->lin; i++){
            for (j = 0; j < img->col; j++)
                printf("%d %d %d\n", tela[i][j].r, tela[i][j].g, tela[i][j].b);
        }
    }
}

void overlay(char *file1, char *file2, char *pfile, int a, int b) {
  pixel **data1;
  pixel **data2;
  pixel **printing; //output
  metadados *img1;
  metadados *img2;
  metadados *pr; //metadados for the output

  img1 = (metadados *)malloc(sizeof(metadados));
  img2 = (metadados *)malloc(sizeof(metadados));
  pr = (metadados *)malloc(sizeof(metadados));

  data2 = read(file2, img2);
  data1 = read(file1, img1);

  pr->col = img1->col;
  pr->lin = img1->lin;
  pr->max = img1->max;
  strcpy(pr->magic_num, "P3");

  printing = (pixel **)malloc((pr->lin) * sizeof(pixel *)); //allocate some rows

  for (int i=0; i < (pr->lin); i++)
    printing[i] = (pixel *)malloc((pr->col) * sizeof(pixel)); //allocate a row for each column

  if(img2->lin+b > pr->lin)
    img2->lin = pr->lin-b;

  if(img2->col+a > pr->col)
    img2->col = pr->col-a;


  for (int i=0; i < pr->lin; i++){
    for (int j=0; j < pr->col; j++) {
      if ((j >= a) && (i >= b) && (j < img2->col+a) && (i < img2->lin+b))
        printing[i][j] = data2[i-b][j-a];
      else
        printing[i][j] = data1[i][j];
    }
  }

  print(pfile, pr, printing);

}


int main(int argc, char *argv[]) {

  char *input2="0", *output="0";
  int f = 0;

  if (argc>4) {  //there might be an input or output file
    int i;
    for (i=0; i<argc; i++) {
        if (strcmp(argv[i], ">") == 0 ) {
            output = argv[i+1];
            if (argc == 7 && i == 5) {
              input2 = argv[4];
              f = 1;
            } else
                f = 2;
        }
      }
      if (f == 0) { //there is an input file but no output
        input2 = argv[4];
      }
    }

  overlay(input2, argv[3], output, atoi(argv[1]), atoi(argv[2]));

  return 0;
}
