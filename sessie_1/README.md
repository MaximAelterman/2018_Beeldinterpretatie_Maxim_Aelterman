# Sessie 1

Om naar het volgend deel van het programma te springen kan gelijk welke toets ingdrukt worden:
- Leest de kleurenafbeeling in en past er huiddetectie op toe.
- Leest de grijswaardenafbeelding in en segmenteert de tekst via OTSU thresholding, histogram equalization en CLAHE
- Past erosie en dilatie toe op het resultaat van de huiddetectie om ruis te onderdrukken, vervolgens worden de contouren gezocht en dit wordt als masker gebruikt op de originele afbeelding.


Gebruik:
```
./sessie_1 --ci <path_to_color_image>  --bi <path_to_grayscale_image>
```
