Un des grands défis de ce projet est de générer un son de façon fluide en fonction du mouvement de l'utilisateur. Pour analyser les mouvements, nous utilisons un accéléromètre, un compostant qui mesure l’accélération reçu selon trois directions. Le type de mouvement analysé par l'accéléromètre va indiquer à la carte électronique quel fichier audio lire.

Pour stocker les fichiers audio, nous avons fait le choix d'utiliser une carte microSD ; elle est formatée avec une partition FAT32, qui a l'avantage d'être simple et lisible sur tous les systèmes d'exploitation. Les fichiers audio sont enregistrés dans le format brut PCM dans un conteneur WAVE appelé communément "format wave". Un format compressé comme le MP3 nécessiterai une puce dédié qui permet de décompresser le fichier ce qui apporte une complexité inutile pour les besoins de notre projet.

# Microcontrolleur
Nous avons utilisé un microcontroleur [ATMEGA328p](http://www.atmel.com/devices/atmega328p.aspx), utilisable avec les librairies "Arduino".
Ce choix est d'abord technologique, un microcontrolleur 8 bits est amplement suffisant pour l'ampleur du projet ; il n'est pas utile d'utiliser un processeur ARM11 80 broches fonctionnant à 60MHz comme ce que nous avions immaginé au départ.
Ensuite c'est un choix de simplicité, nous pouvons utiliser un grand nombre de librairies existantes, relativement bien codées et documentées ce qui nous permet de ne pas *réinventer la roue*.

Par soucis de simplicité de routage et d'emcombrement (nous pouvons difficilement souder des CMS de tailles inférieure à 0805 à la main), nous avons choisis d'utiliser une carte "ARDUINO PRO MINI". Ces cartes se trouvent pour moins de 2€ sur des [vendeurs chinois](http://www.ebay.fr/sch/i.html?_sacat=0&_sop=15&_nkw=arduino+pro+mini&LH_PrefLoc=2&rt=nc&LH_BIN=1) ce qui d'ailleurs est moins cher que le microcontrolleur seul sur des [vendeurs occidentaux](http://fr.farnell.com/atmel/atmega328p-au/micro-8-bits-avr-32k-flash-32tqfp/dp/1715486).


# Gestion de la carte SD

## Protocole SPI
Le microcontrolleur communique avec la carte SD avec le protocole SPI (Serial Peripheral Interface). C'est un protocole série synchrone (l'horloge étant nommée sck) permettant de communiquer en full duplex (le maitre et l'esclave peuvent *parler* en même temps). Le maitre communique sur le fil MOSI (Master Output Salve Input) et le périphérique esclave sur MISO. C'est un protocole optimisé pour la rapidité ; on contrôle avec quel périphérique on *discute* en appliquant un état bas sur le chip select (CS) du périphérique correspondant.
Pour communiquer avec plusieurs périphériques en SPI il faut donc autant de CS que de périphériques, ce qui est assez limitant.

Il faut noter que la carte SD fonctionne en logique 3.3V, si notre microcontrolleur utilise une logique 5V (ce qui est le cas pour nous) il faut faire une adaptation de tension 5/3V3 sur tous les signaux pour ne pas endommager la carte SD. Cela peut se faire avec un composant dédié comme un buffer ou de façon plus simple mais moins performante avec un pont diviseur de tension.

TODO : insérer image


## Format de partition FAT32

La librairie [SdFat](http://arconlab.com/lab/Arduino/Library/SD%20Reader%20-%20Fat32/html/class_sd_file.html) permet de prendre facilement en main un support de stockage formaté en FAT16 ou FAT32 sur des microcontrolleurs atmega.

## Format de fichier WAVE-PCM
Une très bon cours sur le format WAVE est accessible sur le [site de l'université de Stanford](https://ccrma.stanford.edu/courses/422/projects/WaveFormat).
![Wave-PCM file](http://lightsaber.ensea.fr/data/images/wav-sound-format.gif)

Les entêtes du ficher contiennent les informations sur la fréquence d'échantillonnage du fichier son, le nombre de canaux, le nombre de bits par échantillons et ainsi que les données. A la lecture du fichier wave, une fois les informations contenues dans l'entête récupérées, on va lire les données du format PCM et les transmettre au DAC externe. Ceci s'effectue sur interruption dont la période est calculée en fonction de la fréquence d'échantillonage du fichier, ce qui laisse le microcontrolleur libre de traiter plusieurs choses de même temps. 

Les fichiers audio utilisés sont échantillonés entre 16 et 22kHz de telle sorte à diminuer le temps processeur aloué à la lecture du fichier, tout en gardant une qualité correcte. 

Pour lire les fichiers Wave et les transmettre au [DAC externe](http://ww1.microchip.com/downloads/en/DeviceDoc/21897a.pdf) nous utilisons la librairie [WaveHC](https://code.google.com/p/wavehc/) réalisé initialement par Lady Ada pour la carte d'extension d'extension [Wave Shield](http://www.adafruit.com/products/175).


# Gestion de l'acceleromètre
## Quel acceleromètre choisir et comment interprêter les données

L'acceleromètre est le composant crucial du projet pour permettre d'interprêter le mouvement. Après beaucoup de temps passé à élaborrer des algorithmes plus ou moins performant pour détecter le mouvement en focntion des seules composants de l'acceleration, nous nous sommes tournés vers une composant qui fait tout le traitement pour nous : l'[ADXL345](http://lightsaber.ensea.fr/data/documents/adxl345.pdf).
Il possède comme fonctionnalités inbteressantes :

* une détection de choc (*tap*)
* une détection de double choc (*double tap*)
* une détection de chute libre (*freefall*)
* une détection d'activité, inactivité, 
* ...

La détection de choc et d'activité sont particulièrement interessantes pour nous car elles nous permettent de différencier un non mouvement d'un mouvement, et d'un choc (avec un autre sabre ou un objet).

De plus cette détection se fait par interruption ce qui allège le microcontrolleur de calculs ou d'attente active dans la boucle principale !


## Communication en I2C
Le microcontrolleur communique avec l'acceléromètre en I2C. C'est un protocole synchrone, half duplex et bidirectionnel. Il y a seulement deux fils : SDA (Serial DAta line) qui sert à la communication et SCL (Serial Clock Line) qui transmet l'horloge imposé par le maitre. C'est un protocole maitre esclave, chaque communication ne peux être imposé que par le maitre, mais l'esclave peut devenir maitre (rébélion !). C'est un protocole initialement fait pour la domotique ; chaque périphérique étant identifié par un identifiant (dont il doit être unique sur ce bus, bien évidement), l'idée et qu'un microcontrolleur puisse interroger plusieurs centaines de capteurs différents, tous connectés sur un même bus de deux fils (limitant ainsi le nombre de fils et d'I/O).

## Algorithme utilisé

L'algorithme utilisé est en somme très simple. On effectue une machine à état qui va passer d'un son à l'autre selon les données reçu par l'accéléromètre.
L'acceleromètre va changer la variable globale d'état ***state*** à 2 lorsqu'on détecte une activité, ou à 3 lorsqu'on détecte un choc.

Voici le code de la boucle principale simplifié : 

```python

switch(state) {
            case 1 : //ILDE
              if (!wave.isplaying) //si aucun son n'est joué
                  playfile(idle) ; //Joue le son idle
              break;
            case 2 : //SWING
                delay(10); //attente de 10ms pour différencier le choc du mouvement 
                if(state == 3)
                {
                  playHit() ; //Joue un son de type hit
                  delay(200); //attente de 200ms d'anti-rebond
                }
                else
                {
                  playSwing() ; //Joue un son de type swing
                }
                state = 1;
                break ;
            case 3 : //HIT
                playHit() ; 
                delay(200); //attente de 200ms d'anti-rebond
                state = 1;
                break ;
 }
```

## Configuration de l'accéléromètre
Pour faciliter la configuration et l'utilisation de l'accéléromètre, nous utilisons une librairie du projet libre [*I2Cdev library collection*](http://www.i2cdevlib.com/).

Pour utiliser l'accéléromètre selon nos besoins, il nous faut activer l'interruption sur la détection de choc, prociser les filtres de la détection de choc, préciser les axes de détection ; et faire de même pour la détection d'activité.
Avec cette librairie cela donne :

```python
#define THRESH_TAP 70 
#define THRESH_ACT 22 
#define DUR 20    

ADXL345 myACC = ADXL345(); 

void setup()
{
myACC.initialize();

//TAP detection   
myACC.setTapThreshold(THRESH_TAP) ;
myACC.setTapDuration(DUR);
myACC.setIntSingleTapEnabled(true);
myACC.setIntSingleTapPin(0);
myACC.setTapAxisXEnabled(true);
myACC.setTapAxisYEnabled(true);
myACC.setTapAxisZEnabled(true);

//Activity detection
myACC.setActivityThreshold(THRESH_ACT);
myACC.setIntActivityEnabled(true);
myACC.setIntActivityPin(1);
myACC.setActivityXEnabled(true);
myACC.setActivityYEnabled(true);
myACC.setActivityZEnabled(true);
myACC.setActivityAC(true);

//Stop auto sleep
myACC.setSleepEnabled(false);
  }
```

Un front haut est sur la sortie d'interruption 0 de l'accéléromètre est activé en cas de choc, et de même sur la pin 1 en cas de mouvement. Ces deux pins sont reliées aux entrées d'interruption externe de notre microcontrolleur, ce qui nous permet de changer l'état de la machine à état qui gère la lecture des fichiers au sein des fonctions d'interruption.
