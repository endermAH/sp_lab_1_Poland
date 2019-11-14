#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>

const char *optString = "h";

char *outputString = NULL;
char *envVariable = NULL;
int endOfOutputString = 0;
int endOfENVVariable = 0;

struct globalArgs_t {
  char* inputPath;
  char* outputPath;
  FILE* inputFile;
  FILE* outputFile;
} globalArgs;

void addCharToOutput(char target_char) {
  endOfOutputString++;
  outputString = realloc(outputString, endOfOutputString * sizeof(char));
  outputString[endOfOutputString-1] = target_char;
}

void addCharToENVVar(char target_char) {
  endOfENVVariable++;
  envVariable = realloc(envVariable, endOfENVVariable * sizeof(char));
  envVariable[endOfENVVariable-1] = target_char;
  envVariable[endOfENVVariable] = '\0';
}

void addENVToOUTPUT() {
  char *envpointer;
  int envValLen;
  int envNameLen;
  if (getenv(envVariable)) {
    envpointer = getenv(envVariable);
    envValLen = strlen(envpointer);
    for (int i = 0; i < envValLen; i++) {
      addCharToOutput(envpointer[i]);
    }
    envVariable = NULL;
    endOfENVVariable = 0;
  } else {
    addCharToOutput('X'); //Добавляет крест вместо неправильной переменной
    envVariable = NULL;
    endOfENVVariable = 0;
  }
}

void startJob() {
  short int state = 0; // 0 - чтение текста, 1 - чтение переменной, 2 - конец чтения переменной
  char inChar;

  while (!feof(globalArgs.inputFile)){
    fscanf(globalArgs.inputFile,"%c",&inChar);
    if (inChar == '$') {
      state = 1;
    } else if ((inChar == ' ' || inChar == '\n' || inChar == '\0') && (state == 1)) {
      state = 2;
    }
    if (state == 1) {
      if (inChar != '$') {
        addCharToENVVar(inChar);
      }
    } else if (state == 2) {
      addENVToOUTPUT();
      state = 0;
    }
    if (state == 0) {
      addCharToOutput(inChar);
    }
  }
}

void display_usage(char* name) {
  //Отображение странички с помощью
  printf("\nUSAGE:\n%s [-h] \n\nARGS: \n-h: Help\n\n", name);
  exit(EXIT_SUCCESS);
}

int getStartData(int argc, char** argv) {
  int opt = 0;

  //Распределение аргументов массива по переменным
  opt = getopt(argc, argv, optString);
  while (opt != -1) {
    switch (opt) {
      case 'h':
        display_usage(argv[0]);
        break;
      default:
        break;
    }
    opt = getopt(argc, argv, optString);
  }

  //Парсинг путей
  if (optind < argc) {
    globalArgs.inputPath = argv[optind++];
    if (optind < argc) {
      globalArgs.outputPath = argv[optind];
    }
  }

  // //Проверка на верно введенные данные
  // if (globalArgs.optionDecode == globalArgs.optionEncode) {
  //   fprintf(stderr, "Encode and Decode options are same!\n");
  //   exit(EXIT_FAILURE);
  // }
  // if (globalArgs.key == NULL) {
  //   fprintf(stderr, "Can not run without key!\n");
  //   exit(EXIT_FAILURE);
  // }
  return 1;
}

int main(int argc, char** argv) {

  //Инициализация структуры
  globalArgs.inputPath = NULL;
  globalArgs.outputPath = NULL;

  //Обработка ключей
  if (getStartData(argc, argv) == 0) printf("Error occured - programm has stopped\n");

  //Проверка путей
  if (globalArgs.outputPath != NULL) {
    if ((globalArgs.outputFile = fopen(globalArgs.outputPath, "w")) == NULL) {
      fprintf(stderr, "Can not open output file!\n");
      exit(EXIT_FAILURE);
    }
  } else {
    globalArgs.outputFile = stdout;
  }

  if (globalArgs.inputPath != NULL) {
    if ((globalArgs.inputFile = fopen(globalArgs.inputPath, "r")) == NULL) {
      fprintf(stderr, "Can not open input file!\n");
      exit(EXIT_FAILURE);
    }
  } else {
    globalArgs.inputFile = stdin;
  }

  startJob();

  if (globalArgs.inputPath == NULL) {
    endOfOutputString -= 2;
  }

  if (endOfOutputString != 0) {
    for (int i = 0; i <= endOfOutputString; i++) {
      fprintf(globalArgs.outputFile, "%c", outputString[i]);
    }
  }

  return 0;
}
