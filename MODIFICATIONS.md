# Liste des modifications

## Constantes et Nombres Magiques
- Passage de `SEUIL_VALIDATION` (10) et `SEUIL_BLOQUANT` (8) en constantes `float` au lieu d'une énumération, pour correspondre au type des notes.
- Ajout de la constante `MAX_NOTE` (20.0f) pour remplacer les occurrences de "20.0f" (nombre magique) dans le code.
- Remplacement du nombre magique "3" par `NB_ANNEES` dans `calculer_moyenne_annee` et `cmd_bilan`.
- Remplacement de "20.0f" par `MAX_NOTE` dans `cmd_note`.

## Fonctions Utilitaires
- Renommage de `verif_notes_completes` en `verif_notes_incompletes` pour mieux refléter son comportement (renvoie vrai si incomplet).
- Optimisation de `verif_notes_incompletes` : pour les semestres pairs, on ne vérifie plus que le semestre courant (il est impossible d'atteindre un semestre pair sans avoir validé le précédent).
- Modification de `passer_semestre_suivant` :
    - Retourne désormais le nombre d'étudiants ayant changé de statut (au lieu d'utiliser un pointeur).
    - Ajout d'une vérification stricte : la fonction ne s'exécute que si le numéro de semestre est impair.

## Refactoring cmd_cursus
- Réécriture complète de la fonction pour la rendre générique et extensible.
- Utilisation de boucles sur `NB_ANNEES` au lieu de blocs de code dupliqués pour chaque année.
- Utilisation d'un tableau 2D `moyennes_annee[NB_ANNEES][NB_UE]` au lieu de tableaux séparés `moy_b1`, `moy_b2`, `moy_b3`.
- Gestion dynamique de l'affichage des semestres et bilans annuels.

## Refactoring Jury
- Création d'une fonction générique `jury_fin_annee(t_etudiant* etu, int annee)` qui remplace `jury_fin_annee1`, `jury_fin_annee2` et `jury_fin_annee3`.
- Cette fonction unique gère les cas intermédiaires et l'année de diplôme, ainsi que la vérification de la validation des années précédentes.
- Mise à jour de `cmd_jury` pour appeler cette nouvelle fonction de manière générique.

## Documentation
- Mise à jour des commentaires Doxygen pour refléter les changements de noms et de signatures de fonctions.
- Ajout de la documentation pour les nouvelles constantes.
