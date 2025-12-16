# Gestion de la Scolarité des Étudiants de BUT (SAE S1.01)

Ce projet est une application en langage C permettant de gérer la scolarité d'une promotion d'étudiants en Bachelor Universitaire de Technologie (BUT). Il permet de suivre les étudiants tout au long de leur cursus (6 semestres), de gérer leurs notes, leurs statuts et les décisions de jury.

## Auteurs

- **Keziah GEBAUER**
- **Badis RAHLI**

## Fonctionnalités

L'application fonctionne en ligne de commande et accepte les instructions suivantes :

### Gestion des étudiants
- `INSCRIRE [Prenom] [Nom]` : Inscrit un nouvel étudiant dans la promotion.
- `ETUDIANTS` : Affiche la liste complète des étudiants avec leur ID, nom, semestre actuel et statut.
- `CURSUS [ID_Etudiant]` : Affiche le relevé de notes détaillé et le parcours d'un étudiant spécifique.

### Gestion des notes
- `NOTE [ID_Etudiant] [Num_UE] [Note]` : Enregistre ou modifie une note pour une Unité d'Enseignement (UE) donnée au semestre actuel de l'étudiant.

### Gestion administrative et Jurys
- `DEMISSION [ID_Etudiant]` : Enregistre la démission d'un étudiant.
- `DEFAILLANCE [ID_Etudiant]` : Enregistre la défaillance d'un étudiant.
- `JURY [Num_Semestre]` : Lance le jury pour un semestre donné.
  - **Semestres impairs (1, 3, 5)** : Passage automatique au semestre suivant.
  - **Semestres pairs (2, 4, 6)** : Calcul des moyennes annuelles, vérification des conditions de passage (validation, compensation) et attribution du statut (passage, ajournement, diplôme).
- `BILAN [Annee]` : Affiche les statistiques de la promotion pour une année donnée (1, 2 ou 3).

## Compilation

Le projet nécessite un compilateur C standard (comme GCC).

Pour compiler le projet sous Linux/macOS :

```bash
gcc SAE_C.c -o sae_c -lm
```

*(L'option `-lm` est nécessaire pour lier la bibliothèque mathématique utilisée pour les arrondis).*

## Utilisation

Lancez l'exécutable généré :

```bash
./sae_c
```

Ensuite, saisissez les commandes souhaitées. Pour quitter l'application, tapez :

```bash
EXIT
```

### Exemple de session

```text
INSCRIRE Jean Dupont
Inscription enregistree (1)
NOTE 1 1 12.5
Note enregistree
NOTE 1 2 9.0
Note enregistree
...
JURY 1
Semestre termine pour 1 etudiant(s)
```

## Structure du Code

- **Constantes et Types** : Définition des seuils (Validation à 10, Blocage à 8), des structures `t_etudiant` et `t_promotion`.
- **Logique Métier** : Fonctions de calcul de moyennes, vérification des conditions de passage.
- **Interface** : Fonctions d'affichage formaté (codes ADM, ADS, AJ, etc.).
- **Refactoring** : Le code a été optimisé pour être générique et extensible (gestion dynamique des années et semestres).

Pour plus de détails techniques, consultez la documentation Doxygen intégrée dans le fichier source `SAE_C.c`.
