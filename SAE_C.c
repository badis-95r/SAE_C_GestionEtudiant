/**
 * @file SAE_C.c
 * @brief SAE S1.01 - Gestion de la scolarité des étudiants de BUT.
 *
 * Programme de gestion des notes et du parcours scolaire des étudiants
 * en Bachelor Universitaire de Technologie (BUT). Permet l'inscription,
 * la saisie de notes, l'affichage du cursus, la gestion des jurys, etc.
 *
 * @author Keziah GEBAUER
 * @author Badis RAHLI
 * @version 1.0
 * @date 13/11/2025
 */

// ============================================================================
// SAE S1.01 - Gestion de la scolarité des étudiants de BUT
// ============================================================================

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#pragma warning(disable:4996 6031 6054)

// ============================================================================
// CONSTANTES
// ============================================================================

/**
 * @brief Limites du système.
 */
enum {
	MAX_ETUDIANTS = 100, /**< Nombre maximum d'étudiants dans la promotion */
	MAX_NOM = 30,        /**< Longueur maximale d'un nom ou prénom */
	NB_SEMESTRES = 6,    /**< Nombre total de semestres en BUT */
	NB_UE = 6,           /**< Nombre d'Unités d'Enseignement par semestre */
	NB_ANNEES = 3,       /**< Nombre d'années du cursus */
};

/**
 * @brief Seuils de validation et constantes pédagogiques.
 */
enum {
	MIN_RCUE_VALIDES = 4,    /**< Nombre minimal d'UE validées (>10) pour valider une année */
	TOUTES_UE_VALIDEES = 6,  /**< Nombre d'UE à valider pour certaines conditions (diplôme) */
};

const float SEUIL_VALIDATION = 10.0f; /**< Note minimale pour valider une UE ou un semestre (10/20) */
const float SEUIL_BLOQUANT = 8.0f;    /**< Note seuil en dessous de laquelle la compensation est impossible */
const float NOTE_INCONNUE = -1.0f;    /**< Valeur représentant une note non saisie */
const float MAX_NOTE = 20.0f;         /**< Note maximale possible */

// ============================================================================
// TYPES
// ============================================================================

/**
 * @brief Statut administratif d'un étudiant.
 */
typedef enum {
	EN_COURS,    /**< Étudiant en cours de formation */
	DEMISSION,   /**< Étudiant démissionnaire */
	DEFAILLANCE, /**< Étudiant défaillant (absences, etc.) */
	AJOURNE,     /**< Étudiant ajourné (redoublement ou exclusion selon le contexte) */
	DIPLOME,     /**< Étudiant ayant validé son diplôme */
} t_statut;

/**
 * @brief Structure représentant un étudiant.
 */
typedef struct {
	char prenom[MAX_NOM + 1];        /**< Prénom de l'étudiant */
	char nom[MAX_NOM + 1];           /**< Nom de l'étudiant */
	float notes[NB_SEMESTRES][NB_UE];/**< Tableau des notes (semestre, UE) */
	int semestre_actuel;             /**< Numéro du semestre actuel (1 à 6) */
	t_statut statut;                 /**< Statut actuel de l'étudiant */
} t_etudiant;

/**
 * @brief Structure représentant une promotion d'étudiants.
 */
typedef struct {
	t_etudiant etudiants[MAX_ETUDIANTS]; /**< Tableau des étudiants inscrits */
	int nb_inscrits;                     /**< Nombre actuel d'étudiants inscrits */
} t_promotion;

// ============================================================================
// PROTOTYPES
// ============================================================================

// Initialisation
void init_promotion(t_promotion* promo);

// Sprint 1
void cmd_inscrire(t_promotion* promo);
void cmd_note(t_promotion* promo);
void cmd_cursus(const t_promotion* promo);
void cmd_etudiants(const t_promotion* promo);
float calculer_moyenne_annee(const t_etudiant* etu, int annee, int num_ue);
void afficher_note_avec_code(float note, float moy_annee, float moy_suivante);
void afficher_moyenne_avec_code(float moy_annee, float moy_suivante);
void afficher_statut(t_statut statut);

// Sprint 2
void cmd_changer_statut(t_promotion* promo, t_statut nouveau_statut);
void cmd_jury(t_promotion* promo);
int verif_notes_incompletes(const t_promotion* promo, int num_sem);
int passer_semestre_suivant(t_promotion* promo, int num_sem);

// Sprint 3
void jury_fin_annee(t_etudiant* etu, int annee);

// Sprint 4
void cmd_bilan(const t_promotion* promo);

// ============================================================================
// MAIN
// ============================================================================

/**
 * @brief Point d'entrée du programme.
 *
 * Boucle principale qui lit les commandes utilisateur et appelle les
 * fonctions appropriées.
 *
 * @return 0 en cas de succès.
 */
int main() {
	char cmd[MAX_NOM + 1];
	t_promotion promo;

	init_promotion(&promo);

	while (1) {
		scanf("%s", cmd);

		if (strcmp(cmd, "EXIT") == 0) {
			break;
		}
		else if (strcmp(cmd, "INSCRIRE") == 0) {
			cmd_inscrire(&promo);
		}
		else if (strcmp(cmd, "NOTE") == 0) {
			cmd_note(&promo);
		}
		else if (strcmp(cmd, "CURSUS") == 0) {
			cmd_cursus(&promo);
		}
		else if (strcmp(cmd, "ETUDIANTS") == 0) {
			cmd_etudiants(&promo);
		}
		else if (strcmp(cmd, "DEMISSION") == 0) {
			cmd_changer_statut(&promo, DEMISSION);
		}
		else if (strcmp(cmd, "DEFAILLANCE") == 0) {
			cmd_changer_statut(&promo, DEFAILLANCE);
		}
		else if (strcmp(cmd, "JURY") == 0) {
			cmd_jury(&promo);
		}
		else if (strcmp(cmd, "BILAN") == 0) {
			cmd_bilan(&promo);
		}
	}

	return 0;
}

// ============================================================================
// INITIALISATION
// ============================================================================

/**
 * @brief Initialise la promotion avec des valeurs par défaut.
 *
 * Met à zéro le nombre d'inscrits et initialise toutes les notes à NOTE_INCONNUE.
 * Initialise également le statut et le semestre des emplacements vides.
 *
 * @param promo Pointeur vers la structure de promotion à initialiser.
 */
void init_promotion(t_promotion* promo) {
	assert(promo != NULL);

	promo->nb_inscrits = 0;

	for (int i = 0; i < MAX_ETUDIANTS; i++) {
		for (int sem = 0; sem < NB_SEMESTRES; sem++) {
			for (int ue = 0; ue < NB_UE; ue++) {
				promo->etudiants[i].notes[sem][ue] = NOTE_INCONNUE;
			}
		}
		promo->etudiants[i].statut = EN_COURS;
		promo->etudiants[i].semestre_actuel = 1;
	}
}

// ============================================================================
// SPRINT 1 - Commandes de base
// ============================================================================

// --- INSCRIRE ---
/**
 * @brief Inscrit un nouvel étudiant dans la promotion.
 *
 * Lit le prénom et le nom depuis l'entrée standard.
 * Vérifie si l'étudiant existe déjà (doublon).
 * Si la promotion n'est pas pleine, ajoute l'étudiant.
 *
 * @param promo Pointeur vers la promotion.
 */
void cmd_inscrire(t_promotion* promo) {
	assert(promo != NULL);

	char prenom_saisi[MAX_NOM + 1];
	char nom_saisi[MAX_NOM + 1];
	int doublon = 0;

	scanf("%s %s", prenom_saisi, nom_saisi);

	// Vérifier si l'étudiant existe déjà
	for (int i = 0; i < promo->nb_inscrits; i++) {
		if (strcmp(promo->etudiants[i].nom, nom_saisi) == 0 &&
			strcmp(promo->etudiants[i].prenom, prenom_saisi) == 0) {
			doublon = 1;
			break;
		}
	}

	if (doublon) {
		printf("Nom incorrect\n");
	}
	else {
		if (promo->nb_inscrits < MAX_ETUDIANTS) {
			int pos = promo->nb_inscrits;

			strcpy(promo->etudiants[pos].nom, nom_saisi);
			strcpy(promo->etudiants[pos].prenom, prenom_saisi);
			promo->etudiants[pos].statut = EN_COURS;
			promo->etudiants[pos].semestre_actuel = 1;

			promo->nb_inscrits++;
			printf("Inscription enregistree (%d)\n", promo->nb_inscrits);
		}
	}
}

// --- NOTE ---
/**
 * @brief Enregistre ou modifie la note d'un étudiant.
 *
 * Lit l'ID étudiant, le numéro d'UE et la note.
 * Effectue des vérifications (ID valide, étudiant en cours, UE valide, note valide).
 * Enregistre la note pour le semestre actuel de l'étudiant.
 *
 * @param promo Pointeur vers la promotion.
 */
void cmd_note(t_promotion* promo) {
	assert(promo != NULL);

	int id_etu, num_ue;
	float note;

	scanf("%d %d %f", &id_etu, &num_ue, &note);

	// Validation de l'ID
	if (id_etu <= 0 || id_etu > promo->nb_inscrits) {
		printf("Identifiant incorrect\n");
		return;
	}

	int idx = id_etu - 1;

	// Vérifier que l'étudiant est EN_COURS
	if (promo->etudiants[idx].statut != EN_COURS) {
		printf("Etudiant hors formation\n");
		return;
	}

	// Validation du numéro d'UE
	if (num_ue < 1 || num_ue > NB_UE) {
		printf("UE incorrecte\n");
		return;
	}

	// Validation de la note
	if (note < 0.0f || note > MAX_NOTE) {
		printf("Note incorrecte\n");
		return;
	}

	// Enregistrer la note au semestre actuel
	int idx_ue = num_ue - 1;
	int sem_actuel = promo->etudiants[idx].semestre_actuel;
	int idx_sem = sem_actuel - 1;

	promo->etudiants[idx].notes[idx_sem][idx_ue] = note;
	printf("Note enregistree\n");
}

// --- Fonctions de calcul ---

/**
 * @brief Calcule la moyenne annuelle (RCUE) pour une UE.
 *
 * Moyenne des notes d'une même UE sur les deux semestres de l'année.
 *
 * @param etu Pointeur vers l'étudiant.
 * @param annee Année concernée (1, 2 ou 3).
 * @param num_ue Index de l'UE (0 à NB_UE-1).
 * @return La moyenne sur 20, ou NOTE_INCONNUE si une des notes manque.
 */
float calculer_moyenne_annee(const t_etudiant* etu, int annee, int num_ue) {
	assert(etu != NULL && annee >= 1 && annee <= 3 && num_ue >= 0 && num_ue < NB_UE);

	int sem1 = (annee - 1) * 2;
	int sem2 = (annee - 1) * 2 + 1;

	float note1 = etu->notes[sem1][num_ue];
	float note2 = etu->notes[sem2][num_ue];

	if (note1 == NOTE_INCONNUE || note2 == NOTE_INCONNUE) {
		return NOTE_INCONNUE;
	}

	return (note1 + note2) / 2.0f;
}

// --- Fonctions d'affichage ---

/**
 * @brief Affiche une moyenne annuelle avec son code de validation.
 *
 * Codes affichés :
 * - ADM : Admis (>= 10)
 * - ADS : Admis par compensation Année Suivante (>= 10 année suivante)
 * - AJB : Ajourné mais compensable (entre 8 et 10)
 * - AJ  : Ajourné (< 8 ou pas de compensation)
 *
 * @param moy_annee Moyenne de l'année concernée.
 * @param moy_suivante Moyenne de l'année suivante (pour compensation).
 */
void afficher_moyenne_avec_code(float moy_annee, float moy_suivante) {
	if (moy_annee == NOTE_INCONNUE) {
		printf("* (*)");
		return;
	}

	float moy_tronquee = floorf(moy_annee * 10.0f) / 10.0f;

	if (moy_annee >= SEUIL_VALIDATION) {
		printf("%.1f (ADM)", moy_tronquee);
	}
	else if (moy_suivante != NOTE_INCONNUE && moy_suivante >= SEUIL_VALIDATION) {
		printf("%.1f (ADS)", moy_tronquee);  // Compensé par année suivante
	}
	else if (moy_annee < SEUIL_BLOQUANT) {
		printf("%.1f (AJB)", moy_tronquee);
	}
	else {
		printf("%.1f (AJ)", moy_tronquee);
	}
}

/**
 * @brief Affiche une note d'UE semestrielle avec son code de validation.
 *
 * Codes affichés :
 * - ADM : Admis (>= 10)
 * - ADC : Admis par compensation RCUE (Moyenne annuelle >= 10)
 * - ADS : Admis par compensation Année Suivante (Moyenne année suivante >= 10)
 * - AJ  : Ajourné
 *
 * @param note Note du semestre.
 * @param moy_annee Moyenne annuelle correspondante (RCUE).
 * @param moy_suivante Moyenne de l'année suivante.
 */
void afficher_note_avec_code(float note, float moy_annee, float moy_suivante) {
	if (note == NOTE_INCONNUE) {
		printf("* (*)");
		return;
	}

	float note_tronquee = floorf(note * 10.0f) / 10.0f;

	if (note >= SEUIL_VALIDATION) {
		printf("%.1f (ADM)", note_tronquee);
	}
	else if (moy_annee != NOTE_INCONNUE && moy_annee >= SEUIL_VALIDATION) {
		printf("%.1f (ADC)", note_tronquee);  // Compensation par RCUE
	}
	else if (moy_suivante != NOTE_INCONNUE && moy_suivante >= SEUIL_VALIDATION) {
		printf("%.1f (ADS)", note_tronquee);  // Compensé par année suivante
	}
	else {
		printf("%.1f (AJ)", note_tronquee);
	}
}

/**
 * @brief Affiche la chaîne de caractères correspondant à un statut.
 *
 * @param statut Le statut à afficher.
 */
void afficher_statut(t_statut statut) {
	switch (statut) {
	case EN_COURS:    printf("en cours");    break;
	case DEMISSION:   printf("demission");   break;
	case DEFAILLANCE: printf("defaillance"); break;
	case AJOURNE:     printf("ajourne");     break;
	case DIPLOME:     printf("diplome");     break;
	default:          printf("statut_inconnu"); break;
	}
}

// --- CURSUS ---
/**
 * @brief Affiche le parcours complet d'un étudiant.
 *
 * Affiche l'historique des semestres (S1 à S6) avec les notes,
 * les moyennes annuelles (B1, B2, B3) et les statuts.
 * Gère l'affichage des compensations (ADC, ADS) selon l'avancement
 * dans le cursus et les décisions de jury.
 *
 * @param promo Pointeur vers la promotion.
 */
void cmd_cursus(const t_promotion* promo) {
	assert(promo != NULL);

	int id_etu;
	scanf("%d", &id_etu);

	if (id_etu <= 0 || id_etu > promo->nb_inscrits) {
		printf("Identifiant incorrect\n");
		return;
	}

	int idx = id_etu - 1;
	const t_etudiant* etu = &promo->etudiants[idx];

	printf("%d %s %s\n", id_etu, etu->prenom, etu->nom);

	// Calculer toutes les moyennes annuelles
	float moyennes_annee[NB_ANNEES][NB_UE];
	int jury_fait[NB_ANNEES];

	for (int an = 0; an < NB_ANNEES; an++) {
		int annee = an + 1;
		for (int ue = 0; ue < NB_UE; ue++) {
			moyennes_annee[an][ue] = calculer_moyenne_annee(etu, annee, ue);
		}

		// Déterminer si le jury de l'année 'annee' a été fait
		// Conditions approximatives basées sur l'avancement dans les semestres
		int sem_fin_annee = annee * 2;
		jury_fait[an] = (etu->semestre_actuel > sem_fin_annee) ||
			(etu->semestre_actuel == sem_fin_annee && etu->statut != EN_COURS);
	}

	// Boucle sur les années
	for (int an = 0; an < NB_ANNEES; an++) {
		int annee = an + 1;
		int sem_impair = (annee - 1) * 2 + 1;
		int sem_pair = annee * 2;

		// Semestre impair (S1, S3, S5...)
		if (etu->semestre_actuel >= sem_impair) {
			printf("S%d", sem_impair);
			for (int ue = 0; ue < NB_UE; ue++) {
				printf(" - ");
				float moy_compens = jury_fait[an] ? moyennes_annee[an][ue] : NOTE_INCONNUE;
				float moy_suiv = (an < NB_ANNEES - 1 && jury_fait[an + 1]) ? moyennes_annee[an + 1][ue] : NOTE_INCONNUE;
				
				// Pour S5 (dernière année), pas de compensation année suivante
				if (annee == NB_ANNEES) moy_suiv = NOTE_INCONNUE;

				afficher_note_avec_code(etu->notes[sem_impair - 1][ue], moy_compens, moy_suiv);
			}
			printf(" -");
			if (etu->semestre_actuel == sem_impair) {
				printf(" ");
				afficher_statut(etu->statut);
			}
			printf("\n");
		}

		// Semestre pair (S2, S4, S6...)
		if (etu->semestre_actuel >= sem_pair) {
			printf("S%d", sem_pair);
			for (int ue = 0; ue < NB_UE; ue++) {
				printf(" - ");
				float moy_compens = jury_fait[an] ? moyennes_annee[an][ue] : NOTE_INCONNUE;
				float moy_suiv = (an < NB_ANNEES - 1 && jury_fait[an + 1]) ? moyennes_annee[an + 1][ue] : NOTE_INCONNUE;

				// Pour S6, pas de compensation année suivante
				if (annee == NB_ANNEES) moy_suiv = NOTE_INCONNUE;

				afficher_note_avec_code(etu->notes[sem_pair - 1][ue], moy_compens, moy_suiv);
			}
			printf(" -");
			if (etu->semestre_actuel == sem_pair && etu->statut != AJOURNE && etu->statut != DIPLOME) {
				printf(" ");
				afficher_statut(etu->statut);
			}
			printf("\n");
		}

		// Bilan Annuel (B1, B2, B3...)
		if (jury_fait[an] || (etu->semestre_actuel == sem_pair && etu->statut == AJOURNE)) {
			printf("B%d", annee);
			for (int ue = 0; ue < NB_UE; ue++) {
				printf(" - ");
				float moy_suiv = (an < NB_ANNEES - 1 && jury_fait[an + 1]) ? moyennes_annee[an + 1][ue] : NOTE_INCONNUE;
				
				// Pour B3, pas de compensation suivante
				if (annee == NB_ANNEES) moy_suiv = NOTE_INCONNUE;

				afficher_moyenne_avec_code(moyennes_annee[an][ue], moy_suiv);
			}
			printf(" -");
			if (etu->semestre_actuel == sem_pair && etu->statut == AJOURNE) {
				printf(" ajourne");
			} else if (annee == NB_ANNEES && etu->statut == DIPLOME) {
				printf(" ");
				afficher_statut(etu->statut);
			}
			printf("\n");
		}
	}
}

// --- ETUDIANTS ---
/**
 * @brief Affiche la liste de tous les étudiants de la promotion.
 *
 * Affiche l'ID, le nom, le prénom, le semestre actuel et le statut.
 *
 * @param promo Pointeur vers la promotion.
 */
void cmd_etudiants(const t_promotion* promo) {
	assert(promo != NULL);

	for (int i = 0; i < promo->nb_inscrits; i++) {
		const t_etudiant* etu = &promo->etudiants[i];
		int id = i + 1;

		printf("%d - %s %s - S%d - ", id, etu->prenom, etu->nom, etu->semestre_actuel);
		afficher_statut(etu->statut);
		printf("\n");
	}
}

// ============================================================================
// SPRINT 2 - DEMISSION, DEFAILLANCE, JURY (semestres impairs)
// ============================================================================

// --- DEMISSION / DEFAILLANCE ---
/**
 * @brief Change le statut d'un étudiant (Démission ou Défaillance).
 *
 * Vérifie l'ID et si l'étudiant est EN_COURS.
 * Modifie le statut selon le paramètre nouveau_statut.
 *
 * @param promo Pointeur vers la promotion.
 * @param nouveau_statut Nouveau statut à appliquer (DEMISSION ou DEFAILLANCE).
 */
void cmd_changer_statut(t_promotion* promo, t_statut nouveau_statut) {
	assert(promo != NULL);

	int id_etu;
	scanf("%d", &id_etu);

	if (id_etu <= 0 || id_etu > promo->nb_inscrits) {
		printf("Identifiant incorrect\n");
		return;
	}

	int idx = id_etu - 1;

	if (promo->etudiants[idx].statut != EN_COURS) {
		printf("Etudiant hors formation\n");
		return;
	}

	promo->etudiants[idx].statut = nouveau_statut;

	if (nouveau_statut == DEMISSION) {
		printf("Demission enregistree\n");
	}
	else {
		printf("Defaillance enregistree\n");
	}
}

// --- Fonctions auxiliaires pour JURY ---

/**
 * @brief Vérifie si toutes les notes sont saisies pour un semestre donné.
 *
 * Pour les jurys pairs (fin d'année), on vérifie le semestre pair courant.
 * (Il est impossible d'être en semestre pair sans avoir validé le précédent).
 * Pour les jurys impairs, vérifie uniquement le semestre courant.
 *
 * @param promo Pointeur vers la promotion.
 * @param num_sem Numéro du semestre à vérifier.
 * @return 1 si des notes sont manquantes, 0 sinon.
 */
int verif_notes_incompletes(const t_promotion* promo, int num_sem) {
	assert(promo != NULL);

	int idx_sem = num_sem - 1;

	for (int i = 0; i < promo->nb_inscrits; i++) {
		if (promo->etudiants[i].statut != EN_COURS ||
			promo->etudiants[i].semestre_actuel != num_sem) {
			continue;
		}

		for (int ue = 0; ue < NB_UE; ue++) {
			if (promo->etudiants[i].notes[idx_sem][ue] == NOTE_INCONNUE) {
				return 1;
			}
		}
	}

	return 0;
}

/**
 * @brief Fait passer les étudiants au semestre suivant (pour les jurys impairs).
 *
 * @param promo Pointeur vers la promotion.
 * @param num_sem Numéro du semestre actuel.
 * @return Le nombre d'étudiants ayant changé de semestre.
 */
int passer_semestre_suivant(t_promotion* promo, int num_sem) {
	assert(promo != NULL);

	// Vérifie que le semestre est bien impair
	if (num_sem % 2 == 0) {
		return 0;
	}

	int compteur = 0;
	for (int i = 0; i < promo->nb_inscrits; i++) {
		if (promo->etudiants[i].statut == EN_COURS &&
			promo->etudiants[i].semestre_actuel == num_sem) {

			promo->etudiants[i].semestre_actuel++;
			compteur++;
		}
	}
	return compteur;
}

// --- JURY ---
/**
 * @brief Gère les jurys de fin de semestre.
 *
 * Vérifie que toutes les notes sont présentes.
 * Si semestre impair : passage automatique au suivant.
 * Si semestre pair : appel de la fonction générique jury_fin_annee.
 *
 * @param promo Pointeur vers la promotion.
 */
void cmd_jury(t_promotion* promo) {
	assert(promo != NULL);

	int num_sem;
	scanf("%d", &num_sem);

	if (num_sem < 1 || num_sem > NB_SEMESTRES) {
		printf("Semestre incorrect\n");
		return;
	}

	if (verif_notes_incompletes(promo, num_sem)) {
		printf("Des notes sont manquantes\n");
		return;
	}

	int nb_etu_traites = 0;

	if (num_sem % 2 != 0) {
		// SEMESTRES IMPAIRS : Passage automatique
		nb_etu_traites = passer_semestre_suivant(promo, num_sem);
	}
	else {
		// SEMESTRES PAIRS : Application des règles
		for (int i = 0; i < promo->nb_inscrits; i++) {
			if (promo->etudiants[i].statut == EN_COURS &&
				promo->etudiants[i].semestre_actuel == num_sem) {

				nb_etu_traites++;
				jury_fin_annee(&promo->etudiants[i], num_sem / 2);
			}
		}
	}

	printf("Semestre termine pour %d etudiant(s)\n", nb_etu_traites);
}

// ============================================================================
// SPRINT 3 - JURY (semestres pairs)
// ============================================================================

/**
 * @brief Applique les règles de validation d'une année.
 *
 * Cette fonction générique gère tous les jurys de fin d'année (1, 2, 3...).
 *
 * Règles générales :
 * 1. Pour l'année courante :
 *    - Années intermédiaires : Avoir validé au moins MIN_RCUE_VALIDES UE et aucun blocage.
 *    - Année finale : Avoir validé TOUTES_UE_VALIDEES UE.
 * 2. Pour les années précédentes (si annee > 1) :
 *    - Avoir validé toutes les UE de l'année précédente (via compensation si nécessaire).
 *
 * @param etu Pointeur vers l'étudiant.
 * @param annee Année à valider (1, 2, 3...).
 */
void jury_fin_annee(t_etudiant* etu, int annee) {
	assert(etu != NULL && annee >= 1 && annee <= NB_ANNEES);

	float moy_annee[NB_UE];
	float moy_prec[NB_UE]; // Pour stocker les moyennes de l'année précédente si besoin

	int nb_valides = 0;
	int a_rcue_bloquant = 0;
	int annees_prec_ok = 1;

	// Vérification de l'année précédente (si elle existe)
	if (annee > 1) {
		for (int ue = 0; ue < NB_UE; ue++) {
			moy_prec[ue] = calculer_moyenne_annee(etu, annee - 1, ue);
			moy_annee[ue] = calculer_moyenne_annee(etu, annee, ue); // On en a besoin pour vérifier la compensation

			// L'UE de l'année précédente est validée si :
			// - Sa moyenne est >= SEUIL_VALIDATION
			// - OU si elle est compensée par l'UE correspondante de l'année courante (moy_annee >= SEUIL_VALIDATION)
			// Note: Dans le code original jury_fin_annee2/3, la condition était :
			// if (moy_b1[ue] < 10 && moy_b2[ue] < 10) alors KO.
			// Ce qui revient à dire : OK si moy_b1 >= 10 OU moy_b2 >= 10.
			if (moy_prec[ue] < SEUIL_VALIDATION && moy_annee[ue] < SEUIL_VALIDATION) {
				annees_prec_ok = 0;
			}
		}
	}
	else {
		// Pour la première année, il suffit de calculer ses moyennes
		for (int ue = 0; ue < NB_UE; ue++) {
			moy_annee[ue] = calculer_moyenne_annee(etu, annee, ue);
		}
	}

	// Analyse de l'année courante
	for (int ue = 0; ue < NB_UE; ue++) {
		if (moy_annee[ue] >= SEUIL_VALIDATION) {
			nb_valides++;
		}
		if (moy_annee[ue] < SEUIL_BLOQUANT) {
			a_rcue_bloquant = 1;
		}
	}

	// Application de la décision
	int admis = 0;

	if (annee < NB_ANNEES) {
		// Année intermédiaire : Passage si conditions respectées
		if (nb_valides >= MIN_RCUE_VALIDES && a_rcue_bloquant == 0 && annees_prec_ok) {
			admis = 1;
		}
	}
	else {
		// Année finale (Diplôme) : Toutes les UE doivent être validées
		if (nb_valides == TOUTES_UE_VALIDEES && annees_prec_ok) {
			admis = 1;
		}
	}

	if (admis) {
		if (annee < NB_ANNEES) {
			etu->semestre_actuel++; // Passage au S3, S5...
		}
		else {
			etu->statut = DIPLOME;
		}
	}
	else {
		etu->statut = AJOURNE;
	}
}

// ============================================================================
// SPRINT 4 - BILAN
// ============================================================================

// --- BILAN ---
/**
 * @brief Affiche le bilan d'une année spécifique.
 *
 * Comptabilise pour une année donnée (1, 2 ou 3) :
 * - Le nombre de démissions.
 * - Le nombre de défaillances.
 * - Le nombre d'étudiants en cours.
 * - Le nombre d'ajournés.
 * - Le nombre d'étudiants ayant réussi (passés à l'année sup ou diplômés).
 *
 * @param promo Pointeur vers la promotion.
 */
void cmd_bilan(const t_promotion* promo) {
	assert(promo != NULL);

	int annee;
	scanf("%d", &annee);

	if (annee < 1 || annee > NB_ANNEES) {
		printf("Annee incorrecte\n");
		return;
	}

	int nb_dem = 0;
	int nb_def = 0;
	int nb_cours = 0;
	int nb_aj = 0;
	int nb_reussi = 0;

	int sem_debut = (annee - 1) * 2 + 1;
	int sem_fin = annee * 2;

	for (int i = 0; i < promo->nb_inscrits; i++) {
		const t_etudiant* etu = &promo->etudiants[i];

		// Ignorer les étudiants qui n'ont pas atteint cette année
		if (etu->semestre_actuel < sem_debut) {
			continue;
		}

		// CAS 1 : étudiant au semestre impair de l'année
		if (etu->semestre_actuel == sem_debut) {
			if (etu->statut == DEMISSION) {
				nb_dem++;
			}
			else if (etu->statut == DEFAILLANCE) {
				nb_def++;
			}
			else if (etu->statut == EN_COURS) {
				nb_cours++;
			}
		}
		// CAS 2 : étudiant au semestre pair de l'année
		else if (etu->semestre_actuel == sem_fin) {
			if (etu->statut == DEMISSION) {
				nb_dem++;
			}
			else if (etu->statut == DEFAILLANCE) {
				nb_def++;
			}
			else if (etu->statut == EN_COURS) {
				nb_cours++;
			}
			else if (etu->statut == AJOURNE) {
				nb_aj++;
			}
			// CAS PARTICULIER : Les diplômés restent en S6
			else if (etu->statut == DIPLOME && annee == NB_ANNEES) {
				nb_reussi++;
			}
		}
		// CAS 3 : étudiant ayant dépassé cette année
		else if (etu->semestre_actuel > sem_fin) {
			nb_reussi++;
		}
	}

	printf("%d demission(s)\n", nb_dem);
	printf("%d defaillance(s)\n", nb_def);
	printf("%d en cours\n", nb_cours);
	printf("%d ajourne(s)\n", nb_aj);
	printf("%d passe(s)\n", nb_reussi);
}


// ===========================================================================================//
// Keziah GEBAUER Badis RAHLI /// 101 /// Version finale deploy� le 13/11/2025 a 22H37		 //	
// ===========================================================================================//