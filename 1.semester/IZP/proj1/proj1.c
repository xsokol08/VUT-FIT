#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/*
 * Struktura obsahuje argumenty prikazoveho riadku
 * a aj tabulku (pole znakov) s maximalnou dlzkou 1024 znakov
 * pouziva sa vo vsetkych funkciach ako argument (okrem main)
 */
typedef struct Argumenty
{
    char tabulka[1025]; //pole obshujuce tabulku
    char buffer[1025];  //pomocne pole
    long pozicia;       //aktualna pozicia v poli
    int operacia;       //operacia, ktora sa ma vykonat
    double mma;         //premena pre funkcie min, max a priemer(average)
    int pocitadlo;      //premenna pre funkciu priemer
    int zr;             //prvy riadok, ktory ma vypisat (zaciatocny riadok)
    int kr;             //posledny riadok, ktory am vypisat (konecny riadok)
    int zs;             //prvy stlpec, ktory ma vypisat (zaciatocny stlpec)
    int ks;             //posledny stlpec, ktory ma vypisat (konecny riadok)
} argumenty_pr;


/*
 * Funkcia nacita tabulku do pola v strukture a ukonci '\0'
 * nevracia ziadne hodnoty
 */
void spracuj_tabulku(argumenty_pr *args)
{
    int i, c;

    for (i = 0; i < 1025; i++){
        c = getchar();
        if (c == EOF){
            args->tabulka[i] = '\0';
            break;
        }
        args->tabulka[i] = c;
    }

    args->tabulka[1025] = '\0'; //pre istotu este raz, ak by bolo viac ako 1024 znakov
}


/*
 * Funkcia preskoci jeden riadok v tabulke
 * navratove hodnoty*
 *    -2 ak narazi na koniec riadku
 *    -1 ak narazi na koniec pola
 */
int preskoc_riadok(argumenty_pr *args)
{
    while(args->tabulka[args->pozicia] != '\n'){
        if (args->tabulka[args->pozicia] == '\n')
            return -2;

        if (args->tabulka[args->pozicia] == '\0')
            return -1;

        args->pozicia++;
    }
    args->pozicia++;
    return 0;
}


/*
 * Funkcia, ktora preskoci jeden stlpec v tabulke
 * navratove hodnoty*
 *    -1 ak narazi na koniec pola
 *    -2 ak na koniec riadku
 */
int preskoc_stlpec(argumenty_pr *args)
{
    while(args->tabulka[args->pozicia] != '\x20'){
        args->pozicia++;

        if(args->tabulka[args->pozicia] == '\0')
            return -1;

        else if(args->tabulka[args->pozicia] == '\n')
            return -2;
    }

    while(args->tabulka[args->pozicia] == '\x20'){ //preskoci mezdzeru za bunkou
        args->pozicia++;
    }

    return 0;
}


/*
 * Funkcia vyhlada maximum z dvoch hodnôt
 * a priradi do args->mma
 */
void max(argumenty_pr *args)
{
    int cislo;
    char *c;

    if((cislo = strtol(args->buffer, &c, 10)) > args->mma)
        args->mma = cislo;
}


/*
 * Funkcia vyhlada minimum z dvoch hodnot
 * a priradi do args->mma
 * Ak je pocitadlo 0 tak prve cislo priradi bez porovnavania ako najmensie este nema cim porovnat
 */
void min(argumenty_pr *args)
{
    int cislo;
    char *c;

    if(args->pocitadlo == 0){ //prve cislo sa automaticky priradi ako najmensie
        cislo = strtol(args->buffer, &c, 10);
        if(c[0] == '\0'){
           args->mma = cislo; 
           args->pocitadlo++;
        }
    }
    else
        if(args->mma > (cislo = strtol(args->buffer, &c, 10)))
            if(c[0] == '\0')
                args->mma = cislo;
}


/*
 * Funkcia na pocitanie priemeru
 * v skutocnosti priemer nevypocita, len scita vybranu oblast buniek
 * priemer sa vypocita az pri vypise vo funkcii main
 */
void priemer(argumenty_pr *args)
{
    char *c;

    args->mma += strtol(args->buffer, &c, 10);
    args->pocitadlo++;
}


/*
 * funkcia vyberie operaciu ktora sa ma vykonat
 *       1 -> select
 *       2 -> min
 *       3 -> max
 *       4 -> priemer
 */
void operacia(argumenty_pr *args)
{
    switch(args->operacia){
        case 1:
            if(args->buffer[0] != '\0')
                printf("%s\n", args->buffer);
            break;
        case 2:
            if(args->buffer[0] != '\0')
                min(args);
            break;
        case 3:
            if(args->buffer[0] != '\0')
                max(args);
            break;
        case 4:
            if(args->buffer[0] != '\0')
                priemer(args);
            break;
        default:
            break;
    }
}


/*
 * Tato funkcia vyberie bunku z tabulky
 * navratove hodnoty *
 *      -2 ak narazi na koniec radku ('\n')
 *      -1 ak narazi na koniec tabulky
 */
int vyber_bunku(argumenty_pr *args)
{
    int i = 0; //pozicia v bufferi

    while(args->tabulka[args->pozicia] != '\x20'){ //vyberie do bufferu bunku a zakonci \0

        if (args->tabulka[args->pozicia] == '\n'){ //bezpecnostne opatrenia
            args->buffer[i] = '\0';
            operacia(args);
            if(args->zr != 0 && args != 0){
                if(args->operacia == 4)
                    printf("%.10g\n", args->mma/args->pocitadlo);
                else if(args->operacia != 1)
                    printf("%.10g\n", args->mma);
            }
            return -2; // -2 == \n
        } 

        else if(args->tabulka[args->pozicia] == '\0'){
            args->buffer[i] = '\0';
            operacia(args);
            if(args->operacia == 4)
                printf("%.10g\n", args->mma/args->pocitadlo);
            else if(args->operacia != 1)
                printf("%.10g\n", args->mma);
            return -1;
        }

        args->buffer[i] = args->tabulka[args->pozicia];
        args->pozicia++;
        i++;
    }

    args->buffer[i] = '\0';

    while(args->tabulka[args->pozicia] == '\x20'){ //preskoci medzeru za bunkou
        args->pozicia++;
    }

    operacia(args); //vyberie aku operaciu ma vykonat

    if (args->zs == 0 && args->ks == 0) //Ak ma vypisat riadok, tak ho rekurzivne vypise
        vyber_bunku(args);
    return 0;
}


/*
 * funkcia vyberie rozsah z tabulky
 * obsahuje aj rozne chybove hlasenia
 */
int vyber_rozsah(argumenty_pr *args)
{
    int i, j, v; //v je vysledok funkcie

    for (i = 1; i < args->zr; i++){ //preskoci riadky, ak preskoci na riadok za tabulou vypise chybu
        if (preskoc_riadok(args) == -1){
            fprintf(stderr, "Stlpec %d neexistuje\n", args->zs);
            exit(EXIT_FAILURE);
        }
    }

    for (i = args->zr; i <= args->kr; i++){ //najprv iteruje riadky, potom stlpce

        for (j = 1; j < args->zs; j++){  //preskoci stlpce pred bunkou
            if (preskoc_stlpec(args) == -2){
                fprintf(stderr, "Stlpec %d neexistuje\n", j+1);  //ak skoci na bunku za rozsahom tabulky, vypise chybu
                exit(EXIT_FAILURE);
            }
        }

        for (j = args->zs; j <= args->ks; j++){  //Zacne vyberat bunky
            v = vyber_bunku(args); //ukladá výsledok funkcie
            if ((v == -2) && (j != args->ks)){ // Ak je na konci riadku a este ma vypisovať stĺpce vypíše chybu
                fprintf(stderr, "Stlpec %d neexistuje\n", j+1);
                exit(EXIT_FAILURE);
            }
            else if (v == -2 && args->ks == 0) //Prvi vypisovani celeho riadku ak narazi na znak \n tak skonci uspesne
                exit(EXIT_SUCCESS);
            else if (v == -1) //ak narazi na koniec tabulky pri vypisovani celeho riadku skonci uspesne
                exit(EXIT_SUCCESS);
        }

        if (v != -2) // preskočí zvyšné stĺpce za bunkou
            if (preskoc_riadok(args) == -1)
                return 0;
    }

    if(args->zr == 0 && args->kr == 0) //ak ma vybrat cely stlpec, tak ho rekurzivne vypise
        vyber_rozsah(args);


    return 0;
}


/*
 * funkcia napovedy programu, vytlaci help a skonci
 */
int help()
{
    printf("Vitajte v nápovede!\nvytvoril: Juraj Sokol\n\n");
    printf("O Programe:\nvyhľadá a vypíše vybrané bumky z tabuľky, nájde ich minimum, maximum alebo priemer\n");
    printf("Ako používať program:\n    select -> vypíše vybrané bunky z tabuľky\n    min -> vyhľadá a vypíše minimum zo zadaného rozsahu buniek\n");
    printf("    max -> vyhľadá a vypíše maximum zo zadaného rozsahu buniek\n    avg -> vyhľadá a vypíše priemer zo zadaného rozsahu buniek\n");
    printf("    row X značí výběr všech buněk na řádku X (X > 0)\n    col X značí výběr všech buněk ve sloupci X (X > 0)\n    rows X Y značí výběr všech buněk od řádku X (včetně) až po Y (včetně). 0 < X <= Y\n    cols X Y značí výběr všech buněk od sloupce X (včetně) až po Y (včetně). 0 < X <= Y\n    range A B X Y značí výběr buněk od řádku A po řádek B a od sloupce X po sloupec Y (včetně daných řádků a sloupců). 0 < A <= B, 0 < X <= Y\n");
    return 0;
}


int main(int argc, char *argv[])
{
    char *str;
    argumenty_pr args;

    if (strcmp(argv[1], "--help") == 0){
        help();
        exit(EXIT_SUCCESS);
    }

    spracuj_tabulku(&args);

    args.pozicia = 0;
    args.mma = 0;
    args.pocitadlo = 0;

    if (strcmp(argv[1], "select") == 0)
        args.operacia = 1;
    else if(strcmp(argv[1], "min") == 0)
        args.operacia = 2;
    else if(strcmp(argv[1], "max") == 0)
        args.operacia = 3;
    else if(strcmp(argv[1], "avg") == 0)
        args.operacia = 4;
    else{
        fprintf(stderr, "Prvý argument môže byť len \"select\", \"avg\", \"min\" a \"max\", nič iné!\n");
        exit(EXIT_FAILURE);
    }
        

    if (strcmp(argv[2], "row") == 0){

        if(argc != 4){
        fprintf(stderr, "Nesprávny počet argumentov, skús znovu alebo sa pozri do nápovedy\n");
        exit(EXIT_FAILURE);
        }

        args.zr = args.kr = strtol(argv[3], &str, 10);
        args.zs = args.ks = 0;

        if(str[0] != '\0'){
        fprintf(stderr, "Nesprávny typ argumentov, skús znovu alebo sa pozri do nápovedy\n");
        exit(EXIT_FAILURE);
        }
    }

    else if (strcmp(argv[2], "col") == 0){

        if(argc != 4){
        fprintf(stderr, "Nesprávny počet argumentov, skús znovu alebo sa pozri do nápovedy\n");
        exit(EXIT_FAILURE);
        }

        args.zs = args.ks = strtol(argv[3], &str, 10);
        args.zr = args.kr = 0;


        if(str[0] != '\0'){
        fprintf(stderr, "Nesprávny typ argumentov, skús znovu alebo sa pozri do nápovedy\n");
        exit(EXIT_FAILURE);
        }
    }

    else if (strcmp(argv[2], "range") == 0){

        if(argc != 7){
        fprintf(stderr, "Nesprávny počet argumentov, skús znovu alebo sa pozri do nápovedy\n");
        exit(EXIT_FAILURE);
        }

        args.zr = strtol(argv[3], &str, 10);

        if(str[0] != '\0'){
        fprintf(stderr, "Nesprávny typ argumentov, skús znovu alebo sa pozri do nápovedy\n");
        exit(EXIT_FAILURE);
        }
        args.kr = strtol(argv[4], &str, 10);

        if(str[0] != '\0'){
        fprintf(stderr, "Nesprávny typ argumentov, skús znovu alebo sa pozri do nápovedy\n");
        exit(EXIT_FAILURE);
        }
        args.zs = strtol(argv[5], &str, 10);

        if(str[0] != '\0'){
        fprintf(stderr, "Nesprávny typ argumentov, skús znovu alebo sa pozri do nápovedy\n");
        exit(EXIT_FAILURE);
        }
        args.ks = strtol(argv[6], &str, 10);


        if(str[0] != '\0'){
        fprintf(stderr, "Nesprávny typ argumentov, skús znovu alebo sa pozri do nápovedy\n");
        exit(EXIT_FAILURE);
        }
    }

    else if (strcmp(argv[2], "cols") == 0){

        if(argc != 5){
        fprintf(stderr, "Nesprávny počet argumentov, skús znovu alebo sa pozri do nápovedy\n");
        exit(EXIT_FAILURE);
        }

        args.zs = strtol(argv[3], &str, 10);
        args.ks = strtol(argv[4], &str, 10);
        args.zr = args.kr = 0;


        if(str[0] != '\0'){
        fprintf(stderr, "Nesprávny typ argumentov, skús znovu alebo sa pozri do nápovedy\n");
        exit(EXIT_FAILURE);
        }
    }

    else if (strcmp(argv[2], "rows") == 0){

        if(argc != 5){
        fprintf(stderr, "Nesprávny počet argumentov, skús znovu alebo sa pozri do nápovedy\n");
        exit(EXIT_FAILURE);
        }

        args.zr = strtol(argv[3], &str, 10);    
        args.kr = strtol(argv[4], &str, 10);
        args.zs = args.ks = 0;

        if(str[0] != '\0'){
        fprintf(stderr, "Nesprávny typ argumentov, skús znovu alebo sa pozri do nápovedy\n");
        exit(EXIT_FAILURE);
        }
    }

    else{
        fprintf(stderr, "Druhý argument môže byť len \"row\", \"col\" alebo \"range\", nič iné!\n");
        exit(EXIT_FAILURE);
    }

    vyber_rozsah(&args);

    switch(args.operacia){
        case 2:
            printf("%.10g\n", args.mma);
            break;
        case 3:
            printf("%.10g\n", args.mma);
            break;
        case 4:
            printf("%.10g\n", args.mma/args.pocitadlo);
            break;
        default:
            break;
    }

    return 0;
}
