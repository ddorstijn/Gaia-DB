#pragma once

typedef struct _star
{
    char designation[30]; // Unique star id
    double x;             // X position star
    double y;             // Y position star
    double z;             // Z position star
    int colour;           // Colour of the star in hex converted to int
    unsigned long long morton_index; // Morton-code of the star in a 3d-grid
} STAR;