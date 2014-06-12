Un des grands défis de ce projet est de générer un son de façon fluide en fonction du mouvement de l'utilisateur. Pour analyser les mouvements, nous utilisons un accéléromètre, un compostant qui mesure l’accélération reçu selon trois directions. Le type de mouvement analysé par l'accéléromètre va indiquer à la carte électronique quel fichier audio lire.

Pour stocker les fichiers audio, nous avons fait le choix d'utiliser une carte microSD ; elle est formatée avec une partition FAT32, qui a l'avantage d'être simple et lisible sur tous les systèmes d'exploitation. Les fichiers audio sont enregistrés dans le format brut PCM dans un conteneur WAVE appelé communément "format wave", un format compressé de type MP3 nécessite une puce dédié qui permet de décompresser le signal

# Microcontrolleur
Nous avons utilisé un microcontroleur [ATMEGA328p](http://www.atmel.com/devices/atmega328p.aspx), utilisable avec les librairies "Arduino".
Ce choix est d'abord technologique, un microcontrolleur 8 bits est amplement suffisant pour l'ampleur du projet ; il n'est pas utile d'utiliser un processeur ARM 80 broches fonctionnant à 80MHz comme ce que nous avions immaginé au départ.
Ensuite c'est un choix de simplicité, nous pouvons utiliser un grand nombre de librairies existantes, bien faites et documentées ce qui nous permet de ne pas *réinventer la roue*.

Par soucis de simplicité de routage et d'emcombrement (nous pouvons difficilement souder des CMS de tailles inférieure à 0805 à la main), nous avons choisis d'utiliser une carte "ARDUINO PRO MINI". Ces cartes se trouvent pour moins de 2€ sur des [vendeurs chinois](http://www.ebay.fr/sch/i.html?_sacat=0&_sop=15&_nkw=arduino+pro+mini&LH_PrefLoc=2&rt=nc&LH_BIN=1) ce qui d'ailleurs est moins cher que le microcontrolleur seul sur des [vendeurs occidentaux](http://fr.farnell.com/atmel/atmega328p-au/micro-8-bits-avr-32k-flash-32tqfp/dp/1715486).


# Gestion de la carte SD

## Protocole SPI
Le microcontrolleur communique avec la carte SD avec le protocole SPI (Serial Peripheral Interface). C'est un bus de donnée série synchrone (l'horloge est sur le fil sck) permettant de communiquer en full duplex (le maitre et l'esclave peuvent *parler* en même temps).

## Format de partition FAT32


## Format de fichier WAVE-PCM


# Gestion de l'acceleromètre





