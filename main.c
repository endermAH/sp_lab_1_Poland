#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>

const char *optString = "k:cdh";

char *outputString = NULL;
int endOfOutputString = 0;

struct globalArgs_t {
  bool optionEncode;
  bool optionDecode;
  char* key;
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

void startJob() {
  char inChar;
  char outChar;
  int index = 0;
  int len;
  int n = 127 - 32 + 1;

  len = strlen(globalArgs.key);

  while (!feof(globalArgs.inputFile)){
    fscanf(globalArgs.inputFile,"%c",&inChar);
    if (globalArgs.optionEncode) {
        if (inChar >= 32 && inChar <= 127){
          outChar = (((inChar - 32) + (globalArgs.key[index] - 32)) % 96) + 32; //Формула для шифровки символа
          index++;
          if (index == len) index = 0;
        } else outChar = inChar;
        addCharToOutput(outChar);
    } else if (globalArgs.optionDecode) {
      if (inChar >= 32 && inChar <= 127){
        outChar = (((inChar - 32) - (globalArgs.key[index] - 32) + n) % n) + 32; //Формула для расшифровки символа
        index++;
        if (index == len) index = 0;
      } else outChar = inChar;
      addCharToOutput(outChar);
    }
  }
}

void display_usage(char* name) {
  //Отображение странички с помощью
  printf("\nUSAGE:\n%s [-h] [-k <key word>] [-d] [-c] \n\nARGS: \n-k: Key word \n-c: Encoding\n-d: Decoding\n-h: Help\n\n", name);
  exit(EXIT_SUCCESS);
}

int getStartData(int argc, char** argv) {
  int opt = 0;

  //Распределение аргументов массива по переменным
  opt = getopt(argc, argv, optString);
  while (opt != -1) {
    switch (opt) {
      case 'k':
        globalArgs.key = optarg;
        break;
      case 'd':
        globalArgs.optionDecode = true;
        break;
      case 'c':
        globalArgs.optionEncode = true;
        break;
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

  //Проверка на верно введенные данные
  if (globalArgs.optionDecode == globalArgs.optionEncode) {
    fprintf(stderr, "Encode and Decode options are same!\n");
    exit(EXIT_FAILURE);
  }
  if (globalArgs.key == NULL) {
    fprintf(stderr, "Can not run without key!\n");
    exit(EXIT_FAILURE);
  }
  return 1;
}

int main(int argc, char** argv) {

  //Инициализация структуры
  globalArgs.optionEncode = false;
  globalArgs.optionDecode = false;
  globalArgs.key = NULL;
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

  char* sentence;
  int stringNumber = 1;

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
