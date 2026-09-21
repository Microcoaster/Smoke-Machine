// Réglages du module de fumée.
//
// Le firmware reste à écrire : ce fichier fixe le contrat que le code
// devra respecter, et sert de point unique pour les deux valeurs qui
// comptent.

#ifndef CONFIG_H
#define CONFIG_H

// Grille du MOSFET canal N qui commute la machine. Une seule ligne de
// commande suffit : la machine intègre déjà sa résistance et son ventilateur.
#define PIN_HEATER 18

// Durée maximale d'un cycle, en millisecondes.
//
// C'est la seule protection locale du module, et elle n'est pas
// négociable : sur un ordre perdu ou une liaison coupée, une résistance
// laissée sous tension chauffe sans limite. Le contrôleur peut demander
// moins, jamais plus.
#define HEATER_MAX_ON_MS 15000

#endif
