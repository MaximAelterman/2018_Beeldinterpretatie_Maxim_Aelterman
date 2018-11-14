# Sessie 2

Het programma leest een kleurafbeelding in en herkent het rood van een verkeersbord.
Dit verkeersbord wordt dan gescheiden van de rest van de afbeelding.

 - Het eerste deel probeert dit in het rgb-kleurenspectrum: Het voordeel van deze methode is dat het simpel te programmeren is, maar dit ten koste van presieze.
 - Door gelijk welke knop in te drukken gaat het programma naar het tweede deel waar gebruik wordt gemaakt van het hsv-kleurenspectrum: Deze methode brengt meer presiezie met zich mee, maar is moeilijker te interpreteren aangezien de HSV-ruimte niet zo intuïtief is t.o.v. de RGB-ruimte.
Dit deel van het programma maakt gebruik van trackbars om de thresholds waarmee het rood wordt herkent aan te passen.

Het programma wordt gestopt door 'q' in te drukken.


Gebruik:
```
./sessie_2 --i <path_to_color_image>
```
