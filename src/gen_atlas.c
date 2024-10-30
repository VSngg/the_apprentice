#include "raylib.h"
#include <stdio.h>

int main(void) {
    Image img = LoadImage("src/resources/atlas.png");
    bool ok = ExportImageAsCode(img, "src/atlas.h");
    if (!ok) {
        printf("Could not export image!\n");
        return 1;
    }
    UnloadImage(img);
    return;
}