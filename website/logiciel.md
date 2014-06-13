Un des grands défis de ce projet est de générer un son de façon fluide en fonction du mouvement de l'utilisateur. Pour analyser les mouvements, nous utilisons un accéléromètre, un compostant qui mesure l’accélération reçu selon trois directions. Le type de mouvement analysé par l'accéléromètre va indiquer à la carte électronique quel fichier audio lire.

Pour stocker les fichiers audio, nous avons fait le choix d'utiliser une carte microSD ; elle est formatée avec une partition FAT32, qui a l'avantage d'être simple et lisible sur tous les systèmes d'exploitation. Les fichiers audio sont enregistrés dans le format brut PCM dans un conteneur WAVE appelé communément "format wave". Un format compressé comme le MP3 nécessiterai une puce dédié qui permet de décompresser le fichier ce qui apporte une complexité inutile pour les besoins de notre projet.

# Microcontrolleur
Nous avons utilisé un microcontroleur [ATMEGA328p](http://www.atmel.com/devices/atmega328p.aspx), utilisable avec les librairies "Arduino".
Ce choix est d'abord technologique, un microcontrolleur 8 bits est amplement suffisant pour l'ampleur du projet ; il n'est pas utile d'utiliser un processeur ARM 80 broches fonctionnant à 80MHz comme ce que nous avions immaginé au départ.
Ensuite c'est un choix de simplicité, nous pouvons utiliser un grand nombre de librairies existantes, bien faites et documentées ce qui nous permet de ne pas *réinventer la roue*.

Par soucis de simplicité de routage et d'emcombrement (nous pouvons difficilement souder des CMS de tailles inférieure à 0805 à la main), nous avons choisis d'utiliser une carte "ARDUINO PRO MINI". Ces cartes se trouvent pour moins de 2€ sur des [vendeurs chinois](http://www.ebay.fr/sch/i.html?_sacat=0&_sop=15&_nkw=arduino+pro+mini&LH_PrefLoc=2&rt=nc&LH_BIN=1) ce qui d'ailleurs est moins cher que le microcontrolleur seul sur des [vendeurs occidentaux](http://fr.farnell.com/atmel/atmega328p-au/micro-8-bits-avr-32k-flash-32tqfp/dp/1715486).


# Gestion de la carte SD

## Protocole SPI
Le microcontrolleur communique avec la carte SD avec le protocole SPI (Serial Peripheral Interface). C'est un bus de donnée série synchrone (l'horloge est sur le fil sck) permettant de communiquer en full duplex (le maitre et l'esclave peuvent *parler* en même temps).

Il faut noter que la carte SD fonctionne en logique 3.3V, si notre microcontrolleur utilise une logique 5V il faut faire une adaptation de tension pour ne pas endommager la carte SD. Cela peut se faire avec un composant dédié ou de façon plus simple mais moins performante avec un pont diviseur de tension.

TODO : insérer image


## Format de partition FAT32

La librairie [SdFat](http://arconlab.com/lab/Arduino/Library/SD%20Reader%20-%20Fat32/html/class_sd_file.html) permet de prendre facilement en main un support de stockage formaté en FAT16 ou FAT32 sur des microcontrolleurs atmega.

## Format de fichier WAVE-PCM
Une très bon cours sur le format WAVE est accessible sur le [site de l'université de Stanford](https://ccrma.stanford.edu/courses/422/projects/WaveFormat).

Les entêtes du ficher contiennent les informations sur la fréquence d'échantillonnage du fichier son, le nombre de canaux, le nombre de bits par échantillons et ainsi que les données. A la lecture du fichier wave, une fois les informations contenues dans l'entête récupérées, on va lire les données du format PCM et les transmettre au DAC externe. Ceci s'effectue sur interruption dont la période est calculée en fonction de la fréquence d'échantillonage du fichier, ce qui laisse le microcontrolleur libre de traiter plusieurs choses de même temps. 

Les fichiers audio utilisés sont échantillonés entre 16 et 22kHz de telle sorte à diminuer le temps processeur aloué à la lecture du fichier, tout en gardant une qualité correcte. 

Pour lire les fichiers Wave et les transmettre au [DAC externe](http://ww1.microchip.com/downloads/en/DeviceDoc/21897a.pdf) nous utilisons la librairie [WaveHC](https://code.google.com/p/wavehc/) réalisé initialement par Lady Ada pour la carte d'extension d'extension [Wave Shield](http://www.adafruit.com/products/175).

TODO : mettre un bout de code



# Gestion de l'acceleromètre
## Communication en I2C

## Fonctionnalités de l'acceleromètres
## Algotithme utilisé





